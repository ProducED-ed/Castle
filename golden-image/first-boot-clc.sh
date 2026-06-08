#!/bin/bash
# first-boot-clc.sh
# Запускается ОДИН раз после первой загрузки клона из золотого образа.
# Задаёт hostname по запросу оператора.
#
# Tailscale В ЭТОМ СКРИПТЕ НЕ ВКЛЮЧАЕТСЯ — это делает оператор сам:
#   sudo hostnamectl set-hostname CLC-NN-страна   (если хочет поменять что задал тут)
#   sudo systemctl enable --now tailscaled
#   sudo tailscale up
#
# Запуск:
#   автоматически через systemd (firstboot.service) ИЛИ вручную:
#   sudo bash /usr/local/sbin/first-boot-clc.sh [hostname]
#
# Файл-маркер /var/lib/clc-first-boot-done создаётся в конце — повторный запуск
# ничего не делает (защита от случайного повторного вызова).

set -e

MARKER=/var/lib/clc-first-boot-done

if [ -f "$MARKER" ]; then
  echo "first-boot уже выполнен ($(cat $MARKER)). Если нужно повторить — удалите $MARKER."
  exit 0
fi

if [ "$(id -u)" -ne 0 ]; then
  echo "❌ Запуск только под root: sudo bash $0"
  exit 1
fi

# Источники имени по приоритету: аргумент → /boot/clc-hostname.txt → интерактивный
HOSTNAME_NEW=""

if [ -n "$1" ]; then
  HOSTNAME_NEW="$1"
elif [ -f /boot/clc-hostname.txt ]; then
  HOSTNAME_NEW="$(tr -d '[:space:]' < /boot/clc-hostname.txt)"
elif [ -f /boot/firmware/clc-hostname.txt ]; then
  HOSTNAME_NEW="$(tr -d '[:space:]' < /boot/firmware/clc-hostname.txt)"
else
  echo ""
  echo "═══════════════════════════════════════════════════════════════"
  echo "  CLC First Boot — задайте hostname"
  echo "═══════════════════════════════════════════════════════════════"
  echo ""
  echo "  Имя должно быть в формате: clc-NN-страна"
  echo "  Только латиница a-z, цифры, дефис. БЕЗ подчёркиваний и БЕЗ заглавных"
  echo "  (требование Tailscale MagicDNS)."
  echo ""
  echo "  Примеры:  clc-04-russia, clc-05-uae, clc-06-france"
  echo ""
  read -p "Hostname: " HOSTNAME_NEW
fi

# Валидация (RFC 1123 + Tailscale MagicDNS)
if ! [[ "$HOSTNAME_NEW" =~ ^[a-z0-9][a-z0-9-]{1,62}[a-z0-9]$ ]]; then
  echo "❌ Невалидное имя: '$HOSTNAME_NEW'"
  echo "   Допустимо: a-z, 0-9, дефис. Длина 3-63. Не начинать/заканчивать дефисом."
  exit 1
fi

echo ""
echo "→ Устанавливаем hostname: $HOSTNAME_NEW"
hostnamectl set-hostname "$HOSTNAME_NEW"

# Правим /etc/hosts
sed -i "s/127\.0\.1\.1.*/127.0.1.1\t$HOSTNAME_NEW/" /etc/hosts
grep -q "127.0.1.1" /etc/hosts || echo -e "127.0.1.1\t$HOSTNAME_NEW" >> /etc/hosts

# Удаляем /boot/clc-hostname.txt чтобы не запустилось второй раз
rm -f /boot/clc-hostname.txt /boot/firmware/clc-hostname.txt

# machine-id — если ещё пустой (после обезлички образа), пусть systemd сгенерирует
if [ ! -s /etc/machine-id ]; then
  echo "→ Генерируем новый machine-id…"
  systemd-machine-id-setup
fi

# SSH host keys — если их нет, создать сейчас (на случай что
# regenerate-ssh-host-keys.service ещё не отработал)
if [ ! -f /etc/ssh/ssh_host_rsa_key ]; then
  echo "→ Генерируем SSH host keys…"
  ssh-keygen -A
  systemctl restart ssh 2>/dev/null || true
fi

# Маркер выполнения
date -Iseconds > "$MARKER"

echo ""
echo "═══════════════════════════════════════════════════════════════"
echo "  ✓ First boot завершён."
echo ""
echo "  Hostname:    $HOSTNAME_NEW"
echo "  Castle WiFi: 192.168.4.1:3000 (должен работать уже сейчас)"
echo "  Produced:    подключится если в зоне действия"
echo ""
echo "  Tailscale — ЕСЛИ хотите подключить, выполните вручную:"
echo "      sudo systemctl enable --now tailscaled"
echo "      sudo tailscale up"
echo ""
echo "  (URL для авторизации появится в выводе — откройте в браузере)"
echo "═══════════════════════════════════════════════════════════════"
