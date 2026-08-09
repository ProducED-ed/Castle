#!/bin/bash
# fix-wlan-names.sh
# Ставит wlan0 = встроенный WiFi Pi, wlan1 = USB-донгл. Запускается на загрузке
# до hostapd и wpa_supplicant (см. fix-wlan-names.service).
#
# ЗАЧЕМ. Ядро раздаёт имена wlanN в порядке появления устройств, и он не
# гарантирован. Если донгл воткнут в момент загрузки, он может стать wlan0 —
# а hostapd настроен именно на wlan0 и попытается поднять сеть Castle на
# донгле. Донглы RTL8188EU/MT7601U точку доступа не умеют, hostapd падает с
# «nl80211: Could not configure driver mode», сеть Castle не появляется, и
# снаружи это выглядит как «замок не включился»: пульт недоступен, потому что
# подключиться к нему не по чему.
#
# ПОЧЕМУ ПО ШИНЕ, А НЕ ПО MAC. Прежняя версия сверяла два MAC-адреса, зашитых
# в скрипт. На клоне золотого образа они принадлежат ДРУГОМУ замку, никогда не
# совпадают, и скрипт молча превращается в no-op — ровно это и случилось на
# CLC4 (2026-08-09) с образа CLC3. Шина же одинакова на всех комплектах:
# встроенный WiFi Pi 4 сидит на SDIO (brcmfmac, путь содержит mmc), а любой
# донгл — на USB. Значит скрипт клиент-независимый и правки не требует.

BUILTIN=""
USBIF=""

for dev in /sys/class/net/wlan*; do
    [ -e "$dev" ] || continue
    name=$(basename "$dev")
    path=$(readlink -f "$dev/device" 2>/dev/null) || continue
    case "$path" in
        *mmc*)  BUILTIN="$name" ;;
        */usb*) USBIF="$name" ;;
    esac
done

echo "fix-wlan-names: встроенный='${BUILTIN:-нет}' usb='${USBIF:-нет}'"

# Донгла нет — переименовывать нечего, встроенный и так wlan0.
if [ -z "$USBIF" ]; then
    exit 0
fi

if [ -z "$BUILTIN" ]; then
    echo "fix-wlan-names: встроенный WiFi не найден — не трогаю имена"
    exit 0
fi

rename_iface() {
    ip link set "$1" down 2>/dev/null
    ip link set "$1" name "$2" || return 1
    ip link set "$2" up 2>/dev/null
    return 0
}

# 1. Освободить wlan0, если его занял донгл.
if [ "$USBIF" = "wlan0" ]; then
    echo "fix-wlan-names: донгл занял wlan0, отодвигаю"
    rename_iface wlan0 wlanUsbTmp && USBIF=wlanUsbTmp
fi

# 2. Встроенный → wlan0 (только он умеет режим точки доступа).
if [ "$BUILTIN" != "wlan0" ]; then
    echo "fix-wlan-names: встроенный $BUILTIN → wlan0"
    rename_iface "$BUILTIN" wlan0 && BUILTIN=wlan0
fi

# 3. Донгл → wlan1.
if [ "$USBIF" != "wlan1" ]; then
    echo "fix-wlan-names: донгл $USBIF → wlan1"
    rename_iface "$USBIF" wlan1 && USBIF=wlan1
fi

# 4. wpa_supplicant должен подхватить правильное имя.
systemctl restart wpa_supplicant 2>/dev/null || true

echo "fix-wlan-names: итог — wlan0=$BUILTIN wlan1=$USBIF"
exit 0
