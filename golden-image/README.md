# CLC Golden Image — workflow подготовки клонов

Этот каталог содержит скрипты для **тиражирования** CLC-Pi из эталонного образа
(сейчас эталон — CLC3, Россия).

## Зачем

При прямом `dd`-клонировании Pi новый Pi наследует у эталона:
- **Tailscale node key** — новая Pi подключится в Tailscale-сеть **под именем CLC3**
  и будет конфликтовать с реальным CLC3 ([CLC1 vs CLC2 инцидент 12 мая]).
- **machine-id** — конфликт systemd-журнала и dbus.
- **SSH host keys** — клиенты увидят MITM-предупреждение.
- **bash history, логи, кэши** — следы тестирования эталона.

Три скрипта решают это:

| Скрипт | Где запускается | Когда |
|---|---|---|
| **`offline-prepare-for-cloning-clc.sh`** ⭐ | На любой Linux/WSL машине | На **копии** .img-файла, БЕЗ участия живого CLC3 |
| `prepare-for-cloning-clc.sh` | На эталоне (CLC3) | На **живой** Pi, если нужно обезличить именно её SD |
| `first-boot-clc.sh` | На клоне (новая Pi) | Один раз после первой загрузки |

**Рекомендуемый workflow — через offline-вариант**, чтобы не трогать работающий CLC3.

---

## Основной workflow (БЕЗ обезлички живого CLC3)

### Шаг 1. Снять текущий CLC3 как есть

```bash
# Остановить квестовый сервер на Pi
sudo systemctl stop castleserver
sync

# Снять полный образ SD-карты в файл
sudo dd if=/dev/mmcblk0 of=clc3-as-is-$(date +%Y%m%d).img bs=4M status=progress
sudo systemctl start castleserver   # вернуть сервис
```

**Идеальный вариант** — выключить Pi (`sudo poweroff`), вынуть SD, прочитать через
card-reader на VPS/ноутбуке (`dd if=/dev/sdb of=clc3-as-is.img bs=4M`). Это гарантированно консистентный образ. ~30 мин простоя замка, потом SD вернуть назад.

### Шаг 2. Обезличить КОПИЮ образа

```bash
sudo bash offline-prepare-for-cloning-clc.sh clc3-as-is-*.img
```

Скрипт через loop-mount удалит identity-файлы прямо в .img:
- Tailscale state + автозапуск выключен
- machine-id обнулён
- SSH host keys удалены (regenerate.service создаст на первом boot клона)
- логи / journal / bash_history / castle.log / DHCP leases очищены

**WiFi-конфиги (`wpa_supplicant.conf`, `hostapd.conf`) сохраняются** — после клонирования сразу работают и Castle AP, и автоподключение к `Produced`.

### Шаг 3. Сжать пустое место

```bash
# Установить если ещё нет: https://github.com/Drewsif/PiShrink
sudo pishrink.sh clc3-as-is-*.img clc-golden-$(date +%Y%m%d).img
```

`pishrink` уменьшает rootfs до фактически занятого размера. При первом boot клона раздел сам разрастётся до размера новой SD.

### Шаг 4. Записать на ТЕСТОВУЮ SD

```bash
sudo dd if=clc-golden-*.img of=/dev/sdX bs=4M status=progress
sync
```

(Опционально: положить на `/boot` файл `clc-hostname.txt` с одной строкой имени —
`first-boot-clc.sh` подхватит без вопросов.)

### Шаг 5. Проверка на работающем замке

1. **Выключить CLC3**, вынуть рабочую SD, отложить в безопасное место.
2. Воткнуть **тестовую SD** с золотым образом.
3. Включить замок.
4. Подключиться к Castle WiFi (`192.168.4.1`) или Ethernet или HDMI+клавиатура.
5. Запустить first-boot:
   ```bash
   sudo bash /usr/local/sbin/first-boot-clc.sh
   # → задаёт hostname (например clc-test-russia)
   ```
6. Проверить что:
   - Castle WiFi работает (`http://192.168.4.1:3000` открывается)
   - Сервер квеста стартует
   - Если есть `Produced` WiFi в зоне — подключается
   - Tailscale **НЕ запущен** (это правильно)
7. Подключить Tailscale вручную (см. ниже).
8. Если всё ОК — выключить замок, вернуть **рабочую SD** обратно, золотой образ архивировать.

---

## Подключение Tailscale на новой Pi

```bash
sudo systemctl enable --now tailscaled
sudo tailscale up
```

В выводе появится URL — открыть в браузере, авторизовать в Tailscale-админке.
Tailscale возьмёт hostname машины (был задан в `first-boot-clc.sh`).

### 🔑 ОБЯЗАТЕЛЬНО: отключить Expiry

После того как машина появилась в Tailscale-админке:

1. Открыть https://login.tailscale.com/admin/machines
2. Найти новую машину (имя совпадает с hostname)
3. Меню `⋯` → **Disable key expiry**

**Зачем:** по умолчанию Tailscale-ключ истекает через ~6 месяцев. Когда это происходит, машина теряет свой Tailscale IP и пропадает из сети. У клиента это значит «тех. поддержка перестала работать на ровном месте». `Disable key expiry` снимает таймер — IP закреплён за нодой навсегда.

**Альтернатива через CLI** (нужны Tailscale admin scopes у API key):
```bash
tailscale set --auth-key=... --advertise-tags=tag:long-lived
# или через admin-API: PATCH /api/v2/device/{id}/key
```

Но через UI быстрее и нагляднее.

**Если в админке уже есть запись с таким именем** (от прошлого эксперимента) — удалить её **до** `tailscale up`, иначе Tailscale добавит суффикс `-1` к новому имени.

---

## Альтернативный workflow (обезличка ЖИВОГО эталона)

⚠️ Только если планируешь снять образ именно с этой Pi и потом её не использовать
(или вернуть в строй переустановкой). Этот путь убьёт identity текущей CLC3 в
Tailscale-сети.

```bash
# Подключиться через ЛОКАЛЬНУЮ SSH-сессию (192.168.4.1 на Castle WiFi),
# НЕ через Tailscale — он сейчас logout'нется.
ssh pi@192.168.4.1
sudo bash /usr/local/sbin/prepare-for-cloning-clc.sh
sudo poweroff   # критично — НЕ reboot
# вынуть SD, снять dd на другой машине
```

Это менее предпочтительно — теряем работающий CLC3.

---

## Подводные камни

- **Никогда не запускать `prepare-for-cloning-clc.sh` через Tailscale SSH** —
  `tailscale logout` оборвёт твою же сессию посреди скрипта. Скрипт это
  проверяет и просит подтверждения `YES`.
- **Между `prepare` и снятием dd НЕ делать `reboot`** на живой Pi — systemd регенерирует machine-id и SSH-keys обратно, образ перестанет быть «чистым».
- **`/boot/firmware/clc-hostname.txt`** на Bookworm (новый Raspberry Pi OS) — путь к `/boot` сменился на `/boot/firmware/`. Скрипты смотрят оба варианта.
- **dd на живой системе** — приемлемо если предварительно `systemctl stop castleserver && sync`, но в идеале выключить Pi полностью.
- **pishrink** не работает на macOS — нужен Linux/WSL с loop-устройствами.
- **Tailscale Expiry** — забыть отключить = через полгода квест у клиента «упадёт» из тех. поддержки. Это самая частая причина «потерь» нод в админке.

## Связано

- WC-эталон обезлички: `~/.claude/projects/-home-claude-dev-WC--Wizard-s-Chest-/memory/reference_image_clone_tailscale.md`
- CLC1 инцидент про cloned-identity: в `MEMORY.md` → "Клон образа + Tailscale identity"
- WiFi auto-connect: CLC использует wpa_supplicant (НЕ NetworkManager как WC) — конфиг переезжает в образ как файл, никаких patches не требуется.
