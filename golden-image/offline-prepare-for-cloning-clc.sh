#!/bin/bash
# offline-prepare-for-cloning-clc.sh
# Обезличивает .img-файл БЕЗ запуска на живой Pi.
# Работает с холодным образом через loop-mount → удаление identity-файлов → umount.
#
# Использование:
#   sudo bash offline-prepare-for-cloning-clc.sh <path-to-image.img>
#
# Пример:
#   sudo bash offline-prepare-for-cloning-clc.sh /home/user/clc3-as-is.img
#
# Что делает (всё прямо в filesystem .img, без chroot):
#   1. Удаляет Tailscale state (node key + machine key)
#   2. Отключает автозапуск tailscaled (убирает symlink в multi-user.target.wants)
#   3. Очищает machine-id (systemd сгенерирует на первом boot)
#   4. Удаляет SSH host keys (regenerate.service создаст на первом boot)
#   5. Чистит логи, journal, bash_history, DHCP-leases
#   6. Обнуляет castle.log
#   7. Удаляет /boot/clc-hostname.txt если был
#
# Что СОХРАНЯЕТСЯ:
#   - /etc/wpa_supplicant/wpa_supplicant.conf (Produced WiFi)
#   - /etc/hostapd/hostapd.conf (Castle AP)
#   - /etc/systemd/system/castleserver.service
#   - /usr/local/sbin/first-boot-clc.sh
#   - Все прошивки в /home/pi/New/

set -e

IMG="$1"
if [ -z "$IMG" ] || [ ! -f "$IMG" ]; then
  echo "❌ Использование: sudo bash $0 <path-to-image.img>"
  exit 1
fi

if [ "$(id -u)" -ne 0 ]; then
  echo "❌ Запуск только под root: sudo bash $0 $IMG"
  exit 1
fi

# Проверим что нужные утилиты есть
for cmd in losetup partx mount umount sfdisk; do
  command -v $cmd >/dev/null 2>&1 || { echo "❌ Не найден: $cmd"; exit 1; }
done

echo "═══════════════════════════════════════════════════════════════"
echo "  CLC Offline Image Prepare"
echo "  Образ: $IMG"
echo "  Размер: $(du -h "$IMG" | cut -f1)"
echo "═══════════════════════════════════════════════════════════════"
echo ""

# Подключаем loopback (с автоматическим сканом партиций)
LOOP=$(losetup --show -fP "$IMG")
echo "[loopback] $LOOP"
# Дать ядру время появить partition-устройства
sleep 1
partx -u "$LOOP" 2>/dev/null || true

# На Raspberry Pi образ обычно состоит из 2 партиций:
#   ${LOOP}p1 — boot (FAT32)
#   ${LOOP}p2 — rootfs (ext4)
BOOT_PART="${LOOP}p1"
ROOT_PART="${LOOP}p2"

if [ ! -b "$ROOT_PART" ]; then
  echo "❌ Не найдена rootfs-партиция $ROOT_PART"
  losetup -d "$LOOP"
  exit 1
fi

MNT=$(mktemp -d /tmp/clc-prepare.XXXXXX)
MNT_BOOT="$MNT/boot"
mkdir -p "$MNT_BOOT"

# Функция cleanup на любой выход
cleanup() {
  echo ""
  echo "[cleanup] umount + losetup -d"
  sync
  umount "$MNT_BOOT" 2>/dev/null || true
  umount "$MNT" 2>/dev/null || true
  losetup -d "$LOOP" 2>/dev/null || true
  rmdir "$MNT_BOOT" 2>/dev/null || true
  rmdir "$MNT" 2>/dev/null || true
}
trap cleanup EXIT

echo "[mount] $ROOT_PART → $MNT"
mount -o rw "$ROOT_PART" "$MNT"

# /boot/firmware на Bookworm живёт на отдельной партиции
echo "[mount] $BOOT_PART → $MNT_BOOT"
mount -o rw "$BOOT_PART" "$MNT_BOOT" 2>/dev/null || echo "  (boot не примонтирован — пропускаем)"

# ───── ОЧИСТКА ─────

echo ""
echo "[1/8] Tailscale state…"
rm -f "$MNT/var/lib/tailscale/tailscaled.state"
rm -rf "$MNT/var/lib/tailscale/files" 2>/dev/null || true

echo "[2/8] Отключаем автозапуск tailscaled…"
# multi-user.target.wants содержит symlink на /lib/systemd/system/tailscaled.service
# Удаление symlink = systemctl disable. Сам бинарь и unit оставляем.
rm -f "$MNT/etc/systemd/system/multi-user.target.wants/tailscaled.service"
rm -f "$MNT/etc/systemd/system/sysinit.target.wants/tailscaled.service" 2>/dev/null || true

echo "[3/8] machine-id…"
: > "$MNT/etc/machine-id"
rm -f "$MNT/var/lib/dbus/machine-id"
# symlink будет восстановлен systemd при первом boot

echo "[4/8] SSH host keys…"
rm -f "$MNT/etc/ssh/ssh_host_"*

# Создаём oneshot-сервис который сгенерирует ключи на первом boot.
# Используем heredoc прямо в file
SVC_FILE="$MNT/etc/systemd/system/regenerate-ssh-host-keys.service"
cat > "$SVC_FILE" <<'EOF'
[Unit]
Description=Regenerate SSH host keys after image clone
ConditionPathExists=!/etc/ssh/ssh_host_rsa_key
Before=ssh.service

[Service]
Type=oneshot
ExecStart=/usr/bin/ssh-keygen -A
ExecStartPost=/bin/systemctl disable regenerate-ssh-host-keys.service

[Install]
WantedBy=multi-user.target
EOF
# Включаем сервис через создание symlink
mkdir -p "$MNT/etc/systemd/system/multi-user.target.wants"
ln -sf /etc/systemd/system/regenerate-ssh-host-keys.service \
       "$MNT/etc/systemd/system/multi-user.target.wants/regenerate-ssh-host-keys.service"

echo "[5/8] systemd journal + системные логи…"
rm -rf "$MNT/var/log/journal/"* 2>/dev/null || true
rm -f "$MNT/var/log/wtmp" "$MNT/var/log/btmp" "$MNT/var/log/lastlog"
for f in auth.log syslog daemon.log kern.log messages user.log; do
  : > "$MNT/var/log/$f" 2>/dev/null || true
done
rm -f "$MNT/var/log/"*.gz "$MNT/var/log/"*.[0-9] 2>/dev/null || true

echo "[6/8] bash history + кэши пользователей…"
rm -f "$MNT/root/.bash_history" "$MNT/home/pi/.bash_history"
rm -f "$MNT/root/.lesshst" "$MNT/home/pi/.lesshst"
rm -f "$MNT/root/.viminfo" "$MNT/home/pi/.viminfo"

echo "[7/8] castle.log + diag-логи + DHCP leases…"
: > "$MNT/home/pi/New/castle.log" 2>/dev/null || true
rm -f "$MNT/home/pi/New/castle.log."* 2>/dev/null || true
rm -rf "$MNT/var/log/castle_diag/"* 2>/dev/null || true
rm -f "$MNT/var/lib/dhcp/"*.leases 2>/dev/null || true
rm -f "$MNT/var/lib/dhcpcd/"*.lease 2>/dev/null || true
rm -f "$MNT/var/lib/NetworkManager/"*.lease 2>/dev/null || true

echo "[8/8] /boot/clc-hostname.txt + first-boot marker…"
rm -f "$MNT_BOOT/clc-hostname.txt" 2>/dev/null || true
rm -f "$MNT/boot/firmware/clc-hostname.txt" 2>/dev/null || true
rm -f "$MNT/var/lib/clc-first-boot-done"

# Сбросить Bluetooth pref — на новой Pi пользователь сам решит
rm -f "$MNT/var/lib/castle_bluetooth_pref" 2>/dev/null || true

# ───── ПРОВЕРКА ЧТО ВАЖНОЕ СОХРАНЕНО ─────

echo ""
echo "[verify] Эти файлы должны быть на месте:"
check() {
  if [ -e "$MNT$1" ]; then
    echo "  ✓ $1"
  else
    echo "  ❌ $1 — ОТСУТСТВУЕТ! (возможно образ не CLC-эталон)"
  fi
}
check /etc/wpa_supplicant/wpa_supplicant.conf
check /etc/hostapd/hostapd.conf
check /etc/systemd/system/castleserver.service
check /usr/local/sbin/first-boot-clc.sh
check /home/pi/New/CastleServer.py

echo ""
echo "[verify] tailscaled.service автозапуск должен быть ВЫКЛЮЧЕН:"
if [ -L "$MNT/etc/systemd/system/multi-user.target.wants/tailscaled.service" ]; then
  echo "  ❌ Symlink ещё на месте! Проверь скрипт."
else
  echo "  ✓ disabled (symlink удалён)"
fi

echo ""
echo "[verify] regenerate-ssh-host-keys.service должен быть ВКЛЮЧЁН:"
if [ -L "$MNT/etc/systemd/system/multi-user.target.wants/regenerate-ssh-host-keys.service" ]; then
  echo "  ✓ enabled (отработает на первом boot)"
else
  echo "  ❌ Symlink НЕ создан!"
fi

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo "  ✓ Образ обезличен."
echo ""
echo "  Следующие шаги:"
echo "    pishrink.sh $IMG clc-golden-\$(date +%Y%m%d).img"
echo "    sudo dd if=clc-golden-*.img of=/dev/sdX bs=4M status=progress"
echo "═══════════════════════════════════════════════════════════════"
