"""
Клиентская специфика замка — всё, что отличает один комплект CLC от другого.

Зачем это существует. Раньше отличия между замками жили прямо в коде: раскладка
USB-хаба башен была захардкожена в CastleServer.py под CLC3, а калибровка серво
мальчика требовала отдельной сборки прошивки Mega под Канаду. Из-за этого
«единый набор файлов» на самом деле единым не был: перед каждой отгрузкой
приходилось править исходники, а перенос фикса с одного замка на другой каждый
раз начинался с разбора «что здесь клиентское, а что общее».

Теперь код на всех замках одинаковый, а разница лежит здесь и настраивается
с тех-пульта (/tech → «Первый запуск»). Правило простое: константа описывает
ЖЕЛЕЗО конкретного комплекта → сюда; описывает логику игры → остаётся в коде.

Файл: castle_config.json рядом с CastleServer.py (в .gitignore — иначе git pull
на Pi затрёт настройки клиента).

ВАЖНО про дефолты. Значения по умолчанию совпадают с тем, что раньше было
захардкожено. Если файла нет — поведение сервера не меняется ни на байт. Но
usb_hub.confirmed остаётся False, и пульт показывает плашку «раскладка не
подтверждена»: дефолт верен для эталонной сборки, а на замке, собранном иначе,
он молча отправит прошивку не в ту башню. Ровно так и было на CLC3.
"""
import copy
import json
import logging
import os
import re
import shutil

logger = logging.getLogger(__name__)

# Резервная копия вне папки сервера: если папку на Pi перезальют целиком,
# настройки клиента переживут это и поднимутся из бэкапа.
BACKUP_PATH = "/home/pi/castle_config.json.bak"

# Эталонная раскладка USB-хаба. Все новые замки собираются так, и пульт
# проверяет физику по этой таблице (см. «Первый запуск» → «Раскладка USB-хаба»).
STANDARD_HUB_PORTS = {
    "owls":     "1.2.1",
    "dog":      "1.2.2",
    "workshop": "1.2.3",
    "basket":   "1.2.4",
}

TOWERS = ("owls", "dog", "workshop", "basket")

# Границы калибровки серво. Серво без механического ограничителя легко загнать
# в упор — он будет гудеть, греться и в итоге сгорит. Значения вне этих границ
# не принимаем ни из UI, ни из файла.
SERVO_LIMITS = {
    "pulse_min": (400, 1500),
    "pulse_max": (1500, 2600),
    "angle_open": (0, 180),
    "angle_hide": (0, 180),
    "move_ms":   (200, 5000),
}

# RFC 1123 + требование Tailscale MagicDNS (только строчные, без подчёркиваний).
# Тот же regex, что в golden-image/first-boot-clc.sh — держать синхронно.
HOSTNAME_RE = re.compile(r"^[a-z0-9][a-z0-9-]{1,62}[a-z0-9]$")

# Предпочитаемый формат имени. Не блокирует ввод, только предупреждает: имена
# castle-clc-2 / castle-clc-3 уже заведены в Tailscale и в реестре клиентов.
PREFERRED_NAME_RE = re.compile(r"^castle-clc-\d+$")

PORT_RE = re.compile(r"^[0-9]+(\.[0-9]+)*$")

COUNTRY_RE = re.compile(r"^[A-Z]{2}$")

CHECKLIST_KEY_RE = re.compile(r"^[a-z0-9_]{1,40}$")

DEFAULTS = {
    "schema_version": 1,
    # Имя квеста = hostname машины = имя ноды в Tailscale. Формат castle-clc-N.
    "quest_name": "",
    "wifi": {
        "client_iface": "wlan1",
        # Интерфейс точки доступа Castle. На большинстве замков это встроенный
        # wlan0, но на CLC1 (Оман) точка поднята на ap0 — и зашитый в коде
        # wlan0 там означал: индикатор Castle AP всегда красный, а вотчдог
        # hostapd видел «ноль клиентов» и перезапускал точку на живом замке,
        # сбрасывая с неё ESP32 и телефон оператора.
        "ap_iface": "wlan0",
        # Regdomain для wpa_supplicant.conf. Не путать с country_code в
        # hostapd.conf — сеть Castle мы не трогаем вообще.
        "country": "RU",
    },
    "usb_hub": {
        # Путь до порта собирается как base + номер + suffix. Вынесено тремя
        # кусками, чтобы при смене ревизии Pi (другой PCIe-путь) правился
        # конфиг, а не код.
        "base":   "/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:",
        "suffix": ":1.0-port0",
        "confirmed": False,
        "ports": dict(STANDARD_HUB_PORTS),
        # Узел самого хаба в /sys/bus/usb/devices — через него хаб
        # передёргивается программно перед прошивкой башен.
        "usb_device": "1-1.2",
        # Передёргивать автоматически. Выключается, если на каком-то замке это
        # окажется лишним.
        "replug_before_flash": True,
    },
    # Звук на срабатывание датчика из динамиков замка. Живёт ЗДЕСЬ, а не в
    # браузере: монтажник включает его, чтобы проверять датчики на слух, не
    # завися от связи с пультом. Если бы решение принимал пульт, обрыв вайфая
    # или закрытая вкладка молча выключали бы звук — ровно то, ради чего его и
    # выбирали вместо звука из браузера.
    "sensor_beep": {
        "castle": False,
        "volume": 40,       # проценты
    },
    # Как башня подключена к Pi во время прошивки.
    #   "hub"    — четыре кабеля постоянно сидят в USB-хабе, у каждой башни свой
    #              порт (так собраны CLC1, CLC2, CLC3);
    #   "direct" — в Pi воткнут ОДИН кабель от той башни, которую прошиваем.
    # Дефолт "hub": на замках, где конфига ещё нет, поведение не меняется.
    "flash_mode": "hub",
    # Серво мальчика на пине 49 главной Mega. Дефолты — то, что было зашито
    # в прошивке CLC3 (DS3218). У Канады другая партия: 544/2400/130/0/500.
    "boy_servo": {
        "pulse_min": 500,
        "pulse_max": 2500,
        "angle_open": 170,
        "angle_hide": 0,
        "move_ms": 1000,
    },
    "tailscale": {
        "authorized": False,
        # Ставится вручную галочкой на пульте. Отдельный флаг нужен, потому что
        # «Disable key expiry» делается в веб-админке Tailscale и никак не
        # виден с Pi, а забытый шаг через полгода роняет удалённый доступ.
        "key_expiry_disabled": False,
    },
    # Чек-лист приёмки: {ключ: bool}. Тексты живут в i18n Tech.html, сервер их
    # не знает — так связность минимальна.
    "checklist": {},
}


def _clamp(value, low, high):
    return max(low, min(high, value))


def _merge_defaults(data, defaults):
    """Рекурсивно дополнить прочитанный конфиг недостающими ключами.

    Нужно для совместимости вперёд и назад: старый файл без нового блока не
    должен ронять сервер, а лишние ключи из будущей версии — теряться при
    сохранении не должны только те, что мы знаем."""
    out = copy.deepcopy(defaults)
    if not isinstance(data, dict):
        return out
    for key, value in data.items():
        if isinstance(value, dict) and isinstance(out.get(key), dict):
            out[key] = _merge_defaults(value, out[key])
        else:
            out[key] = value
    return out


def validate_hostname(name):
    """(ok, warn) — годится ли имя и надо ли предупредить про формат."""
    name = (name or "").strip()
    if not HOSTNAME_RE.match(name):
        return False, ""
    if not PREFERRED_NAME_RE.match(name):
        return True, "not_preferred"
    return True, ""


class CastleConfig:
    def __init__(self, config_path):
        self.config_path = config_path
        self.data = copy.deepcopy(DEFAULTS)
        self.load()

    # ---------- persistence ----------
    def load(self):
        path = self.config_path
        if not os.path.exists(path):
            # Файла нет — пробуем поднять настройки из бэкапа. Это спасает,
            # когда папку сервера на Pi перезалили целиком.
            if os.path.exists(BACKUP_PATH):
                logger.warning(f"CONFIG: {path} отсутствует, восстанавливаю из {BACKUP_PATH}")
                path = BACKUP_PATH
            else:
                logger.info(f"CONFIG: {path} нет — создаю с дефолтами")
                self.save()
                return
        try:
            with open(path, "r") as f:
                raw = json.load(f)
            self.data = self._sanitize(_merge_defaults(raw, DEFAULTS))
            logger.info(
                f"CONFIG: загружен (quest={self.data['quest_name'] or '—'}, "
                f"hub_confirmed={self.data['usb_hub']['confirmed']}, "
                f"ports={self.data['usb_hub']['ports']})"
            )
            if path != self.config_path:
                self.save()
        except Exception as e:
            # Битый JSON не должен ронять сервер: квест важнее настроек.
            logger.warning(f"CONFIG: не читается {path} ({e}) — работаю на дефолтах")
            self.data = copy.deepcopy(DEFAULTS)

    def save(self):
        try:
            tmp = self.config_path + ".tmp"
            with open(tmp, "w") as f:
                json.dump(self.data, f, indent=2, ensure_ascii=False)
            os.replace(tmp, self.config_path)   # атомарно, без полупустого файла
        except Exception as e:
            logger.warning(f"CONFIG: не могу сохранить {self.config_path}: {e}")
            return
        try:
            shutil.copyfile(self.config_path, BACKUP_PATH)
        except Exception as e:
            logger.debug(f"CONFIG: бэкап в {BACKUP_PATH} не сделан: {e}")

    def _sanitize(self, data):
        """Привести прочитанное к безопасным значениям.

        Конфиг правится и руками, и с пульта. Ошибка здесь уходит прямо в
        аргумент avrdude или в команду серво, поэтому чистим на входе, а не
        надеемся на то, что записали правильно."""
        hub = data.setdefault("usb_hub", {})
        ports = hub.setdefault("ports", {})
        for tower in TOWERS:
            port = str(ports.get(tower, STANDARD_HUB_PORTS[tower]))
            if not PORT_RE.match(port):
                logger.warning(f"CONFIG: порт '{port}' для {tower} невалиден — беру эталонный")
                port = STANDARD_HUB_PORTS[tower]
            ports[tower] = port

        beep = data.setdefault("sensor_beep", {})
        beep["castle"] = bool(beep.get("castle", DEFAULTS["sensor_beep"]["castle"]))
        try:
            beep["volume"] = _clamp(int(beep.get("volume", DEFAULTS["sensor_beep"]["volume"])), 5, 100)
        except (TypeError, ValueError):
            beep["volume"] = DEFAULTS["sensor_beep"]["volume"]

        mode = str(data.get("flash_mode", DEFAULTS["flash_mode"]))
        if mode not in ("hub", "direct"):
            logger.warning(f"CONFIG: неизвестный режим прошивки '{mode}' — беру хаб")
            mode = DEFAULTS["flash_mode"]
        data["flash_mode"] = mode

        servo = data.setdefault("boy_servo", {})
        for key, (low, high) in SERVO_LIMITS.items():
            try:
                servo[key] = _clamp(int(servo.get(key, DEFAULTS["boy_servo"][key])), low, high)
            except (TypeError, ValueError):
                servo[key] = DEFAULTS["boy_servo"][key]
        if servo["pulse_min"] >= servo["pulse_max"]:
            logger.warning("CONFIG: pulse_min >= pulse_max — возвращаю дефолты серво")
            servo["pulse_min"] = DEFAULTS["boy_servo"]["pulse_min"]
            servo["pulse_max"] = DEFAULTS["boy_servo"]["pulse_max"]

        wifi = data.setdefault("wifi", {})
        country = str(wifi.get("country", "RU")).upper()
        wifi["country"] = country if COUNTRY_RE.match(country) else "RU"

        name = str(data.get("quest_name", "") or "").strip()
        data["quest_name"] = name if (not name or HOSTNAME_RE.match(name)) else ""

        checklist = data.setdefault("checklist", {})
        data["checklist"] = {
            k: bool(v) for k, v in checklist.items()
            if isinstance(k, str) and CHECKLIST_KEY_RE.match(k)
        }
        return data

    # ---------- имя квеста ----------
    def quest_name(self):
        return self.data.get("quest_name", "")

    def set_quest_name(self, name):
        ok, _warn = validate_hostname(name)
        if not ok:
            return False
        self.data["quest_name"] = name.strip()
        self.save()
        return True

    # ---------- раскладка USB-хаба ----------
    def tower_port(self, tower):
        """Номер порта хаба, например '1.2.3'."""
        return self.data["usb_hub"]["ports"].get(tower, STANDARD_HUB_PORTS.get(tower, ""))

    def tower_dev(self, tower):
        """Полный путь устройства для avrdude."""
        hub = self.data["usb_hub"]
        return f"{hub['base']}{self.tower_port(tower)}{hub['suffix']}"

    def sensor_beep(self):
        """{'castle': bool, 'volume': 5..100} — звук на срабатывание из замка."""
        return dict(self.data.get("sensor_beep", DEFAULTS["sensor_beep"]))

    def set_sensor_beep(self, castle=None, volume=None):
        beep = self.data.setdefault("sensor_beep", dict(DEFAULTS["sensor_beep"]))
        if castle is not None:
            beep["castle"] = bool(castle)
        if volume is not None:
            try:
                beep["volume"] = _clamp(int(volume), 5, 100)
            except (TypeError, ValueError):
                pass
        self.save()
        return dict(beep)

    def flash_mode(self):
        """'hub' — башни постоянно в хабе, 'direct' — один кабель в Pi."""
        return self.data.get("flash_mode", DEFAULTS["flash_mode"])

    def set_flash_mode(self, mode):
        if mode not in ("hub", "direct"):
            return False
        self.data["flash_mode"] = mode
        self.save()
        return True

    def hub_ports(self):
        return dict(self.data["usb_hub"]["ports"])

    def hub_confirmed(self):
        return bool(self.data["usb_hub"].get("confirmed", False))

    def hub_usb_device(self):
        """Узел хаба в /sys/bus/usb/devices — например '1-1.2'."""
        node = str(self.data["usb_hub"].get("usb_device", "1-1.2"))
        return node if re.fullmatch(r"[0-9]+-[0-9.]+", node) else "1-1.2"

    def hub_replug_enabled(self):
        return bool(self.data["usb_hub"].get("replug_before_flash", True))

    def set_hub_replug(self, enabled):
        self.data["usb_hub"]["replug_before_flash"] = bool(enabled)
        self.save()

    def hub_is_standard(self):
        return self.hub_ports() == STANDARD_HUB_PORTS

    def set_tower_port(self, tower, port):
        """Назначить башне порт. Если порт занят другой башней — меняем их местами:
        физически кабели именно так и переставляют, а два разных имени на одном
        порту означали бы, что одна из башен станет непрошиваемой."""
        if tower not in TOWERS or not PORT_RE.match(str(port)):
            return False
        port = str(port)
        ports = self.data["usb_hub"]["ports"]
        for other, other_port in ports.items():
            if other != tower and other_port == port:
                ports[other] = ports[tower]
                break
        ports[tower] = port
        self.data["usb_hub"]["confirmed"] = False   # раскладка изменилась — подтвердить заново
        self.save()
        return True

    def set_hub_confirmed(self, confirmed):
        self.data["usb_hub"]["confirmed"] = bool(confirmed)
        self.save()

    def reset_hub_to_standard(self):
        self.data["usb_hub"]["ports"] = dict(STANDARD_HUB_PORTS)
        self.data["usb_hub"]["confirmed"] = False
        self.save()

    # ---------- WiFi ----------
    def wifi_iface(self):
        return self.data["wifi"].get("client_iface", "wlan1")

    def ap_iface(self):
        """Интерфейс точки доступа Castle: wlan0 везде, ap0 на CLC1."""
        return self.data["wifi"].get("ap_iface", "wlan0")

    def wifi_country(self):
        return self.data["wifi"].get("country", "RU")

    def set_wifi_country(self, code):
        code = str(code or "").upper().strip()
        if not COUNTRY_RE.match(code):
            return False
        self.data["wifi"]["country"] = code
        self.save()
        return True

    # ---------- калибровка серво ----------
    def boy_servo(self):
        return dict(self.data["boy_servo"])

    def set_boy_servo(self, values):
        """Принять калибровку из UI. Возвращает (ok, вычищенные значения)."""
        servo = dict(self.data["boy_servo"])
        for key, (low, high) in SERVO_LIMITS.items():
            if key not in values:
                continue
            try:
                servo[key] = _clamp(int(values[key]), low, high)
            except (TypeError, ValueError):
                return False, self.boy_servo()
        if servo["pulse_min"] >= servo["pulse_max"]:
            return False, self.boy_servo()
        self.data["boy_servo"] = servo
        self.save()
        return True, dict(servo)

    def boy_servo_command(self, values=None):
        """Строка команды для Mega: boycal:<min>:<max>:<open>:<hide>:<ms>.

        values позволяет проиграть ещё не сохранённую калибровку — так работает
        кнопка «проверить» на пульте: оператор крутит числа, сразу видит ход
        серво и только потом жмёт «Сохранить»."""
        s = dict(self.data["boy_servo"])
        if values:
            for key, (low, high) in SERVO_LIMITS.items():
                if key in values:
                    try:
                        s[key] = _clamp(int(values[key]), low, high)
                    except (TypeError, ValueError):
                        pass
        return (f"boycal:{s['pulse_min']}:{s['pulse_max']}:"
                f"{s['angle_open']}:{s['angle_hide']}:{s['move_ms']}")

    # ---------- Tailscale ----------
    def tailscale_flags(self):
        return dict(self.data["tailscale"])

    def set_tailscale_flag(self, key, value):
        if key not in ("authorized", "key_expiry_disabled"):
            return False
        self.data["tailscale"][key] = bool(value)
        self.save()
        return True

    # ---------- чек-лист приёмки ----------
    def checklist(self):
        return dict(self.data["checklist"])

    def set_checklist(self, key, checked):
        if not CHECKLIST_KEY_RE.match(str(key or "")):
            return False
        if checked:
            self.data["checklist"][key] = True
        else:
            self.data["checklist"].pop(key, None)
        self.save()
        return True

    def reset_checklist(self):
        self.data["checklist"] = {}
        self.save()

    # ---------- для /tech ----------
    def as_status(self):
        return {
            "quest_name": self.quest_name(),
            "wifi": {"iface": self.wifi_iface(), "country": self.wifi_country()},
            "usb_hub": {
                "ports": self.hub_ports(),
                "standard": dict(STANDARD_HUB_PORTS),
                "confirmed": self.hub_confirmed(),
                "is_standard": self.hub_is_standard(),
            },
            "flash_mode": self.flash_mode(),
            "sensor_beep": self.sensor_beep(),
            "boy_servo": self.boy_servo(),
            "servo_limits": {k: list(v) for k, v in SERVO_LIMITS.items()},
            "tailscale": self.tailscale_flags(),
            "checklist": self.checklist(),
        }
