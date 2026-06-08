#!/bin/bash
# prepare-for-cloning-clc.sh
# Запускается на ЭТАЛОННОЙ CLC Pi (CLC3) непосредственно перед снятием dd-образа.
# Стирает identity (Tailscale node key, machine-id, SSH host keys, журналы).
# WiFi-конфиги (wpa_supplicant + hostapd) сохраняются — на каждой клонированной Pi
# они должны работать сразу же без правки.
#
# ВАЖНО: после этого скрипта Pi нужно ВЫКЛЮЧИТЬ командой `sudo poweroff`
# (НЕ перезагружать — иначе systemd сгенерирует новые machine-id/SSH-keys
# обратно и образ снова станет «персональным»).
#
# Запуск:  sudo bash prepare-for-cloning-clc.sh
#
# ⚠️ Не запускать через Tailscale-SSH! После `tailscale logout` соединение оборвётся.
# Подключаться через локальный SSH (Castle WiFi 192.168.4.1, или HDMI+клавиатура,
# или Ethernet через офисный ПК).

set -e

if [ "$(id -u)" -ne 0 ]; then
  echo "❌ Запуск только под root: sudo bash $0"
  exit 1
fi

# Защита от случайного запуска по Tailscale
if [ -n "$SSH_CLIENT" ]; then
  CLIENT_IP=$(echo "$SSH_CLIENT" | awk '{print $1}')
  if [[ "$CLIENT_IP" =~ ^100\. ]]; then
    echo "⚠️  Похоже вы зашли через Tailscale ($CLIENT_IP)."
    echo "    После tailscale logout вы потеряете SSH-сессию и не сможете завершить."
    echo "    Подключитесь через локальную сеть (192.168.4.1 на Castle WiFi)."
    read -p "Всё равно продолжить? (введите YES): " confirm
    [ "$confirm" = "YES" ] || exit 1
  fi
fi

echo "═══════════════════════════════════════════════════════════════"
echo "  CLC Golden Image — обезличивание перед клонированием"
echo "═══════════════════════════════════════════════════════════════"
echo ""

# 1. Остановить квестовый сервер чтобы не писал в логи
echo "[1/9] Останавливаем castleserver…"
systemctl stop castleserver 2>/dev/null || true

# 2. Tailscale: logout + удалить state, выключить автозапуск
echo "[2/9] Сбрасываем Tailscale identity и ВЫКЛЮЧАЕМ автозапуск…"
tailscale logout 2>/dev/null || true
systemctl stop tailscaled 2>/dev/null || true
rm -f /var/lib/tailscale/tailscaled.state
# На золотом образе tailscaled НЕ должен стартовать сам — клиент сначала
# меняет hostname, потом сам делает `systemctl enable --now tailscaled`.
systemctl disable tailscaled 2>/dev/null || true

# 3. machine-id — пустой; systemd-firstboot.service сгенерирует новый при следующем boot
echo "[3/9] Стираем machine-id (систему сгенерирует на первой загрузке)…"
truncate -s 0 /etc/machine-id
rm -f /var/lib/dbus/machine-id
ln -sf /etc/machine-id /var/lib/dbus/machine-id

# 4. SSH host keys — удалить, regenerate.service создаст на первой загрузке
echo "[4/9] Удаляем SSH host keys (regenerate на первом boot)…"
rm -f /etc/ssh/ssh_host_*

# Включаем сервис который сгенерирует ключи при следующем boot
if [ ! -f /etc/systemd/system/regenerate-ssh-host-keys.service ]; then
  cat > /etc/systemd/system/regenerate-ssh-host-keys.service <<'EOF'
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
  systemctl enable regenerate-ssh-host-keys.service
fi

# 5. Журналы systemd, bash history, last-login, временные файлы
echo "[5/9] Чистим журналы и историю…"
journalctl --rotate 2>/dev/null || true
journalctl --vacuum-time=1s 2>/dev/null || true
rm -rf /var/log/journal/*
rm -f /var/log/wtmp /var/log/btmp /var/log/lastlog
> /var/log/auth.log 2>/dev/null || true
> /var/log/syslog 2>/dev/null || true
> /var/log/daemon.log 2>/dev/null || true
rm -f /root/.bash_history /home/pi/.bash_history
rm -f /root/.lesshst /home/pi/.lesshst
history -c 2>/dev/null || true

# 6. Логи castleserver — обнуляем, чтобы не уехали в продакшен с тестами CLC3
echo "[6/9] Очищаем castle.log…"
> /home/pi/New/castle.log 2>/dev/null || true
rm -f /home/pi/New/castle.log.* 2>/dev/null || true
rm -rf /var/log/castle_diag/* 2>/dev/null || true

# 7. /boot/clc-hostname.txt — если был на тестах, убрать (на клонах кладёт оператор)
echo "[7/9] Убираем /boot/clc-hostname.txt если был…"
rm -f /boot/clc-hostname.txt /boot/firmware/clc-hostname.txt

# 8. ARP/DHCP кеш
echo "[8/9] Чистим DHCP-leases…"
rm -f /var/lib/dhcp/*.leases /var/lib/dhcpcd/*.lease 2>/dev/null || true

# 9. Проверочный вывод что важное на месте
echo "[9/9] Проверка что эти файлы СОХРАНЕНЫ (должны быть):"
echo -n "  wpa_supplicant.conf (Produced WiFi): "; [ -f /etc/wpa_supplicant/wpa_supplicant.conf ] && echo "✓" || echo "❌"
echo -n "  hostapd.conf (Castle AP):            "; [ -f /etc/hostapd/hostapd.conf ] && echo "✓" || echo "❌"
echo -n "  castleserver сервис:                 "; [ -f /etc/systemd/system/castleserver.service ] && echo "✓" || echo "❌"
echo -n "  first-boot-clc.sh:                   "; [ -f /usr/local/sbin/first-boot-clc.sh ] && echo "✓" || echo "❌"
echo -n "  regenerate-ssh-keys сервис:          "; systemctl is-enabled regenerate-ssh-host-keys.service 2>/dev/null || echo "не enabled!"

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo "  ✓ Готово. Pi обезличена."
echo ""
echo "  СЛЕДУЮЩИЙ ШАГ:  sudo poweroff"
echo "  (НЕ reboot — иначе machine-id и SSH-keys регенерятся обратно"
echo "   и образ станет «персональным»)"
echo ""
echo "  Затем выньте SD-карту и снимите dd-образ на другой машине:"
echo "    sudo dd if=/dev/mmcblk0 of=clc-golden-\$(date +%Y%m%d).img bs=4M status=progress"
echo "    pishrink.sh clc-golden-*.img   # сжать пустое место"
echo "═══════════════════════════════════════════════════════════════"
