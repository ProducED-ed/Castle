"""
Первичная настройка замка: то, что раньше делалось руками через SSH.

Здесь живёт вся системная работа для раздела «Первый запуск» на /tech — имя
квеста, скан и подключение WiFi, Tailscale, перечисление и опознание портов
USB-хаба. CastleServer.py вызывает эти функции из тонких socket-хендлеров.

ГЛАВНОЕ ПРАВИЛО ЭТОГО ФАЙЛА: сервер работает на eventlet, и любой блокирующий
вызов замораживает весь процесс — вместе с игрой, звуком и связью с Mega. Один
`iw scan`, зависший на 20 секунд, оборвал бы историю посреди квеста. Поэтому
здесь НЕТ ни одного subprocess.run / check_output: всё идёт через run_cmd(),
который читает пайпы неблокирующе и отдаёт управление хабу.

Второе правило: никаких shell=True. SSID, пароль и имя квеста приходят от
пользователя и уходят в системные файлы и аргументы команд.
"""
import json
import logging
import os
import re
import subprocess
import time

import eventlet

logger = logging.getLogger(__name__)

HOSTS_PATH = "/etc/hosts"
HOSTS_BACKUP = "/etc/hosts.clc-bak"
SERIAL_BY_PATH = "/dev/serial/by-path"

# Из пути /dev/serial/by-path/...-usb-0:1.2.3:1.0-port0 достаём номер порта хаба.
SERIAL_PORT_RE = re.compile(r"usb-0:(?P<port>[\d.]+):1\.0-port0$")

# Баннер, который башни печатают в USB-Serial при старте (см. setup() в
# owls/workshop/basket3). Нужен, чтобы пульт мог сказать «воткнуто правильно».
TOWER_BANNER_RE = re.compile(r"CLC-TOWER:([a-z0-9_]+)")

TAILSCALE_URL_RE = re.compile(r"https://login\.tailscale\.com/\S+")


# ==========================================================================
#  Запуск команд без блокировки eventlet
# ==========================================================================

def run_cmd(argv, timeout=15, input_text=None):
    """Выполнить команду. Возвращает (rc, stdout, stderr).

    Popen — это доли миллисекунды на fork/exec, всё остальное время мы спим
    через eventlet.sleep и читаем пайпы неблокирующе. Хаб продолжает крутиться,
    поэтому игра и звук не замечают даже долгих команд.

    rc = -1 означает «не удалось запустить», rc = -9 — «убито по таймауту»."""
    try:
        proc = subprocess.Popen(
            argv,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            stdin=subprocess.PIPE if input_text is not None else None,
        )
    except Exception as e:
        logger.warning(f"SETUP: не запускается {argv[0]}: {e}")
        return -1, "", str(e)

    if input_text is not None:
        try:
            proc.stdin.write(input_text.encode())
            proc.stdin.close()
        except Exception:
            pass

    # Читаем в отдельных гринтредах, а таймаут выносим в сторожевой таймер.
    #
    # Так сделано не от красоты. Под eventlet пайпы процесса — зелёные файлы, и
    # чтение из них паркуется до появления данных или EOF, игнорируя O_NONBLOCK:
    # цикл «почитал — проверил дедлайн — поспал» до проверки дедлайна просто не
    # доходит, и команда, зависшая на полчаса, столько и висит. Для хаба это
    # безопасно (парковка гринтреда его не блокирует), но нам нужен именно
    # таймаут — иначе один залипший iw scan навсегда занял бы кнопку на пульте.
    chunks = {"out": [], "err": []}

    def reader(stream, key):
        while True:
            try:
                data = stream.read(65536)
            except Exception:
                break
            if not data:
                break                   # EOF
            chunks[key].append(data)

    g_out = eventlet.spawn(reader, proc.stdout, "out")
    g_err = eventlet.spawn(reader, proc.stderr, "err")

    timed_out = [False]

    def killer():
        timed_out[0] = True
        logger.warning(f"SETUP: таймаут {timeout}с на {' '.join(argv[:3])}")
        try:
            proc.kill()
        except Exception:
            pass

    timer = eventlet.spawn_after(timeout, killer)
    try:
        proc.wait()
    except Exception:
        pass
    finally:
        timer.cancel()

    # Дочитываем хвосты, но не ждём вечно: убитый процесс мог оставить потомка,
    # который держит пайп открытым.
    for greenthread in (g_out, g_err):
        try:
            eventlet.with_timeout(3, greenthread.wait, timeout_value=None)
        except Exception:
            pass
        greenthread.kill()

    out, err = _decode(chunks["out"]), _decode(chunks["err"])
    rc = proc.returncode if proc.returncode is not None else -1
    _close(proc)
    if timed_out[0]:
        return -9, out, "timeout"
    return rc, out, err


def _decode(chunk_list):
    return b"".join(chunk_list).decode("utf-8", errors="replace")


def _close(proc):
    for stream in (proc.stdout, proc.stderr, proc.stdin):
        try:
            if stream:
                stream.close()
        except Exception:
            pass


# ==========================================================================
#  Имя квеста (hostname)
# ==========================================================================

def current_hostname():
    """Имя машины. Читаем файл, а не вызываем команду: статус опрашивается
    каждые несколько секунд, и плодить процессы ради одной строки незачем."""
    try:
        with open("/etc/hostname", "r") as f:
            return f.read().strip()
    except Exception:
        return ""


def rewrite_hosts(text, name):
    """Подставить новое имя в строку 127.0.1.1 файла /etc/hosts.

    Чистая функция — именно она проверяется тестами. Остальные строки файла
    (localhost, IPv6, комментарии клиента) не трогаем: /etc/hosts на боевой Pi
    иногда правят руками, и затирать это нельзя."""
    lines = text.splitlines()
    out = []
    found = False
    for line in lines:
        stripped = line.strip()
        if stripped and not stripped.startswith("#") and stripped.split()[0] == "127.0.1.1":
            out.append(f"127.0.1.1\t{name}")
            found = True
        else:
            out.append(line)
    if not found:
        out.append(f"127.0.1.1\t{name}")
    return "\n".join(out) + "\n"


def set_hostname(name):
    """Применить имя квеста. Возвращает (ok, сообщение)."""
    rc, _out, err = run_cmd(["sudo", "hostnamectl", "set-hostname", name], timeout=20)
    if rc != 0:
        return False, f"hostnamectl: {err.strip() or rc}"

    try:
        with open(HOSTS_PATH, "r") as f:
            original = f.read()
    except Exception as e:
        return False, f"не читается {HOSTS_PATH}: {e}"

    tmp = "/tmp/clc_hosts_new"
    try:
        with open(tmp, "w") as f:
            f.write(rewrite_hosts(original, name))
    except Exception as e:
        return False, f"не пишется временный файл: {e}"

    # Бэкап один раз — чтобы было куда откатиться, если строка окажется битой.
    if not os.path.exists(HOSTS_BACKUP):
        run_cmd(["sudo", "cp", HOSTS_PATH, HOSTS_BACKUP], timeout=10)

    # ИМЕННО cp, а не mv. mv из /tmp принесёт владельца pi:pi и права временного
    # файла. Сломанный /etc/hosts означает, что система перестаёт резолвить
    # собственное имя: каждый sudo начинает висеть на DNS-таймауте, а с ним
    # встают прошивка и systemctl. cp пишет в существующий inode и сохраняет
    # root:root 0644.
    rc, _out, err = run_cmd(["sudo", "cp", tmp, HOSTS_PATH], timeout=10)
    if rc != 0:
        return False, f"не применяется {HOSTS_PATH}: {err.strip() or rc}"

    rc, _out, _err = run_cmd(["getent", "hosts", name], timeout=10)
    if rc != 0:
        logger.warning(f"SETUP: hostname {name} задан, но getent его не резолвит")
        return True, "resolve_warning"
    return True, ""


def tailscale_set_hostname(name):
    """Переименовать ноду в Tailscale. Само оно за hostname машины не следует —
    без этого поддержка продолжит видеть в админке старое имя."""
    rc, _out, err = run_cmd(["sudo", "tailscale", "set", f"--hostname={name}"], timeout=30)
    return rc == 0, (err.strip() if rc != 0 else "")


# ==========================================================================
#  WiFi: скан доступных сетей
# ==========================================================================

def _iface_is_up(iface):
    try:
        with open(f"/sys/class/net/{iface}/operstate", "r") as f:
            return f.read().strip() in ("up", "unknown", "dormant")
    except Exception:
        return False


def iface_exists(iface):
    return os.path.exists(f"/sys/class/net/{iface}")


def _decode_ssid(raw):
    """SSID из wpa_cli приходит с escape-последовательностями вида \\xd0\\xbf.
    Для латиницы это неважно, а вот русские и арабские имена сетей без
    декодирования превращаются в кашу — и оператор не находит нужную сеть."""
    if "\\x" not in raw:
        return raw
    try:
        return raw.encode("latin-1", "backslashreplace").decode("unicode_escape") \
                  .encode("latin-1").decode("utf-8", errors="replace")
    except Exception:
        return raw


def _classify(flags):
    """Тип защиты сети по строке флагов.

    Разбираем на токены, потому что два источника пишут по-разному: wpa_cli
    даёт `[WPA2-PSK+SAE-CCMP][ESS]`, а iw — отдельной строкой `Authentication
    suites: PSK`. Поиск подстроки «-PSK» первый формат ловил, а второй нет, и
    защищённая сеть из iw-скана показывалась как открытая."""
    tokens = set(re.split(r"[^A-Z0-9]+", (flags or "").upper()))
    has_sae = "SAE" in tokens
    has_psk = "PSK" in tokens
    if has_sae and has_psk:
        return "wpa2/3", False          # смешанный режим — WPA2-клиент подключится
    if has_sae:
        return "wpa3", True             # только WPA3 — донгл её не увидит
    if has_psk:
        return "wpa2", False
    if "WEP" in tokens:
        return "wep", False
    return "open", False


def _quality(dbm):
    """dBm → 0..100 для полосок сигнала в списке."""
    return max(0, min(100, int(round((dbm + 90) * 100 / 60))))


def parse_wpa_cli_scan(text):
    """Разобрать вывод `wpa_cli scan_results`.

    Колонки: bssid / frequency / signal level / flags / ssid. SSID берём как
    весь остаток строки — в имени сети теоретически может быть таб."""
    nets = []
    for line in text.splitlines():
        if not line.strip() or line.startswith("bssid") or line.startswith("Selected"):
            continue
        parts = line.split("\t")
        if len(parts) < 5:
            continue
        try:
            freq = int(parts[1])
            signal = int(float(parts[2]))
        except ValueError:
            continue
        flags = parts[3]
        ssid = _decode_ssid("\t".join(parts[4:])).strip()
        if not ssid or ssid.strip("\x00 ") == "":
            continue                    # скрытая сеть — для неё есть ручной ввод
        security, wpa3_only = _classify(flags)
        nets.append({
            "ssid": ssid, "signal": signal, "quality": _quality(signal),
            "freq": freq, "band": "5" if freq >= 5000 else "2.4",
            "security": security, "wpa3_only": wpa3_only, "open": security == "open",
        })
    return nets


def parse_iw_scan(text):
    """Разобрать вывод `iw dev <iface> scan`."""
    nets = []
    cur = None

    def flush():
        if cur and cur.get("ssid"):
            security, wpa3_only = _classify(" ".join(cur["suites"]))
            freq = cur.get("freq", 0)
            nets.append({
                "ssid": cur["ssid"], "signal": cur.get("signal", -90),
                "quality": _quality(cur.get("signal", -90)),
                "freq": freq, "band": "5" if freq >= 5000 else "2.4",
                "security": security, "wpa3_only": wpa3_only, "open": security == "open",
            })

    for line in text.splitlines():
        s = line.strip()
        if s.startswith("BSS "):
            flush()
            cur = {"ssid": "", "suites": [], "signal": -90, "freq": 0}
            continue
        if cur is None:
            continue
        if s.startswith("SSID:"):
            cur["ssid"] = s[5:].strip()
        elif s.startswith("signal:"):
            try:
                cur["signal"] = int(float(s.split()[1]))
            except (ValueError, IndexError):
                pass
        elif s.startswith("freq:"):
            try:
                cur["freq"] = int(float(s.split()[1]))
            except (ValueError, IndexError):
                pass
        elif s.startswith("RSN:") or s.startswith("WPA:"):
            cur["suites"].append("RSN" if s.startswith("RSN:") else "WPA")
        elif "Authentication suites:" in s:
            cur["suites"].append(s.split("Authentication suites:")[1].strip())
        elif s.startswith("WEP"):
            cur["suites"].append("WEP")
    flush()
    return nets


def merge_networks(nets, limit=40):
    """Схлопнуть дубли и отсортировать по сигналу.

    Одна сеть почти всегда видна как несколько BSSID (репитеры, mesh, два
    диапазона). Оператору нужен один пункт списка — берём самый сильный."""
    best = {}
    for net in nets:
        prev = best.get(net["ssid"])
        if prev is None or net["signal"] > prev["signal"]:
            best[net["ssid"]] = net
    out = sorted(best.values(), key=lambda n: n["signal"], reverse=True)
    return out[:limit]


def scan_networks(iface):
    """Просканировать эфир. Возвращает (сети, метод, ошибка).

    Метод возвращаем наружу и показываем в UI: при удалённой диагностике знать,
    каким путём прошёл скан, экономит час гадания."""
    if not iface_exists(iface):
        return [], "", "no_iface"

    if not _iface_is_up(iface):
        # Поднимаем мягко. text_to_wlan1_reset() здесь звать НЕЛЬЗЯ: он делает
        # link down + up и оборвал бы уже установленное соединение, а вместе с
        # ним свет Hue и умную розетку — прямо во время игры.
        run_cmd(["sudo", "rfkill", "unblock", "wifi"], timeout=10)
        run_cmd(["sudo", "ip", "link", "set", iface, "up"], timeout=10)
        eventlet.sleep(2)

    # 1. wpa_cli — не рвёт текущую ассоциацию, поэтому основной путь.
    for prefix, method in ((["wpa_cli"], "wpa_cli"), (["sudo", "wpa_cli"], "sudo wpa_cli")):
        rc, out, _err = run_cmd(prefix + ["-i", iface, "scan"], timeout=10)
        if rc != 0 or "FAIL" in out:
            continue
        eventlet.sleep(3)
        rc, out, _err = run_cmd(prefix + ["-i", iface, "scan_results"], timeout=10)
        if rc == 0 and out.strip():
            nets = merge_networks(parse_wpa_cli_scan(out))
            if nets:
                return nets, method, ""

    # 2. iw — запасной путь для свежего замка, где wpa_supplicant ещё не поднят.
    for attempt in range(2):
        rc, out, err = run_cmd(["sudo", "iw", "dev", iface, "scan"], timeout=25)
        if rc == 0 and out.strip():
            return merge_networks(parse_iw_scan(out)), "iw", ""
        if "busy" in (err or "").lower() and attempt == 0:
            eventlet.sleep(3)
            continue
        break

    return [], "", "scan_failed"


def wpa_escape(value, max_bytes=None):
    """Экранировать строку для wpa_supplicant.conf.

    SSID и пароль подставляются в конфиг как есть, и SSID с кавычкой и переводом
    строки позволил бы дописать в файл произвольные директивы. Управляющие
    символы выбрасываем, кавычку и слэш экранируем, длину ограничиваем
    стандартом (SSID до 32 байт, PSK до 63)."""
    value = "".join(ch for ch in (value or "") if ch.isprintable())
    if max_bytes:
        value = value.encode("utf-8")[:max_bytes].decode("utf-8", errors="ignore")
    return value.replace("\\", "\\\\").replace('"', '\\"')


def build_wpa_config(ssid, password, country, security="wpa2"):
    """Собрать wpa_supplicant.conf под конкретную сеть.

    Раньше здесь всегда стоял key_mgmt=WPA-PSK. Для открытой сети это давало
    невалидный psk="" — подключение молча не происходило, и выглядело как
    «донгл не видит сеть»."""
    head = (
        "ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\n"
        "update_config=1\n"
        f"country={country}\n\n"
    )
    ssid_q = wpa_escape(ssid, max_bytes=32)

    if security == "open":
        body = (
            "network={\n"
            f'    ssid="{ssid_q}"\n'
            "    key_mgmt=NONE\n"
            "    priority=1\n"
            "    scan_ssid=1\n"
            "}\n"
        )
    else:
        # Для смешанных WPA2/WPA3-сетей перечисляем оба метода: если донгл
        # умеет только WPA2, он выберет PSK и подключится.
        key_mgmt = "WPA-PSK SAE" if security in ("wpa3", "wpa2/3") else "WPA-PSK"
        extra = "    ieee80211w=1\n" if security in ("wpa3", "wpa2/3") else ""
        body = (
            "network={\n"
            f'    ssid="{ssid_q}"\n'
            f'    psk="{wpa_escape(password, max_bytes=63)}"\n'
            f"    key_mgmt={key_mgmt}\n"
            f"{extra}"
            "    priority=1\n"
            "    scan_ssid=1\n"
            "}\n"
        )
    return head + body


# ==========================================================================
#  Tailscale
# ==========================================================================

def tailscale_status():
    """Состояние демона: (BackendState, AuthURL, IP). Пустой стейт — демон не
    поднят или ещё стартует."""
    rc, out, _err = run_cmd(["tailscale", "status", "--json"], timeout=15)
    if rc != 0 or not out.strip():
        return "", "", ""
    try:
        data = json.loads(out)
    except ValueError:
        return "", "", ""
    state = data.get("BackendState", "")
    auth_url = data.get("AuthURL", "") or ""
    ip = ""
    self_node = data.get("Self") or {}
    ips = self_node.get("TailscaleIPs") or []
    for candidate in ips:
        if candidate.startswith("100."):
            ip = candidate
            break
    return state, auth_url, ip


def find_auth_url(text):
    match = TAILSCALE_URL_RE.search(text or "")
    return match.group(0) if match else ""


# ==========================================================================
#  Порты USB-хаба и опознание башен
# ==========================================================================

def list_serial_ports(hub_prefix="1.2.", exclude_tty=None):
    """Что воткнуто в USB-хаб башен.

    Только чтение файловой системы: ни одного os.open() на tty. Открытие
    последовательного порта дёргает DTR и перезагружает плату — фоновый опрос
    USB когда-то ресетил башни каждые несколько секунд, и они теряли игровое
    состояние. Опознание (которое порт всё-таки открывает) вынесено отдельно и
    делается только по явной команде оператора.

    ГЛАВНАЯ ПЛАТА СЮДА ПОПАДАТЬ НЕ ДОЛЖНА. Она сидит не в хабе, а прямо на порту
    Pi (1.1), и её путь тоже оканчивается на ':1.0-port0'. В первой версии
    фильтр этого не учитывал: «Опознать башни» открывало порт работающей Mega,
    роняло DTR, перезагружало главную плату и потом гоняло по ней avrdude.
    Поэтому здесь два независимых отсева — по префиксу хаба и по фактическому
    tty главной платы."""
    ports = []
    try:
        entries = sorted(os.listdir(SERIAL_BY_PATH))
    except Exception:
        return ports

    main_tty = ""
    if exclude_tty:
        try:
            main_tty = os.path.realpath(exclude_tty)
        except Exception:
            main_tty = ""

    for entry in entries:
        match = SERIAL_PORT_RE.search(entry)
        if not match:
            continue
        port = match.group("port")
        if hub_prefix and not port.startswith(hub_prefix):
            continue                      # не хаб — скорее всего главная плата
        full = os.path.join(SERIAL_BY_PATH, entry)
        try:
            tty = os.path.realpath(full)
        except Exception:
            tty = ""
        if main_tty and tty == main_tty:
            continue                      # это главная плата, трогать нельзя
        ports.append({
            "port": port,
            "path": full,
            "tty": tty,
            "vidpid": _tty_vidpid(tty),
        })
    return ports


def _tty_vidpid(tty):
    """VID:PID устройства за /dev/ttyUSBn — читаем из sysfs, порт не открываем."""
    if not tty:
        return ""
    name = os.path.basename(tty)
    base = f"/sys/class/tty/{name}/device/../.."
    try:
        with open(os.path.join(base, "idVendor")) as f:
            vid = f.read().strip()
        with open(os.path.join(base, "idProduct")) as f:
            pid = f.read().strip()
        return f"{vid}:{pid}"
    except Exception:
        return ""


def identify_tower(tty, timeout=6.0):
    """Опознать башню по баннеру CLC-TOWER в USB-Serial.

    Открытие порта роняет DTR, башня перезагружается и печатает баннер в
    setup() — именно этого мы и ждём. Работает для Owls/Workshop/Basket: у них
    связь с главной платой идёт по Serial1, а USB-Serial свободен. Dog так
    опознать нельзя — у него единственный UART делится с линией к Mega, поэтому
    для него есть identify_dog_by_signature()."""
    try:
        import serial
    except ImportError:
        return ""
    try:
        with serial.Serial(tty, 9600, timeout=0.3, dsrdtr=False, rtscts=False) as ser:
            deadline = time.monotonic() + timeout
            buf = ""
            while time.monotonic() < deadline:
                try:
                    chunk = ser.read(256).decode("utf-8", errors="replace")
                except Exception:
                    break
                if chunk:
                    buf += chunk
                    match = TOWER_BANNER_RE.search(buf)
                    if match:
                        return match.group(1)
                    buf = buf[-512:]
                eventlet.sleep(0.1)
    except Exception as e:
        logger.debug(f"SETUP: не опознать {tty}: {e}")
    return ""


# Пробу Dog через avrdude (чтение сигнатуры atmega328p) здесь СОЗНАТЕЛЬНО нет.
# Она стоила до 25 секунд на порт, из-за чего опознание выглядело зависшим, а
# главное — avrdude ресетит плату, к которой обращается, и на Dog это лезет в
# UART, разделённый с главной Mega (тот самый, из-за которого прошивка Dog идёт
# через три шага с silence.ino). Dog определяется вычитанием: три остальные
# башни называют себя баннером сами, значит оставшийся порт — его.
