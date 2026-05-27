import eventlet
import subprocess
eventlet.monkey_patch()

# Словарь для расшифровки событий в логах
EVENT_DESCRIPTIONS = {
    # --- Сообщения от MAIN_BOARD ---
    "startgo": "Квест начался (все двери закрыты)",
    "open_door": "Открыта стартовая дверь",
    "close_door": "Закрыта стартовая дверь",
    "dragon_crystal": "Кристалл дракона активирован",
    "dragon_crystal_repeat": "Повторная подсказка про кристалл дракона",
    "clock1": "Активировано 12 часов (часы)",
    "clock2": "Активированы 42 минуты (часы)",
    "steps": "Шаги (прячется студент)",
    "kay_repeat": "Повторная подсказка мальчика",
    "galet_on": "Игра с 5 галетниками пройдена",
    "galet_off": "Галетник выключен (в режиме ожидания)",
    "flagsendmr": "Игра с флагами пройдена",
    "door_owl": "Дверь к совам открыта",
    "owl_flew": "Сова вылетела",
    "owl_end": "Игра с совами пройдена",
    "door_witch": "Дверь к ведьме открыта (кошка)",
    "first_bottle": "Правильно поставлена 1-я бутылка",
    "second_bottle": "Правильно поставлена 2-я бутылка",
    "third_bottle": "Правильно поставлена 3-я бутылка",
    "four_bottle": "Игра с бутылками пройдена",
    "mistake_bottle": "Ошибка в игре с бутылками",
    "door_dog": "Дверь к собаке открыта (замок)",
    "dog_sleep": "Собака уснула",
    "dog_growl": "Собака рычит",
    "dog_lock": "Игра с собакой пройдена",
    "door_cave": "Дверь в пещеру открыта",
    "cave_search1": "В пещере найден 1-й предмет",
    "cave_search2": "В пещере найден 2-й предмет",
    "cave_search3": "В пещере найден 3-й предмет",
    "cave_end": "Игра с троллем в пещере пройдена",
    "miror": "Открыта дверь в банк (подули в окно)",
    "safe_turn": "Поворот ручки сейфа",
    "safe_end": "Сейф в банке открыт",
    "material_end": "Все материалы для крафта у мастера собраны",
    "fire1": "Зажжен первый огонь в печке",
    "fire2": "Зажжен второй огонь в печке",
    "fire3": "Зажжен третий огонь в печке",
    "fire0": "Огонь в печке потушен",
    "item_find": "Найден предмет для крафта",
    "item_add": "Предмет для крафта добавлен",
    "broom": "Создана метла",
    "helmet": "Создан шлем",
    "story_35": "Мастерская пройдена",
    "ghost_knock": "Воспроизведен звук стука (призрак)",
    "h_clock": "Активирован первый тумблер (призрак)",
    "uf_clock": "Активирован второй галетник (призрак)",
    "punch": "Игра с призраком пройдена (удар)",
    "star_hint": "Запрошена подсказка по звездам",
    "fire": "Камин горит",
    "lib_door": "Дверь в библиотеку открыта",
    "door_basket": "Дверь на балкон (баскетбол) открыта",
    "door_spell": "Дверь в комнату заклинаний открыта",
    "cristal_up": "Кристаллы подняты",
    "start_crystal": "Начата игра с кристаллами",
    "true_crystal": "Правильный кристалл убран",
    "mistake_crystal": "Ошибка в игре с кристаллами",
    "first_level": "Пройден 1-й уровень воспоминаний",
    "second_level": "Пройден 2-й уровень воспоминаний",
    "third_level": "Пройден 3-й уровень воспоминаний",
    "memory_room_end": "Комната воспоминаний пройдена",
    "crime_end": "Комната преступления пройдена",
    "boy_in": "Мальчик вставлен (начало баскетбола)",
    "lesson_goal": "Забит гол в тренировке",
    "flying_ball": "Мяч в полете",
    "catch1": "Мяч пойман (датчик 1)",
    "catch2": "Мяч пойман (датчик 2)",
    "catch3": "Мяч пойман (датчик 3)",
    "catch4": "Мяч пойман (датчик 4)",
    "goal_1_player": "Гол игрока в 1-е кольцо",
    "goal_2_player": "Гол игрока во 2-е кольцо",
    "goal_3_player": "Гол игрока в 3-е кольцо",
    "goal_4_player": "Гол игрока в 4-е кольцо",
    "goal_1_bot": "Гол бота в 1-е кольцо",
    "goal_2_bot": "Гол бота во 2-е кольцо",
    "goal_3_bot": "Гол бота в 3-е кольцо",
    "goal_4_bot": "Гол бота в 4-е кольцо",
    "start_snitch": "Золотой мяч активирован",
    "red_ball": "Красный мяч активирован",
    "enemy_catch1": "Бот поймал красный мяч (датчик 1)",
    "enemy_catch2": "Бот поймал красный мяч (датчик 2)",
    "enemy_catch3": "Бот поймал красный мяч (датчик 3)",
    "enemy_catch4": "Бот поймал красный мяч (датчик 4)",
    "win": "Игрок победил в баскетболе",
    "win_robot": "Бот победил в баскетболе",
    "last_on": "Финальная статуэтка поставлена на место",
    "main_board_ready": "Arduino Mega (MAIN_BOARD) готова к работе.",
    "boy_in": "Мальчик установлен в клетку (режим ожидания)",
    "boy_out": "Мальчик извлечен из клетки (режим ожидания)",

    # --- Сообщения от basket3.ino (Баскетбол/Пещера) ---
    "basket3_ready": "Башня 'Баскетбол' готова.",
    "level_1_b": "Баскетбол: Начальный уровень.",
    "level_2_b": "Баскетбол: Идет тренировка.",
    "level_3_b": "Баскетбол: Основная игра.",
    "level_4_b": "Баскетбол: Игра окончена (победа).",
    "level_5_b": "Баскетбол: Игра окончена (проигрыш).",
    "level_6_b": "Пещера: Начальный уровень.",
    "level_7_b": "Пещера: Идет поиск предметов.",
    "level_8_b": "Пещера: Уровень пройден.",

    # --- Сообщения от workshop.ino (Мастерская) ---
    "workshop_ready": "Башня 'Мастерская' готова.",
    "level_1_w": "Мастерская: Начальный уровень.",
    "level_2_w": "Мастерская: Ожидание активации печи.",
    "level_3_w": "Мастерская: Идет сбор предметов.",
    "level_4_w": "Мастерская: Уровень пройден.",

    # --- Сообщения от owls.ino (Совы) ---
    "owls_ready": "Башня 'Совы' готова.",
    "level_1_o": "Совы: Начальный уровень.",
    "level_2_o": "Совы: Идет игра.",
    "level_3_o": "Совы: Уровень пройден.",

    # --- Сообщения от dog.ino (Собака) ---
    "dog_ready": "Башня 'Собака' готова.",
    "level_1_d": "Собака: Начальный уровень.",
    "level_2_d": "Собака: Идет игра.",
    "level_3_d": "Собака: Уровень пройден.",

    # --- Сообщения от train.ino (ESP32 Карта) ---
    "train_ready": "ESP32 'Карта' готова.",
    "projector: end": "ESP32 Карта: Игра с проектором пройдена.",
    "train: end": "ESP32 Карта: Игра с поездом пройдена.",
    "ghost: end": "ESP32 Карта: Игра с призраком пройдена (получен сигнал).",

    # --- Сообщения от wolf.ino (ESP32 Волк) ---
    "wolf_ready": "ESP32 'Волк' готов.",
    "wolf: end": "ESP32 Волк: Игра пройдена.",

    # --- Сообщения от chest.ino (ESP32 Чемоданы) ---
    "suitcase_ready": "ESP32 'Чемоданы' готов.",
    "suitcase: end": "ESP32 Чемоданы: Игра пройдена.",

    # --- Сообщения от safe.ino (ESP32 Сейф) ---
    "safe_ready": "ESP32 'Сейф' готов.",
    "safe: end": "ESP32 Сейф: Игра пройдена.",
    
    # --- Системные и UI команды ---
    "restart": "[UI] Нажата кнопка RESTART.",
    "start": "[UI] Нажата кнопка START.",
    "ready": "[UI] Нажата кнопка READY (проверка системы).",
    "pause": "[UI] Нажата кнопка PAUSE.",
    "soundon": "[SND] Приглушение фоновой музыки.",
    "soundoff": "[SND] Восстановление громкости фоновой музыки.",
}

#импорт библиотек  flask фреймворк для работы с сервером так же дополнение socketIO 
from datetime import timedelta
from flask import Flask, send_file, request, jsonify, session, redirect, url_for, render_template_string
from flask import render_template, request
from flask_socketio import SocketIO 
# билиотека для работы с потоками прочти документацию по ней возможно пригодится
import threading
from threading import Thread
#библа для работы со временем тоже читай документацию в основном только метод sleep
import time
# для конфигурации сервера не дает отсылать в консоль данные по пакетам
from engineio.payload import Payload
#сериал тут и обьяснять не нужно но тоже на досуге почитать документацию
import serial
from serial import Serial
import serial.tools.list_ports
# библиотека для работы со звуками и графикой в нашем случае используется только музыка но тоже может пригодится почтай на досуге там не много
import pygame
import pygame.mixer
#библа для логирования тоже убирает лишние данные с консоли
import logging
import os
from logging.handlers import RotatingFileHandler
#библиотека для работы с системой много крутых фишек тоже почитай
from subprocess import call
import os
import requests
from requests.exceptions import RequestException
import eventlet.queue
import random

# ==========================================
# ОПТИМИЗАЦИЯ: КАРТА ЯЗЫКОВ И ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
# ==========================================

# Соответствие ID языка и суффикса файла
LANG_SUFFIXES = {
    1: 'ru',
    2: 'en',
    3: 'ar',
    4: 'fr',
    5: 'uk',
    6: 'pl'
}

def scale_vol(ui_vol):
    """Переводит шкалу пульта 0-20 в физическую шкалу DFPlayer 0-30"""
    return int((float(ui_vol) / 20.0) * 30)

def play_localized_audio(base_name, loops=0):
    """
    Автоматически формирует имя файла на основе текущего языка и воспроизводит его.
    Пример: play_localized_audio("story_1") при language=1 включит "story_1_ru.wav"
    """
    global language
    
    # Получаем суффикс (по умолчанию ru, если что-то сломалось)
    suffix = LANG_SUFFIXES.get(language, 'ru')
    
    # Собираем имя файла. Убедитесь, что все файлы имеют формат name_lang.wav
    filename = f"{base_name}_{suffix}.wav"
    
    # Вызываем существующую функцию воспроизведения
    play_story(filename, loops=loops)

# ФУНКЦИЯ ДЛЯ ОПРЕДЕЛЕНИЯ ИМЕНИ УСТРОЙСТВА В ЛОГАХ
def get_device_tag(text):
    """Определяет имя устройства по тексту команды или лога."""
    t = text.lower()
    
    # 1. Если это лог от устройства (log:device:...)
    if "log:" in t:
        if ":owls:" in t: return "[Owls]"
        if ":dog:" in t: return "[Dog]"
        if ":basket:" in t: return "[Basket]"
        if ":workshop:" in t: return "[Workshop]"
        if ":chest:" in t: return "[Chest]"
        if ":safe:" in t: return "[Safe]"
        if ":wolf:" in t: return "[Wolf]"
        if ":train:" in t: return "[Train]"
        if ":main:" in t: return "[Main Board]"
        return "[Main Board]"

    # 2. Если это команда (эвристика по ключевым словам)
    if "owl" in t or "flew" in t: return "[Owls]"
    if "dog" in t or "knight" in t or "padlock" in t: return "[Dog]"
    if "workshop" in t or "fire" in t or "item" in t or "broom" in t or "helmet" in t: return "[Workshop]"
    if "basket" in t or "ball" in t or "goal" in t or "snitch" in t or "win" in t or "troll" in t or "cave" in t or "mine" in t: return "[Basket]"
    
    # По умолчанию считаем, что это Main Board или системное
    return "[Main Board]"

# Переменные для отслеживания корректного выключения
LATCH_FILE = "startup_latch.txt"
show_improper_shutdown_warning = False

# Create logs directory if it doesn't exist
if not os.path.exists('logs'):
    os.mkdir('logs')


# Класс фильтра для консоли, чтобы не выводить определенные сообщения
class ConsoleFilter(logging.Filter):
    def filter(self, record):
        # Проверяем, что сообщение является строкой
        if not isinstance(record.msg, str):
            return True # Не фильтруем, если это не строка
        
        # Список нежелательных сообщений для консоли
        excluded_keywords = [
            'soundon', 
            'soundoff',
            # --- Фильтры ---
            '(RAW: log:basket:Received command (in HandleMessagges): light_off)',
            '(RAW: library_flicker_start)',
            '(RAW: log:dog:Received command: light_off)',
            '(RAW: log:',
            'PLAY [Фон]:',
            'SENT [ESP32 -', # Скрывает отправку на все ESP
            'RECEIVED [ESP32 Log]:',
            'RECEIVED [ESP32 Log]:',
            'RAW: train_uf_light_on',
            'RAW: train_light_off',
            'RAW: case_finish',
            'RAW: fish_finish'
        ]
        
        # Возвращаем True (показать), если ни одно из ключевых слов не найдено в сообщении
        return not any(keyword in record.msg for keyword in excluded_keywords)


# Get the root logger
logger = logging.getLogger()
logger.setLevel(logging.DEBUG)  # Set the lowest level for the root logger

# Создаем ДВА разных форматтера ---
 
# Форматтер 1: Детальный (для ФАЙЛА)
file_formatter = logging.Formatter(
    '%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)

# Форматтер 2: Короткий (только сообщение, для КОНСОЛИ)
console_formatter = logging.Formatter('%(message)s')

# Create a rotating file handler for DEBUG messages (Подробный лог в файл)
file_handler = RotatingFileHandler('logs/castle.log', maxBytes=5*1024*1024, backupCount=3, encoding='utf-8')
def custom_namer(default_name):
    # default_name приходит как 'logs/castle.log.1'
    # Мы меняем его на 'logs/castle1.log'
    base, num = default_name.split('.log.')
    return f"{base}{num}.log"

file_handler.namer = custom_namer
# Файл пишет INFO+ для тех-поддержки (без DEBUG-шума периодических poll'ов).
# Если нужна глубокая диагностика — временно понизить до logging.DEBUG.
file_handler.setLevel(logging.INFO)
file_handler.setFormatter(file_formatter) # Используем ДЕТАЛЬНЫЙ форматтер

# Create a stream handler for INFO messages (Короткий лог в консоль)
stream_handler = logging.StreamHandler()
stream_handler.setLevel(logging.INFO) # Консоль пишет только INFO и выше
stream_handler.setFormatter(console_formatter) # ИСПОЛЬЗУЕМ КОРОТКИЙ ФОРМАТТЕР

# Применяем новый ConsoleFilter к обработчику консоли
console_filter = ConsoleFilter()
stream_handler.addFilter(console_filter)

# Add handlers to the logger
logger.addHandler(file_handler)
logger.addHandler(stream_handler)

# Reduce verbosity of other libraries
logging.getLogger('werkzeug').setLevel(logging.ERROR)
logging.getLogger('engineio').setLevel(logging.ERROR)
logging.getLogger('socketio').setLevel(logging.ERROR)
logging.getLogger('urllib3').setLevel(logging.WARNING)
logging.getLogger('requests').setLevel(logging.WARNING)

serial_write_queue = eventlet.queue.Queue()
pending_commands = {} # Словарь для хранения последних команд
device_timers = {}    # Словарь для отслеживания активных таймеров
#----переменные
pending_restart = False
pending_ready = False
last_system_cmd_time = 0
ESP32_IP_WOLF = "192.168.4.201" 
ESP32_IP_TRAIN = "192.168.4.202" 
ESP32_IP_SUITCASE = "192.168.4.203" 
ESP32_IP_SAFE = "192.168.4.204" 
ESP32_API_WOLF_URL = f"http://{ESP32_IP_WOLF}/data"
ESP32_API_TRAIN_URL = f"http://{ESP32_IP_TRAIN}/data"
ESP32_API_SUITCASE_URL = f"http://{ESP32_IP_SUITCASE}/data"
ESP32_API_SAFE_URL = f"http://{ESP32_IP_SAFE}/data"

# Маппинг device_id (используется на /tech диагностической вкладке) -> ESP32 URL
DIAG_DEVICE_URLS = {
    'train':     ESP32_API_TRAIN_URL,
    'wolf':      ESP32_API_WOLF_URL,
    'suitcases': ESP32_API_SUITCASE_URL,
    'safe':      ESP32_API_SAFE_URL,
}
# Устройства которые dialect-ируются через Mega serial (а не HTTP).
# Mega проксирует команды башням через свои Serial1/2/3/mySerial.
DIAG_SERIAL_DEVICES = {'main', 'workshop', 'basket', 'dog', 'owls'}
DIAG_LOG_DIR = "/home/pi/New/diag_logs"  # pi user может писать без sudo
is_system_flashing = False
lesson_intro_seq = 0
rating = 0
star = 0
socklist = [50]
devices = []
rateTime = ''
hintCount=0
caveCounter = 0
storyBasketFlag = 0
catchCount = 0
enemyCatchCount = 0
goalCount = 0
enemyGoalCount = 0
current_client_sid = None
sintchEnemyCatchCount = 0
redSintchEnemyCatchCount = 0
redClickSintchEnemyCatchCount = 0
mapClickHints = 0
mapClickOut = 0 
story13Flag = 0
fireFlag = 0
fire2Flag = 0
fire0Flag = 0
owlFlewCount = 0
last_story_55_play_time = 0
last_boy_in_time = 0
level_18_intro_played = False
is_processing_ready = False
is_time_synced = False
mansard_galets = set()
last_mansard_count = 0
last_owl_flew_time = 0
last_lesson_goal_time = 0
arduino_level = 0
# --- ДЛЯ BLUETOOTH ---
# 2026-05-27: определяем реальное начальное состояние Bluetooth через bluetoothctl,
# а не просто False. Иначе после рестарта сервера toggle на пульте показывает OFF
# хотя устройство ещё подключено по BT.
def _detect_bluetooth_state():
    # Проверяем ТОЛЬКО Powered: yes — после старта сервера BT-адаптер может быть
    # включён но НЕ discoverable (телефон уже спарен раньше). Это всё ещё значит
    # что BT "активен" с точки зрения пользователя.
    try:
        result = subprocess.run("bluetoothctl show", shell=True, capture_output=True, text=True, timeout=5)
        return "Powered: yes" in result.stdout
    except Exception:
        return False
bluetooth_active = _detect_bluetooth_state()

def set_bluetooth_state(state):
    """Функция жесткого включения/выключения Bluetooth"""
    global bluetooth_active
    try:
        if state:
            # Сначала жестко гасим адаптер на низком уровне и отключаем SSP
            subprocess.run("sudo btmgmt power off", shell=True, check=False)
            subprocess.run("sudo btmgmt ssp on", shell=True, check=False)
            subprocess.run("sudo btmgmt power on", shell=True, check=False)
            cleanup_cmd = "bluetoothctl devices | cut -f2 -d' ' | while read mac; do bluetoothctl remove $mac; done"
            subprocess.run(cleanup_cmd, shell=True, check=False)
            
            # Затем делаем видимым для поиска
            subprocess.run("bluetoothctl discoverable on", shell=True, check=False)
            subprocess.run("bluetoothctl pairable on", shell=True, check=False)
            bluetooth_active = True
            logger.info("BLUETOOTH: Включен и ждет PIN-код (SSP отключен).")
        else:
            subprocess.run("bluetoothctl discoverable off", shell=True, check=False)
            subprocess.run("bluetoothctl pairable off", shell=True, check=False)
            # Выключаем питание адаптера
            subprocess.run("bluetoothctl power off", shell=True, check=False)
            bluetooth_active = False
            logger.info("BLUETOOTH: Выключен.")
            
        # Уведомляем пульт о реальном статусе
        socketio.emit('bt_state', bluetooth_active, to=None)
    except Exception as e:
        logger.error(f"BLUETOOTH ERROR: {e}")
# -------------------------------

# --- Периодическая проверка системного статуса для Tech Dashboard ---
# Snapshot статуса башен — обновляется парсером входящих логов от Mega при ответе
# на команду 'check_towers' (см. ниже в serial-reader). None = ещё ни разу не отвечала.
# Также храним timestamp последнего обновления: если значение старше TTL — сбрасываем в None
# (Mega перестала отвечать = не доверяем устаревшим данным).
_tower_status_snapshot = {'workshop': None, 'basket': None, 'dog': None, 'owls': None}
_tower_status_updated_at = 0.0  # time.time() последнего обновления snapshot
_TOWER_SNAPSHOT_TTL = 15.0       # сек: дольше — считаем что Mega молчит, snapshot устарел

def _read_text(path):
    try:
        with open(path) as _f:
            return _f.read().strip()
    except Exception:
        return ''

def _iface_state(iface):
    """Возвращает (up: bool, ipv4: str | '') — читает из /sys и /proc без subprocess."""
    operstate = _read_text(f'/sys/class/net/{iface}/operstate')
    if not operstate:
        return False, ''
    # tailscale0 в /sys обычно 'unknown' но фактически активен
    up = operstate in ('up', 'unknown')
    if not up:
        return False, ''
    # Читаем IPv4 из /proc/net/fib_trie (универсально, без subprocess)
    # Проще: вызовем internal call через ctypes-free путь — через netlink сложно.
    # Используем /proc/net/route + читать адрес через socket ioctl SIOCGIFADDR.
    ipv4 = ''
    try:
        import socket as _sock, fcntl as _fcntl, struct as _struct
        s = _sock.socket(_sock.AF_INET, _sock.SOCK_DGRAM)
        try:
            packed = _fcntl.ioctl(s.fileno(), 0x8915,  # SIOCGIFADDR
                                  _struct.pack('256s', iface.encode()[:15]))
            ipv4 = _sock.inet_ntoa(packed[20:24])
        finally:
            s.close()
    except Exception:
        pass
    return up, ipv4

def _check_tcp(host, port, timeout=1.5):
    """Eventlet-aware TCP-connect — для проверки интернета и ESP32."""
    try:
        import socket as _sock
        with _sock.create_connection((host, port), timeout=timeout):
            return True
    except Exception:
        return False

def _check_esp32(ip):
    """HTTP POST 'hc' на ESP32 с коротким таймаутом."""
    try:
        r = requests.post(f'http://{ip}/data', json='hc', timeout=1.5)
        return r.ok
    except Exception:
        return False

_diag_loggers = {}  # device -> logging.Logger (создаются лениво на первый snapshot)

def _get_diag_logger(device):
    """Один RotatingFileHandler на устройство: 5 MB × 3 backup = 20 MB max.
    Аналогично castle.log (RotatingFileHandler) — пишет в <device>.log, при заполнении
    переименовывает в <device>1.log, <device>2.log, <device>3.log; самый старый удаляется.
    Итого ~5.5 часов истории активной диагностики на устройство."""
    lg = _diag_loggers.get(device)
    if lg is not None:
        return lg
    os.makedirs(DIAG_LOG_DIR, exist_ok=True)
    lg = logging.getLogger(f"diag.{device}")
    lg.setLevel(logging.INFO)
    lg.propagate = False  # не дублировать в castle.log
    fname = os.path.join(DIAG_LOG_DIR, f"{device}.log")
    handler = RotatingFileHandler(fname, maxBytes=5 * 1024 * 1024, backupCount=3, encoding='utf-8')
    def _namer(default_name):
        # default_name = "...<device>.log.1" → "...<device>1.log" (как у castle.log)
        base, num = default_name.split('.log.')
        return f"{base}{num}.log"
    handler.namer = _namer
    handler.setFormatter(logging.Formatter('%(message)s'))  # timestamp пишем сами в snap-строку
    lg.addHandler(handler)
    _diag_loggers[device] = lg
    return lg

def _log_diag_snapshot(device, snap):
    """Append одну строку snapshot'а в /home/pi/New/diag_logs/<device>.log с ротацией.
    Вызывается на каждый POST {"diag": {...}} от ESP32 (5/сек на устройство в diag-mode).
    Используется для post-mortem анализа: монтажник тыкал железо → потом смотрит лог."""
    try:
        from datetime import datetime as _dt
        now = _dt.now()
        _get_diag_logger(device).info(
            f"{now.isoformat(timespec='milliseconds')} {json.dumps(snap, separators=(',', ':'))}"
        )
    except Exception as e:
        logger.warning(f"[DIAG] Failed to log snapshot for {device}: {e}")

def gather_system_status():
    """Собирает текущий статус устройств и сетей для технического пульта."""
    import os

    # ESP32 — живой опрос (не полагаемся на старый глобал `devices`)
    esp32 = {
        'wolf':      _check_esp32('192.168.4.201'),
        'platform':  _check_esp32('192.168.4.202'),
        'suitcases': _check_esp32('192.168.4.203'),
        'safe':      _check_esp32('192.168.4.204'),
    }

    # Башни: main = по /dev/ttyUSB_MAIN. Остальные — из snapshot который
    # обновляется когда Mega отвечает на 'check_towers' (heartbeat).
    # Если snapshot устарел (> TTL сек) — Mega молчит, snapshot не доверяем →
    # возвращаемся к fallback (main_ok). Это охватывает и случай когда сервер
    # только что стартовал и snapshot пустой.
    import time as _time
    main_ok = os.path.exists('/dev/ttyUSB_MAIN')
    snap = _tower_status_snapshot
    fresh = (_time.time() - _tower_status_updated_at) < _TOWER_SNAPSHOT_TTL if _tower_status_updated_at else False
    def _tower(name):
        v = snap[name]
        if not fresh or v is None:   # snapshot устарел или не было ответа — fallback
            return main_ok
        return bool(v)
    towers = {
        'main':     main_ok,
        'owls':     _tower('owls'),
        'basket':   _tower('basket'),
        'workshop': _tower('workshop'),
        'dog':      _tower('dog'),
    }

    # Сети — через /sys и SIOCGIFADDR (без subprocess)
    networks = {'castle_ap': False, 'client_wifi': False, 'tailscale': False, 'internet': False}

    # wlan0 — Castle AP. Должен быть UP и иметь 192.168.4.1
    up0, ip0 = _iface_state('wlan0')
    networks['castle_ap'] = up0 and ip0 == '192.168.4.1'

    # wlan1 — клиент любой Wi-Fi сети. UP + IP не link-local
    up1, ip1 = _iface_state('wlan1')
    networks['client_wifi'] = up1 and bool(ip1) and not ip1.startswith('169.254.')

    # tailscale0 — VPN-канал. UP + IP в 100.64.0.0/10 (CGNAT диапазон Tailscale)
    upt, ipt = _iface_state('tailscale0')
    networks['tailscale'] = upt and ipt.startswith('100.')

    # Интернет — TCP-connect к 1.1.1.1:53 (быстро, eventlet-aware)
    networks['internet'] = _check_tcp('1.1.1.1', 53, timeout=1.5)

    # USB устройства (по VID:PID в /sys/bus/usb/devices/)
    usb_targets = {
        'audio':       ('0c76', '1676'),   # JMTek USB PnP Audio Device
        'wifi_dongle': ('2357', '0111'),   # TP-Link USB-WiFi
        'hub':         ('214b', '7250'),   # Huasheng USB-hub (башни)
        'mega_main':   ('1a86', '7523'),   # CH340 (Mega главная)
    }
    usb = {k: False for k in usb_targets}
    try:
        for dev in os.listdir('/sys/bus/usb/devices'):
            if not dev or not dev[0].isdigit():
                continue
            vid = _read_text(f'/sys/bus/usb/devices/{dev}/idVendor')
            pid = _read_text(f'/sys/bus/usb/devices/{dev}/idProduct')
            if not vid or not pid:
                continue
            for name, (tvid, tpid) in usb_targets.items():
                if vid == tvid and pid == tpid:
                    usb[name] = True
                    break
    except Exception:
        pass

    # Per-port проверка для USB-CH340 башен: если CH340 на хабе "залип"
    # (errcode -110 на USB control transfer), tcgetattr вернёт EIO. Используем
    # неблокирующий os.open + termios — намного быстрее чем subprocess stty,
    # и не блокирует eventlet loop.
    import termios as _termios
    import fcntl as _fcntl
    tower_usb_paths = {
        'usb_owls':     '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.1:1.0-port0',
        'usb_dog':      '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.2:1.0-port0',
        'usb_basket':   '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.3:1.0-port0',
        'usb_workshop': '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.4:1.0-port0',
    }
    for key, path in tower_usb_paths.items():
        fd = -1
        try:
            if not os.path.exists(path):
                usb[key] = False
                continue
            fd = os.open(path, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
            _termios.tcgetattr(fd)  # USB control transfer — EIO если CH340 не отвечает
            usb[key] = True
        except Exception:
            usb[key] = False
        finally:
            if fd >= 0:
                try: os.close(fd)
                except: pass

    return {
        'esp32':     esp32,
        'towers':    towers,
        'networks':  networks,
        'usb':       usb,
        'bluetooth': bluetooth_active,
    }


# Предыдущее состояние устройств — для логирования только изменений (state changes).
# Хранятся как dict { name: bool|None }. None означает "ещё не проверяли".
_prev_status_state = {
    'esp32_wolf': None, 'esp32_platform': None, 'esp32_suitcases': None, 'esp32_safe': None,
    'tower_main': None, 'tower_owls': None, 'tower_basket': None, 'tower_workshop': None, 'tower_dog': None,
    'network_castle_ap': None, 'network_client_wifi': None, 'network_tailscale': None, 'network_internet': None,
    'usb_audio': None, 'usb_wifi_dongle': None, 'usb_hub': None, 'usb_mega_main': None,
    'usb_usb_owls': None, 'usb_usb_basket': None, 'usb_usb_workshop': None, 'usb_usb_dog': None,
}

# Имена для логов: показываем понятные русские/английские лейблы вместо ключей.
_status_labels = {
    'esp32_wolf': 'ESP32 Wolf', 'esp32_platform': 'ESP32 Platform', 'esp32_suitcases': 'ESP32 Suitcases', 'esp32_safe': 'ESP32 Safe',
    'tower_main': 'Main Board', 'tower_owls': 'Tower Owls', 'tower_basket': 'Tower Basket', 'tower_workshop': 'Tower Workshop', 'tower_dog': 'Tower Dog',
    'network_castle_ap': 'Castle AP', 'network_client_wifi': 'Client WiFi', 'network_tailscale': 'Tailscale', 'network_internet': 'Internet',
    'usb_audio': 'USB Audio', 'usb_wifi_dongle': 'USB WiFi-dongle', 'usb_hub': 'USB Hub', 'usb_mega_main': 'USB Mega-main',
    'usb_usb_owls': 'USB Owls (1.2.1)', 'usb_usb_basket': 'USB Basket (1.2.3)', 'usb_usb_workshop': 'USB Workshop (1.2.4)', 'usb_usb_dog': 'USB Dog (1.2.2)',
}


_status_fail_counter = {}  # key -> сколько последовательных fail'ов
OFFLINE_FAIL_THRESHOLD = 2  # 2026-05-27: ALERT только после 2 подряд неудачных проверок (~10s)

def _log_status_changes(status):
    """Сравнивает текущий статус с предыдущим и пишет ALERT/OK в лог
    только при изменении. Это убирает спам и оставляет только важные события.

    2026-05-27: добавлен fail-counter — ALERT только после OFFLINE_FAIL_THRESHOLD
    подряд неудачных проверок. Защита от ложных alert'ов из-за транзитных
    потерь пакетов / WiFi reconnect (Pi RTL8189FS бывает шумит)."""
    flat = {}
    for k, v in status.get('esp32', {}).items():     flat[f'esp32_{k}'] = bool(v)
    for k, v in status.get('towers', {}).items():    flat[f'tower_{k}'] = bool(v)
    for k, v in status.get('networks', {}).items():  flat[f'network_{k}'] = bool(v)
    for k, v in status.get('usb', {}).items():       flat[f'usb_{k}'] = bool(v)

    for key, current in flat.items():
        # Обновляем fail-counter
        if current:
            _status_fail_counter[key] = 0
        else:
            _status_fail_counter[key] = _status_fail_counter.get(key, 0) + 1

        # Текущее "эффективное" состояние: считаем offline только если набрался threshold
        effective_current = current or (_status_fail_counter.get(key, 0) < OFFLINE_FAIL_THRESHOLD)

        prev = _prev_status_state.get(key)
        if prev is None:
            # Первое наблюдение — фиксируем без alert
            _prev_status_state[key] = effective_current
            continue
        if prev != effective_current:
            label = _status_labels.get(key, key)
            if effective_current:
                logger.info(f'STATUS OK: {label} restored to ONLINE')
            else:
                logger.warning(f'STATUS ALERT: {label} went OFFLINE ({_status_fail_counter.get(key, 0)} consecutive fails)')
            _prev_status_state[key] = effective_current


# Время последнего ответа от Mega (для детектора "Mega silent")
_last_mega_response = 0.0


def wlan1_watchdog():
    """Постоянный мониторинг wlan1 (USB-WiFi-донгла). Если интерфейс упал
    >30 сек — soft reset (ip link down/up + dhcpcd). Если >100 сек —
    hard reset через USB authorized=0/1 (физический re-enumerate
    устройства, эквивалент передёргиванию кабеля).

    Причина: USB-донгл на CLC3 (TP-Link 2357:0111) на порту 1-1.3
    периодически отваливается из-за нестабильного контакта/удлинителя.
    Клиенту приходилось передёргивать руками. Теперь сервер делает сам.

    Привязка к порту 1-1.3 одинакова для CLC1/CLC2/CLC3 (см.
    [[clc-arduino-usb-port-binding]] — invariant port mapping)."""
    import subprocess
    eventlet.sleep(120)  # дать time bootstrap + hostapd_watchdog отработать первым
    consecutive_down = 0
    USB_WIFI_PATH = '/sys/bus/usb/devices/1-1.3/authorized'
    while True:
        try:
            up, ip = _iface_state('wlan1')
            healthy = up and bool(ip) and not ip.startswith('169.254.')
            if not healthy:
                consecutive_down += 1
                if consecutive_down == 3:  # ~30 сек down
                    logger.warning("WLAN1 WATCHDOG: wlan1 down ~30s — soft reset")
                    try:
                        text_to_wlan1_reset()
                        subprocess.run(['sudo', 'systemctl', 'restart', 'dhcpcd'],
                                       check=False, timeout=8)
                    except Exception as _e:
                        logger.error(f"wlan1 soft reset failed: {_e}")
                elif consecutive_down == 10:  # ~100 сек down
                    logger.warning("WLAN1 WATCHDOG: still down — USB power-cycle (authorized=0/1)")
                    try:
                        subprocess.run(['sudo', 'sh', '-c', f'echo 0 > {USB_WIFI_PATH}'],
                                       check=False, timeout=3)
                        eventlet.sleep(2)
                        subprocess.run(['sudo', 'sh', '-c', f'echo 1 > {USB_WIFI_PATH}'],
                                       check=False, timeout=3)
                    except Exception as _e:
                        logger.error(f"USB power-cycle failed: {_e}")
                elif consecutive_down == 30:  # ~5 мин down
                    logger.error("WLAN1 WATCHDOG: wlan1 down >5 min — manual intervention needed")
            else:
                if consecutive_down >= 3:
                    logger.info(f"WLAN1 WATCHDOG: recovered (was down ~{consecutive_down*10}s)")
                consecutive_down = 0
        except Exception as e:
            logger.error(f"wlan1_watchdog error: {e}")
        eventlet.sleep(10)


def hostapd_bootstrap_watchdog():
    """После старта сервера: если за 60 сек ни одна станция не ассоциирована
    с Castle AP (wlan0), но при этом hostapd active — значит hostapd встал
    кривой при boot (race condition с fix-wlan-names или RTL8189fs драйвером).
    Один раз рестартим hostapd. Если и после этого 0 stations через 90 сек —
    больше не вмешиваемся (ESP32 могут быть выключены физически).
    Покрывает 99% сценариев "клиент включил квест и Castle AP мёртв"."""
    import subprocess
    eventlet.sleep(60)  # дать клиентам время подключиться при нормальном boot

    def count_stations():
        try:
            r = subprocess.run(['sudo', 'iw', 'dev', 'wlan0', 'station', 'dump'],
                               capture_output=True, timeout=3, text=True)
            return r.stdout.count('Station ')
        except Exception:
            return -1

    n = count_stations()
    if n == 0:
        logger.warning(f"HOSTAPD WATCHDOG: 0 stations через 60s после старта — рестартим hostapd")
        try:
            subprocess.run(['sudo', 'systemctl', 'restart', 'hostapd'],
                           check=False, timeout=10)
        except Exception as _e:
            logger.error(f"hostapd restart failed: {_e}")
        eventlet.sleep(90)
        n2 = count_stations()
        if n2 == 0:
            logger.warning(f"HOSTAPD WATCHDOG: 0 stations и после рестарта — ESP32 видимо выключены физически, больше не вмешиваемся")
        else:
            logger.info(f"HOSTAPD WATCHDOG: после рестарта подключено {n2} stations — OK")
    else:
        logger.info(f"HOSTAPD WATCHDOG: {n} stations подключено за 60s — bootstrap прошёл штатно, watchdog не нужен")


def mega_boot_watchdog():
    """Watchdog для cold-boot Mega.

    Симптом 24 мая 2026: после power-on квеста ~50% случаев Mega не
    отправляет QUEST_SYSTEM_READY (клиент слышит тишину, нет on.wav).
    Помогает physical reset кнопка — клиенту не доступна.

    Эскалация (всё после 25 сек тишины):
      1. DTR pulse 0.5с — должно ресетить Mega через её reset-цепь
         (то же что нажатие RESET кнопки на плате).
      2. Если за 25 сек не ответила → ser.close()+open() — переоткрытие
         CDC pipe, сбрасывает state CH340 + вторая попытка DTR.
      3. Если ещё 25 сек тишина → USB power-cycle на порт 1-1.1
         (где сидит Mega CH340) — жёсткий ресет всего USB-устройства.
      4. После каждого шага — 25 сек wait. Если в любой момент пришёл
         QUEST_SYSTEM_READY → watchdog завершается, флаг подтверждён.
    """
    global mega_initial_boot_received, ser

    def _wait_or_done(seconds):
        """Спим N сек, возвращаем True если Mega ожила за это время."""
        for _ in range(seconds * 2):
            if mega_initial_boot_received:
                return True
            eventlet.sleep(0.5)
        return False

    # Шаг 0: начальное ожидание нормального boot
    if _wait_or_done(25):
        return

    # Шаг 1: DTR pulse 0.5 сек
    logger.warning("MEGA BOOT WATCHDOG: 25с тишины — DTR pulse 0.5с")
    try:
        ser.dtr = False
        eventlet.sleep(0.5)
        ser.dtr = True
    except Exception as e:
        logger.error(f"MEGA BOOT WATCHDOG: DTR toggle failed: {e}")
    if _wait_or_done(25):
        logger.info("MEGA BOOT WATCHDOG: Mega ожила после DTR pulse ✅")
        return

    # Шаг 2: ser.close()+open() — пересоздание CDC pipe (более жёсткий reset)
    logger.warning("MEGA BOOT WATCHDOG: всё ещё тишина — пересоздаю serial connection")
    try:
        ser.close()
        eventlet.sleep(1)
        ser.open()
        # После open() pyserial автоматически делает DTR pulse
    except Exception as e:
        logger.error(f"MEGA BOOT WATCHDOG: serial reopen failed: {e}")
    if _wait_or_done(25):
        logger.info("MEGA BOOT WATCHDOG: Mega ожила после ser.close/open ✅")
        return

    # Шаг 3: USB power-cycle на порт Mega (1-1.1)
    logger.warning("MEGA BOOT WATCHDOG: serial reopen не помог — USB power-cycle 1-1.1")
    try:
        USB_MEGA_PATH = '/sys/bus/usb/devices/1-1.1/authorized'
        subprocess.run(['sudo', 'sh', '-c', f'echo 0 > {USB_MEGA_PATH}'], check=False, timeout=3)
        eventlet.sleep(2)
        subprocess.run(['sudo', 'sh', '-c', f'echo 1 > {USB_MEGA_PATH}'], check=False, timeout=3)
        eventlet.sleep(3)  # ждём re-enumeration
        # После re-enumeration udev пересоздаёт /dev/ttyUSB_MAIN
        # Нужно переоткрыть ser
        try:
            ser.close()
        except: pass
        eventlet.sleep(1)
        ser = serial.Serial(ARDUINO_PORT, 115200, timeout=1)
    except Exception as e:
        logger.error(f"MEGA BOOT WATCHDOG: USB power-cycle failed: {e}")
    if _wait_or_done(25):
        logger.info("MEGA BOOT WATCHDOG: Mega ожила после USB power-cycle ✅")
        return

    logger.error("MEGA BOOT WATCHDOG: ВСЕ ПОПЫТКИ ИСЧЕРПАНЫ — Mega молчит. Нужно ручное вмешательство (reset на плате Mega или замена платы)")


def tailscale_watchdog():
    """Авто-восстановление Tailscale на Pi.

    Симптом сбоя: `tailscale status` показывает logged out / NoState / failed to
    resolve controlplane.tailscale.com — Pi выпадает из Tailnet, ни клиент,
    ни оператор не могут зайти на 100.79.189.15:3000/tech удалённо.

    Лечение: `sudo tailscale up --reset --accept-dns=false`. Опытным путём — этого
    достаточно, реавторизации обычно НЕ требуется (узел остаётся в учётке клиента,
    --reset поднимает локальный демон со свежим состоянием).

    Цикл: каждые 120с проверяем BackendState из `tailscale status --json`.
    После восстановления — 5 мин cooldown, чтобы не зациклиться при затяжных проблемах."""
    import subprocess
    eventlet.sleep(180)  # дать сервису время на normal startup (Tailscale стартует ~30-60с)

    while True:
        try:
            r = subprocess.run(['tailscale', 'status', '--json'],
                               capture_output=True, text=True, timeout=8)
            state = None
            if r.returncode == 0 and r.stdout:
                try:
                    state = json.loads(r.stdout).get('BackendState')
                except Exception:
                    state = None

            if state == 'Running':
                # OK — лог только при восстановлении после прошлой проблемы
                if getattr(tailscale_watchdog, '_was_broken', False):
                    logger.info(f"TAILSCALE WATCHDOG: восстановлен (BackendState={state})")
                    tailscale_watchdog._was_broken = False
            else:
                logger.warning(f"TAILSCALE WATCHDOG: Tailscale unhealthy (BackendState={state}, rc={r.returncode}) — пытаюсь --reset")
                tailscale_watchdog._was_broken = True
                try:
                    res = subprocess.run(
                        ['sudo', 'tailscale', 'up', '--reset', '--accept-dns=false'],
                        capture_output=True, text=True, timeout=30
                    )
                    logger.info(f"TAILSCALE WATCHDOG: tailscale up rc={res.returncode} "
                                f"out={(res.stdout or '').strip()[:200]} "
                                f"err={(res.stderr or '').strip()[:200]}")
                except Exception as _e:
                    logger.error(f"TAILSCALE WATCHDOG: --reset failed: {_e}")
                eventlet.sleep(300)  # cooldown 5 мин чтобы не зациклиться
                continue
        except Exception as e:
            logger.error(f"tailscale_watchdog error: {e}")
        eventlet.sleep(120)


def system_status_loop():
    """Раз в 5 секунд эмитит system_status для технического пульта."""
    global _last_mega_response
    while True:
        try:
            # Запросить у Mega текущий статус башен (она ответит "tower_status:..." -
            # его поймает парсер в serial() и обновит _tower_status_snapshot).
            # 2026-05-26 ОТКЛЮЧЕНО (подтверждённый root cause циклов door_owl
            # на этапе сов на CLC3). Архивная Mega (от 3.4.26) НЕ имеет handler
            # для команды check_towers → попадает в Unlocks(buff) fall-through и
            # broadcastится через все 4 Serial (Serial1=Workshop, Serial2=Basket,
            # Serial3=Dog, mySerial=Owls). Башни echo команду через sendLog → Mega
            # substring match buff.indexOf("door_owl") срабатывает на fragment →
            # ghost door_owl event → зацикленный hint_14_c. См. memory:
            # project_clc_check_towers_root_cause_2026_05_26.md
            # Восстановить можно ТОЛЬКО после добавления явного handler'а
            # `check_towers` в Mega (no-op + ответ tower_status:...).
            # try:
            #     serial_write_queue.put('check_towers')
            #     process_serial_queue()
            # except Exception:
            #     pass
            status = gather_system_status()
            socketio.emit('system_status', status, to=None)
            _log_status_changes(status)
            # Mega-silence detector: если tower_status snapshot устарел > 30 сек
            # (т.е. Mega не отвечает на наши check_towers) — алерт.
            import time as _time
            if _tower_status_updated_at:
                silent_for = _time.time() - _tower_status_updated_at
                if silent_for > 30 and not getattr(system_status_loop, '_mega_silent_logged', False):
                    logger.warning(f'STATUS ALERT: Main Board silent for {int(silent_for)}s (no heartbeat response)')
                    system_status_loop._mega_silent_logged = True
                elif silent_for < 15 and getattr(system_status_loop, '_mega_silent_logged', False):
                    logger.info(f'STATUS OK: Main Board heartbeat resumed')
                    system_status_loop._mega_silent_logged = False
        except Exception as e:
            logger.error(f"system_status_loop error: {e}")
        eventlet.sleep(5)
# -------------------------------
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)
pygame.mixer.pre_init(44100, -16, 2, 2048)
pygame.init()
pygame.mixer.init()

# === Layer 3: pygame audio safety monkey-patch (2026-05-27) ===
# pygame.mixer.Sound() и music.load() поддерживают ТОЛЬКО 16-bit PCM WAV.
# Если клиент зальёт через Samba 24/32-bit WAV из Audacity/Reaper —
# pygame бросит "Unable to open file". Без этой обёртки модульная
# инициализация (Sound('door_act.wav') etc.) упадёт и сервер не стартует.
# С обёрткой — battery 400-byte silence-buffer, channel.play() не падает.
# См. reference: pygame-wav-24-bit
_orig_Sound = pygame.mixer.Sound
def _safe_Sound(*a, **kw):
    try:
        return _orig_Sound(*a, **kw)
    except Exception as e:
        logging.warning(f"Sound load failed {a}: {e} — returning silence stub")
        try:
            return _orig_Sound(buffer=bytes(400))  # ~0.1s тишины
        except Exception:
            return None
pygame.mixer.Sound = _safe_Sound

_orig_music_load = pygame.mixer.music.load
def _safe_music_load(*a, **kw):
    try:
        return _orig_music_load(*a, **kw)
    except Exception as e:
        logging.warning(f"music.load failed {a}: {e}")
        return None
pygame.mixer.music.load = _safe_music_load
# === END Layer 3 ===

#------инициализация звуковых каналов
# Явно запрашиваем 8 каналов у Pygame (стандартно, но для надежности)
pygame.mixer.set_num_channels(8)

channel1 = pygame.mixer.Channel(0) # background music (Фон)
# channel2 оставляем как переменную для совместимости, но использовать будем список
channel2 = pygame.mixer.Channel(1) 
channel3 = pygame.mixer.Channel(2) # story (Истории)

# --- Пул каналов для эффектов ---
# Создаем список из 4 каналов: 1 (старый) и 3, 4, 5 (новые)
# Канал 0 занят фоном, Канал 2 занят историями.
effects_pool = [
    pygame.mixer.Channel(1),
    pygame.mixer.Channel(3),
    pygame.mixer.Channel(4),
    pygame.mixer.Channel(5)
]
# Индекс для циклического перебора, если все каналы заняты
current_effect_index = 0

#------эффекты в формате wav (ЗВУКИ ОСТАВЛЯЕМ КАК ОБЪЕКТЫ, ЭТО ПРАВИЛЬНО)
door_act = pygame.mixer.Sound('door_act.wav')
h_clock = pygame.mixer.Sound('h_clock.wav')
uf_clock = pygame.mixer.Sound('uf_clock.wav')
steps = pygame.mixer.Sound('steps.wav')
clock2 = pygame.mixer.Sound('uf_clock.wav')
door_attic = pygame.mixer.Sound('door_attic.wav')
flags = pygame.mixer.Sound('flags.wav')
train = pygame.mixer.Sound('train.wav')
door_owl = pygame.mixer.Sound('door_owl.wav')
owl_flew = pygame.mixer.Sound('door_owl.wav')
map_click = pygame.mixer.Sound('map_click.wav')
map_out = pygame.mixer.Sound('map_out.wav')
door_witch = pygame.mixer.Sound('door_witch.wav')
bottle1 =  pygame.mixer.Sound('bottle1.wav')
bottle2 =  pygame.mixer.Sound('bottle2.wav')
bottle3 =  pygame.mixer.Sound('bottle3.wav')
bottle4 =  pygame.mixer.Sound('bottle4.wav')
bottle_fall =  pygame.mixer.Sound('bottle_fall.wav')
bottle_end = pygame.mixer.Sound('bottle_end.wav')
door_dog = pygame.mixer.Sound('door_dog.wav')
dog_growl = pygame.mixer.Sound('dog_growl.wav')
dog_sleep = pygame.mixer.Sound('dog_sleep.wav')
dog_lock = pygame.mixer.Sound('dog_lock.wav')
door_bank = pygame.mixer.Sound('door_bank.wav')
safe_turn = pygame.mixer.Sound('safe_turn.wav')
safe_fix = pygame.mixer.Sound('safe_fix.wav')
safe_end = pygame.mixer.Sound('safe_end.wav')
cave_click = pygame.mixer.Sound('cave_click.wav')
door_cave = pygame.mixer.Sound('door_cave.wav')
door_workshop = pygame.mixer.Sound('door_workshop.wav')
cave_search = pygame.mixer.Sound('cave_search.wav')
cave_end = pygame.mixer.Sound('cave_end.wav')
fire1 = pygame.mixer.Sound('fire1.wav')
fire2 = pygame.mixer.Sound('fire2.wav')
fire3 = pygame.mixer.Sound('fire3.wav')
fire0 = pygame.mixer.Sound('fire0.wav')
item_find = pygame.mixer.Sound('item_find.wav')
item_add = pygame.mixer.Sound('item_add.wav')
craft_success = pygame.mixer.Sound('craft_success.wav')
door_library = pygame.mixer.Sound('door_library.wav')
star_hint = pygame.mixer.Sound('star_hint.wav')
lib_door = pygame.mixer.Sound('lib_door.wav')
door_top = pygame.mixer.Sound('door_top.wav')
door_basket = pygame.mixer.Sound('door_basket.wav')
lose1 = pygame.mixer.Sound('lose1.wav')
door_spell = pygame.mixer.Sound('door_spell.wav')
cristal_up = pygame.mixer.Sound('cristal_up.wav')
true_crystal = pygame.mixer.Sound('true_crystal.wav')
brain_end = pygame.mixer.Sound('brain_end.wav')
level_up = pygame.mixer.Sound('level_up.wav')
mistake_crystal = pygame.mixer.Sound('mistake_crystal.wav')
start_crystal = pygame.mixer.Sound('start_crystal.wav')
applause = pygame.mixer.Sound('applause.wav')
flying_ball = pygame.mixer.Sound('flying_ball.wav')
catch1 = pygame.mixer.Sound('catch1.wav')
catch2 = pygame.mixer.Sound('catch2.wav')
catch3 = pygame.mixer.Sound('catch3.wav')
catch4 = pygame.mixer.Sound('catch3.wav')
goal1 = pygame.mixer.Sound('goal1.wav')
goal2 = pygame.mixer.Sound('goal2.wav')
goal3 = pygame.mixer.Sound('goal3.wav')
goal4 = pygame.mixer.Sound('goal4.wav')
goal5 = pygame.mixer.Sound('goal5.wav')
goal6 = pygame.mixer.Sound('goal6.wav')
goal7 = pygame.mixer.Sound('goal7.wav')
enemy_catch1 = pygame.mixer.Sound('enemy_catch1.wav')
enemy_catch2 = pygame.mixer.Sound('enemy_catch2.wav')
enemy_catch3 = pygame.mixer.Sound('enemy_catch3.wav')
enemy_catch4 = pygame.mixer.Sound('enemy_catch4.wav')
enemy_goal1 = pygame.mixer.Sound('enemy_goal1.wav')
enemy_goal2 = pygame.mixer.Sound('enemy_goal2.wav')
enemy_goal3 = pygame.mixer.Sound('enemy_goal3.wav')
enemy_goal4 = pygame.mixer.Sound('enemy_goal4.wav')
win = pygame.mixer.Sound('win.wav')
timeout = pygame.mixer.Sound('timeout.wav')
swipe_r = pygame.mixer.Sound('swipe_r.wav')
swipe_l = pygame.mixer.Sound('swipe_l.wav')
fireplace = pygame.mixer.Sound('fireplace.wav')
knock_castle = pygame.mixer.Sound('knock_castle.wav')
kay_in = pygame.mixer.Sound('kay_in.wav')
kay_out = pygame.mixer.Sound('kay_out.wav')
on_sound = pygame.mixer.Sound('on.wav')

# --- ОПТИМИЗАЦИЯ: СПИСКИ ЗВУКОВ ---
player_goal_sounds = [goal2, goal3, goal4, goal5, goal6, goal7]
enemy_goal_sounds = [enemy_goal1, enemy_goal2, enemy_goal3, enemy_goal4]

# --- ОПТИМИЗАЦИЯ: СПИСКИ БАЗОВЫХ ИМЕН ИСТОРИЙ ---
# Здесь мы храним только "корень" имени файла. Суффиксы (_ru, _en и т.д.) будут подставляться автоматически.

# Голы игрока
player_goal_stories_base = [
    "story_61_b", "story_61_c", "story_61_d", "story_61_e", 
    "story_61_f", "story_61_g", "story_61_h", "story_61_i", "story_61_j"
]

# Голы бота
enemy_goal_stories_base = [
    "story_65_a", "story_65_b", "story_65_c", "story_65_d", "story_65_e", 
    "story_65_f", "story_65_g", "story_65_h", "story_65_i", "story_65_j"
]

# Тайм-аут игрока
timeout_stories_base = [
    "story_62_a", "story_62_b", "story_62_c", "story_62_d", "story_62_e"
]

# Враг перехватил мяч (Red Ball)
red_ball_active_stories_base = [
    "story_63_a", "story_63_b", "story_63_c", "story_63_d", "story_63_e",
    "story_63_f", "story_63_g", "story_63_h", "story_63_i", "story_63_j"
]

# Игрок перехватил мяч
enemy_catch_stories_base = [
    "story_64_a", "story_64_b"
]


# --- Карта для логирования имен звуковых файлов ---
try:
    _SOUND_NAME_MAP = {
        # Эффекты (оставляем, так как это объекты Sound)
        door_act: "door_act.wav",
        h_clock: "h_clock.wav",
        uf_clock: "uf_clock.wav",
        steps: "steps.wav",
        clock2: "uf_clock.wav",
        door_attic: "door_attic.wav",
        flags: "flags.wav",
        train: "train.wav",
        door_owl: "door_owl.wav",
        owl_flew: "door_owl.wav",
        map_click: "map_click.wav",
        map_out: "map_out.wav",
        door_witch: "door_witch.wav",
        bottle1: "bottle1.wav",
        bottle2: "bottle2.wav",
        bottle3: "bottle3.wav",
        bottle4: "bottle4.wav",
        bottle_fall: "bottle_fall.wav",
        bottle_end: "bottle_end.wav",
        door_dog: "door_dog.wav",
        dog_growl: "dog_growl.wav",
        dog_sleep: "dog_sleep.wav",
        dog_lock: "dog_lock.wav",
        door_bank: "door_bank.wav",
        safe_turn: "safe_turn.wav",
        safe_fix: "safe_fix.wav",
        safe_end: "safe_end.wav",
        cave_click: "cave_click.wav",
        door_cave: "door_cave.wav",
        door_workshop: "door_workshop.wav",
        cave_search: "cave_search.wav",
        cave_end: "cave_end.wav",
        fire1: "fire1.wav",
        fire2: "fire2.wav",
        fire3: "fire3.wav",
        fire0: "fire0.wav",
        item_find: "item_find.wav",
        item_add: "item_add.wav",
        craft_success: "craft_success.wav",
        door_library: "door_library.wav",
        star_hint: "star_hint.wav",
        lib_door: "lib_door.wav",
        door_top: "door_top.wav",
        door_basket: "door_basket.wav",
        lose1: "lose1.wav",
        door_spell: "door_spell.wav",
        cristal_up: "cristal_up.wav",
        true_crystal: "true_crystal.wav",
        brain_end: "brain_end.wav",
        level_up: "level_up.wav",
        mistake_crystal: "mistake_crystal.wav",
        start_crystal: "start_crystal.wav",
        applause: "applause.wav",
        flying_ball: "flying_ball.wav",
        catch1: "catch1.wav",
        catch2: "catch2.wav",
        catch3: "catch3.wav",
        catch4: "catch3.wav",
        goal1: "goal1.wav",
        goal2: "goal2.wav",
        goal3: "goal3.wav",
        goal4: "goal4.wav",
        goal5: "goal5.wav",
        goal6: "goal6.wav",
        goal7: "goal7.wav",
        enemy_catch1: "enemy_catch1.wav",
        enemy_catch2: "enemy_catch2.wav",
        enemy_catch3: "enemy_catch3.wav",
        enemy_catch4: "enemy_catch4.wav",
        enemy_goal1: "enemy_goal1.wav",
        enemy_goal2: "enemy_goal2.wav",
        enemy_goal3: "enemy_goal3.wav",
        enemy_goal4: "enemy_goal4.wav",
        win: "win.wav",
        timeout: "timeout.wav",
        swipe_r: "swipe_r.wav",
        swipe_l: "swipe_l.wav",
        fireplace: "fireplace.wav",
        knock_castle: "knock_castle.wav",
        kay_in: "kay_in.wav",
        kay_out: "kay_out.wav",
        on_sound: "on.wav",
    }
except NameError:
    _SOUND_NAME_MAP = {}
    logger.warning("NameError during _SOUND_NAME_MAP creation. Some sounds may not be defined yet.")

#---------читаем файлы записываем в переменные 
f1 = open('1.txt','r')
a1=f1.read(4)
f1.close() 

f5 = open('5.txt','r')
a5=f5.read(4)
f5.close() 
wolfLevel = int(a5)

f6 = open('6.txt','r')
a6=f6.read(4)
f6.close() 
trainLevel = int(a6)

f7 = open('7.txt','r')
a7=f7.read(4)
f7.close() 
suitcaseLevel = int(a7)

f8 = open('8.txt','r')
a8=f8.read(4)
f8.close() 
safeLevel = int(a8)

f2 = open('2.txt','r')
a2=f2.read(4)
f2.close() 

f3 = open('3.txt','r')
a3=f3.read(4)
f3.close() 

f4 = open('4.txt','r')
a4=f4.read(4)
language=int(a4)
f4.close() 

# --- Читаем настройку музыки для Ready (НАДЕЖНЫЙ ПУТЬ) ---
READY_MUSIC_FILE = '/home/pi/New/ready_music.txt'
play_ready_music = True
try:
    if os.path.exists(READY_MUSIC_FILE):
        with open(READY_MUSIC_FILE, 'r') as f:
            play_ready_music = (f.read().strip() == "1")
except Exception:
    pass
# ---------------------------------------------------------

# --- Internet sharing (NAT wlan0 → wlan1) ---
INTERNET_SHARE_FILE = '/home/pi/New/internet_share.txt'
internet_sharing = False
try:
    if os.path.exists(INTERNET_SHARE_FILE):
        with open(INTERNET_SHARE_FILE, 'r') as f:
            internet_sharing = (f.read().strip() == "1")
except Exception:
    pass


def apply_internet_sharing(enable):
    """Управляет раздачей интернета С Castle AP (wlan0) НА wlan1.
    НЕ трогает ip_forward и NAT MASQUERADE — они нужны Tailscale для своих
    функций (subnet routing, exit-node). Также не задевает собственный
    интернет Pi и Tailscale-соединение.

    Принцип: вставляет DROP-правило в начало FORWARD-цепочки строго для
    пути wlan0→wlan1 (Castle clients → outside). Pi-local трафик и
    tailscale0-трафик идут другими маршрутами и не задеваются.

    Когда ON: убираем DROP, default ACCEPT policy пропускает.
    Когда OFF: вставляем DROP перед всем остальным.
    Идемпотентно."""
    import subprocess
    drop_rule = ['FORWARD', '-i', 'wlan0', '-o', 'wlan1', '-j', 'DROP']
    try:
        check = subprocess.run(['sudo', 'iptables', '-C'] + drop_rule,
                               capture_output=True, timeout=3)
        exists = (check.returncode == 0)
        if enable:
            # ON: интернет разрешён — снимаем DROP, если он есть
            while exists:
                subprocess.run(['sudo', 'iptables', '-D'] + drop_rule,
                               check=False, capture_output=True, timeout=3)
                check = subprocess.run(['sudo', 'iptables', '-C'] + drop_rule,
                                       capture_output=True, timeout=3)
                exists = (check.returncode == 0)
        else:
            # OFF: блокируем — вставляем DROP в начало цепочки если ещё нет
            if not exists:
                subprocess.run(['sudo', 'iptables', '-I'] + drop_rule,
                               check=False, capture_output=True, timeout=3)
    except Exception as _e:
        logger.error(f"apply_internet_sharing error: {_e}")

# Применяем восстановленное состояние при старте
try:
    apply_internet_sharing(internet_sharing)
except Exception:
    pass
# ---------------------------------------------------------

channel1.set_volume(float(a1),float(a1))
channel2.set_volume(float(a2),float(a2))
channel3.set_volume(float(a3),float(a3))
#------------дополнительные переменные для звука и флаги
sound = float(a1)
go = 0
starts =0
flagS = 0
voiceLevel = a3
effectLevel = a2
phoneLevel = float(a1)
story_fade_active = False

# Убираем автопоиск и жёстко прописываем порт Arduino
try:
    # Указываем порт, найденный через утилиту serial.tools.list_ports
    ARDUINO_PORT = '/dev/ttyUSB_MAIN'
    ser = serial.Serial(ARDUINO_PORT, 115200, timeout=1)
    logger.info(f"Successfully connected to Arduino on port {ARDUINO_PORT}")
except serial.SerialException as e:
    logger.critical(f"Could not open serial port {ARDUINO_PORT}. Error: {e}")
    logger.critical("HINT: Check connection and port name. Make sure you have permissions (sudo usermod -a -G dialout pi).")
    exit() # Завершаем работу, если не удалось подключиться

# === MEGA BOOT WATCHDOG ===
# При cold power-on квеста ~50% случаев Mega остаётся в halfway state и не
# отправляет QUEST_SYSTEM_READY — клиент слышит тишину, должен вручную ребутить.
# Watchdog: ждём 30 сек после server start. Если QUEST_SYSTEM_READY не пришёл —
# программно дёргаем DTR на ttyUSB (эквивалент нажатия reset кнопки на Mega).
# Этот код срабатывает ОДИН РАЗ при server boot. После QUEST_SYSTEM_READY флаг
# выключается и watchdog бездействует.
mega_initial_boot_received = False

# --- SERIAL BUFFER ACCUMULATOR ---
# Вместо ser.readline() (который может вернуть неполную строку при таймауте),
# мы накапливаем байты в буфере и обрабатываем только полные строки (по \n).
serial_buffer = ""

def serial_read_lines():
    """Читает доступные байты из serial, добавляет в буфер, возвращает список полных строк."""
    global serial_buffer
    lines = []
    try:
        if ser.in_waiting > 0:
            raw = ser.read(ser.in_waiting)
            serial_buffer += raw.decode('utf-8', errors='ignore')
            
            # Разбиваем буфер на строки
            while '\n' in serial_buffer:
                line, serial_buffer = serial_buffer.split('\n', 1)
                line = line.rstrip('\r').strip()
                if line:  # Игнорируем пустые строки
                    lines.append(line)
            
            # Защита от переполнения буфера (если \n так и не пришел)
            if len(serial_buffer) > 500:
                logger.warning(f"Serial buffer overflow ({len(serial_buffer)} chars), flushing: {serial_buffer[:80]}...")
                serial_buffer = ""
    except Exception as e:
        logger.error(f"Serial read error: {e}")
    return lines

# --- ТАБЛИЦА ВОССТАНОВЛЕНИЯ ПОВРЕЖДЕННЫХ КОМАНД ---
# Критические игровые команды, которые НЕ ДОЛЖНЫ быть потеряны из-за corruption.
# Формат: (минимальный_префикс, полная_команда, минимальная_длина_для_совпадения)
CRITICAL_COMMANDS_RECOVERY = [
    # Двери и ключевые переходы
    ("door_owl",       "door_owl"),
    ("door_dog",       "door_dog"),
    ("door_cave",      "door_cave"),
    ("door_witch",     "door_witch"),
    ("open_bank",      "open_bank"),
    ("open_door",      "open_door"),
    ("dog_lock",       "dog_lock"),
    ("dog_sleep",      "dog_sleep"),
    ("dog_growl",      "dog_growl"),
    ("owl_end",        "owl_end"),
    ("owl_flew",       "owl_flew"),
    ("cave_click",     "cave_click"),
    ("cave_end",       "cave_end"),
    ("cave_search1",   "cave_search1"),
    ("cave_search2",   "cave_search2"),
    ("cave_search3",   "cave_search3"),
    ("safe_end",       "safe_end"),
    ("safe_turn",      "safe_turn"),
    ("safe_close",     "safe_close"),
    ("safe_open",      "safe_open"),
    ("miror",          "miror"),
    ("mansard_finish", "mansard_finish"),
    ("flagsendmr",     "flagsendmr"),
    ("four_bottle",    "four_bottle"),
    ("first_bottle",   "first_bottle"),
    ("second_bottle",  "second_bottle"),
    ("third_bottle",   "third_bottle"),
    ("material_end",   "material_end"),
    ("three_game_end", "three_game_end"),
    ("startgo",        "startgo"),
    ("clock1",         "clock1"),
    ("clock2",         "clock2"),
    ("steps",          "steps"),
]

def try_recover_corrupted_command(corrupted_line):
    """
    Пытается восстановить поврежденную команду, сравнивая с таблицей критических команд.
    Используется расстояние Левенштейна (макс. 2 символа отличия) для коротких команд
    и prefix matching для длинных.
    Возвращает восстановленную команду или None.
    """
    # Не пытаемся восстановить логи (они начинаются с "log:")
    if corrupted_line.startswith("log:"):
        return None
    # Не пытаемся восстановить уже известные команды (level_, flag_, hint_, story_, galet)
    if corrupted_line.startswith(("level_", "flag", "hint_", "story_", "galet", "soundon", "soundoff", "light", "dark", "help", "kay")):
        return None
        
    best_match = None
    best_distance = 999
    
    for prefix, full_command in CRITICAL_COMMANDS_RECOVERY:
        # 1. Точное совпадение (быстрый путь)
        if corrupted_line == full_command:
            return None  # Не нужно восстановление
        
        # 2. Prefix match: corrupted содержит начало команды (мин. 70% длины)
        min_len = max(3, int(len(prefix) * 0.7))
        if len(corrupted_line) >= min_len and prefix.startswith(corrupted_line[:min_len]):
            # corrupted_line — это обрезанная версия команды
            dist = len(prefix) - len(corrupted_line)
            if 0 < dist <= 3 and dist < best_distance:
                best_distance = dist
                best_match = full_command
                continue
        
        # 3. Расстояние Левенштейна (для коротких команд, макс. 2 ошибки)
        if abs(len(corrupted_line) - len(prefix)) <= 2:
            d = _levenshtein(corrupted_line, prefix)
            if 0 < d <= 2 and d < best_distance:
                best_distance = d
                best_match = full_command
    
    if best_match:
        logger.warning(f"SERIAL RECOVERY: '{corrupted_line}' → '{best_match}' (distance={best_distance})")
    return best_match

def _levenshtein(s1, s2):
    """Простой расчет расстояния Левенштейна."""
    if len(s1) < len(s2):
        return _levenshtein(s2, s1)
    if len(s2) == 0:
        return len(s1)
    prev_row = range(len(s2) + 1)
    for i, c1 in enumerate(s1):
        curr_row = [i + 1]
        for j, c2 in enumerate(s2):
            insertions = prev_row[j + 1] + 1
            deletions = curr_row[j] + 1
            substitutions = prev_row[j] + (c1 != c2)
            curr_row.append(min(insertions, deletions, substitutions))
        prev_row = curr_row
    return prev_row[-1]

#---конфиг сервера
Payload.max_decode_packets = 200
#async_mode = None  
app = Flask('feedback')
app.secret_key = 'super_secret_quest_key' # Нужно для работы "Запомнить меня"
ADMIN_PASSWORD = 'questquest' # ПАРОЛЬ ОТ СЕТИ CASTLE
app.config['SECRET_KEY'] = 'secret!'
app.static_folder = 'static'
def text_to_wlan1_reset():
    """Принудительная перезагрузка интерфейса wlan1"""
    try:
        logger.info("WI-FI: Выполняем принудительный сброс wlan1...")
        
        # 1. Снимаем системную блокировку Wi-Fi
        subprocess.run("sudo rfkill unblock wifi", shell=True, check=False)
        
        # 2. Мягко опускаем и поднимаем интерфейс через ip link (это надежнее ifconfig)
        subprocess.run("sudo ip link set wlan1 down", shell=True, check=False)
        eventlet.sleep(2)
        subprocess.run("sudo ip link set wlan1 up", shell=True, check=False)
        
        logger.info("WI-FI: Интерфейс wlan1 перезапущен.")
    except Exception as e:
        logger.error(f"WI-FI: Ошибка при сбросе wlan1: {e}")
socketio = SocketIO(app, cors_allowed_origins="*", allow_unsafe_werkzeug=True)

#основной декоратор срабатывает при запросе браузера страницы отправляет наш файл с интерфейсом и все необходимые дополняющие css js icon
#html храниться в папке templates все остальное в папке static
@app.route('/')
def index():
     return send_file('templates/Front.html')

import subprocess
import json
import os
from datetime import datetime

# Файл, куда будем сохранять даты успешных прошивок
FLASH_STATS_FILE = 'flash_stats.json'

# Пути к скетчам для проверки даты их изменения
HEX_PATHS = {
    'dog': '/home/pi/New/Sketches/dog/dog.ino.hex',
    'main': '/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex',
    'owls': '/home/pi/New/Sketches/owls/owls.ino.hex',
    'basket': '/home/pi/New/Sketches/basket3/basket3.ino.hex',
    'workshop': '/home/pi/New/Sketches/workshop/workshop.ino.hex',
    # --- ПУТИ К .bin ДЛЯ ESP32 ---
    'train': '/home/pi/New/Sketches/train/train.ino.bin',
    'chest': '/home/pi/New/Sketches/chest/chest.ino.bin',
    'safe': '/home/pi/New/Sketches/safe/safe.ino.bin',
    'wolf': '/home/pi/New/Sketches/wolf/wolf.ino.bin'
}

@app.route('/tech')
def tech_panel():
    return send_file('templates/Tech.html')

# === Test audio toggle (RU заглушки) ===
# Использование на сборке: монтажник прогоняет квест на коротких .wav файлах
# чтобы за минуты пройти все этапы. После сборки toggle OFF — восстанавливаются
# полные игровые истории.
# Заменяются ТОЛЬКО те файлы, что есть в TEST_AUDIO_DIR. Остальные нетронуты.
GAME_AUDIO_DIR = '/home/pi/New'
# 2026-05-27 ВАЖНО: в /home/pi/New/test_ru/ лежат КОПИИ оригинальных файлов
# (md5 совпадает с игровыми). Настоящие "заглушки" — короткие версии — лежат
# в подпапке 'Заглушки историй'. Только их и надо использовать.
TEST_AUDIO_DIR = '/home/pi/New/test_ru/Заглушки историй'
TEST_AUDIO_BACKUP_DIR = '/home/pi/New/test_ru_backup'

def _test_audio_is_active():
    """В test-mode если backup-папка существует с файлами."""
    return os.path.isdir(TEST_AUDIO_BACKUP_DIR) and any(
        f.endswith('.wav') for f in os.listdir(TEST_AUDIO_BACKUP_DIR)
    )

def _test_audio_enable():
    """Backup current game files → copy test stubs over them."""
    import shutil
    if _test_audio_is_active():
        return {'swapped': 0, 'note': 'already_active'}
    os.makedirs(TEST_AUDIO_BACKUP_DIR, exist_ok=True)
    swapped = 0
    for fname in os.listdir(TEST_AUDIO_DIR):
        if not fname.endswith('.wav'):
            continue
        test_path = os.path.join(TEST_AUDIO_DIR, fname)
        game_path = os.path.join(GAME_AUDIO_DIR, fname)
        if not os.path.isfile(game_path):
            continue  # нет игровой версии — пропускаем
        backup_path = os.path.join(TEST_AUDIO_BACKUP_DIR, fname)
        shutil.move(game_path, backup_path)  # bk оригинал
        shutil.copy2(test_path, game_path)   # ставим заглушку
        swapped += 1
    return {'swapped': swapped}

def _test_audio_disable():
    """Restore game files from backup, delete backup dir."""
    import shutil
    if not _test_audio_is_active():
        return {'restored': 0, 'note': 'not_active'}
    restored = 0
    for fname in os.listdir(TEST_AUDIO_BACKUP_DIR):
        if not fname.endswith('.wav'):
            continue
        backup_path = os.path.join(TEST_AUDIO_BACKUP_DIR, fname)
        game_path = os.path.join(GAME_AUDIO_DIR, fname)
        shutil.move(backup_path, game_path)  # move overwrites test version
        restored += 1
    try:
        shutil.rmtree(TEST_AUDIO_BACKUP_DIR)
    except Exception:
        pass
    return {'restored': restored}

# === 2026-05-27: auto-конвертация WAV к 16-bit PCM ===
# Helper-функция: сканирует GAME_AUDIO_DIR, конвертит не-16-bit через sox.
# Вызывается из handle_audio_check (перед проверкой) и при старте сервера.
# Раньше была отдельная кнопка на /tech, убрали — должно быть автоматически (как WC).
def _auto_convert_audio_bitdepth(emit_progress=False):
    """Возвращает dict: {ok, scanned, converted: [{file, from_bits}], skipped, errors: [{file, error}]}"""
    import wave as wave_module
    import shutil as _sh
    import subprocess as _sp
    if not _sh.which('sox'):
        return {'ok': False, 'error': 'sox not installed (apt install -y sox)',
                'converted': [], 'errors': [], 'scanned': 0, 'skipped': 0}
    scan_dir = GAME_AUDIO_DIR
    scanned = 0
    converted_list = []
    skipped_list = []
    errors_list = []
    try:
        files = sorted([f for f in os.listdir(scan_dir) if f.lower().endswith('.wav')])
    except Exception as e:
        return {'ok': False, 'error': f'list dir failed: {e}',
                'converted': [], 'errors': [], 'scanned': 0, 'skipped': 0}
    total = len(files)
    for i, fname in enumerate(files):
        path = os.path.join(scan_dir, fname)
        try:
            with wave_module.open(path, 'rb') as w:
                sampwidth = w.getsampwidth()
        except Exception as e:
            errors_list.append({'file': fname, 'error': f'wave read: {e}'})
            continue
        scanned += 1
        if sampwidth == 2:
            skipped_list.append(fname)
            continue
        if emit_progress:
            socketio.emit('audio_convert_progress',
                          {'phase': 'scan', 'current': i+1, 'total': total,
                           'file': fname, 'bits': sampwidth*8}, to=None)
        tmp = path + '.16bit.tmp.wav'
        try:
            r = _sp.run(['sox', path, '-b', '16', tmp],
                        capture_output=True, timeout=60, text=True)
            if r.returncode == 0 and os.path.exists(tmp):
                os.replace(tmp, path)
                converted_list.append({'file': fname, 'from_bits': sampwidth*8})
                logger.info(f"[AUDIO-CONVERT] {fname}: {sampwidth*8}-bit → 16-bit")
            else:
                if os.path.exists(tmp):
                    try: os.remove(tmp)
                    except: pass
                errors_list.append({'file': fname, 'error': f'sox rc={r.returncode}: {r.stderr[:200]}'})
        except Exception as e:
            errors_list.append({'file': fname, 'error': f'sox exception: {e}'})
    result = {'ok': True, 'scanned': scanned,
              'converted': converted_list,
              'skipped': len(skipped_list),
              'errors': errors_list}
    if converted_list or errors_list:
        logger.info(f"[AUDIO-CONVERT] Done: scanned={scanned}, converted={len(converted_list)}, "
                    f"skipped={len(skipped_list)}, errors={len(errors_list)}")
    return result

@socketio.on('test_audio_set')
def handle_test_audio_set(data):
    active = bool((data or {}).get('active'))
    try:
        if active:
            result = _test_audio_enable()
            logger.info(f"[TEST-AUDIO] Enabled — swapped {result.get('swapped')} files")
            # Подсчитаем сколько backup-файлов лежит — это и есть кол-во активных stub'ов
            swapped = len([f for f in os.listdir(TEST_AUDIO_BACKUP_DIR) if f.endswith('.wav')]) \
                      if os.path.isdir(TEST_AUDIO_BACKUP_DIR) else 0
            socketio.emit('test_audio_state', {'active': True, 'swapped': swapped}, to=None)
        else:
            result = _test_audio_disable()
            logger.info(f"[TEST-AUDIO] Disabled — restored {result.get('restored')} files")
            socketio.emit('test_audio_state', {'active': False}, to=None)
    except Exception as e:
        logger.error(f"[TEST-AUDIO] Error: {e}")
        socketio.emit('test_audio_error', {'error': str(e)}, to=None)
        # Также шлём текущее реальное состояние
        try:
            swapped = len([f for f in os.listdir(TEST_AUDIO_BACKUP_DIR) if f.endswith('.wav')]) \
                      if os.path.isdir(TEST_AUDIO_BACKUP_DIR) else 0
            socketio.emit('test_audio_state',
                          {'active': _test_audio_is_active(), 'swapped': swapped}, to=None)
        except Exception:
            pass

# --- ПРОВЕРКА АУДИОФАЙЛОВ (с прогрессом через WebSocket) ---
@socketio.on('start_audio_check')
def handle_audio_check():
    def run_check():
        import re
        import wave as wave_module

        # 2026-05-27: автоконвертация 24/32-bit WAV в 16-bit ДО проверки.
        # Раньше была отдельная кнопка, теперь это часть "Проверить аудио"
        # (как WC делает на upload). Эмитит audio_convert_progress/result в UI.
        conv_result = _auto_convert_audio_bitdepth(emit_progress=True)
        socketio.emit('audio_convert_result', conv_result, to=None)

        all_suffixes = ['ru', 'en', 'ar', 'fr', 'uk', 'pl']

        # --- 1. СКАНИРУЕМ СЕРВЕР (CastleServer.py) ---
        try:
            server_file = os.path.abspath(__file__)
            with open(server_file, 'r', encoding='utf-8') as f:
                code = f.read()
        except Exception as e:
            socketio.emit('audio_check_done', {'error': f'Не удалось прочитать файл сервера: {e}'})
            return

        # Убираем docstrings и комментарии, чтобы не подхватить примеры
        code_clean = re.sub(r'""".*?"""', '', code, flags=re.DOTALL)
        code_clean = re.sub(r"'''.*?'''", '', code_clean, flags=re.DOTALL)
        code_clean = re.sub(r'#[^\n]*', '', code_clean)

        # Прямые вызовы play_localized_audio("story_X")
        localized_from_calls = set(re.findall(
            r'play_localized_audio\s*\(\s*["\']([^"\']+)["\']\s*[\),]', code_clean
        ))
        # Строки вида "story_X" в списках (_stories_base = [...])
        localized_from_lists = set(re.findall(
            r'["\']([^"\']*story_[^"\']+)["\']\s*[,\]]', code_clean
        ))
        # Эффекты: pygame.mixer.Sound('file.wav')
        effect_files = sorted(set(re.findall(
            r'pygame\.mixer\.Sound\s*\(\s*["\']([^"\']+\.wav)["\']\s*\)', code_clean
        )))

        # --- 2. СКАНИРУЕМ .INO ФАЙЛЫ для hint_* команд ---
        # hint_X приходят с Arduino как Serial-команды и обрабатываются через
        # play_localized_audio(flag) — где flag переменная, регулярки её не видят.
        ino_paths = [
            '/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino',
            '/home/pi/New/Sketches/basket3/basket3.ino',
            '/home/pi/New/Sketches/workshop/workshop.ino',
            '/home/pi/New/Sketches/owls/owls.ino',
            '/home/pi/New/Sketches/dog/dog.ino',
        ]
        hint_bases_from_ino = set()
        for ino_path in ino_paths:
            try:
                with open(ino_path, 'r', encoding='utf-8', errors='ignore') as f:
                    ino_code = f.read()
                # Ищем все "hint_X" строки — это то что Arduino отправляет по Serial
                found = re.findall(r'"(hint_[^"]+)"', ino_code)
                hint_bases_from_ino.update(found)
            except Exception:
                pass  # Файл не найден — пропускаем молча

        # --- 3. ОБЪЕДИНЯЕМ ВСЕ БАЗОВЫЕ ИМЕНА ---
        localized_story_bases = sorted(
            localized_from_calls | localized_from_lists | hint_bases_from_ino
        )

        # --- 4. ФУНКЦИЯ ПРОВЕРКИ ФАЙЛА ---
        def check_wav_file(filename):
            size = os.path.getsize(filename)
            if size < 44:
                return False, f"Файл слишком мал: {size} байт"

            # Проверка 1: raw-заголовок на audioFormat.
            # pygame воспроизводит ТОЛЬКО PCM (audioFormat=1).
            # wave.open() иногда открывает non-PCM без ошибки, но pygame падает.
            # Именно так ломался hint_6_b_pl.wav — заголовок валидный, pygame не мог открыть.
            try:
                with open(filename, 'rb') as f:
                    raw = f.read(36)
                if len(raw) >= 22:
                    audio_format = int.from_bytes(raw[20:22], 'little')
                    if audio_format != 1:
                        return False, f"Не-PCM формат: audioFormat={audio_format:#06x} (pygame воспроизводит только PCM=1)"
            except Exception as e:
                return False, f"Ошибка чтения заголовка: {e}"

            # Проверка 2: открываем через wave и читаем реальные данные.
            # Ловит файлы с валидным заголовком но повреждёнными данными внутри.
            try:
                with wave_module.open(filename, 'rb') as wf:
                    channels = wf.getnchannels()
                    framerate = wf.getframerate()
                    sampwidth = wf.getsampwidth()
                    nframes = wf.getnframes()
                    if channels < 1 or channels > 2:
                        return False, f"Неверное число каналов: {channels}"
                    if framerate < 8000 or framerate > 192000:
                        return False, f"Неверная частота: {framerate} Hz"
                    if sampwidth not in (1, 2, 3, 4):
                        return False, f"Неверная битность: {sampwidth * 8} бит"
                    if nframes == 0:
                        return False, "Нет аудиоданных (0 фреймов)"
                    # Читаем первые реальные сэмплы — ловит обрезанные/повреждённые файлы
                    data = wf.readframes(min(nframes, 4096))
                    if len(data) == 0:
                        return False, "Не удалось прочитать аудиоданные"
                return True, ""
            except wave_module.Error as e:
                return False, f"Ошибка WAV: {e}"
            except Exception as e:
                return False, str(e)

        # --- 5. СОБИРАЕМ ПОЛНЫЙ СПИСОК ФАЙЛОВ ---
        all_files = []
        for base in localized_story_bases:
            for suffix in all_suffixes:
                all_files.append((f"{base}_{suffix}.wav", "localized"))
        for fn in effect_files:
            all_files.append((fn, "effect"))

        total = len(all_files)
        ok_count = 0
        missing = []
        broken = []

        socketio.emit('audio_check_start', {'total': total})

        for i, (filename, ftype) in enumerate(all_files):
            if not os.path.exists(filename):
                missing.append(filename)
                socketio.emit('audio_check_progress', {
                    'current': i + 1, 'total': total,
                    'file': filename, 'status': 'missing'
                })
            else:
                ok, err = check_wav_file(filename)
                if ok:
                    ok_count += 1
                    socketio.emit('audio_check_progress', {
                        'current': i + 1, 'total': total,
                        'file': filename, 'status': 'ok'
                    })
                else:
                    broken.append({'file': filename, 'error': err})
                    socketio.emit('audio_check_progress', {
                        'current': i + 1, 'total': total,
                        'file': filename, 'status': 'broken', 'error': err
                    })
            eventlet.sleep(0)

        socketio.emit('audio_check_done', {
            'total': total,
            'ok': ok_count,
            'missing': len(missing),
            'broken': len(broken),
            'missing_files': missing,
            'broken_files': broken
        })

    socketio.start_background_task(target=run_check)

# --- ЗАПРОС СТАТИСТИКИ (Даты изменения и последних прошивок) ---
@socketio.on('get_board_stats')
def handle_get_stats():
    stats = {}
    # Читаем сохраненные даты успешных прошивок
    last_flashes = {}
    if os.path.exists(FLASH_STATS_FILE):
        try:
            with open(FLASH_STATS_FILE, 'r') as f:
                last_flashes = json.load(f)
        except: pass

    # Собираем данные по каждой плате
    for board, path in HEX_PATHS.items():
        mtime_str = "Файл не найден"
        if os.path.exists(path):
            mtime = os.path.getmtime(path)
            mtime_str = datetime.fromtimestamp(mtime).strftime('%d.%m.%Y:%H:%M')
        
        stats[board] = {
            'mtime': mtime_str,
            'last_flash': last_flashes.get(board, "Никогда")
        }
    socketio.emit('board_stats_data', stats)

# --- ПРОШИВКА ---
@socketio.on('start_flash')
def handle_flash(board_id):
    global is_system_flashing
    global ser

    # Dog: специальный 3-шаговый flow (см. ниже после commands)
    if board_id == 'dog':
        return _handle_dog_flash()

    commands = {
        'main': 'sudo avrdude -v -p atmega2560 -c wiring -P /dev/ttyUSB_MAIN -b 115200 -D -U flash:w:/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex:i',
        'owls': 'sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.1:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/owls/owls.ino.hex:i',
        'basket': 'sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.3:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/basket3/basket3.ino.hex:i',
        'workshop': 'sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.4:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/workshop/workshop.ino.hex:i',
        'train': 'cd /home/pi/New && python3 espota.py -i 192.168.4.202 -p 3232 --host_ip 192.168.4.1 -f /home/pi/New/Sketches/train/train.ino.bin',
        'chest': 'cd /home/pi/New && python3 espota.py -i 192.168.4.203 -p 3232 --host_ip 192.168.4.1 -f /home/pi/New/Sketches/chest/chest.ino.bin',
        'safe': 'cd /home/pi/New && python3 espota.py -i 192.168.4.204 -p 3232 --host_ip 192.168.4.1 -f /home/pi/New/Sketches/safe/safe.ino.bin',
        'wolf': 'cd /home/pi/New && python3 espota.py -i 192.168.4.201 -p 3232 --host_ip 192.168.4.1 -f /home/pi/New/Sketches/wolf/wolf.ino.bin'
    }

    if board_id not in commands: return
    cmd = commands[board_id]

    try:
        # 1. Освобождение портов
        # Башни Mega (workshop/basket/dog) делят UART-линии с USB-CH340. Если Mega
        # шлёт "ping_main" каждые 5с, она ломает STK500-протокол avrdude → sync
        # никогда не происходит → флэш висит вечно. Перед прошивкой говорим Mega
        # перевести её TX/RX в Z-state. Owls на SoftwareSerial — пока работает без.
        #
        _release_map = {'workshop': 'serial1', 'basket': 'serial2'}
        if board_id in _release_map:
            sn = _release_map[board_id]
            socketio.emit('flash_log', {'board': board_id, 'msg': f'Отправка команды release_{sn} на Главную плату...\n'})
            serial_write_queue.put(f'release_{sn}')
            eventlet.sleep(1.5)
        elif board_id == 'main':
            is_system_flashing = True
            eventlet.sleep(1)
            try: ser.close()
            except: pass

        socketio.emit('flash_log', {'board': board_id, 'msg': 'Запуск прошивки (это может занять около минуты)...\n'})
        
        # 2. Выполнение консольной команды
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        out_msg = result.stdout + '\n' + result.stderr
        
        # Печатаем лог в терминал
        socketio.emit('flash_log', {'board': board_id, 'msg': out_msg})
        
        # Печатаем скрытый системный код ответа (0 - успех, всё остальное - ошибка)
        socketio.emit('flash_log', {'board': board_id, 'msg': f'\n[DEBUG] Системный код завершения: {result.returncode}\n'})
        
        # 3. ИДЕАЛЬНАЯ ПРОВЕРКА НА УСПЕХ
        lower_out = out_msg.lower()
        is_success = False
        
        # Разделяем логику успеха для кабеля и воздуха
        if board_id in ['main', 'dog', 'owls', 'basket', 'workshop']:
            # Для Arduino (avrdude) - ищем фразу о проверке памяти
            if "bytes of flash verified" in lower_out:
                is_success = True
        else:
            # Для ESP32 (espota.py) - верим нулевому коду возврата системы, 
            # даже если скрипт не успел написать "Success" из-за ребута платы
            if result.returncode == 0:
                is_success = True
            elif "result: ok" in lower_out or "success" in lower_out:
                is_success = True

        # 4. Обработка результатов
        if is_success:
            now_str = datetime.now().strftime('%d.%m.%Y %H:%M')
            flashes = {}
            if os.path.exists(FLASH_STATS_FILE):
                with open(FLASH_STATS_FILE, 'r') as f: flashes = json.load(f)
            flashes[board_id] = now_str
            with open(FLASH_STATS_FILE, 'w') as f: json.dump(flashes, f)
            
            socketio.emit('flash_success', {'board': board_id, 'time': now_str})
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[SYSTEM] ОШИБОК НЕ ОБНАРУЖЕНО. Прошивка успешна!\n'})
        else:
            socketio.emit('flash_failed', {'board': board_id})
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[SYSTEM] ВНИМАНИЕ! Обнаружена ошибка. Плата НЕ прошилась!\n'})

        # 5. Восстановление портов
        if board_id in _release_map:
            sn = _release_map[board_id]
            socketio.emit('flash_log', {'board': board_id, 'msg': f'Восстановление связи (restore_{sn})...'})
            serial_write_queue.put(f'restore_{sn}')
            eventlet.sleep(0.5)
            socketio.emit('flash_log', {'board': board_id, 'msg': '\nСвязь успешно восстановлена! [END]'})
        elif board_id == 'main':
            try: ser.open()
            except: pass
            is_system_flashing = False
            socketio.emit('flash_log', {'board': board_id, 'msg': '\nПерезапускаю сервер для гарантированного восстановления связи... [END]'})
            # После прошивки Main Board часто остаётся "stale fd" на ttyUSB_MAIN —
            # ser.open() формально успешен, но реально пакеты уходят в пустоту.
            # Mega → ничего не получает → Restart с пульта не работает физически.
            # Самый надёжный способ — полный рестарт процесса через systemd.
            eventlet.sleep(1)  # дать UI время показать сообщение
            logger.warning("[FLASH] Main Board flashed — auto-restart of castleserver to clear stale serial fd")
            import signal  # os уже импортирован глобально (не делать import os здесь —
                           # иначе Python считает os локальной во ВСЕЙ функции и падает
                           # на os.path.exists(FLASH_STATS_FILE) выше при прошивке не-main плат)
            os.kill(os.getpid(), signal.SIGTERM)  # systemd рестартанёт через Restart=on-failure / always
        else:
            socketio.emit('flash_log', {'board': board_id, 'msg': '\n[END]'})

    except Exception as e:
        socketio.emit('flash_failed', {'board': board_id})
        socketio.emit('flash_log', {'board': board_id, 'msg': f'\n[SYSTEM] КРИТИЧЕСКАЯ ОШИБКА: {str(e)} [END]'})
        is_system_flashing = False
        try: ser.open()
        except: pass


def _handle_dog_flash():
    """3-шаговая прошивка Dog (Nano на DMC_TOP):
    1. Mega ← silence.ino (Mega становится "немой" на Serial3 → освобождает RX/TX к Nano)
    2. Nano ← dog.ino (через USB-CH340 на хабе)
    3. Mega ← MAIN_BOARD_V5_COM5.ino (восстанавливаем игровую прошивку)

    Почему так: на DMC_TOP линии RX/TX делятся между Mega и Nano. На скорости
    115200 STK500-sync ломается из-за LED+R+capacitance даже когда Mega пины
    в high-Z через release_serial3. silence.ino полностью убирает Mega-firmware
    как источник сигнала на этих линиях — Mega после flash сидит в setup() без
    активной UART → линия чистая → avrdude синхронизируется."""
    global is_system_flashing, ser
    SILENCE_HEX = '/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/silence.ino.hex'
    MEGA_HEX    = '/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex'
    DOG_HEX     = '/home/pi/New/Sketches/dog/dog.ino.hex'
    DOG_PORT    = '/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.2:1.0-port0'

    avrdude_mega = lambda hex_path: (
        f'sudo avrdude -v -p atmega2560 -c wiring -P /dev/ttyUSB_MAIN '
        f'-b 115200 -D -U flash:w:{hex_path}:i'
    )
    avrdude_dog = (
        f'sudo avrdude -v -p atmega328p -c arduino -P {DOG_PORT} '
        f'-b 115200 -D -U flash:w:{DOG_HEX}:i'
    )

    def _run(cmd, label):
        socketio.emit('flash_log', {'board': 'dog', 'msg': f'\n[{label}] {cmd}\n\n'})
        r = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        out = (r.stdout or '') + '\n' + (r.stderr or '')
        socketio.emit('flash_log', {'board': 'dog', 'msg': out})
        socketio.emit('flash_log', {'board': 'dog', 'msg': f'\n[{label}] rc={r.returncode}\n'})
        ok = ('bytes of flash verified' in out.lower())
        return ok, r.returncode

    try:
        # --- Шаг 1: silence Mega ---
        socketio.emit('flash_log', {'board': 'dog', 'msg': '=== ШАГ 1/3: Прошиваем silence.ino на Mega (отключает Mega от Nano) ===\n'})
        is_system_flashing = True
        eventlet.sleep(1)
        try: ser.close()
        except: pass
        ok1, _ = _run(avrdude_mega(SILENCE_HEX), 'Шаг 1: silence → Mega')
        if not ok1:
            socketio.emit('flash_failed', {'board': 'dog'})
            socketio.emit('flash_log', {'board': 'dog', 'msg': '\n[SYSTEM] ШАГ 1 НЕ УДАЛСЯ: silence не прошился на Mega. Mega может быть в неопределённом состоянии — перезагрузи квест! [END]\n'})
            try: ser.open()
            except: pass
            is_system_flashing = False
            return
        socketio.emit('flash_log', {'board': 'dog', 'msg': '\n✅ silence на Mega — Mega теперь молчит на Serial3.\n'})

        # --- Шаг 2: dog ---
        socketio.emit('flash_log', {'board': 'dog', 'msg': '\n=== ШАГ 2/3: Прошиваем dog.ino на Nano (через USB-CH340) ===\n'})
        eventlet.sleep(1)
        ok2, _ = _run(avrdude_dog, 'Шаг 2: dog → Nano')
        if not ok2:
            socketio.emit('flash_log', {'board': 'dog', 'msg': '\n⚠️  ШАГ 2 НЕ УДАЛСЯ: dog не прошился. Восстанавливаю Mega...\n'})
            # Не возвращаемся — попробуем хоть Mega вернуть в рабочее состояние
        else:
            socketio.emit('flash_log', {'board': 'dog', 'msg': '\n✅ dog на Nano прошит.\n'})

        # --- Шаг 3: восстановить Mega ---
        socketio.emit('flash_log', {'board': 'dog', 'msg': '\n=== ШАГ 3/3: Восстанавливаем MAIN_BOARD на Mega ===\n'})
        eventlet.sleep(1)
        ok3, _ = _run(avrdude_mega(MEGA_HEX), 'Шаг 3: MAIN_BOARD → Mega')

        # Финал
        if ok1 and ok2 and ok3:
            now_str = datetime.now().strftime('%d.%m.%Y %H:%M')
            flashes = {}
            if os.path.exists(FLASH_STATS_FILE):
                with open(FLASH_STATS_FILE, 'r') as f: flashes = json.load(f)
            flashes['dog'] = now_str
            with open(FLASH_STATS_FILE, 'w') as f: json.dump(flashes, f)
            socketio.emit('flash_success', {'board': 'dog', 'time': now_str})
            socketio.emit('flash_log', {'board': 'dog', 'msg': '\n[SYSTEM] ✅ Все 3 шага прошли успешно! Dog обновлён.\n'})
        else:
            socketio.emit('flash_failed', {'board': 'dog'})
            socketio.emit('flash_log', {'board': 'dog', 'msg': f'\n[SYSTEM] ⚠️ silence={ok1} dog={ok2} mega_restore={ok3}\n'})

        # После flash Mega — нужен рестарт сервера для свежего fd (см. commit 7cb898d).
        # os._exit(0) вместо SIGTERM: SIGTERM иногда не доходит в eventlet greenlet
        # (особенно когда вложенная функция вызвана из @socketio.on handler).
        # _exit — bypass всех handlers, immediate exit. systemd Restart=always
        # подхватит и поднимет свежий процесс.
        socketio.emit('flash_log', {'board': 'dog', 'msg': '\nПерезапускаю сервер для гарантированного восстановления связи... [END]\n'})
        eventlet.sleep(1)
        logger.warning("[FLASH] Dog flashed (3-step via silence) — auto-restart of castleserver")
        os._exit(0)

    except Exception as e:
        socketio.emit('flash_failed', {'board': 'dog'})
        socketio.emit('flash_log', {'board': 'dog', 'msg': f'\n[SYSTEM] КРИТИЧЕСКАЯ ОШИБКА: {str(e)} [END]\n'})
        is_system_flashing = False
        try: ser.open()
        except: pass


# --- ЧТЕНИЕ ЛОГОВ (Эмулятор tail -f на чистом Python) ---
live_log_active = False

@socketio.on('start_live_logs')
def start_live_logs():
    global live_log_active
    if live_log_active: 
        return
    live_log_active = True
    
    def log_reader():
        # Путь к файлу (относительный, такой же, как при создании логгера)
        log_path = 'logs/castle.log' 
        
        # 1. Сначала выводим последние 30 строк для контекста
        try:
            with open(log_path, 'r', encoding='utf-8') as f:
                lines = f.readlines()
                last_lines = lines[-30:] if len(lines) > 30 else lines
                for line in last_lines:
                    socketio.emit('live_log_data', line)
        except Exception as e:
            socketio.emit('live_log_data', f"[Ошибка чтения истории логов: {e}]\n")

        # 2. Теперь начинаем следить за концом файла в реальном времени
        try:
            with open(log_path, 'r', encoding='utf-8') as f:
                f.seek(0, 2) # Прыгаем в самый конец файла
                while live_log_active:
                    line = f.readline()
                    if not line:
                        # Если новых строк нет, ждем полсекунды и проверяем снова
                        eventlet.sleep(0.5) 
                        continue
                    # Если появилась новая строка, сразу отправляем в браузер
                    socketio.emit('live_log_data', line)
        except Exception as e:
            if live_log_active:
                socketio.emit('live_log_data', f"\n[Ошибка мониторинга файла: {e}]\n")
                
    # Запускаем чтение в фоновом потоке, чтобы не тормозить сервер
    socketio.start_background_task(target=log_reader)

@socketio.on('stop_live_logs')
def stop_live_logs():
    global live_log_active
    live_log_active = False

# --- ПЕРЕЗАПУСК СЕРВЕРА ---
@socketio.on('tech_restart_server')
def tech_restart_server():
    def restart():
        eventlet.sleep(1) # Даем секунду, чтобы кнопка в браузере успела отжаться
        import sys
        # Идеальный перезапуск скрипта в той же среде
        os.chdir('/home/pi/New')
        os.execv(sys.executable, ['python3', 'CastleServer.py'])
        
    socketio.start_background_task(target=restart)

@socketio.on('tech_jump_basket')
def tech_jump_basket():
    global go, starts, socklist, level_18_intro_played
    global storyBasketFlag, catchCount, goalCount, enemyGoalCount, timeoutCount, basket_timeout_timer
    
    # --- ИСПРАВЛЕНИЕ: ЖЕСТКИЙ СБРОС ВСЕХ ФОНОВЫХ ПРОЦЕССОВ ПИТОНА ---
    global lesson_intro_seq, current_seq
    lesson_intro_seq += 1 # Убиваем все старые фоновые задачи (салют, голос)
    
    # Очищаем очередь команд, чтобы старые (например, от Мастерской) не вылезли
    while not serial_write_queue.empty():
        try: serial_write_queue.get_nowait()
        except: pass
    # -----------------------------------------------------------------

    logger.info("TECH COMMAND: Jumping to Basket (Level 18)")
    level_18_intro_played = False # <--- СБРАСЫВАЕМ ПАМЯТЬ УРОКА
    storyBasketFlag = 0  
    catchCount = 0       
    goalCount = 0        
    enemyGoalCount = 0   
    timeoutCount = 0     
    if basket_timeout_timer:         
        basket_timeout_timer.cancel()
        basket_timeout_timer = None
    
    # Переводим сервер в активный режим
    go = 1
    starts = 1
    
    # Подготавливаем интерфейс
    socklist.clear()
    socklist.append('start_game')
    socklist.append('active_basket')
    socketio.emit('level', 'start_game', to=None)
    socketio.emit('level', 'active_basket', to=None)
    
    # Гасим текущие звуки рестарта/ожидания
    pygame.mixer.music.stop()
    stop_all_effects()
    channel3.stop() # Глушим любые истории, которые могли застрять

    # Отправляем спец-команду на Ардуино
    for _ in range(3):
            serial_write_queue.put('jump18')
            eventlet.sleep(0.15) 
    
    # Усыпляем ESP32, чтобы они не шумели стартовой музыкой
    send_esp32_command(ESP32_API_WOLF_URL, "day_off")
    send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
    send_esp32_command(ESP32_API_SAFE_URL, "day_off")
    # Переводим карту (Поезд) в предфинальное состояние, чтобы выключить проектор
    send_esp32_command(ESP32_API_TRAIN_URL, "stage_12")

# === DIAG: тех-диагностика устройств (вкладка /tech → "Диагностика") ===
# Workflow: оператор включает diag_on → ESP32 замораживает игру и шлёт snapshot
# I/O каждые 200ms на /api?device=<id>. Сервер ретранслирует через socketio как
# '<device>_diag_state'. Toggle выходов идут через tech_diag_command.
def _diag_send(device, action, body=None):
    """Транспорт-агностичная отправка diag-команды.
    Для ESP32-устройств (train/wolf/suitcases/safe) — HTTP POST на /data.
    Для main/towers — пишет UPPERCASE строку в serial_write_queue (Mega поймает).
    action: 'on' | 'off' | <cmd-string-already-formatted>"""
    if device in DIAG_SERIAL_DEVICES:
        # Сериализуем как: DIAG_ON:<device>, DIAG_OFF:<device>, DIAG_SET:<device>:<key>:<val>
        if action == 'on':
            line = f"DIAG_ON:{device}"
        elif action == 'off':
            line = f"DIAG_OFF:{device}"
        else:  # action содержит уже "diag_set:key:val" (lowercase от UI) — конвертим
            # Ожидаемый формат от UI: "diag_set:KEY:VAL". Переписываем в uppercase prefix.
            if action.startswith("diag_set:"):
                rest = action[len("diag_set:"):]
                line = f"DIAG_SET:{device}:{rest}"
            else:
                logger.warning(f"[DIAG] Unknown serial action: {action}")
                return
        serial_write_queue.put(line)
        return
    # ESP32 path
    url = DIAG_DEVICE_URLS.get(device)
    if not url:
        logger.warning(f"[DIAG] Unknown device: {device}")
        return
    cmd = action if action in ('diag_on', 'diag_off') else (
        'diag_on' if action == 'on' else ('diag_off' if action == 'off' else action)
    )
    send_esp32_command(url, cmd, debounce=False)

# 2026-05-27: глобальный set активных DIAG-устройств. Каждый раз при изменении
# рассылается событие `diag_active_set` всем клиентам, чтобы Front.html
# (игровой пульт) мог блокировать game-control кнопки.
_active_diag_devices = set()

def _broadcast_diag_state():
    socketio.emit('diag_active_set', {'devices': sorted(_active_diag_devices)}, to=None)

@socketio.on('tech_diag_enter')
def tech_diag_enter(data):
    device = (data or {}).get('device')
    if not device:
        return
    logger.info(f"[DIAG] Entering diag mode for {device}")
    _diag_send(device, 'on')
    _active_diag_devices.add(device)
    _broadcast_diag_state()

@socketio.on('tech_diag_exit')
def tech_diag_exit(data):
    device = (data or {}).get('device')
    if not device:
        return
    logger.info(f"[DIAG] Exiting diag mode for {device}")
    _diag_send(device, 'off')
    _active_diag_devices.discard(device)
    _broadcast_diag_state()

@socketio.on('tech_send_serial')
def tech_send_serial(data):
    """Отправка существующей game-команды на Mega через serial_write_queue.
    Используется новым (read-only-friendly) диаг-пультом для Main+башен.
    Whitelist допустимых команд — чтобы из UI нельзя было послать что попало."""
    cmd = (data or {}).get('cmd', '').strip()
    if not cmd:
        return
    SAFE_DIAG_CMDS = {
        # Двери Main Board
        'open_mansard_door', 'open_crime_door', 'open_bank_door',
        'open_potion_door', 'open_low_tower_door', 'open_high_tower_door',
        'open_library_door', 'open_safe_door', 'open_memory_door',
        # Двери башен (Mega транслирует на нужный Serial)
        'open_owl_door', 'open_dog_door', 'open_workshop_door',
        'open_basket_door', 'open_mine_door',
        # Сервисные
        'check_towers',
    }
    if cmd not in SAFE_DIAG_CMDS:
        logger.warning(f"[TECH-DIAG] rejected non-whitelisted cmd: {cmd}")
        return
    logger.info(f"[TECH-DIAG] sending {cmd} via Mega serial")
    serial_write_queue.put(cmd)

@socketio.on('tech_diag_command')
def tech_diag_command(data):
    """Прозрачный прокси команд от UI к устройству в diag-режиме.
    data = {'device': 'train', 'cmd': 'diag_set:tunnel_led:1'}"""
    device = (data or {}).get('device')
    cmd = (data or {}).get('cmd')
    if not device or not cmd:
        logger.warning(f"[DIAG] Bad diag_command: device={device} cmd={cmd}")
        return
    logger.debug(f"[DIAG] {device}: {cmd}")
    _diag_send(device, cmd)

@socketio.on('tech_diag_log_request')
def tech_diag_log_request(data):
    """Отдаёт последние N строк snapshot-лога для пост-мортем анализа.
    data = {'device': 'train', 'lines': 200}"""
    device = (data or {}).get('device', 'train')
    n = int((data or {}).get('lines', 200))
    n = max(1, min(n, 5000))
    try:
        fname = os.path.join(DIAG_LOG_DIR, f"{device}.log")
        if not os.path.exists(fname):
            socketio.emit('tech_diag_log_data', {'device': device, 'lines': []})
            return
        with open(fname, 'r') as f:
            all_lines = f.readlines()
        socketio.emit('tech_diag_log_data',
                      {'device': device, 'lines': [l.rstrip('\n') for l in all_lines[-n:]]})
    except Exception as e:
        logger.warning(f"[DIAG] log read failed: {e}")
        socketio.emit('tech_diag_log_data', {'device': device, 'lines': [f"[ERROR] {e}"]})

#декоратор работы socket отвечает за настройки wifi
@socketio.on('WLAN')
def WLAN(ssid):
     s = ssid
     pos = 2
     try:
         with open('/etc/hostapd/hostapd.conf', 'r') as f:
             L = f.readlines()
             
         if (pos >= 0) and (pos < len(L)):
             if (pos==len(L)-1):
                 L[pos] = s
             else:
                 L[pos] = s + '\n'
                 
         # Записываем во временный файл
         with open('/tmp/hostapd.conf', 'w') as f:
             for line in L:
                 f.write(line)
                 
         # Переносим с правами root
         subprocess.run(['sudo', 'mv', '/tmp/hostapd.conf', '/etc/hostapd/hostapd.conf'], check=True)
         subprocess.run(['sudo', 'chown', 'root:root', '/etc/hostapd/hostapd.conf'], check=False)
         
         # Перезагружаемся после всего
         os.system("sudo reboot")
     except Exception as e:
         print(f"WLAN change error: {e}")


@socketio.on('connect')
def handle_connect():
    global current_client_sid
    global socklist
    # Отправка предупреждения о некорректном выключении ---
    global show_improper_shutdown_warning
    if show_improper_shutdown_warning:
        # Отправляем команду на показ модального окна ТОЛЬКО этому клиенту
        socketio.emit('show_shutdown_warning', to=request.sid)
        show_improper_shutdown_warning = False
        # Мы НЕ сбрасываем флаг. Он останется активным,
        # пока сервер не будет перезапущен ПОСЛЕ корректного выключения.
    current_client_sid = request.sid
    socketio.emit('volume', str(phoneLevel))
    socketio.emit('volume1', str(effectLevel))
    socketio.emit('volume2', str(voiceLevel))
    socketio.emit('wolf', str(wolfLevel))
    socketio.emit('platform', str(trainLevel))
    socketio.emit('suitcases', str(suitcaseLevel))
    socketio.emit('safe', str(safeLevel))
    socketio.emit('ready_music_state', play_ready_music, to=request.sid)
    socketio.emit('internet_state', internet_sharing, to=request.sid)
    # 2026-05-27: текущее состояние DIAG-устройств для свежеподключённого клиента
    socketio.emit('diag_active_set', {'devices': sorted(_active_diag_devices)}, to=request.sid)
    # 2026-05-27: текущее состояние test-audio toggle (могло быть включено до подключения)
    try:
        swapped = len([f for f in os.listdir(TEST_AUDIO_BACKUP_DIR) if f.endswith('.wav')]) \
                  if os.path.isdir(TEST_AUDIO_BACKUP_DIR) else 0
        socketio.emit('test_audio_state',
                      {'active': _test_audio_is_active(), 'swapped': swapped}, to=request.sid)
    except Exception as e:
        logger.warning(f"[TEST-AUDIO] sync on connect failed: {e}")
    # 2026-05-27: текущее состояние Bluetooth toggle (фикс: ранее не синкалось → залипало OFF)
    socketio.emit('bt_state', bluetooth_active, to=request.sid)
    # Отправка всей истории новому клиенту ---
    # Отправляем один раз при подключении, чтобы 'синхронизировать' клиента
    # Мы отправляем только этому 'sid', чтобы не спамить других
    # logger.debug(f"New client connected ({request.sid}). Sending full state history ({len(socklist)} items).")
    for i in socklist:
        socketio.emit('level', i, to=request.sid)
    # Отправляем текущий язык новому клиенту, чтобы пульт показал выбранный язык
    lang_map = {1: 'russian', 2: 'english', 3: 'arabian', 4: 'french', 5: 'ukrainian', 6: 'polish'}
    if language in lang_map:
        socketio.emit('level', lang_map[language], to=request.sid)

# --- WI-FI LOGIC ---
# --- WI-FI LOGIC (PERSISTENT) ---

@socketio.on('connect_wifi')
def handle_wifi_connect(data):
    ssid = data.get('ssid')
    password = data.get('password')
    # Создаем флаг "Интернет разрешен"
    subprocess.run(['touch', '/home/pi/wifi_enabled'], check=False)
    print(f"Wi-Fi Request: Connecting to {ssid} on wlan1...")

    # 1. Сразу меняем статус на "Connecting..."
    socketio.emit('wifi_status', {'status': 'connecting', 'message': 'Connecting...', 'ssid': ssid})
    
    config_text = f"""ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=RU

network={{
    ssid="{ssid}"
    psk="{password}"
    key_mgmt=WPA-PSK
    priority=1
    scan_ssid=1
}}
"""
    try:
        # 2. Записываем настройки
        with open('/tmp/wpa_supplicant.conf', 'w') as f:
            f.write(config_text)
            
        subprocess.run(['sudo', 'mv', '/tmp/wpa_supplicant.conf', '/etc/wpa_supplicant/wpa_supplicant.conf'], check=True)
        subprocess.run(['sudo', 'chown', 'root:root', '/etc/wpa_supplicant/wpa_supplicant.conf'], check=False)
        
        # 3. Перезапускаем интерфейс
        text_to_wlan1_reset()
        
        # 4. Перезапускаем демона dhcpcd (он управляет wpa_supplicant на Raspberry)
        subprocess.run(['sudo', 'systemctl', 'restart', 'dhcpcd'], check=False)
        
        # ДАЕМ ПАУЗУ, чтобы служба поднялась и инициализировала адаптер
        eventlet.sleep(5)
        
        # 5. ЦИКЛ ОЖИДАНИЯ (ждем до 30 секунд, проверяя соединение)
        connected = False
        for i in range(30):
            eventlet.sleep(1)
            try:
                # Проверяем SSID
                ssid_bytes = subprocess.check_output("iwgetid -r wlan1", shell=True)
                current_ssid = ssid_bytes.decode('utf-8').strip()
                
                if current_ssid == ssid:
                    # Если подключились, пытаемся узнать IP
                    ip_addr = "Wait IP..."
                    try:
                        ip_cmd = "ip -4 addr show wlan1 | grep -oP '(?<=inet\s)\d+(\.\d+){3}'"
                        ip_bytes = subprocess.check_output(ip_cmd, shell=True)
                        ip_addr = ip_bytes.decode('utf-8').strip()
                    except:
                        pass

                    socketio.emit('wifi_status', {
                        'status': 'success', 
                        'message': f'Connected: {current_ssid} ({ip_addr})', 
                        'ssid': current_ssid,
                        'ip': ip_addr
                    })
                    print(f"Wi-Fi Success: {current_ssid} at {ip_addr}")
                    connected = True
                    break
            except:
                pass

        # 6. Если за 15 секунд так и не подключились
        if not connected:
            socketio.emit('wifi_status', {'status': 'failed', 'message': 'Not connected (Time out)', 'ssid': ''})
            print("Wi-Fi Fail: Timeout")
        
    except Exception as e:
        print(f"Wi-Fi Connect Error: {e}")
        socketio.emit('wifi_status', {'status': 'error', 'message': f'Error: {e}', 'ssid': ''})

@socketio.on('disconnect_wifi')
def handle_wifi_disconnect():
    print("Disabling Wi-Fi connection on wlan1...")
    # Удаляем флаг "Интернет разрешен"
    subprocess.run(['rm', '-f', '/home/pi/wifi_enabled'], check=False)
    try:
        # Очищаем файл настроек
        empty_conf = """ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
country=RU
"""
        # Обход прав через временный файл и sudo mv
        with open('/tmp/wpa_supplicant.conf', 'w') as f:
            f.write(empty_conf)
            
        subprocess.run(['sudo', 'mv', '/tmp/wpa_supplicant.conf', '/etc/wpa_supplicant/wpa_supplicant.conf'], check=True)
        subprocess.run(['sudo', 'chown', 'root:root', '/etc/wpa_supplicant/wpa_supplicant.conf'], check=False)
            
        # Применяем пустоту (это отключит текущую сеть)
        subprocess.run(['sudo', 'wpa_cli', '-i', 'wlan1', 'reconfigure'], check=True)
        
        # Отправляем статус "Disconnected" и очищаем SSID
        socketio.emit('wifi_status', {'status': 'disconnected', 'message': 'Disconnected', 'ssid': ''})
    except Exception as e:
        print(f"Disconnect Error: {e}")

@socketio.on('check_wifi_status')
def handle_check_status():
    try:
        # 1. Узнаем имя сети
        ssid_bytes = subprocess.check_output("iwgetid -r wlan1", shell=True)
        ssid = ssid_bytes.decode('utf-8').strip()
        
        # 2. Узнаем IP-адрес именно на wlan1
        # Команда hostname -I выдает все адреса, нам нужно найти тот, что не 192.168.4.1
        # Но надежнее спросить ip у интерфейса:
        ip_cmd = "ip -4 addr show wlan1 | grep -oP '(?<=inet\s)\d+(\.\d+){3}'"
        try:
            ip_bytes = subprocess.check_output(ip_cmd, shell=True)
            ip_addr = ip_bytes.decode('utf-8').strip()
        except:
            ip_addr = "Unknown IP"

        if ssid:
            # Отправляем и статус, и имя, и IP
            socketio.emit('wifi_status', {
                'status': 'success', 
                'message': f'Connected: {ssid} ({ip_addr})', 
                'ssid': ssid,
                'ip': ip_addr
            })
        else:
            socketio.emit('wifi_status', {'status': 'disconnected', 'message': 'Not connected', 'ssid': ''})
            
    except Exception:
        socketio.emit('wifi_status', {'status': 'disconnected', 'message': 'Not connected', 'ssid': ''})

@socketio.on('disconnect')
def handle_disconnect():
    global current_client_sid
    if request.sid == current_client_sid:
        current_client_sid = None
        
@socketio.on('client_time_sync')
def handle_client_time(time_string):
    """
    Poluchayet vremya ot brauzera i pytaetsya ustanovit' yego kak sistemnoye.
    """
    global is_time_synced
    # Proveryayem, ne byla li uzhe provedena sinkhronizatsiya ---
    if is_time_synced:
        # My uzhe sinkhronizirovalis', ignoriruyem posleduyushchiye zaprosy
        return
    
    try:
        # Uroven' ponizhen s INFO do DEBUG (uberet iz konsoli) ---
        logger.debug(f"Poluchena komanda sinkhronizatsii vremeni: {time_string}")
        
        # ETO SPERVA POTREBUYET PRAV SUDO (sm. instruktsiyu v otvete)
        return_code = call(f'sudo date -s "{time_string}"', shell=True)
        
        if return_code == 0:
            # Uroven' ponizhen s INFO do DEBUG ---
            logger.debug("Sistemnoye vremya uspeshno sinkhronizirovano s brauzerom.")
            
            # Vzvodim flag, chtoby bol'she ne obnovlyat' ---
            is_time_synced = True
        else:
            logger.warning(f"Oshibka sinkhronizatsii vremeni. Kod: {return_code}. (Server zapushchen s sudo?)")
    except Exception as e:
        logger.error(f"Isklyucheniye pri sinkhronizatsii vremeni: {e}")

#декоратор для громкости фона     
@socketio.on('Phone')
def Phone(phone):
     global sound
     global fs
     global phoneLevel
     #что бы громкость звука нельзя было изменить в то время когда работает алгоритм плавного нарастания и убывания звука
     if fs==0:
          # обязательно округляем данные флоат здесь он хранит не точные значения
          ph = round(phone/20,2)
          #записываем в файл измененные настройки здесь блок кода для записи
          f1 = open('1.txt','w')
          f1.write(str(ph))
          f1.close()   
          #####
          #записали в переменную
          phoneLevel = float(ph)
          #выставляем громкость
          pygame.mixer.music.set_volume(float(ph))
     #print(phone)    
     # отправляем данные на других клиентов синхронизируем 
          socketio.emit('volume', ph, to=None)
     
#все тоже самое для других звуковых каналов             
@socketio.on('Voice')
def Voice(voice):
     global voiceLevel
     voi = round(voice/20,2)
     f1 = open('3.txt','w')
     f1.write(str(voi))
     f1.close()   
     voiceLevel = float(voi)
     channel3.set_volume(float(voi),float(voi))  
     socketio.emit('volume2', voi, to=None)    
@socketio.on('Effects')
def Effects(effects):
     global effectLevel
     eff = round(effects/20,2)
     f1 = open('2.txt','w')
     f1.write(str(eff))
     f1.close()   
     effectLevel = float(eff)
     
     # --- Применяем громкость ко ВСЕМ каналам эффектов ---
     for ch in effects_pool:
         ch.set_volume(float(eff), float(eff))
     # ---------------------------------------------------------------
     
     socketio.emit('volume1', eff, to=None)

# --- ESP32 volume controls ---
# Защита от двойного срабатывания на UI:
# 1. Server-side dedup: *Up игнорируется если v <= текущего, *Down — если v >= текущего
# 2. Rate-limit per sid: если client шлёт volume event чаще 300мс — игнор
#    (защита от stuck setInterval в старых вкладках)

_volume_last_event = {}  # {(sid, kind): timestamp}

def _volume_rate_limited(sid, kind, min_interval=0.3):
     import time
     key = (sid, kind)
     now = time.time()
     last = _volume_last_event.get(key, 0)
     if now - last < min_interval:
         return True
     _volume_last_event[key] = now
     return False

@socketio.on('WolfUp')
def WolfSound(wolfsound):
     from flask import request as _req
     global wolfLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'wolf'): return
     v = int(wolfsound)
     if v <= wolfLevel:
         return  # WolfUp не должен снижать громкость
     wolfLevel = v
     with open('5.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('wolf', wolfLevel, to=None)

@socketio.on('WolfDown')
def WolfSoundDown(wolfsound):
     from flask import request as _req
     global wolfLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'wolf'): return
     v = int(wolfsound)
     if v >= wolfLevel:
         return  # WolfDown не должен повышать громкость
     wolfLevel = v
     with open('5.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('wolf', wolfLevel, to=None)

@socketio.on('PlatformUp')
def TrainSound(platformsound):
     from flask import request as _req
     global trainLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'platform'): return
     v = int(platformsound)
     if v <= trainLevel: return
     trainLevel = v
     with open('6.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('platform', trainLevel, to=None)

@socketio.on('PlatformDown')
def TrainSoundDown(platformsound):
     from flask import request as _req
     global trainLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'platform'): return
     v = int(platformsound)
     if v >= trainLevel: return
     trainLevel = v
     with open('6.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('platform', trainLevel, to=None)

@socketio.on('SuitcasesUp')
def SuitcasesSound(suitcasessound):
     from flask import request as _req
     global suitcaseLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'suitcases'): return
     v = int(suitcasessound)
     if v <= suitcaseLevel: return
     suitcaseLevel = v
     with open('7.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('suitcases', suitcaseLevel, to=None)

@socketio.on('SuitcasesDown')
def SuitcasesSoundDown(suitcasessound):
     from flask import request as _req
     global suitcaseLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'suitcases'): return
     v = int(suitcasessound)
     if v >= suitcaseLevel: return
     suitcaseLevel = v
     with open('7.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('suitcases', suitcaseLevel, to=None)

@socketio.on('SafeUp')
def SafeSound(safesound):
     from flask import request as _req
     global safeLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'safe'): return
     v = int(safesound)
     if v <= safeLevel: return
     safeLevel = v
     with open('8.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('safe', safeLevel, to=None)

@socketio.on('SafeDown')
def SafeSoundDown(safesound):
     from flask import request as _req
     global safeLevel
     sid = getattr(_req, 'sid', '?')
     if _volume_rate_limited(sid, 'safe'): return
     v = int(safesound)
     if v >= safeLevel: return
     safeLevel = v
     with open('8.txt','w') as f: f.write(str(v))
     send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{scale_vol(v)}", debounce=True)
     socketio.emit('safe', safeLevel, to=None)

#декоратор для управления квестом с интерфейса
@socketio.on('Remote')
def Remote(check):
     global go
     global language
     global sound
     global starts
     global name
     global rateTime
     global hintCount
     global rating
     global star
     global socklist
     global mansard_galets, last_mansard_count
     b = 0
     с = 0
     #------переключаем языки
     if check == 'russian':
          #----открываем файл 4.txt хранит в себе числовое представление языка
       f4 = open('4.txt','w')
       #-----записываем язык что был выбран 1- russian
       f4.write('1')
       #----закрываем файл
       f4.close() 
       #----присваиваем значение переменной
       language = 1
       logger.info("Language switched to Russian (ID: 1)")
       send_esp32_command(ESP32_API_WOLF_URL, "language_1")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_1")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_1")
       send_esp32_command(ESP32_API_SAFE_URL, "language_1")

     if check == 'english':
       f4 = open('4.txt','w')
       f4.write('2')
       f4.close() 
       language = 2
       logger.info("Language switched to English (ID: 2)")
       send_esp32_command(ESP32_API_WOLF_URL, "language_2")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_2")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_2")
       send_esp32_command(ESP32_API_SAFE_URL, "language_2")
          
     if check == 'arabian':
       f4 = open('4.txt','w')
       f4.write('3')
       f4.close() 
       language = 3
       logger.info("Language switched to Arabian (ID: 3)")
       send_esp32_command(ESP32_API_WOLF_URL, "language_3")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_3")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_3")
       send_esp32_command(ESP32_API_SAFE_URL, "language_3")
          
     if check == 'french':       # Если нажали "French"
       f4 = open('4.txt','w')    # Открываем файл настроек
       f4.write('4')             # Записываем цифру 4 (это будет код французского)
       f4.close() 
       language = 4              # Переключаем переменную
       logger.info("Language switched to French (ID: 4)")
       # Отправляем команду на внешние устройства (если они прошиты под это)
       send_esp32_command(ESP32_API_WOLF_URL, "language_4")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_4")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_4")
       send_esp32_command(ESP32_API_SAFE_URL, "language_4")

     if check == 'ukrainian':
       f4 = open('4.txt','w')
       f4.write('5')
       f4.close() 
       language = 5
       logger.info("Language switched to Ukrainian (ID: 5)")
       send_esp32_command(ESP32_API_WOLF_URL, "language_5")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_5")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_5")
       send_esp32_command(ESP32_API_SAFE_URL, "language_5")

     if check == 'polish':
       f4 = open('4.txt','w')
       f4.write('6')
       f4.close() 
       language = 6
       logger.info("Language switched to Polish (ID: 6)")
       send_esp32_command(ESP32_API_WOLF_URL, "language_6")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_6")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_6")
       send_esp32_command(ESP32_API_SAFE_URL, "language_6")

    #----нажали выключить
     if check == 'off':
             # Добавлено удаление файла-метки и остановка музыки ---

             # 1. Логируем событие
             logger.debug("SHUTDOWN: Получена команда 'off' от пульта...")
             # 2026-05-27: Гасим LED на всех ESP32 чтобы устройства перестали
             # светиться сразу после нажатия Выключить (до фактического shutdown Pi).
             send_esp32_command(ESP32_API_WOLF_URL, "day_off")
             send_esp32_command(ESP32_API_TRAIN_URL, "day_off")
             send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
             send_esp32_command(ESP32_API_SAFE_URL, "day_off")
             
             # 2. Удаляем файл-метку, сигнализируя о КОРРЕКТНОМ завершении
             try:
                 if os.path.exists(LATCH_FILE):
                     os.remove(LATCH_FILE)
                     logger.debug(f"SHUTDOWN: Файл '{LATCH_FILE}' успешно удален.")
             except Exception as e:
                 logger.error(f"SHUTDOWN: Не удалось удалить '{LATCH_FILE}': {e}")
             
             # 3. НЕМЕДЛЕННО ОСТАНАВЛИВАЕМ ФОНОВУЮ МУЗЫКУ
             pygame.mixer.music.stop()
             
             # 4. Воспроизводим звук выключения
             try:
                 play_effect(timeout) 
             except Exception as e:
                 logger.error(f"Не удалось воспроизвести звук выключения: {e}")

             # 5. Даем 0.5 секунды, чтобы звук успел начаться
             eventlet.sleep(0.5) 
             
             # 6. Отправляем команду на выключение
             call("sudo shutdown -h now", shell=True)
     if check == 'open_stash':
             serial_write_queue.put('open_stash')
     if check == 'open_basket_door_skip':
         if starts == 1: # --- В режиме ИГРЫ (эмулируем 'cup')
             # Отправляем 'cup' в serial, как будто нажали 'cup'
             serial_write_queue.put('cup')
             # Копируем UI-логику из 'cup'
             socketio.emit('level', 'cup',to=None)
             socklist.append('cup')
             name = "story_2"
             eventlet.sleep(5) 
             socketio.emit('level', 'active_spell',to=None)
             socklist.append('active_spell')
         
         elif go == 0 or go == 2 or go == 3: # --- В режиме РЕСТАРТА
             serial_write_queue.put('open_basket_door')
             # Также эмулируем 'cup' для UI, чтобы кнопка стала зеленой
             socketio.emit('level', 'cup',to=None)
             socklist.append('cup')
     #есть отдельная кнопка которая открывает все тайники на меге обрабатывается и отправляет башням
     # -------если пришло сообщение startgo в serial игра начинается и мы можем управлять квесто
     if starts == 1:
        if check == 'skip_start_door':
             serial_write_queue.put('skip_start_door')
        if check == 'first_clock':
             #----отправли на клиента
             socketio.emit('level', 'first_clock',to=None)
             #-----отправили на мегу
             serial_write_queue.put('first_clock')
             #----добавили в историю
             socklist.append('first_clock')
             eventlet.sleep(3)
             #------активируем блок с галетниками
             socketio.emit('level', 'active_second_clock',to=None)
             socklist.append('active_second_clock') 
             #----изменяем переменную для повторения
             name = "story_1"  

        if check=='second_clock':
             #-----отправка клиенту 
             socketio.emit('level', 'second_clock',to=None)
             #-----добавить в историю
             socklist.append('second_clock')
             #----отправить на мегу
             serial_write_queue.put('second_clock')
             name = "story_2"  
             #---ждем 3 секунды
             eventlet.sleep(3) 
             #-----активируем блок
             socketio.emit('level', 'active_open_mansard_door',to=None)
             socklist.append('active_open_mansard_door')
        if check=='open_mansard_door':
             # 1. Логика, которая уже была (отправка на Arduino и UI)
             socketio.emit('level', 'open_mansard_door',to=None)
             socklist.append('open_mansard_door') # Исправлено (было 'galet_on')
             serial_write_queue.put('open_mansard_door')
             serial_write_queue.put('student_hide') # Эта команда была в оригинале, оставляем
             # --- Логика для 100% прогресс-бара при СКИПЕ ---
             
             # 1. Отправляем 100% на пульт
             socketio.emit('level', 'mansard_progress_100', to=None)
             
             # 2. Очищаем старые значения прогресса из socklist (0-80%)
             for i in range(0, 100, 20): 
                old_event_name = f"mansard_progress_{i}"
                while old_event_name in socklist:
                    try:
                        socklist.remove(old_event_name)
                    except ValueError:
                        pass
             
             # 3. Добавляем 100% в socklist
             if 'mansard_progress_100' not in socklist:
                socklist.append('mansard_progress_100')
             
             # 4. Обновляем внутреннее состояние сервера, чтобы оно соответствовало 100%
             # (Это "защита" от будущих физ. сигналов 'galet_off')
             mansard_galets.update(['g1', 'g2', 'g3', 'g4', 'g5']) # Имитируем, что все 5 включены
             last_mansard_count = 5 # Устанавливаем счетчик на 5
             
             # 2. Добавляем всю пропущенную логику из 'if flag=="galet_on":'
             play_background_music("fon6.mp3", loops=-1)
             play_effect(door_attic) # Эффект открытия
             
             # 3. Воспроизводим истории (ОПТИМИЗИРОВАНО)
             play_localized_audio("story_5")

             while channel3.get_busy()==True and go == 1: 
                 eventlet.sleep(0.1)
             
             play_localized_audio("story_6")

             while channel3.get_busy()==True and go == 1: 
                 eventlet.sleep(0.1)

             # 4. Активируем ESP32 и Train
             send_esp32_command(ESP32_API_WOLF_URL, "game")
             send_esp32_command(ESP32_API_SUITCASE_URL, "game")
             send_esp32_command(ESP32_API_SAFE_URL, "game")
             send_esp32_command(ESP32_API_TRAIN_URL, "stage_2")

             name = "story_2"
        if check=='suitcase':
             #-----отправка клиенту 
             socketio.emit('level', 'suitcase',to=None)
             #-----добавить в историю
             socklist.append('suitcase')
             #----отправить на мегу
             serial_write_queue.put('suitcase_end')
             name = "story_2" 
             send_esp32_command(ESP32_API_SUITCASE_URL, "skip")
             send_esp32_command(ESP32_API_TRAIN_URL, "case_finish")
        if check=='animals':
             #-----отправка клиенту 
             socketio.emit('level', 'animals',to=None)
             #-----добавить в историю
             socklist.append('animals')
             #----отправить на мегу
             serial_write_queue.put('safe_end')
             name = "story_2"  
             send_esp32_command(ESP32_API_SAFE_URL, "skip")
             send_esp32_command(ESP32_API_TRAIN_URL, "safe_finish")
        if check == 'wolf':
             #-----отправка клиенту 
             socketio.emit('level', 'wolf',to=None)
             #-----добавить в историю
             socklist.append('wolf')
             #----отправить на мегу
             serial_write_queue.put('wolf_end')
             name = "story_2"  
             send_esp32_command(ESP32_API_WOLF_URL, "skip")
             send_esp32_command(ESP32_API_TRAIN_URL, "wolf_finish")
        #-----нажали пропустить флаги     
        if check=='open_mansard_stash':
             #----отправка на клиента 
             socketio.emit('level', 'flag1_on',to=None)
             socketio.emit('level', 'flag2_on',to=None)
             socketio.emit('level', 'flag3_on',to=None)
             socketio.emit('level', 'flag4_on',to=None)
             #----добавили в историю
             socklist.append('flag1_on')
             socklist.append('flag2_on')
             socklist.append('flag3_on')
             socklist.append('flag4_on')
             #-----отправили на мегу
             serial_write_queue.put('m2lck')

             #socketio.emit('level', 'active_owl',to=None)
             #socklist.append('active_owl')
             #socketio.emit('level', 'active_pedlock',to=None)
             #socklist.append('active_pedlock')
             #socketio.emit('level', 'active_cat',to=None)
             #socklist.append('active_cat')
             #socketio.emit('level', 'active_projector',to=None)
             #socklist.append('active_projector')
        if check == 'open_bank_door':
             #-----отправка клиенту 
             socketio.emit('level', 'open_bank_door',to=None)
             #-----добавить в историю
             socklist.append('open_bank_door')
             #----отправить на мегу
             serial_write_queue.put('open_bank_door')
             name = "story_2"     
        if check == 'pedlock':
             #-----отправка клиенту 
             socketio.emit('level', 'pedlock',to=None)
             #-----добавить в историю
             socklist.append('pedlock')
             #----отправить на мегу
             serial_write_queue.put('pedlock')
             name = "story_2"     
             eventlet.sleep(1) 
             #-----активируем блок
             socketio.emit('level', 'active_dog',to=None)
             socklist.append('active_dog')
        if check == 'dog':
             # FIX: Останавливаем зацикленный dog_growl при скипе
             dog_growl.stop()
             # FIX: Ставим метку, чтобы dog_lock от Arduino не дублировал действия
             socklist.append('dog_end_processed')
             #-----отправка клиенту 
             socketio.emit('level', 'dog',to=None)
             #-----добавить в историю
             socklist.append('dog')
             #----отправить на мегу
             serial_write_queue.put('dog')
             # FIX: Воспроизводим key_finish и story_21 как при обычном прохождении
             send_esp32_command(ESP32_API_TRAIN_URL, "key_finish")
             play_effect(dog_lock)
             play_localized_audio("story_21")
             eventlet.sleep(1) 
             name = "story_2"
        if check == 'cat':
             #-----отправка клиенту 
             socketio.emit('level', 'cat',to=None)
             #-----добавить в историю
             socklist.append('cat')
             #----отправить на мегу
             serial_write_queue.put('cat')
             # --- Имитируем реакцию сервера на door_witch ---
             play_effect(door_witch) # 1. Воспроизводим звук открытия
             send_esp32_command(ESP32_API_TRAIN_URL, "fish_open") # 2. Гасим LED рыбы (24) на карте
             eventlet.sleep(1) 
             # 3. Воспроизводим историю (ОПТИМИЗИРОВАНО)
             play_localized_audio("story_17")

             # 4. Активируем следующий этап
             socketio.emit('level', 'active_open_potions_stash',to=None) #
             socklist.append('active_open_potions_stash')

             #-----активируем блок
             socketio.emit('level', 'active_open_potions_stash',to=None)
             socklist.append('active_open_potions_stash')
        if check == 'open_potions_stash':
             #-----отправка клиенту 
             socketio.emit('level', 'open_potions_stash',to=None)
             #-----добавить в историю
             socklist.append('open_potions_stash')
             #----отправить на мегу
             serial_write_queue.put('open_potions_stash')
             eventlet.sleep(1) 
             name = "story_2"
        if check == 'owl':
             # 2026-05-25: Remote('owl') теперь делает только активацию совы на карте —
             # то же что клик на карте Train (data['map'] == 'owl'). Раньше тут был
             # auto-skip: шёл owl_door + auto-play door_owl/story_13/14_a, что
             # создавало баг "дверь сов открывается сама". Стандартный flow:
             # клик активирует совы (owlCommandReceived=true в owls.ino) → игроки в
             # течение таймера нажимают геркон на башне → door_owl event → проигрыш
             # эффекта + историй + физическое открытие лока (см. door_owl handler).
             # Для полного skip этапа есть отдельная кнопка Remote('owls').
             logger.debug("Remote 'owl' triggered (manual map activation).")
             serial_write_queue.put('owl')
             send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
             send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks")
             play_effect(map_click)
             # active_owls indicator (UI прогресс-бар) ставится только когда придёт
             # door_owl event от геркона — см. door_owl handler ниже.
        if check == 'owls':
             #-----отправка клиенту 
             socketio.emit('level', 'owls',to=None)
             #-----добавить в историю
             socklist.append('owls')
             # --- Добавляем событие 100% для прогресс-бара ---
             # (Команда 'owl_end' обрабатывается в scripts.js для 100%)
             socketio.emit('level', 'owl_end', to=None)
             socklist.append('owl_end')
             #----отправить на мегу
             serial_write_queue.put('owl_skip')
             #----отправить на ESP32 карты
             play_effect(owl_flew)
             # FIX: Сначала owl_open (очищает ClickLeds радугу), потом owl_finish (гасит ActiveLeds)
             send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
             send_esp32_command(ESP32_API_TRAIN_URL, "owl_finish")
             # ОПТИМИЗИРОВАНО
             play_localized_audio("story_14_b")

             eventlet.sleep(1)
        if check == 'projector':
             #-----отправка клиенту 
             socketio.emit('level', 'projector',to=None)
             #-----добавить в историю
             socklist.append('projector')
             #----отправить на мегу
             send_esp32_command(ESP32_API_TRAIN_URL, "projector")
             serial_write_queue.put('train_active')
             name = "story_2"     
             eventlet.sleep(3) 
             #-----активируем блок
             socketio.emit('level', 'active_train',to=None)
             socklist.append('active_train') 
        if check == 'train':
             #-----отправка клиенту 
             socketio.emit('level', 'train',to=None)
             #-----добавить в историю
             socklist.append('train')
             send_esp32_command(ESP32_API_TRAIN_URL, "skip")
             #----отправить на мегу
             serial_write_queue.put('train_end')
             name = "story_2" 
             socketio.emit('level', 'active_mine',to=None)
             socklist.append('active_mine')
        if check == 'mine':
             #-----отправка клиенту 
             socketio.emit('level', 'mine',to=None)
             #-----добавить в историю
             socklist.append('mine')
             #----отправить на мегу
             serial_write_queue.put('mine')
             name = "story_2" 
             eventlet.sleep(1) 
             socketio.emit('level', 'active_troll',to=None)
             socklist.append('active_troll')
        if check == 'troll':
             #-----отправка клиенту 
             socketio.emit('level', 'troll',to=None)
             #-----добавить в историю
             socklist.append('troll')
             socketio.emit('level', 'cave_end', to=None)
             socklist.append('cave_end')
             #----отправить на мегу
             serial_write_queue.put('troll')
             name = "story_2" 

        if check == 'safe':
             #-----отправка клиенту 
             socketio.emit('level', 'safe',to=None)
             #-----добавить в историю
             socklist.append('safe')
             # --- Добавляем событие 100% для прогресс-бара ---
             # (Команда 'safe_end' уже обрабатывается в scripts.js для 100%)
             socketio.emit('level', 'safe_end', to=None)
             socklist.append('safe_end')
             #----отправить на мегу
             serial_write_queue.put('safe')
             name = "story_2"     
             eventlet.sleep(1) 
             #-----активируем блок
             #socketio.emit('level', 'active_workshop',to=None)
             #socklist.append('active_workshop')
        if check == 'workshop':
             #-----отправка клиенту 
             socketio.emit('level', 'workshop',to=None)
             #-----добавить в историю
             socklist.append('workshop')
             #----отправить на мегу
             serial_write_queue.put('workshop')
             name = "story_2"     
             eventlet.sleep(5) 
             #-----активируем блок
             socketio.emit('level', 'active_first_clock_2',to=None)
             socklist.append('active_first_clock_2')
        if check == 'first_clock_2':
             #-----отправка клиенту 
             socketio.emit('level', 'first_clock_2',to=None)
             #-----добавить в историю
             socklist.append('first_clock_2')
             #----отправить на мегу
             serial_write_queue.put('first_clock_2')
             name = "story_2"     
             eventlet.sleep(5) 
             #-----активируем блок
             socketio.emit('level', 'active_second_clock_2',to=None)
             socklist.append('active_second_clock_2')
        if check == 'second_clock_2':
             #-----отправка клиенту 
             socketio.emit('level', 'second_clock_2',to=None)
             #-----добавить в историю
             socklist.append('second_clock_2')
             #----отправить на мегу
             serial_write_queue.put('second_clock_2')
             name = "story_2"     
             eventlet.sleep(5) 
             #-----активируем блок
             socketio.emit('level', 'active_ghost',to=None)
             socklist.append('active_ghost')
        if check == 'ghost_step_1': # Кнопка 1 (Train)
             # Просто запускаем первый шаг
             serial_write_queue.put('ghost')
             send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game_end")
        if check == 'ghost_step_2': # Кнопка 2 (Wolf)
             # Выключаем ESP Волка (Шаг 1)
             send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
             # Запускаем Шаг 2
             serial_write_queue.put('ghost')

        if check == 'ghost_step_3': # Кнопка 3 (Train 2)
             # Выключаем оба ESP (Шаг 1 и 2)
             send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
             send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game_end")
             # Запускаем Шаг 3
             serial_write_queue.put('ghost') 

        if check == 'ghost_step_4': # Кнопка 4 (Library/Punch)
             # Выключаем оба ESP
             send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
             send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game_end")
             
             # Запускаем пропуск физического удара
             serial_write_queue.put('ghost_skip') 
             
             # Мгновенно обновляем UI (так как 'ghost_skip' не ждет 'punch')
             socketio.emit('level', 'ghost',to=None)
             socklist.append('ghost')
             socketio.emit('level', 'punch', to=None)
             socklist.append('punch')
             name = "story_2" 
        
        if check == 'ghost_step_5': # Кнопка 5 (Star hint)
             # Выключаем оба ESP
             send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
             send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game_end")
             
             # Запускаем Шаг 5 (запрос подсказки по звездам)
             serial_write_queue.put('ghost') 
             
             # Мгновенно обновляем UI
             socketio.emit('level', 'set_time', to=None)
             socklist.append('set_time')
        if check == 'skip_lib_door':
             serial_write_queue.put('skip_lib_door')
        if check == 'cup':
             #-----отправка клиенту 
             socketio.emit('level', 'cup',to=None)
             #-----добавить в историю
             socklist.append('cup')
             #----отправить на мегу
             serial_write_queue.put('cup')
             name = "story_2"
             eventlet.sleep(5) 
             #-----активируем блок
             socketio.emit('level', 'active_spell',to=None)
             socklist.append('active_spell')  
        if check == 'spell':
             #-----отправка клиенту 
             socketio.emit('level', 'spell',to=None)
             #-----добавить в историю
             socklist.append('spell')
             # Добавляем 100% на skip ---
             socketio.emit('level', 'spell_step_5', to=None)
             socklist.append('spell_step_5') # Отправляем команду 5-го шага
             #----отправить на мегу
             serial_write_queue.put('spell')
             name = "story_2"
             eventlet.sleep(5) 
             #-----активируем блок
             socketio.emit('level', 'active_crystals',to=None)
             socklist.append('active_crystals')
        if check == 'crystals':
             #-----отправка клиенту 
             socketio.emit('level', 'crystals',to=None)
             #-----добавить в историю
             socklist.append('crystals')
             #----отправить на мегу
             serial_write_queue.put('crystals')
             name = "story_2" 
        if check == 'open_memory_stash':
             send_esp32_command(ESP32_API_TRAIN_URL, "stage_0") 
             #-----отправка клиенту 
             socketio.emit('level', 'first_level',to=None)
             #-----добавить в историю
             socklist.append('first_level')
             socketio.emit('level', 'memory_room_end',to=None)
             #-----добавить в историю
             if 'memory_room_end' not in socklist:
                 socklist.append('memory_room_end')
             # FIX: Добавляем действия, которые при обычном прохождении выполняет serial-обработчик memory_room_end
             play_effect(brain_end)
             socketio.emit('level', 'active_crime',to=None)
             socklist.append('active_crime')
             # Отправляем на Arduino чтобы он тоже завершил (радуга, свет, дверь)
             # memory_room_skip_done будет поставлен serial-обработчиком когда Arduino ответит echo
             def _send_memory_skip():
                 for attempt in range(4):
                     if 'memory_room_skip_done' in socklist:
                         break
                     serial_write_queue.put('memory_room_end')
                     logger.info(f"SENT [Main Board]: Комната воспоминаний пройдена (скип попытка {attempt+1}/4) (RAW: memory_room_end)")
                     eventlet.sleep(1.5)
             socketio.start_background_task(_send_memory_skip)
        if check == 'crime':
             #-----отправка клиенту 
             socketio.emit('level', 'crime',to=None)
             #-----добавить в историю
             socklist.append('crime')
             #----отправить на мегу
             serial_write_queue.put('crime')     
                         
        if check == 'basket':
             #-----отправка клиенту
             socketio.emit('level', 'basket',to=None)
             socklist.append('basket')

             # 2026-05-26: НЕ ставим preemptive 'win_player' в socklist.
             # Дедуп-guard в flag=="win" (`if 'win_player' in socklist: continue`)
             # блокировал победную музыку (fon19 + story_66 + win.wav + салют)
             # когда Basket отвечал на наш force_win_bask. Теперь дадим
             # flag=="win" handler самому сыграть всю последовательность,
             # он сам поставит 'win_player' после.

             # --- УНИКАЛЬНОЕ ИМЯ КОМАНДЫ ---
             serial_write_queue.put('force_win_bask')

             send_esp32_command(ESP32_API_WOLF_URL, "firework")
             send_esp32_command(ESP32_API_TRAIN_URL, "firework")
             send_esp32_command(ESP32_API_SUITCASE_URL, "firework")
             send_esp32_command(ESP32_API_SAFE_URL, "firework")
                 
        
     #------- обработка в режиме рестарта   
     elif go == 0 or go == 2 or go == 3: # Используем elif вместо if для режима рестарта ---
        if check=='open_mansard_door':
             serial_write_queue.put('open_mansard_door')
        if check=='suitcase': 
             send_esp32_command(ESP32_API_SUITCASE_URL, "open_door")
             #serial_write_queue.put('m2lck')
        if check=='animals': 
             send_esp32_command(ESP32_API_SAFE_URL, "open_door")
             #serial_write_queue.put('rrt3lck')
        if check=='wolf':
             send_esp32_command(ESP32_API_WOLF_URL, "open_door")
             #serial_write_queue.put('rlt2lck')
        if check=='open_bank_door': 
             serial_write_queue.put('open_bank_door')
        if check=='pedlock': 
             serial_write_queue.put('open_dog_door')
        if check=='cat': 
             serial_write_queue.put('open_potion_door')
        if check=='owl':
             serial_write_queue.put('open_owl_door')
        if check=='mine': 
             serial_write_queue.put('open_mine_door')
        if check=='safe':
             serial_write_queue.put('open_safe_door')
        if check=='workshop': 
             serial_write_queue.put('open_workshop_door')
        if check=='ghost_step_4':
             serial_write_queue.put('open_library_door')
        if check=='cup':
             serial_write_queue.put('open_high_tower_door')
        if check=='spell':
             serial_write_queue.put('open_low_tower_door')
        if check=='crystals':
             serial_write_queue.put('open_memory_door')
        if check=='basket':
             pass
        if check=='crime':
             serial_write_queue.put('open_crime_door')     


@app.route('/api/log', methods=['POST'])
def log_event():
    """
    Универсальная конечная точка для логирования сообщений от устройств.
    Ожидает JSON вида: {"device": "название_устройства", "message": "сообщение"}.
    """
    data = request.get_json()
    if not data or 'device' not in data or 'message' not in data:
        return jsonify({"status": "error", "message": "Invalid JSON format. Required keys: 'device', 'message'."}), 400
    
    device_name = data.get('device', 'Unknown Device')
    message = data.get('message', '')
    
    # Логируем в формате: RECEIVED [ESP32 Log - <device_name>]: <message>
    logging.info(f'RECEIVED [ESP32 Log - {device_name}]: {message}')
    
    return jsonify({"status": "success"}), 200


@app.route('/api', methods=['GET', 'POST'])
def handle_data():
    global mapClickHints
    global mapClickOut
    if request.method == 'POST':
        data = request.get_json()
        # === DIAG snapshot от ESP32: {"diag": {"in":[...], "enc":[...], ...}}
        # POST'ится на /api?device=<id> каждые 200ms в diag-режиме (см. /tech)
        if isinstance(data, dict) and 'diag' in data and isinstance(data['diag'], dict):
            device = request.args.get('device', 'unknown')
            socketio.emit(f'{device}_diag_state', data['diag'])
            _log_diag_snapshot(device, data['diag'])
            return jsonify({"status": "diag_ok"})
        if 'train_enc' in data:
            socketio.emit('level', data['train_enc'], to=None)
            logging.info(f"TRAIN ENCODER STATUS: {data['train_enc']}")
            return jsonify({"status": "success"})
        # Добавляем обработку простых логов от ESP32.
        # Уровень: INFO для важных событий (FALLBACK, ERROR, ALERT, ВНИМАНИЕ),
        # DEBUG для всех остальных (рутинные ready/playing-сообщения, не для скан-листа).
        if 'log' in data:
             msg = str(data.get('log', ''))
             important = any(kw in msg for kw in ('FALLBACK', 'ERROR', 'ALERT', 'ВНИМАНИЕ', 'Error', 'fail', 'Fail'))
             if important:
                 logging.info(f"RECEIVED [ESP32 Log]: {msg}")
             else:
                 logging.debug(f"RECEIVED [ESP32 Log]: {msg}")
             # Возвращаем успех, чтобы ESP32 не висела
             return jsonify({"status": "success"})
        # Улучшенное логирование входящих сообщений от ESP32
        # Мы итерируемся по ключам в data, чтобы найти соответствующее событие
        description_found = False
        if isinstance(data, dict):
            for key, value in data.items():
                event_key = f"{key}: {value}" # Собираем ключ вида "wolf: end"
                if event_key in EVENT_DESCRIPTIONS:
                    description = EVENT_DESCRIPTIONS.get(event_key)
                    logging.info(f'RECEIVED [ESP32 API]: {description} (RAW: {data})')
                    description_found = True
                    break # Логируем только первое найденное событие
        if not description_found:
             # Логируем как есть, если описание не найдено
             logging.info(f'RECEIVED [ESP32 API]: {data}')

        # 2026-05-25: для game-end событий возвращаем `{"confirm": true}` прямо
        # в response body. ESP32 видит это и ставит локальный *_endConfirmed=true
        # без необходимости ждать встречный HTTP POST с командой confirm_*_end.
        # Это исключает recursion lock и спам POST'ов от ESP32 (см. проблему
        # Safe ESP32 ушёл offline через 14 сек после safe:end — clc-safe-offline).
        # Также оставляем send_esp32_command(...) для обратной совместимости со
        # старыми прошивками которые ещё ждут отдельный confirm_*_end.
        ack_response = None
        if 'suitcase' in data and data['suitcase'] == 'end':
          logger.debug("'suitcase: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "case_finish")
          serial_write_queue.put('suitcase_end')
          socketio.emit('level', 'suitcase',to=None)
          socklist.append('suitcase')
          send_esp32_command(ESP32_API_SUITCASE_URL, "confirm_suitcase_end")
          ack_response = jsonify({"confirm": True, "event": "suitcase_end"})
        if 'safe' in data and data['safe'] == 'end':
          logger.debug("'safe: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "safe_finish")
          serial_write_queue.put('safe_end')
          socketio.emit('level', 'animals',to=None)
          socklist.append('animals')
          send_esp32_command(ESP32_API_SAFE_URL, "confirm_safe_end")
          ack_response = jsonify({"confirm": True, "event": "safe_end"})
        if 'wolf' in data and data['wolf'] == 'end':
          logger.debug("'wolf: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "wolf_finish")
          serial_write_queue.put('wolf_end')
          socketio.emit('level', 'wolf',to=None)
          socklist.append('wolf')
          send_esp32_command(ESP32_API_WOLF_URL, "confirm_wolf_end")
          ack_response = jsonify({"confirm": True, "event": "wolf_end"})
        if ack_response is not None:
          return ack_response
         
        # --- ИЗМЕНЕНИЕ: Добавляем логику для TRAIN (projector end) ---
        elif 'projector' in data and data['projector'] == 'end':
            logger.debug("'projector: end' logic triggered.")
            socketio.emit('level', 'projector',to=None)
            socklist.append('projector')
            serial_write_queue.put('train_active')
            # Активируем следующий этап (игра с поездом)
            socketio.emit('level', 'active_train',to=None)
            socklist.append('active_train')
        # --- КОНЕЦ ИЗМЕНЕНИЯ ---

        if 'map' in data and data['map'] == 'owl':
          logger.debug("'map: owl' logic triggered.")
          serial_write_queue.put('owl')
          eventlet.sleep(1.0)
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               play_localized_audio("story_12_a")
          elif mapClickHints == 1:
               mapClickHints = 0
               play_localized_audio("story_12_b")

        if 'map' in data and data['map'] == 'fish':
          logger.debug("'map: fish' logic triggered.")
          serial_write_queue.put('fish')
          eventlet.sleep(1.0)
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               play_localized_audio("story_12_a")
          elif mapClickHints == 1:
               mapClickHints = 0
               play_localized_audio("story_12_b")

        if 'map' in data and data['map'] == 'key':
          logger.debug("'map: key' logic triggered.")
          serial_write_queue.put('key')
          eventlet.sleep(1.0)
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               play_localized_audio("story_12_a")
          elif mapClickHints == 1:
               mapClickHints = 0
               play_localized_audio("story_12_b")

        if 'map' in data and data['map'] == 'train':
          logger.debug("'map: train' logic triggered.")
          eventlet.sleep(0.5)
          play_effect(map_click)
          if mapClickHints == 0:
               mapClickHints = 1
               play_localized_audio("story_12_a")
          elif mapClickHints == 1:
               mapClickHints = 0
               play_localized_audio("story_12_b")

        if 'train' in data and data['train'] == 'end':
            logger.debug("'train: end' logic triggered.")
            serial_write_queue.put('train_end')
            socketio.emit('level', 'train',to=None)
            socklist.append('train')
            socketio.emit('level', 'active_mine',to=None)
            socklist.append('active_mine')
            # Отправляем подтверждение обратно на ESP32, чтобы он перестал слать запросы
            send_esp32_command(ESP32_API_TRAIN_URL, "confirm_train_end")

        if 'projector' in data and data['projector'] == 'end':
            logger.debug("'projector: end' logic triggered.")
            socketio.emit('level', 'projector',to=None)
            socklist.append('projector')
            socketio.emit('level', 'active_train',to=None)
            socklist.append('active_train')

        if 'train' in data and data['train'] == 'skin':
            logger.debug("'train: skin' logic triggered.")
            serial_write_queue.put('skin')
            # play_effect(item_find)
  
        if 'map' in data and data['map'] == 'out':
          logger.debug("'map: out' logic triggered.")
          serial_write_queue.put('out')
          play_effect(map_out)
          while effects_are_busy() and go == 1:
             eventlet.sleep(0.1) 
          if mapClickOut == 0:
               mapClickOut = 1
               play_localized_audio("story_12_c")
          elif mapClickOut == 1:
               mapClickOut = 0
               play_localized_audio("story_12_d")

        if 'ghost' in data and data['ghost'] == 'end':
            logger.debug("'ghost: end' logic triggered.")
            serial_write_queue.put('ghost')
                 

        # Обработка POST завершена — возвращаем успех.
        # (Раньше тут была закомментированная строка с requests.post(ESP32_API_URL...)
        # и попытка прочитать `response.json()` от несуществующей переменной —
        # это валило ВСЕ ESP32 POST в HTTP 500 → их HTTPClient блокировал loop
        # пытаясь повторять до получения 200. Симптом: Safe в GAME_WON флапает
        # WiFi каждые 30-60 сек, "зависает игра".)
        return jsonify({"status": "success"})
            
    return jsonify({"status": "ready", "message": "Send POST request with data"})
                     
#декоратор хранящий в себе состояния режимов игры старт рестарт пауза     
@socketio.on('time')
def tmr(res): 
     global go
     global flag
     global a1
     global a2
     global a3
     global starts
     global language
     global name
     global flagS
     global rateTime
     global hintCount
     global rating
     global star
     global mansard_galets, last_mansard_count
     global is_processing_ready
     global devices
     global socklist
     global wolfLevel, trainLevel, suitcaseLevel, safeLevel
     global pending_restart, pending_ready
     global goalCount, enemyGoalCount, owlFlewCount
     global storyBasketFlag, catchCount, timeoutCount, basket_timeout_timer
     # --- Локальный флаг для отслеживания прогресса поезда ---
     # Помогает избежать отката на stage_1, если уже прошел stage_2
     train_stage_2_active = False
     logger.info(f"[SOCKET_IN] Command: {res}")
    #----на всякий случай отправим данные о выборе языка 
     if language==1: 
         socketio.emit('lang', 'russian', to=None)
     if language==2: 
         socketio.emit('lang', 'english', to=None)
     if language==3: 
         socketio.emit('lang', 'arabian', to=None) 
     if language==4: 
         socketio.emit('lang', 'french', to=None) 
     if language==5: 
         socketio.emit('lang', 'ukrainian', to=None) 
     if language==6: 
         socketio.emit('lang', 'polish', to=None) 
    
     #----если пришло сообщение о паузе проверяем что бы игра находилась в режиме старта
     if res =='pause':
          if go==1 and starts==1:
               socketio.emit('level', 'pause_game',to=None)
               if 'start_game' in socklist:
                     socklist.remove('start_game')
               if 'rest' in socklist:
                     socklist.remove('rest')      
               socklist.append('pause')
               starts = 0
               pygame.mixer.music.pause()
               channel3.pause() 
               for ch in effects_pool:
                   ch.pause()
               go=-1
               logger.debug("State changed: Game paused.")
     #-----пришло сообщение старт с пульта          
     if res =='start':
        set_bluetooth_state(False)
        #-----обнулим флаги
        name=""
        flagS = 0
        flag=""
        #----если была в паузе
        if go == -1:
             #--удаляем из истории паузу
              socklist.remove('pause')
              #----добавляем старт
              socklist.append('start_game')  
              go=1
              starts = 1
              #----снимаем с паузы каналы 
              pygame.mixer.music.unpause()
              channel3.unpause() 
              for ch in effects_pool:
                  ch.unpause() 
              #----отправим на клиента
              socketio.emit('level', 'start_game',to=None)
              logger.debug("State changed: Game unpaused.")
        #----если была в рестарте       
        if go == 3 and starts==3:
             socklist.clear()
             send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{scale_vol(wolfLevel)}")
             send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{scale_vol(trainLevel)}")
             send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{scale_vol(suitcaseLevel)}")
             send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{scale_vol(safeLevel)}")
             logger.info("Start command received")
             #----очищаем историю 
             socklist.clear() 
             serial_write_queue.put('start')
             serial_write_queue.put('start')
             serial_write_queue.put('start')
             go=1
             starts = 1
             logger.debug("State changed: Game started from 'ready' state.")
        
     #----нажали на рестарт   
     if res =='restart':
         logger.debug("State changed: Game restarted.")
         pending_restart = True
         pending_ready = False
         # Эмитим отдельный event для блокировки кнопки Ready на клиенте.
         # 'level':'rest' идёт в socklist и реплеится при reconnect — мы не хотим
         # чтобы клиент запускал 10s timer повторно при каждом reconnect, поэтому
         # отдельный event 'restart_pressed' который НЕ сохраняется.
         logger.info("[RESTART] Emitting restart_pressed (Ready block ON, 10s timer)")
         socketio.emit('restart_pressed', {}, to=None)
         # Через 10 сек шлём ready_block_off — клиент снимает disabled с Ready.
         # Промежуточные 'rest' events (replay из socklist) не сбивают timer.
         def _unblock_ready_after_10s():
             eventlet.sleep(10)
             logger.info("[RESTART] Emitting ready_block_off (Ready активна)")
             socketio.emit('ready_block_off', {}, to=None)
         eventlet.spawn_n(_unblock_ready_after_10s)
         serial_write_queue.put('restart')
         serial_write_queue.put('restart')
         serial_write_queue.put('restart')
         send_esp32_command(ESP32_API_WOLF_URL, "restart")
         send_esp32_command(ESP32_API_TRAIN_URL, "restart")
         send_esp32_command(ESP32_API_SUITCASE_URL, "restart")
         send_esp32_command(ESP32_API_SAFE_URL, "restart")
         rating = 0
         star = 0
         rateTime = ''
         hintCount=0
         starts = 2
         name=""
         flagS = 0
         goalCount = 0
         enemyGoalCount = 0
         owlFlewCount = 0
         storyBasketFlag = 0
         catchCount = 0
         timeoutCount = 0
         if basket_timeout_timer:
             basket_timeout_timer.cancel()
             basket_timeout_timer = None
         socketio.emit('level', 'rest',to=None)
         socklist.clear()
         socklist.append('rest')
         socklist.append('50')
         flag=""
         go=2
         channel3.stop() 
         stop_all_effects()
         pygame.mixer.music.stop()
         #pygame.mixer.music.unload()
         
         #ser.write('restart')
         go=2
          
     #----если нажали на реди
     if res =='ready':
         set_bluetooth_state(False)
         devices.clear()
         if is_processing_ready:
             print("INFO: 'Ready' command is already being processed.")
             return 

         is_processing_ready = True
         pending_ready = True
         pending_restart = False
         socketio.emit('level', 'ready_processing') 
         logger.info("Ready command received")
         
         goalCount = 0
         enemyGoalCount = 0
         owlFlewCount = 0
         storyBasketFlag = 0
         catchCount = 0
         timeoutCount = 0
         if basket_timeout_timer:
             basket_timeout_timer.cancel()
             basket_timeout_timer = None
         
         if go == 2 or go == 0:
               socketio.emit('clear_check_flags', to=None)
               
               # --- ИСПРАВЛЕНИЕ: ПОЛНАЯ ОЧИСТКА ИСТОРИИ ---
               # Удаляем все старые данные, чтобы проверка началась с чистого листа.
               # Это решает проблему "зависших" ошибок.
               socklist.clear() 
               socklist.append('ready') # Добавляем метку, что мы в режиме ready
               # -------------------------------------------

               # 1. Проверяем ESP 
               test_esp32()

               # --- СИНХРОНИЗАЦИЯ ЯЗЫКА на ESP32 ---
               # ESP32 после перезагрузки сервера стартуют с языком по умолчанию (Русский).
               # Отправляем текущий язык из файла 4.txt на все устройства каждый раз при Ready.
               lang_cmd = f"language_{language}"
               send_esp32_command(ESP32_API_WOLF_URL, lang_cmd)
               send_esp32_command(ESP32_API_TRAIN_URL, lang_cmd)
               send_esp32_command(ESP32_API_SUITCASE_URL, lang_cmd)
               send_esp32_command(ESP32_API_SAFE_URL, lang_cmd)
               logger.info(f"Language synced to ESP32 on Ready: {lang_cmd}")
               # ------------------------------------

               # 2026-05-27: Гасим LED на всех ESP32 чтобы устройства не светились
               # в режиме Ready (когда монтажник/оператор готовит квест).
               send_esp32_command(ESP32_API_WOLF_URL, "day_off")
               send_esp32_command(ESP32_API_TRAIN_URL, "day_off")
               send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
               send_esp32_command(ESP32_API_SAFE_URL, "day_off")
               logger.info("day_off sent to all ESP32 on Ready")

               # 2. ОТПРАВЛЯЕМ 'ready' на Arduino.
               serial_write_queue.put('ready')
               serial_write_queue.put('ready')
               serial_write_queue.put('ready')

               # 3. Ждем
               eventlet.sleep(5.5)
               
               # --- ФИНАЛЬНАЯ ПРОВЕРКА (без изменений) ---
               
               # 1. Проверка флагов (ищем в новом, чистом socklist)
               all_flag_triggers = [
                   "flag1_on", "flag2_on", "flag3_on", "flag4_on",
                   "workshop_flag1_on", "dog_flag3_on", "owls_flag4_on"
               ]
               for trig in all_flag_triggers:
                   if trig in socklist:
                       if 'Check Flags' not in devices: devices.append('Check Flags')
                       break 
               
               # 2. Проверка галетников (ПОЛНАЯ)
               # Мастерская (SUN)
               if "workshop_galet_on" in socklist:
                   if "Check SUN Switch" not in devices: devices.append("Check SUN Switch")
               
               # Совы (BOAT)
               if "owls_galet_on" in socklist:
                   if "Check BOAT Switch" not in devices: devices.append("Check BOAT Switch")
               
               # Собака (ROSE)
               if "dog_galet_on" in socklist:
                   if "Check ROSE Switch" not in devices: devices.append("Check ROSE Switch")
               
               # Баскетбол (WAVE)
               if "basket_galet_on" in socklist:
                   if "Check WAVE Switch" not in devices: devices.append("Check WAVE Switch")

               # Центральный (TREE)
               if "galet_on" in socklist or "central_galet_on" in socklist:
                   if "Check TREE Switch" not in devices: devices.append("Check TREE Switch")
                   
               # 3. Проверка стартовой двери
               if "open_door" in socklist:
                   if "Check Start Door" not in devices: devices.append("Check Start Door")
               # -----------------------------------------------------------

               # 4. Теперь проверяем 'devices'
               if len(devices) == 0:
                    # --- ВСЕ В ПОРЯДКЕ ---
                    logger.info("Ready: OK")
                    socklist.clear()
                    socketio.emit('level', 'modal_end', to=None)
                    socketio.emit('level', 'ready',to=None)
                    
                    send_esp32_command(ESP32_API_WOLF_URL, "ready")
                    send_esp32_command(ESP32_API_TRAIN_URL, "ready")
                    send_esp32_command(ESP32_API_SUITCASE_URL, "ready")
                    send_esp32_command(ESP32_API_SAFE_URL, "ready")
                    
                    go=3 
                    starts = 3
                    socklist.append('ready')
                    channel3.stop() 
                    stop_all_effects()
                    pygame.mixer.music.stop()
                    global play_ready_music
                    if play_ready_music:
                        play_background_music("fon1.mp3", loops=-1)
                    # Читаем файл прямо в момент нажатия Ready
                    play_r_music = True
                    try:
                        if os.path.exists('ready_music.txt'):
                            with open('ready_music.txt', 'r') as f:
                                play_r_music = (f.read().strip() == "1")
                    except:
                        pass
                    
                    if play_r_music:
                        play_background_music("fon1.mp3", loops=-1)
                    logger.debug("State changed: System is ready for game start.")
               else:
                    # --- НАЙДЕНЫ ОШИБКИ ---
                    logger.warning(f"Ready: FAILED. Devices: {devices}")
                    
                    final_string = ', '.join(str(device) for device in set(devices))
                    socketio.emit('devices', final_string, to=None)
                    
         socketio.emit('level', 'ready_finished')
         is_processing_ready = False

#данный декоратор срабатывает каждые 100 мс сюда заносятся файлы которые очень легко могут потеряться например кнопки если быстро нажимать и отправляем данные по игре
@socketio.on('Game')
def checkQuesst(receivedData):
     global flag
     global socklist
     global go 
     global rateTime
     global hintCount
     global rating
     global star
     
     # 1. Выносим отправку громкости и 'devices' из цикла
     socketio.emit('volume', str(phoneLevel))
     socketio.emit('volume1', str(effectLevel))
     socketio.emit('volume2', str(voiceLevel))
     socketio.emit('wolf', str(wolfLevel))
     socketio.emit('platform', str(trainLevel))
     socketio.emit('suitcases', str(suitcaseLevel))
     socketio.emit('safe', str(safeLevel))
     
     if go==2:
          socketio.emit('level', 'rest',to=None)  
     if go==3:
          socketio.emit('level', 'ready',to=None)      
     if go == -1:
          socketio.emit('level', 'pause_game',to=None)
     if flag=="HIGH":
          socketio.emit('level', 'HIGH',to=None)
          
     if flag=="NORMAL":
          socketio.emit('level', 'NORMAL',to=None)
     if flag=="LOW":
          socketio.emit('level', 'LOW',to=None)
     # --- ИЗМЕНЕНИЕ: Закомментирован цикл - он вызывает 'state-storm' и race conditions ---
     # --- Синхронизация истории теперь происходит ОДИН РАЗ в @socketio.on('connect') ---
     # for i in socklist:
     #
     #    socketio.emit('level', i ,to=None)

     
     
          
     socketio.emit('rate', rateTime,to=None)
     socketio.emit('hintCount', str(hintCount),to=None)
     #rating = rating+hintCount
     if rating<=75:
          star = 5
     elif rating<=99 and rating>75:
          star = 4
     elif rating <=123 and rating>99:
          star = 3
     elif rating <=150 and rating>123:
          star = 2
     elif rating>=180:
          star = 1                    
     socketio.emit('rating', str(star),to=None) 
     time.sleep(0)
     #print(rating)    

#метод проверяющий строку на число показывает напряжение
def is_number(str):
    try:
        float(str)
        return True
    except ValueError:
        return False  

def play_story(audio_source, loops=0, volume_file='3.txt'):
    # --- ОБНОВЛЕННАЯ ЛОГИКА: ЛЕНИВАЯ ЗАГРУЗКА ---
    global story_fade_active, phoneLevel, go
    
    sound_object = None
    filename_for_log = "Unknown"

    # 1. Проверяем, что нам передали: строку (имя файла) или объект Sound
    if isinstance(audio_source, str):
        # Если это строка, значит нужно загрузить файл сейчас
        filename_for_log = audio_source
        try:
            # Загружаем аудио в память только сейчас
            sound_object = pygame.mixer.Sound(audio_source)
        except Exception as e:
            logging.error(f"Ошибка загрузки файла истории {audio_source}: {e}")
            serial_write_queue.put('soundoff')
            return
    elif isinstance(audio_source, pygame.mixer.Sound):
        # Если это уже объект (для совместимости), используем как есть
        sound_object = audio_source
        # Пытаемся найти имя для лога (если есть в карте)
        filename_for_log = _SOUND_NAME_MAP.get(audio_source, "Sound Object")
    else:
        logging.error("play_story: получен неверный тип аудио источника")
        return

    logging.info(f"PLAY [История/Подсказка]: {filename_for_log}")

    # СТРАХОВКА: Если канал историй молчит, значит фейд точно не активен.
    if not channel3.get_busy():
        story_fade_active = False
    
    # 2. Приглушаем фоновую музыку (только если она еще не приглушена)
    if not story_fade_active and go == 1:
        story_fade_active = True
        serial_write_queue.put('soundon')
        
        # Получаем текущий phoneLevel
        try:
            with open('1.txt', 'r') as f:
                phoneLevel = float(f.read(4))
        except Exception as e:
            logger.error(f"Ошибка чтения файла громкости 1.txt (в play_story): {e}")
            pass 

        temp_vol = phoneLevel 
        target_vol = 0.1 # Целевая громкость 10%
        
        while temp_vol > target_vol and go == 1:
            temp_vol = round(temp_vol, 2) - 0.01
            if temp_vol < target_vol: temp_vol = target_vol 
            pygame.mixer.music.set_volume(round(temp_vol, 2))
            eventlet.sleep(0.05) 
        
        eventlet.sleep(0.1)
    
    # 3. Воспроизводим
    if sound_object:
        # ИСПРАВЛЕНИЕ: Читаем громкость из файла и применяем ТОЛЬКО через канал.
        # Раньше громкость ставилась и на sound_object, и на channel3 (через Voice handler),
        # что приводило к двойному умножению: эффективная громкость = sound.vol × channel.vol.
        # Теперь sound.volume всегда 1.0, а channel3.set_volume() ставится сразу после play().
        try:
            with open(volume_file, 'r') as f:
                volume = float(f.read(4))
        except Exception as e:
            logger.error(f"Ошибка чтения файла громкости {volume_file} (в play_story): {e}")
            volume = 0.5  # Безопасное значение по умолчанию
        
        sound_object.set_volume(1.0)  # Нейтральная громкость на объекте
        channel3.play(sound_object, loops=loops)
        channel3.set_volume(volume, volume)  # Громкость только через канал
            
def effects_are_busy():
    """Возвращает True, если играет ЛЮБОЙ из каналов эффектов"""
    for ch in effects_pool:
        if ch.get_busy():
            return True
    return False

def play_effect(audio_file, loops=0, volume_file='2.txt'):
    global current_effect_index
    
    # --- Улучшено логирование эффектов ---
    try:
        audio_name = _SOUND_NAME_MAP.get(audio_file, "Неизвестный аудиофайл эффекта")
        logging.info(f"PLAY [Эффект]: {audio_name}")
    except Exception as e:
        logging.error(f"Ошибка логирования имени эффекта: {e}")
        
    # --- НОВАЯ ЛОГИКА РАСПРЕДЕЛЕНИЯ КАНАЛОВ ---
    selected_channel = None

    # 1. Ищем первый СВОБОДНЫЙ канал в пуле
    for ch in effects_pool:
        if not ch.get_busy():
            selected_channel = ch
            break
    
    # 2. Если все заняты, берем следующий по кругу (Round Robin)
    # Это гарантирует, что звук не пропадет, а прервет самый "старый" из звучащих
    if selected_channel is None:
        selected_channel = effects_pool[current_effect_index]
        # Сдвигаем индекс для следующего раза
        current_effect_index = (current_effect_index + 1) % len(effects_pool)
        logging.debug(f"Все каналы эффектов заняты. Принудительно используем канал {current_effect_index}")

    # 3. Воспроизводим, затем сразу (без sleep) ставим громкость.
    # ИСПРАВЛЕНИЕ: Сначала нейтрализуем громкость на Sound-объекте (1.0),
    # чтобы эффективная громкость = 1.0 × channel.volume, без двойного умножения.
    # Читаем громкость из файла ДО play(), чтобы минимизировать задержку между play и set_volume.
    try:
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
    except Exception as e:
        logger.error(f"Ошибка чтения файла громкости эффекта: {e}")
        volume = 0.5  # Безопасное значение по умолчанию
    
    audio_file.set_volume(1.0)  # Нейтральная громкость на объекте
    selected_channel.play(audio_file, loops=loops)
    selected_channel.set_volume(volume, volume)  # Громкость только через канал
        
def stop_all_effects():
    """Останавливает звук на всех каналах эффектов"""
    for ch in effects_pool:
        ch.stop()

def send_esp32_command(api_url, command, timeout=6, max_retries=4, retry_delay=1, async_mode=True):

    def _send_command():
        current_delay = retry_delay
        for attempt in range(max_retries):
            try:
                response = requests.post(api_url, json=command, timeout=timeout)
                response.raise_for_status()
                logger.info(f"ESP32 command '{command}' to {api_url} successful.")
                return response
            except RequestException as e:
                logger.error(f"ESP32 command '{command}' to {api_url} FAILED after all retries.")
                if attempt < max_retries - 1:
                    time.sleep(current_delay)
                    current_delay *= 2
                else:
                    return None
    
    if async_mode:
        # Запуск в отдельном потоке
        thread = threading.Thread(target=_send_command)
        thread.daemon = True
        thread.start()
        return thread
    else:
        # Синхронный вызов
        return _send_command()      

def test_esp32():
     # Добавляет ошибки ESP в глобальный список.
    global devices
    device_configs = [
        ("A wolf in the forest", ESP32_API_WOLF_URL),
        ("Platform", ESP32_API_TRAIN_URL),
        ("Suitcases", ESP32_API_SUITCASE_URL),
        ("Golden safe", ESP32_API_SAFE_URL)
    ]
    
    #devices = []  # Недоступные устройства
    success_count = 0

    for device_name, url in device_configs:
        try:
            response = requests.post(url, json="hc", timeout=5)
            response.raise_for_status()
            success_count += 1
            # --- Если ESP заработал, но был в списке ошибок, удаляем ---
            if device_name in devices:
                devices.remove(device_name)
        except RequestException as e:
            # --- Добавляем, только если еще не в списке ---
            if device_name not in devices:
                devices.append(device_name)

    # Результат
    
    logger.info(f"Hardware check: {success_count}/4 ESP32 devices available.")
    logger.debug(f"Hardware check: Raw 'devices' list: {devices}")
    final_string = ', '.join(str(device) for device in set(devices))
    logger.debug(f"Hardware check: Final string for UI: '{final_string}'")
    socketio.emit('devices',final_string ,to=None)
    return success_count == 4 and len(devices) == 0 
                
def _send_command_internal(api_url, command, timeout=6, max_retries=4, retry_delay=1):
    """Внутренняя функция: выполняет фактический HTTP-запрос с повторами."""
    current_delay = retry_delay
    for attempt in range(max_retries):
        try:
            # ИЗМЕНЕНО: Улучшенное логирование исходящих команд на ESP32
            # Логируем ПЕРЕД отправкой
            description = EVENT_DESCRIPTIONS.get(command, '-')
            logging.info(f"SENT [ESP32 - {api_url}]: {description} (RAW: {command})")
            
            response = requests.post(api_url, json=command, timeout=timeout)
            response.raise_for_status()
            # Убираем старый лог, чтобы избежать дублирования
            # logger.debug(f"ESP32 command '{command}' to {api_url} successful.") 
            return response
        except RequestException as e:
            # Логируем ошибку с тем же уровнем детализации
            logging.error(f"ОШИБКА ОТПРАВКИ [ESP32 - {api_url}]: Команда {command} не удалась после {attempt + 1} попыток. Ошибка: {e}")
            if attempt < max_retries - 1:
                time.sleep(current_delay)
                current_delay *= 2
            else:
                logger.error(f"ESP32 command '{command}' to {api_url} FAILED after all retries.")
                return None

def send_esp32_command(api_url, command, debounce=False, delay=0.5):
    """
    Отправляет команду на ESP32.
    - debounce=False (default): Отправляет немедленно (для 'start', 'skip' и т.д.).
    - debounce=True: Использует дебаунсер (ТОЛЬКО для команд громкости).
    """
    
    if not debounce:
        # --- НЕМЕДЛЕННАЯ ОТПРАВКА (для 'start', 'skip', 'restart' и т.д.) ---
        # print(f"DEBUG: Немедленная отправка {command} на {api_url}")
        # Запускаем отправку в отдельном потоке, чтобы не блокировать сервер
        t = threading.Thread(target=_send_command_internal, args=(api_url, command))
        t.daemon = True
        t.start()
        return

    # --- ОТПРАВКА С ДЕБАУНСЕРОМ (только для громкости) ---
    global pending_commands, device_timers
    
    # 1. Сохраняем самую последнюю команду громкости
    pending_commands[api_url] = command
    
    # 2. Если таймер для этого URL уже запущен, ничего не делаем.
    if api_url in device_timers and device_timers[api_url].is_alive():
        # print(f"DEBUG: Дебаунсер активен. Команда {command} в очереди.")
        return

    # 3. Если таймера нет, создаем и запускаем его.
    def sender_worker(url, cmd_delay):
        global pending_commands, device_timers
        
        # Ждем "успокоения"
        time.sleep(cmd_delay)
        
        # Забираем последнюю команду из очереди
        command_to_send = pending_commands.pop(url, None)
        
        if command_to_send:
            # print(f"DEBUG: Дебаунсер сработал. Отправка {command_to_send} на {url}")
            _send_command_internal(url, command_to_send) # Вызываем отправку
        
        # Удаляем таймер из списка активных
        device_timers.pop(url, None)
    
    # print(f"DEBUG: Запуск дебаунсера для {api_url} с командой {command}")
    t_debouncer = threading.Thread(target=sender_worker, args=(api_url, delay))
    t_debouncer.daemon = True
    device_timers[api_url] = t_debouncer
    t_debouncer.start()
                
def play_background_music(music_file, volume_file='1.txt', loops=-1):
    # --- ИЗМЕНЕНО: Улучшено логирование фоновой музыки ---
    try:
        logging.info(f"PLAY [Фон]: {music_file}")
        # Загружаем, устанавливаем громкость ДО play(), чтобы не было вспышки
        pygame.mixer.music.load(music_file)
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
            pygame.mixer.music.set_volume(volume)
        pygame.mixer.music.play(loops)
            
    except FileNotFoundError:
        logger.error(f"Ошибка: файл {music_file} или {volume_file} не найден")
    except Exception as e:
        logger.error(f"Ошибка при воспроизведении музыки: {e}")

def check_story_and_fade_up():
    # Проверяет, не завершилась ли история, и восстанавливает громкость фона.
    # Вызывается в главном цикле serial().
    global story_fade_active, phoneLevel, go
    
    # Проверяем, был ли фон приглушен И канал3 (истории) теперь свободен
    if story_fade_active and not channel3.get_busy() and go == 1:
        
        # 1. Отправляем команду "черепу" (если нужно)
        serial_write_queue.put('soundoff')
        
        # 2. Плавно восстанавливаем громкость фона
        current_bg_vol = pygame.mixer.music.get_volume()
        
        # Перечитываем целевую громкость из файла (на случай, если ее изменили)
        try:
            with open('1.txt', 'r') as f:
                phoneLevel = float(f.read(4))
        except Exception as e:
            logger.error(f"Ошибка чтения файла громкости 1.txt (в check_story_fade_up, 1-я проверка): {e}")
            pass # Используем старое значение phoneLevel
        
        temp_vol = current_bg_vol
        while temp_vol < phoneLevel and go == 1:
            temp_vol = round(temp_vol, 2) + 0.01
            if temp_vol > phoneLevel: temp_vol = phoneLevel # Гарантия, что не превысим
            pygame.mixer.music.set_volume(round(temp_vol, 2))
            eventlet.sleep(0.01) # Быстрое восстановление
        
        # Устанавливаем точное значение (если игра не была остановлена)
        if go == 1:
            pygame.mixer.music.set_volume(phoneLevel)
        
        # 3. Сбрасываем флаг
        story_fade_active = False
        
basket_timeout_timer = None # Переменная для хранения таймера
timeoutCount = 0            # Счетчик для story_62

def handle_basket_timeout():
    """Вызывается если игрок долго не бросал мяч (тайм-аут)"""
    global timeoutCount, basket_timeout_timer
    
    # 1. Сбрасываем ссылку на таймер (он уже сработал или отменён вручную)
    basket_timeout_timer = None
    
    # 2. Логируем
    logging.info("BASKETBALL: Player timeout — story_62 triggered")

    # 3. Рандомный звук enemy_catch — мяч перехватил противник
    enemy_catch_sounds = [enemy_catch1, enemy_catch2, enemy_catch3, enemy_catch4]
    chosen_enemy_sound = random.choice(enemy_catch_sounds)
    play_effect(chosen_enemy_sound)
    logger.debug(f"BASKET TIMEOUT: Playing enemy catch sound: {_SOUND_NAME_MAP.get(chosen_enemy_sound, '?')}")

    # 4. Выбираем историю story_62 из списка по счётчику
    if timeoutCount < len(timeout_stories_base):
        base_name = timeout_stories_base[timeoutCount]
    else:
        base_name = timeout_stories_base[-1]
    
    play_localized_audio(base_name)
    logger.debug(f"BASKET TIMEOUT: Playing {base_name}")
            
    # 5. Увеличиваем счётчик с цикличным сбросом
    timeoutCount += 1
    if timeoutCount >= len(timeout_stories_base):
        timeoutCount = 0 
        
    # 6. Перезапускаем механику мяча на MAIN_BOARD
    serial_write_queue.put('start_game_basket')
    process_serial_queue()  # Принудительно выталкиваем команду сразу
    logging.info("BASKETBALL: Sent start_game_basket after timeout")

def send_command_with_confirmation(command, success_log_part, max_retries=3):
    """
    Отправляет команду и ждет подтверждения в логах от Arduino.
    Если подтверждения нет — пробует снова (max_retries раз).
    ФИКС багJ: добавлена проверка go==1 чтобы при pause/restart не зависать.

    2026-05-26: Логика retry уточнена. Серво на workshop крутится 3-5 сек,
    а старый таймаут 2.0 сек вызывал ложный retry → duplicate команда servo
    → серво начинал движение заново и недокручивал на половину
    (баг student_open half-turn). Теперь:
      - Базовый таймаут увеличен до 6.0 сек (с запасом на медленный серво).
      - Если Arduino уже ответил "<command>_start" — мы знаем что команда
        принята и выполняется → больше НЕ retry'им, просто ждём _success
        до 10 секунд. Только если даже _start не пришёл — делаем retry.
    """
    start_log_part = f"{command}_start"
    logging.info(f"HANDSHAKE: Starting sequence for '{command}'...")

    for attempt in range(max_retries):
        if go != 1:
            logging.info(f"HANDSHAKE ABORTED: game stopped (go={go})")
            return False

        # 1. Отправляем команду
        serial_write_queue.put(command)
        process_serial_queue()

        # 2. Ждем подтверждения
        start_wait = time.time()
        got_start = False
        wait_limit = 6.0
        while time.time() - start_wait < wait_limit:
            if go != 1:
                logging.info(f"HANDSHAKE ABORTED mid-wait: game stopped (go={go})")
                return False

            lines = serial_read_lines()
            for line in lines:
                if success_log_part in line:
                    logging.info(f"HANDSHAKE SUCCESS: Arduino confirmed '{command}' (Log: {line})")
                    return True
                if not got_start and start_log_part in line:
                    # Arduino принял команду — больше НЕ retry'им, ждём
                    # _success до 10 сек суммарно (серво может ехать долго).
                    got_start = True
                    wait_limit = 10.0
                    logging.info(f"HANDSHAKE START ACK: '{command}_start' received, waiting for success...")
                logging.info(f"RECEIVED [During Wait]: {line}")

            process_serial_queue()
            eventlet.sleep(0.05)

        if got_start:
            # _start был, но _success так и не пришёл — это не "Arduino не услышал",
            # это "Arduino делает, но не отрепортил". Retry не имеет смысла
            # (повторное servo прервёт текущее движение). Сдаёмся без retry.
            logging.warning(f"HANDSHAKE STALL: '{command}_start' получено, но _success нет за {wait_limit}s. НЕ retry (избегаем duplicate servo).")
            return False
        logging.warning(f"HANDSHAKE TIMEOUT: No confirmation for '{command}' (Attempt {attempt+1}/{max_retries})")

    logging.error(f"HANDSHAKE FAILED: '{command}' was sent {max_retries} times but not confirmed.")
    return False

#здесь уже обрабатываем все сообщения приходящие из меги и отображаем на пульте
def serial():
     global pending_restart, pending_ready, last_system_cmd_time
     global lesson_intro_seq
     global flag
     global mus
     global mansard_galets, last_mansard_count
     global socklist 
     global devices
     global ser

     train_stage_2_active = False
     flag = "0"
     mus = 1
     global go
     global name
     global language 
     global starts
     global flagS
     global sound
     global rateTime
     global hintCount
     global rating
     global star
     global fs
     global effectLevel
     global voiceLevel
     global phoneLevel
     global story_fade_active
     global caveCounter
     global storyBasketFlag
     global catchCount
     global enemyCatchCount
     global sintchEnemyCatchCount
     global redSintchEnemyCatchCount
     global redClickSintchEnemyCatchCount
     global last_boy_in_time
     global story13Flag 
     global fire0Flag
     global fire1Flag
     global fire2Flag
     global devices
     global goalCount
     global enemyGoalCount
     global owlFlewCount
     global last_owl_flew_time
     global level_18_intro_played
     global last_lesson_goal_time
     global basket_timeout_timer
     global arduino_level
     lesson_start_process_active = False
     effectLeveltmp = 0
     voiceLeveltmp = 0
     phoneLeveltmp = 0
     b = 0
     с = 0
     fs = 0
     a20 = 0.0
     a4=0.0
     a10 = 0.0
     f5=0.0 
     nextTrack = 0
     #алгоритм на понижение громкости работает хитро сорян за имена переменных лучше его не трогай намучаешься капец сам делал долго связано в округлением данных float и урпавлением во время эффекта
     #если нужно быстрее или медленне измени значения sleep
     while True:
          try:
              # Если идет прошивка Главной платы, ставим цикл на паузу
              if is_system_flashing:
                  eventlet.sleep(1)
                  continue
                  
              check_story_and_fade_up() # Проверяем, не закончила ли история играть
              process_serial_queue()
              # --- МОНИТОРИНГ СЕРВЕРА ---
              # 1. Проверяем очередь Python (Лаг логики)
              q_size = serial_write_queue.qsize()
              if q_size > 5: # Если скопилось больше 5 команд
                  logger.warning(f"⚠️ SERVER LAG: В очереди {q_size} команд. Сервер не успевает отправлять!")

              # 2. Проверяем буфер USB (Физический затор)
              try:
                  # out_waiting показывает байты, которые застряли в Linux и не ушли в кабель
                  usb_buffer = ser.out_waiting 
                  if usb_buffer > 100: # Если больше 100 байт ждут отправки
                       logger.warning(f"⚠️ USB CABLE JAM: {usb_buffer} байт застряло на выходе. Скорость 9600 не справляется!")
              except:
                  pass 
              # ---------------------------
              try:
                  message_to_send = serial_write_queue.get_nowait()
                  # Улучшенное логирование исходящих команд на Arduino
                  description = EVENT_DESCRIPTIONS.get(message_to_send, '-')
                  tag = get_device_tag(message_to_send)
                  logging.info(f'SENT {tag}: {description} (RAW: {message_to_send})')
                  ser.write(str.encode(message_to_send + '\n'))
                  # Мы даем Arduino 50-100 мс, чтобы обработать команду,
                  # прежде чем слать следующую из очереди.
                  eventlet.sleep(0.05)
              except eventlet.queue.Empty:
                  pass # Если очередь пуста, ничего не делаем
              #---- иногда для ассинхрона нужно добавлять eventlet.sleep(0)для переключения на другой метод
              eventlet.sleep(0.01)
              if pygame.mixer.music.get_busy() == False:
                   if nextTrack == 1:
                        play_background_music("fon8.mp3", loops=-1)
                        # ОПТИМИЗИРОВАНО
                        play_localized_audio("story_11")
                        nextTrack = 0

                              #----активируем игру
                        socketio.emit('level', 'active_pedlock',to=None)
                        socklist.append('active_pedlock')
                        socketio.emit('level', 'active_owl',to=None)
                        socklist.append('active_owl')
                        socketio.emit('level', 'active_cat',to=None)
                        socklist.append('active_cat')
                        socketio.emit('level', 'active_projector',to=None)
                        socklist.append('active_projector')
                   
              # Добавлен 'elif' для сброса флагов при неактивной игре (restart/pause) ---
              elif go != 1:
                   fs = 0      # Сбрасываем флаг, чтобы fade-in не запустился
                   flagS = 0   # Сбрасываем флаг, чтобы 'soundoff' не отправился
                   a20 = phoneLevel # Сбрасываем громкость

              # --- БУФЕРИЗИРОВАННОЕ ЧТЕНИЕ SERIAL (ЗАЩИТА ОТ ОБРЕЗКИ И ПОВРЕЖДЕНИЯ) ---
              # Вместо ser.readline() (который может вернуть неполную строку),
              # читаем все доступные байты и обрабатываем только полные строки.
              serial_lines = serial_read_lines()
              for line in serial_lines:
                   flag = line
                   
                   # --- ВОССТАНОВЛЕНИЕ ПОВРЕЖДЕННЫХ КОМАНД ---
                   # Проверяем, не повреждена ли команда (1-2 символа отличия от критической)
                   recovered = try_recover_corrupted_command(flag)
                   if recovered:
                       flag = recovered
                   
                   if flag == "QUEST_SYSTEM_READY":
                       pending_ready = False
                       # Сигнал boot-watchdog'у что Mega успешно загрузилась
                       globals()['mega_initial_boot_received'] = True
                       logger.info("SYSTEM CHECK COMPLETE: Playing startup sound.")
                       play_effect(on_sound)

                   # --- DIAG snapshot от Mega (или прокинутый от башни) ---
                   # Формат: "DIAG_SNAPSHOT:<device>:{...JSON...}"
                   # device = main | workshop | basket | dog | owls
                   if isinstance(flag, str) and flag.startswith("DIAG_SNAPSHOT:"):
                       try:
                           rest = flag[len("DIAG_SNAPSHOT:"):]
                           colon = rest.find(':')
                           if colon > 0:
                               device = rest[:colon]
                               body   = rest[colon+1:]
                               snap = json.loads(body)
                               socketio.emit(f'{device}_diag_state', snap)
                               _log_diag_snapshot(device, snap)
                       except Exception as _e:
                           logger.debug(f"DIAG snapshot parse failed: {_e}")
                       continue

                   # --- Heartbeat башен (ответ Mega на check_towers) ---
                   # Формат: "tower_status:workshop=1,basket=0,dog=1,owls=1"
                   if isinstance(flag, str) and flag.startswith("tower_status:"):
                       try:
                           body = flag[len("tower_status:"):]
                           parts = dict(kv.split('=', 1) for kv in body.split(',') if '=' in kv)
                           prev = dict(_tower_status_snapshot)
                           for tower in ('workshop', 'basket', 'dog', 'owls'):
                               if tower in parts:
                                   _tower_status_snapshot[tower] = (parts[tower].strip() == '1')
                           globals()['_tower_status_updated_at'] = time.time()
                           # Логируем raw snapshot на DEBUG (детальная диагностика);
                           # пользовательские ALERT'ы о состоянии каждой башни уже
                           # эмитятся в _log_status_changes() с понятными лейблами.
                           if prev != _tower_status_snapshot:
                               logger.debug(f"TOWERS: {_tower_status_snapshot}")
                           # Broadcast в /tech диаг-панель (heartbeat 5/сек)
                           socketio.emit('tech_tower_heartbeat',
                                         dict(_tower_status_snapshot), to=None)
                       except Exception as _e:
                           logger.debug(f"Failed to parse tower_status '{flag}': {_e}")
                       # Это служебное сообщение — не пропускаем дальше в общую логику
                       continue

                   flag_on_commands = ["workshop_flag1_on", "dog_flag3_on", "owls_flag4_on"]
                   flag_off_commands = ["workshop_flag1_off", "dog_flag3_off", "owls_flag4_off"]
                   
                   # --- БЛОК ПЕРЕВОДА ГАЛЕТНИКОВ (ARDUINO -> SERVER) ---
                   
                   # 1. Central (Tree) - galet1
                   if flag == "galet1": 
                       flag = "central_galet_on"
                       if "central_galet_on" not in socklist: socklist.append("central_galet_on")
                   if flag == "galet1_off": 
                       flag = "central_galet_off" # <--- ВАЖНО: Переименовываем
                       if "central_galet_on" in socklist: socklist.remove("central_galet_on")

                   # 2. Workshop (Sun) - galet2
                   if flag == "galet2": 
                       flag = "workshop_galet_on"
                       if "workshop_galet_on" not in socklist: socklist.append("workshop_galet_on")
                       socketio.emit('level', 'workshop_galet_on', to=None)
                   if flag == "galet2_off":
                       flag = "workshop_galet_off" # <--- ВАЖНО: Переименовываем
                       if "workshop_galet_on" in socklist: socklist.remove("workshop_galet_on")
                       socketio.emit('level', 'workshop_galet_off', to=None)

                   # 3. Basket (Wave/Moon) - galet3
                   if flag == "galet3": 
                       flag = "basket_galet_on"
                       if "basket_galet_on" not in socklist: socklist.append("basket_galet_on")
                   if flag == "galet3_off":
                       flag = "basket_galet_off" # <--- ВАЖНО: Переименовываем
                       if "basket_galet_on" in socklist: socklist.remove("basket_galet_on")

                   # 4. Dog (Rose) - galet4
                   if flag == "galet4": 
                       flag = "dog_galet_on"
                       if "dog_galet_on" not in socklist: socklist.append("dog_galet_on")
                       socketio.emit('level', 'dog_galet_on', to=None)
                   if flag == "galet4_off":
                       flag = "dog_galet_off" # <--- ВАЖНО: Переименовываем
                       if "dog_galet_on" in socklist: socklist.remove("dog_galet_on")
                       socketio.emit('level', 'dog_galet_off', to=None)

                   # 5. Owls (Boat) - galet5
                   if flag == "galet5": 
                       flag = "owls_galet_on"
                       if "owls_galet_on" not in socklist: socklist.append("owls_galet_on")
                       socketio.emit('level', 'owls_galet_on', to=None)
                   if flag == "galet5_off":
                       flag = "owls_galet_off" # <--- ВАЖНО: Переименовываем
                       if "owls_galet_on" in socklist: socklist.remove("owls_galet_on")
                       socketio.emit('level', 'owls_galet_off', to=None)
                   # ----------------------------------------------------

                   # Обработка выключения (удаляем из истории)
                   if flag == "galet1_off": 
                       if 'central_galet_on' in socklist: socklist.remove('central_galet_on')
                   if flag == "galet2_off":
                       if 'workshop_galet_on' in socklist: socklist.remove('workshop_galet_on')
                   if flag == "galet3_off":
                       if 'basket_galet_on' in socklist: socklist.remove('basket_galet_on')
                   if flag == "galet4_off":
                       if 'dog_galet_on' in socklist: socklist.remove('dog_galet_on')
                   if flag == "galet5_off":
                       if 'owls_galet_on' in socklist: socklist.remove('owls_galet_on')
                   # ----------------------------------------------------

                   if flag in flag_on_commands:
                       base_command = flag.replace('_on', '')
                       socketio.emit('level', f'{base_command}_on', to=None)
                       if f'{base_command}_on' not in socklist: socklist.append(f'{base_command}_on')
                       if f'{base_command}_off' in socklist: socklist.remove(f'{base_command}_off')
                       logging.debug(f"Processed {flag}")

                   if flag in flag_off_commands:
                       base_command = flag.replace('_off', '')
                       socketio.emit('level', f'{base_command}_off', to=None)
                       if f'{base_command}_off' not in socklist: socklist.append(f'{base_command}_off')
                       if f'{base_command}_on' in socklist: socklist.remove(f'{base_command}_on')
                       logging.debug(f"Processed {flag}")
                   # --------------------------------------------------------
                   # --- ПРОВЕРКА ПЕРЕПОЛНЕНИЯ ---
                   if "BUFFER CRITICAL" in flag:
                       logger.critical(f"🔥🔥🔥 {flag} 🔥🔥🔥") # Красные эмодзи для заметности
                       # Можно даже отправить звук ошибки на колонки, если хотите:
                       # play_effect(timeout) 
                   # -----------------------------------
                   logger.debug(f"Raw serial data received: {line}")
                   if recovered:
                       logger.debug(f"Recovered from corruption: '{line}' → '{flag}'")
                   eventlet.sleep(0.1)
                   # ИЗМЕНЕНО: Улучшенное логирование входящих сообщений от Arduino
                   description = EVENT_DESCRIPTIONS.get(flag, '-')
                   tag = get_device_tag(flag)
                   # Периодические heartbeat'ы — на DEBUG (не пишутся в файл-лог).
                   # 2026-05-26: добавлены фильтры спама от башен (log:*:Received command: ping,
                   # CMD: check_state, CMD: ping_main), чтобы не засорять INFO-лог
                   # и не забивать UI-терминал на /tech.
                   is_noise = (
                       isinstance(flag, str) and (
                           flag.startswith('log:main:HB ') or
                           'Received command: ping' in flag or
                           'Received command: chec' in flag or  # ловит "check_state", "check_towers" (truncated)
                           'CMD: ping_main' in flag or
                           'CMD: check_state' in flag or
                           flag == 'pong'
                       )
                   )
                   if is_noise:
                       logging.debug(f'RECEIVED {tag}: {description} (RAW: {flag})')
                   else:
                       logging.info(f'RECEIVED {tag}: {description} (RAW: {flag})')

                   # Broadcast в /tech диагностическую панель — read-only мониторинг
                   # серийки без модификации Mega-кода. Каждое сообщение прилетает
                   # клиенту с device-tag для фильтрации в UI.
                   if isinstance(flag, str) and not flag.startswith('log:main:HB '):
                       _device = tag.strip('[]').lower().replace(' board', '').replace(' ', '_')
                       try:
                           socketio.emit('tech_serial_log',
                                         {'device': _device, 'text': flag,
                                          'ts': time.time()}, to=None)
                       except Exception:
                           pass
                   # Бродкаст game level в Main Board diag tab
                   if isinstance(flag, str) and flag.startswith("level_"):
                       try:
                           socketio.emit('tech_game_level',
                                         {'level': int(flag.split('_')[1])}, to=None)
                       except Exception:
                           pass
                   # Логирование смены уровня ---
                   if flag.startswith("level_"):
                       try:
                           # Извлекаем номер уровня (например, "level_5" -> "5")
                           level_number = flag.split('_')[1]
                           # Логируем на уровне DEBUG (только в файл)
                           logger.debug(f"ARDUINO LEVEL: Main board transitioned to level {level_number}")
                           # --- ПОДТВЕРЖДЕНИЕ ОТ ARDUINO ---
                           if level_number == "25":
                               pending_restart = False
                               logger.info("RESTART УСПЕШНО ПОДТВЕРЖДЕН!")
                           if level_number == "26":
                               pending_ready = False
                               logger.info("READY УСПЕШНО ПОДТВЕРЖДЕН!")
                           if level_number == "12":
                               # Это означает, что level 11 (Library) был пройден
                               # (либо стуком, либо 'rrt3lck'). 
                               # 'punch' уже останавливает channel2, но 'rrt3lck' - нет.
                               # Эта проверка остановит звук в любом случае.
                               logger.info("Level 12 reached. Stopping ghost knock sound (channel 2).")
                               channel2.stop()
                           # Активация кнопок 3-х игр ТОЛЬКО по факту наступления уровня 5
                           if level_number == "5":
                               socketio.emit('level', 'active_suitcase', to=None)
                               socklist.append('active_suitcase')
                               socketio.emit('level', 'active_animals', to=None)
                               socklist.append('active_animals')
                               socketio.emit('level', 'active_wolf', to=None)
                               socklist.append('active_wolf')
                               logger.info("УРОВЕНЬ 5 НАСТУПИЛ: Кнопки Suitcase, Safe, Wolf активированы.")
                           # 0. Safe (Уровень 9)
                           if level_number == "9":
                               socketio.emit('level', 'active_safe', to=None)
                               socklist.append('active_safe')
                               logger.info("УРОВЕНЬ 9 НАСТУПИЛ: Кнопка Safe Open активирована.")
                           
                           # 1. Workshop (Уровень 10)
                           if level_number == "10":
                               socketio.emit('level', 'active_workshop', to=None)
                               socklist.append('active_workshop')
                               # Добавляем safe, чтобы он оставался активным, если нужно, или убираем, если он должен исчезнуть
                               socketio.emit('level', 'safe', to=None) 
                               socklist.append('safe')
                               logger.info("УРОВЕНЬ 10 НАСТУПИЛ: Кнопка Workshop активирована.")

                           # 2. 12 Hours / Library Clock (Уровень 11)
                           if level_number == "11":
                               socketio.emit('level', 'active_first_clock_2', to=None)
                               socklist.append('active_first_clock_2')
                               logger.info("УРОВЕНЬ 11 НАСТУПИЛ: Кнопка 12 hours активирована.")

                           # 3. Cup (Уровень 13)
                           if level_number == "13":
                               socketio.emit('level', 'active_cup', to=None)
                               socklist.append('active_cup')
                               logger.info("УРОВЕНЬ 13 НАСТУПИЛ: Кнопка Cup активирована.")
                           
                           # -----------------------------------------------
                       except Exception as e:
                           logger.warning(f"Could not parse level from Arduino: {flag}. Error: {e}")
                   # --- 
                   # --- Этот цикл также вызывает 'state-storm' и race conditions ---
                   # --- Он не нужен, т.к. @socketio.on('Game') шлет громкость, ---
                   # --- а эта функция (serial) сама отправляет 'level' (напр., 'open_door') ниже по коду ---
                   # for i in socklist:
                   #      #----- постоянно обновляем данные по громкости синхроним 
                   #      socketio.emit('volume', str(phoneLevel))
                   #      #     phoneLeveltmp = phoneLevel
                   #      #     print(phoneLevel)
                   #      ##eventlet.sleep(0.01)
                   #      #if effectLevel is not effectLeveltmp:
                   #      # ----постоянно обновляем данные по громкости синхроним 
                   #      socketio.emit('volume1', str(effectLevel))
                   #      #     effectLeveltmp = effectLevel
                   #      ##eventlet.sleep(0.01)
                   #      #if voiceLevel is not voiceLeveltmp:
                   #      # ----постоянно обновляем данные по громкости синхроним 
                   #      socketio.emit('volume2', str(voiceLevel))
                   #      socketio.emit('level', i ,to=None)
                   #проверяем если пришло значение в виде цифры отправляем на метод на клиенте volt
                   if is_number(flag):
                        socketio.emit('volt', flag,to=None)
                   #сперва ждем этого сообщения если оно придет тогда смело начинаем квест обработка аналогичная как и наверху это сообщение придет если все двери будут закрыты 
                   if flag == "startgo":
                         #-----очистим историю
                         socklist.clear()
                         lesson_start_process_active = False
                         train_stage_2_active = False
                         mansard_galets.clear()
                         last_mansard_count = 0
                         fire2Flag = 0
                         fire1Flag = 0
                         fire0Flag = 0
                         caveCounter = 0
                         storyBasketFlag = 0
                         catchCount = 0
                         enemyCatchCount = 0
                         sintchEnemyCatchCount = 0
                         redSintchEnemyCatchCount = 0
                         redClickSintchEnemyCatchCount = 0
                         story13Flag = 0
                         goalCount = 0
                         enemyGoalCount = 0
                         owlFlewCount = 0
                         level_18_intro_played = False
                         if language==1: 
                             send_esp32_command(ESP32_API_WOLF_URL, "language_1")
                             send_esp32_command(ESP32_API_TRAIN_URL, "language_1")
                             send_esp32_command(ESP32_API_SUITCASE_URL, "language_1")
                             send_esp32_command(ESP32_API_SAFE_URL, "language_1")
                         if language==2: 
                             send_esp32_command(ESP32_API_WOLF_URL, "language_2")
                             send_esp32_command(ESP32_API_TRAIN_URL, "language_2")
                             send_esp32_command(ESP32_API_SUITCASE_URL, "language_2")
                             send_esp32_command(ESP32_API_SAFE_URL, "language_2")
                         if language==3:  
                             send_esp32_command(ESP32_API_WOLF_URL, "language_3")
                             send_esp32_command(ESP32_API_TRAIN_URL, "language_3")
                             send_esp32_command(ESP32_API_SUITCASE_URL, "language_3")
                             send_esp32_command(ESP32_API_SAFE_URL, "language_3")
                         if language==4: 
                             send_esp32_command(ESP32_API_WOLF_URL, "language_4")
                             send_esp32_command(ESP32_API_TRAIN_URL, "language_4")
                             send_esp32_command(ESP32_API_SUITCASE_URL, "language_4")
                             send_esp32_command(ESP32_API_SAFE_URL, "language_4")
                         #----добавим в истори старт
                         socklist.append('50')
                         socklist.append('start_game')
                         socketio.emit('level', 'start_game',to=None) 
                         starts = 1
                         go = 1
                         send_esp32_command(ESP32_API_WOLF_URL, "start")
                         send_esp32_command(ESP32_API_TRAIN_URL, "start")
                         # 1. Отправляем 15 для сброса логики загадок (Hard Reset в train.ino)
                         send_esp32_command(ESP32_API_TRAIN_URL, "set_level_15")
                         
                         # 2. Читаем реальную громкость из файла 6.txt
                         try:
                             f6 = open('6.txt','r')
                             real_train_vol = f6.read(4)
                             f6.close()
                             # 3. Восстанавливаем громкость через небольшую паузу
                             eventlet.sleep(0.5) 
                             send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{scale_vol(real_train_vol)}")
                             logging.info(f"Train volume restored to {real_train_vol} after reset.")
                         except Exception as e:
                             logging.error(f"Error restoring train volume: {e}")
                         send_esp32_command(ESP32_API_SUITCASE_URL, "start")
                         send_esp32_command(ESP32_API_SAFE_URL, "start")
                         #----отправим на клиента старт
                         socketio.emit('level', 'start_game',to=None)
                         #-----играем фон
                         play_background_music("fon2.mp3", loops=-1)
                         eventlet.sleep(8.0)
                         #-----играем историю (ОПТИМИЗИРОВАНО)
                         play_localized_audio("story_1")

                         #-----меняем значение переменной
                         name = "start_story_1"    
                         
                         while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                         check_story_and_fade_up()
                         send_esp32_command(ESP32_API_TRAIN_URL, "train_light_off")
                         # ОПТИМИЗИРОВАНО
                         play_localized_audio("story_2_a")

                   #---режим для событий в ресте показывает что нужно вернуть на свои места
                   
                   # Добавляем проверку для "грязных" сообщений выключения
                   if "galet_off" in flag:
                       # Принудительно обрабатываем как выключение
                       socketio.emit('level', 'owls_galet_off', to=None) # Предполагаем Owls, т.к. они чаще всего шлют это
                       if 'owls_galet_off' not in socklist: socklist.append('owls_galet_off')
                       if 'owls_galet_on' in socklist: socklist.remove('owls_galet_on')

                       # И для Main Board (просто galet_on/off)
                       if 'galet_on' in socklist: socklist.remove('galet_on')

                       logging.debug(f"Detected 'galet_off' inside garbage: {flag}")
                   
                   if starts == 2 or starts == 0 or starts == 3:
                         

                         flags_active = False
                         all_flag_triggers = [
                             "flag1_on", "flag2_on", "flag3_on", "flag4_on",
                             "workshop_flag1_on", "dog_flag3_on", "owls_flag4_on"
                         ]
                         
                         # Проверяем, является ли текущее сообщение флагом ошибки
                         if flag in all_flag_triggers:
                             flags_active = True
                         
                         # Проверяем историю
                         if not flags_active:
                             for trig in all_flag_triggers:
                                 if trig in socklist:
                                     flags_active = True
                                     break
                         
                         if flags_active:
                            if 'Check Flags' not in devices:
                                devices.append('Check Flags')

                         # Проверка Галетников (Switches) с учетом текущего флага
                         if "workshop_galet_on" in socklist or flag == "workshop_galet_on":
                             if "Check SUN Switch" not in devices: devices.append("Check SUN Switch")
                         
                         if "owls_galet_on" in socklist or flag == "owls_galet_on":
                             if "Check BOAT Switch" not in devices: devices.append("Check BOAT Switch")
                             
                         if "dog_galet_on" in socklist or flag == "dog_galet_on":
                             if "Check ROSE Switch" not in devices: devices.append("Check ROSE Switch")
                         
                         if "galet_on" in socklist or flag == "galet_on":
                              if "Check TREE Switch" not in devices: devices.append("Check TREE Switch")

                         if flag == "open_door":
                              if 'close_door' in socklist:
                                       socklist.remove('close_door')
                              socketio.emit('level', 'open_door',to=None)
                              socklist.append('open_door')
                              if 'Check Start Door' not in devices:
                                devices.append('Check Start Door') 
                         if flag == "close_door":
                              if 'Check Start Door' in devices:
                                       devices.remove('Check Start Door')
                              if 'open_door' in socklist:
                                       socklist.remove('open_door')
                              socketio.emit('level', 'close_door',to=None)
                              socklist.append('close_door')   
                         # --- ВОССТАНОВЛЕНО: Обработка flag2 ---
                         if flag=="flag2_on":
                              while 'flag2_off' in socklist:
                                       socklist.remove('flag2_off')
                              socketio.emit('level', 'flag2_on',to=None)
                              socklist.append('flag2_on')
                         if flag=="flag2_off":
                              while 'flag2_on' in socklist:
                                       socklist.remove('flag2_on')
                              socketio.emit('level', 'flag2_off',to=None)
                              socklist.append('flag2_off')
                         # --- КОНЕЦ ---
                         # --- ИЗМЕНЕНО: Обработка уникальных galet-команд ---
                         if flag in ["workshop_galet_on", "owls_galet_on", "dog_galet_on"]:
                             tower_name = flag.split('_')[0] # "workshop", "owls", или "dog"
                             # Общая логика для UI (если нужна)
                             socketio.emit('level', f'{tower_name}_galet_on', to=None)
                             if f'{tower_name}_galet_on' not in socklist: socklist.append(f'{tower_name}_galet_on')
                             if f'{tower_name}_galet_off' in socklist: socklist.remove(f'{tower_name}_galet_off')
                             # Добавляем в логгер для диагностики
                             logging.debug(f"Processed {flag}")

                         if flag in ["workshop_galet_off", "owls_galet_off", "dog_galet_off"]:
                             tower_name = flag.split('_')[0]
                             socketio.emit('level', f'{tower_name}_galet_off', to=None)
                             if f'{tower_name}_galet_off' not in socklist: socklist.append(f'{tower_name}_galet_off')
                             if f'{tower_name}_galet_on' in socklist: socklist.remove(f'{tower_name}_galet_on')
                             logging.debug(f"Processed {flag}")
                         # --- КОНЕЦ ИЗМЕНЕНИЯ ---

                         if flag=="cristal_up":
                              if 'Check Crystals' not in devices:
                                devices.append('Check Crystals')
                              if 'crystals_down' in socklist:
                                       socklist.remove('crystals_down')
                              socketio.emit('level', 'crystals',to=None)
                              socklist.append('crystals')   

                         if flag=="cristal_down":
                              if 'Check Crystals' in devices:
                                       devices.remove('Check Crystals')
                              if 'crystals' in socklist:
                                       socklist.remove('crystals')
                              socketio.emit('level', 'crystals_down',to=None)
                              socklist.append('crystals_down')   

                         if flag=="boy_in":
                              if 'Check Kay' in devices:
                                       devices.remove('Check Kay')
                              
                              if 'start_players' in socklist:
                                    socklist.remove('start_players')
                              socketio.emit('level', 'stop_players_rest',to=None)
                              socklist.append('stop_players_rest')

                         if flag=="boy_out":
                              if 'Check Kay' not in devices:
                                devices.append('Check Kay')
                              if 'stop_players_rest' in socklist:
                                    socklist.remove('stop_players_rest')
                              socketio.emit('level', 'start_players',to=None)
                              socklist.append('start_players') 

                         if flag=="lib_door":
                              if 'Check Library' not in devices:
                                devices.append('Check Library')
                              if 'close_door_puzzle' in socklist:
                                    socklist.remove('close_door_puzzle')
                              socketio.emit('level', 'open_door_puzzle',to=None)
                              socklist.append('open_door_puzzle')

                         if flag=="lib_door_in":
                              if 'Check Library' in devices:
                                       devices.remove('Check Library')
                              if 'open_door_puzzle' in socklist:
                                    socklist.remove('open_door_puzzle')
                              socketio.emit('level', 'close_door_puzzle',to=None)
                              socklist.append('close_door_puzzle')

                         if flag=="safe_close":
                              if 'Check Bank Safe' in devices:
                                       devices.remove('Check Bank Safe')
                              if 'safe' in socklist:
                                    socklist.remove('safe')
                              socketio.emit('level', 'safe_close',to=None)
                              socklist.append('safe_close')

                         if flag=="safe_open":
                              if 'Check Bank Safe' not in devices:
                                devices.append('Check Bank Safe')
                              if 'safe_close' in socklist:
                                    socklist.remove('safe_close')
                              socketio.emit('level', 'safe',to=None)
                              socklist.append('safe')

                         if flag=="crime_close":
                              if 'Check Crime' in devices:
                                       devices.remove('Check Crime')
                              if 'crime' in socklist:
                                    socklist.remove('crime')
                              socketio.emit('level', 'crime_close',to=None)
                              socklist.append('crime_close')

                         if flag=="crime_open":
                              if 'Check Crime' not in devices:
                                devices.append('Check Crime')
                              if 'crime_close' in socklist:
                                    socklist.remove('crime_close')
                              socketio.emit('level', 'crime',to=None)
                              socklist.append('crime')
                         # --- Логика для "мальчика" (Kay) в режиме ожидания ---
                         if flag == "boy_out": # Мальчик ВЫНУТ
                             # 1. Воспроизводим звук ТОЛЬКО если игра не начата
                             if go != 1:
                                 play_effect(kay_out)
                             
                             # 2. Обновляем 'devices' (как в 'ready' проверке)
                             if 'Check Kay' not in devices:
                                 devices.append('Check Kay')
                         
                         if flag == "boy_in": # Мальчик ВСТАВЛЕН
                             # 1. Воспроизводим звук
                             play_effect(kay_in)
                             
                             # 2. Обновляем 'devices' (как в 'ready' проверке)
                             if 'Check Kay' in devices:
                                 devices.remove('Check Kay')
                                                                                    
                   #----если нажали на старт и пришло сообщение от меги что можно играть начинаем обрабатывать сообщения
                   if go == 1 and starts == 1:
                        #-----игроки открыли стартовую дверь
                         if flag == "dragon_crystal":
                              #----играем историю (ОПТИМИЗИРОВАНО)
                              play_localized_audio("story_2_b")

                         if flag == "dragon_crystal_repeat":
                              #----играем историю (ОПТИМИЗИРОВАНО)
                              play_localized_audio("story_2_r")

                         if flag == "open_door":
                              #----отправили на клиента
                              socketio.emit('level', 'open_door',to=None)
                              #-----добавили в список
                              socklist.append('open_door')
                              play_background_music("fon3.mp3", loops=-1)
                              #----играем эффект
                              play_effect(door_act)

                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1) 
                              #----играем историю (ОПТИМИЗИРОВАНО)
                              play_localized_audio("story_3")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              eventlet.sleep(1.1) 
                              serial_write_queue.put('kay_repeat') 
                              eventlet.sleep(1.1)     
                              #------активируем блок на пульте с тумблером
                              socketio.emit('level', 'active_first_clock',to=None)
                              socklist.append('active_first_clock')
                         if flag == "clock1":
                              #----шлем на клиента
                              socketio.emit('level', 'first_clock',to=None)
                              #----добавляем в историю
                              socklist.append('first_clock')
                              #----играем эффект
                              play_effect(h_clock)
                              #-----ждем окончания эффекта
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1) 
                              #----играем историю (ОПТИМИЗИРОВАНО)
                              play_localized_audio("story_3_a")

                              #-----изменяем переменную
                              name = "story_1"  
                              #-----активируем блок с галетниками
                              socketio.emit('level', 'active_second_clock',to=None)
                              socklist.append('active_second_clock') 
                         if flag == "clock2":
                              #----шлем на клиента
                              play_background_music("fon4.mp3", loops=-1)
                              send_esp32_command(ESP32_API_TRAIN_URL, "train_uf_light_on")
                              send_esp32_command(ESP32_API_TRAIN_URL, "ghost_knock")
                              socketio.emit('level', 'second_clock', to=None)
                              #----добавляем в историю
                              socklist.append('second_clock')
                              play_effect(uf_clock)
                              #-----ждем окончания эффекта
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1) 
                              #----играем историю (ОПТИМИЗИРОВАНО)
                              play_localized_audio("story_3_b")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)  

                              serial_write_queue.put('after_story_clock2')
                              eventlet.sleep(1.0)     
                              #-----изменяем переменную
                              name = "story_1"  
                              #-----активируем блок с галетниками
                              socketio.emit('level', 'active_open_mansard_door',to=None)
                              socklist.append('active_open_mansard_door') 
                         if flag == "steps":
                              #----играем эффект
                              play_effect(steps)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_3_c")
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              # Ждем лог "student_hide_success" от Arduino
                              send_command_with_confirmation('student_hide', 'student_hide_success')
                              # --- Проверяем, не активен ли уже 2 этап ---
                              if not train_stage_2_active:
                                  send_esp32_command(ESP32_API_TRAIN_URL, "stage_1")
                              else:
                                  logger.info("SKIP stage_1 because stage_2 is already active.")
                              eventlet.sleep(1.0)

                              send_esp32_command(ESP32_API_TRAIN_URL, "train_uf_light_off")
                              send_esp32_command(ESP32_API_TRAIN_URL, "train_light_on")
                              play_background_music("fon5.mp3", loops=-1)
                              play_localized_audio("story_4")
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              # --- Разрешаем Arduino начать игру ---
                              serial_write_queue.put('start_galet_logic') 
                              logger.info("SENT [Main Board]: start_galet_logic (Story 4 finished)")

                              #-----изменяем переменную
                         if flag == "kay_repeat":
                              play_localized_audio("story_3_r")

                         # --- Логика для Прогресс-бара Mansard Game (5 галетников) ---
                              
                         # 1. Определяем сигналы (согласно MAIN_BOARD_V5_COM5.ino)
                         galet_signals = {
                             "central_galet_on": "g1",   # Central (Tree)
                             "workshop_galet_on": "g2",  # Workshop (Sun)
                             "basket_galet_on": "g3",    # Basket (Moon)
                             "dog_galet_on": "g4",       # Dog (Rose)
                             "owls_galet_on": "g5"       # Owls (Boat)
                         }
                         galet_off_signals = {
                             "central_galet_off": "g1",  # Central Off
                             "workshop_galet_off": "g2", # Workshop Off
                             "basket_galet_off": "g3",   # Basket Off
                             "dog_galet_off": "g4",      # Dog Off
                             "owls_galet_off": "g5"      # Owls Off
                         }
                      
                         changed = False
                      
                         # 2. Проверяем, пришел ли сигнал ВКЛ
                         if flag in galet_signals:
                             if galet_signals[flag] not in mansard_galets:
                                 mansard_galets.add(galet_signals[flag])
                                 changed = True
                      
                         # 3. Проверяем, пришел ли сигнал ВЫКЛ
                         if flag in galet_off_signals:
                             if galet_off_signals[flag] in mansard_galets:
                                 mansard_galets.discard(galet_off_signals[flag])
                                 changed = True

                         # 4. Если состояние изменилось, обновляем UI
                         if changed:
                             # Блокируем любые пересчеты шкалы, если квест уже пройден на 100%
                             if 'mansard_progress_100' not in socklist:
                                 
                                 current_count = len(mansard_galets)
                              
                                 # Проверяем, отличается ли новое значение от старого (чтобы не спамить)
                                 if current_count != last_mansard_count:
                                     raw_percent = current_count * 20
                                     
                                     # ОГРАНИЧЕНИЕ: Не показываем 100% автоматически.
                                     # Максимум 80%, пока не придет команда 'mansard_finish'.
                                     if raw_percent >= 100:
                                         percent = 80
                                     else:
                                         percent = raw_percent
                                         
                                     event_name = f"mansard_progress_{percent}"
                                  
                                     # 4.1. Отправляем в SocketIO
                                     socketio.emit('level', event_name, to=None)
                                  
                                     # 4.2. Обновляем историю (socklist)
                                     # Сначала удаляем ВСЕ старые значения прогресса, чтобы избежать конфликтов
                                     for i in range(0, 120, 20):
                                         t_name = f"mansard_progress_{i}"
                                         while t_name in socklist:
                                             try:
                                                 socklist.remove(t_name)
                                             except ValueError:
                                                 pass
                                          
                                     socklist.append(event_name)
                                  
                                     # 4.3. Обновляем последнее известное значение
                                     last_mansard_count = current_count
                                     logger.debug(f"Mansard progress updated: {current_count} galets ({percent}%)")
                         
                         # ОБРАБОТЧИК ПОБЕДЫ
                         if flag == "mansard_finish":
                              if 'mansard_finish' in socklist:
                                  logger.debug("Игнорируем повторный mansard_finish")
                              else:
                                  socklist.append('mansard_finish')
                                  # 1. Принудительно ставим 100% на шкале
                                  socketio.emit('level', 'mansard_progress_100', to=None)
                                  
                                  # Чистим историю от старых процентов
                                  for i in range(0, 100, 20):
                                      t_name = f"mansard_progress_{i}"
                                      while t_name in socklist: socklist.remove(t_name)
                                  
                                  # Записываем 100% в историю
                                  if 'mansard_progress_100' not in socklist:
                                      socklist.append('mansard_progress_100')
                                  
                                  # Фиксируем состояние счетчика на 5
                                  mansard_galets.update(['g1', 'g2', 'g3', 'g4', 'g5'])
                                  last_mansard_count = 5 

                                  # 2. ЗАПУСКАЕМ СЦЕНАРИЙ ПОБЕДЫ
                                  train_stage_2_active = True
                                  play_background_music("fon6.mp3", loops=-1)
                                  
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  
                                  play_localized_audio("story_5")

                                  while channel3.get_busy()==True and go == 1: 
                                      eventlet.sleep(0.1)
                                  
                                  socketio.emit('level', 'open_mansard_door',to=None)
                                  socklist.append('open_mansard_door')
                                  
                                  ser.write(b'open_mansard_door\n')
                                  ser.flush()
                                  
                                  play_effect(door_attic)
                                  
                                  play_localized_audio("story_6")

                                  while channel3.get_busy()==True and go == 1: 
                                      eventlet.sleep(0.1)
                                  
                                  # Активируем следующие этапы
                                  send_esp32_command(ESP32_API_WOLF_URL, "game")
                                  send_esp32_command(ESP32_API_SUITCASE_URL, "game")
                                  send_esp32_command(ESP32_API_SAFE_URL, "game")
                                  send_esp32_command(ESP32_API_TRAIN_URL, "stage_2")

                         if flag=="three_game_end":
                              if 'three_game_end' in socklist:
                                  logger.debug("Игнорируем повторный three_game_end")
                              else:
                                  socklist.append('three_game_end')
                                  send_esp32_command(ESP32_API_TRAIN_URL, "flag_on")
                                  socketio.emit('level', 'active_open_mansard_stash',to=None)
                                  socklist.append('active_open_mansard_stash')
                                 
                        #---если пришло сообщение что поставили красный флаг проверяем не было ли в истории сообщения что флаг сняли если было удаляем из истории
                         if "flag1_on" in flag:
                              if 'flag1_off' in socklist:
                                       socklist.remove('flag1_off')
                             #----отправляем на клиента
                              socketio.emit('level', 'flag1_on',to=None)
                              #----добавили в историю
                              if 'flag1_on' not in socklist: socklist.append('flag1_on')
                              
                              # --- СТРАХОВКА ПОБЕДЫ: Если все 4 флага на месте, завершаем уровень ---
                              if 'flag1_on' in socklist and 'flag2_on' in socklist and 'flag3_on' in socklist and 'flag4_on' in socklist:
                                  serial_write_queue.put('m2lck')

                         if "flag2_on" in flag:
                              if 'flag2_off' in socklist:
                                       socklist.remove('flag2_off')
                              socketio.emit('level', 'flag2_on',to=None)
                              if 'flag2_on' not in socklist: socklist.append('flag2_on')
                              
                              # --- СТРАХОВКА ПОБЕДЫ ---
                              if 'flag1_on' in socklist and 'flag2_on' in socklist and 'flag3_on' in socklist and 'flag4_on' in socklist:
                                  serial_write_queue.put('m2lck')

                         if "flag3_on" in flag:
                              if 'flag3_off' in socklist:
                                       socklist.remove('flag3_off')
                              socketio.emit('level', 'flag3_on',to=None)
                              if 'flag3_on' not in socklist: socklist.append('flag3_on')
                              
                              # --- СТРАХОВКА ПОБЕДЫ ---
                              if 'flag1_on' in socklist and 'flag2_on' in socklist and 'flag3_on' in socklist and 'flag4_on' in socklist:
                                  serial_write_queue.put('m2lck')

                         if "flag4_on" in flag:
                              if 'flag4_off' in socklist:
                                       socklist.remove('flag4_off')
                              socketio.emit('level', 'flag4_on',to=None)
                              if 'flag4_on' not in socklist: socklist.append('flag4_on') 
                              
                              # --- СТРАХОВКА ПОБЕДЫ ---
                              if 'flag1_on' in socklist and 'flag2_on' in socklist and 'flag3_on' in socklist and 'flag4_on' in socklist:
                                  serial_write_queue.put('m2lck')

                         # --- Обработка снятия флагов (также ищем подстроку) ---
                         if "flag1_off" in flag:
                              if 'flag1_on' in socklist:
                                       socklist.remove('flag1_on')
                              socketio.emit('level', 'flag1_off',to=None)
                              if 'flag1_off' not in socklist: socklist.append('flag1_off')

                         if "flag2_off" in flag:
                              if 'flag2_on' in socklist:
                                       socklist.remove('flag2_on')
                              socketio.emit('level', 'flag2_off',to=None)
                              if 'flag2_off' not in socklist: socklist.append('flag2_off')

                         if "flag3_off" in flag:
                              if 'flag3_on' in socklist:
                                       socklist.remove('flag3_on')
                              socketio.emit('level', 'flag3_off',to=None)
                              if 'flag3_off' not in socklist: socklist.append('flag3_off')

                         if "flag4_off" in flag:
                              if 'flag4_on' in socklist:
                                       socklist.remove('flag4_on')
                              socketio.emit('level', 'flag4_off',to=None)
                              if 'flag4_off' not in socklist: socklist.append('flag4_off')
                        #-------закончили игру с флагами
                         if flag=="flagsendmr":
                              if 'flagsendmr' in socklist:
                                  logger.debug("Игнорируем повторный flagsendmr")
                              else:
                                  socklist.append('flagsendmr')
                                  #----играем эффект 
                                  pygame.mixer.music.stop()
                                  play_effect(flags)
                                  send_esp32_command(ESP32_API_TRAIN_URL, "flag_off")
                                  send_esp32_command(ESP32_API_TRAIN_URL, "stage_3")
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  play_background_music("fon7.mp3", loops=0) 
                                  # ОПТИМИЗИРОВАНО
                                  play_localized_audio("story_10")

                                  nextTrack = 1
                              

                         if "door_owl" in flag:
                              # Проверка на повтор: если 'owl' уже есть в истории, игнорируем
                              if 'owl' in socklist:
                                  logger.debug("Игнорируем повторный door_owl")
                              else:
                                  #----играем эффект 
                                  play_effect(door_owl)
                                  socketio.emit('level', 'owl',to=None)
                                  #-----добавили в историю
                                  socklist.append('owl')
                                  serial_write_queue.put('open_owl_door') 
                                  socketio.emit('level', 'active_owls', to=None)
                                  if 'active_owls' not in socklist:
                                      socklist.append('active_owls')
                                  send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
                                  send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks") # Отключаем клики
                                  #while effects_are_busy() and go == 1: 
                                      #eventlet.sleep(0.1)
                                  eventlet.sleep(2.0)
                                  if story13Flag == 0:
                                       story13Flag = 1
                                       play_localized_audio("story_13")
         
                                       while channel3.get_busy()==True and go == 1: 
                                            eventlet.sleep(0.1)
                                       
                                  play_localized_audio("story_14_a")

                                  # ФИКС: фоновый поток — Serial не блокируется пока играет story.
                                  # Бутылки и другие события обрабатываются мгновенно.
                                  def _after_story_14a():
                                      while channel3.get_busy() and go == 1:
                                          eventlet.sleep(0.1)
                                      send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks")
                                      socketio.emit('level', 'active_owls', to=None)
                                      socklist.append('active_owls')
                                  socketio.start_background_task(_after_story_14a)

                         if flag=="owl_flew":
                              # [FIX] Защита от дребезга звука (0.5 сек)
                              # Используем last_owl_flew_time (надо объявить глобально)
                              if time.time() - last_owl_flew_time > 0.5:
                                  last_owl_flew_time = time.time()
                                  #----играем эффект 
                                  play_effect(owl_flew)
                                  owlFlewCount += 1
                                  if owlFlewCount > 4: owlFlewCount = 4 # Ограничитель
                                  event_name = f'owl_flew_{owlFlewCount}'
                                  socketio.emit('level', event_name, to=None)
                                  socklist.append(event_name)
                                  #----активируем игру с совами

                         if flag=="owl_end":
                              if 'owls' in socklist:
                                  logger.debug("Игнорируем повторный owl_end")
                              else:
                                  #----играем эффект 
                                  play_effect(owl_flew)
                                  socketio.emit('level', 'owls',to=None)
                                  #-----добавили в историю
                                  socklist.append('owls')
                                  owlFlewCount = 4 # Гарантируем 100%
                                  socketio.emit('level', 'owl_flew_4', to=None)
                                  socklist.append(f'owl_flew_4')
                                  send_esp32_command(ESP32_API_TRAIN_URL, "owl_finish")
                                  play_localized_audio("story_14_b")

                         if flag=="door_witch":
                              #----играем эффект 
                              play_effect(door_witch)
                              socketio.emit('level', 'cat',to=None)
                              #-----добавили в историю
                              socklist.append('cat')
                              send_esp32_command(ESP32_API_TRAIN_URL, "fish_open")
                              send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks") # Отключаем клики
                              #while effects_are_busy() and go == 1: 
                              #    eventlet.sleep(0.1)
                              eventlet.sleep(2.0)
                              if story13Flag == 0:
                                   story13Flag = 1
                                   play_localized_audio("story_13")
     
                                   while channel3.get_busy()==True and go == 1: 
                                        eventlet.sleep(0.1)     

                              play_localized_audio("story_17")

                              send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks") # Включаем клики обратно
                              #----активируем игру
                              socketio.emit('level', 'active_open_potions_stash',to=None)
                              socklist.append('active_open_potions_stash')                   
                         #-----поставили первую бутылку парвильно    
                         if flag=="first_bottle":
                              #----бежим по истории ищем сообщения об ошибке с бутылками
                              for i in range(len(socklist)):
                                    if socklist[i] == 'mistake_bottle':
                                       b=b+1
                              #----удаляем столько раз сколько нашли         
                              for i in range(b):
                                  if 'mistake_bottle' in socklist:
                                       socklist.remove('mistake_bottle')
                              #-----еще раз на всякий         
                              if 'mistake_bottle' in socklist:
                                   socklist.remove('mistake_bottle')
                              #------отправляем на клиента     
                              socketio.emit('level', 'first_bottle',to=None)
                              #-----добавляем в историю
                              socklist.append('first_bottle')
                              #----играем эффект
                              play_effect(bottle1)
                         #-----поставили 2 правильную бутылку     
                         if flag=="second_bottle":
                              #-----отправили клиенту
                              socketio.emit('level', 'second_bottle',to=None)
                              #----добавили в историю
                              socklist.append('second_bottle')
                              #----играем эффект
                              play_effect(bottle2)
                         #------поставили 3 правильную бутылку     
                         if flag=="third_bottle":
                              socketio.emit('level', 'third_bottle',to=None)
                              socklist.append('third_bottle')
                              play_effect(bottle3)
                         #-----поставили последнюю бутылку правильно     
                         if flag=="four_bottle":
                              send_esp32_command(ESP32_API_TRAIN_URL, "fish_finish")
                              for i in range(len(socklist)):
                                    if socklist[i] == 'mistake_bottle':
                                       b=b+1
                              #----удаляем столько раз сколько нашли         
                              for i in range(b):
                                  if 'mistake_bottle' in socklist:
                                       socklist.remove('mistake_bottle')
                              #-----еще раз на всякий         
                              if 'mistake_bottle' in socklist:
                                   socklist.remove('mistake_bottle')
                              #-----отправили на клиента
                              socketio.emit('level', 'second_bottle',to=None)
                              #------добавили в историю
                              socklist.append('second_bottle')
                              socketio.emit('level', 'first_bottle',to=None)
                              #------добавили в историю
                              socklist.append('first_bottle')
                              socketio.emit('level', 'third_bottle',to=None)
                              #------добавили в историю
                              socklist.append('third_bottle')
                              socketio.emit('level', 'four_bottle',to=None)
                              #------добавили в историю
                              socklist.append('four_bottle')
                              #-----играем эффект другой
                              play_effect(bottle_end)

                              # FIX: Выносим ожидание в фоновый поток, чтобы не блокировать serial-цикл
                              def _after_four_bottle():
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  #------играем голос    
                                  play_localized_audio("story_18")
                                  while channel3.get_busy()==True and go == 1: 
                                      eventlet.sleep(0.1)    
                              socketio.start_background_task(_after_four_bottle)
                              #----активируем игру с метлой
                         #------сделали ошибку с бутылкой     
                         if flag=="mistake_bottle":
                              #----удалим из истории если были на каком либо этапе
                              if 'first_bottle' in socklist:
                                       socklist.remove('first_bottle')
                              if 'second_bottle' in socklist:
                                       socklist.remove('second_bottle')
                              if 'third_bottle' in socklist:
                                       socklist.remove('third_bottle')
                              if 'four_bottle' in socklist:
                                       socklist.remove('four_bottle')      
                              #------отпраялем клиенту                              
                              socketio.emit('level', 'mistake_bottle',to=None)
                              #------добавили в историю
                              socklist.append("mistake_bottle")
                              #-----играем эффект
                              play_effect(bottle_fall)   
       
                         if flag=="door_dog":
                              #----играем эффект 
                              socketio.emit('level', 'pedlock',to=None)
                              #-----добавили в историю
                              socklist.append('pedlock')
                              play_effect(door_dog)
                              send_esp32_command(ESP32_API_TRAIN_URL, "key_open")
                              send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks") # Отключаем клики
                              # Убираем ожидание завершения эффекта
                              # while effects_are_busy() and go == 1: 
                              #    eventlet.sleep(0.1)
                              
                              # Добавляем фиксированную задержку 2 секунды
                              eventlet.sleep(2.0)
     
                              if story13Flag == 0:
                                   story13Flag = 1
                                   play_localized_audio("story_13")
     
                                   while channel3.get_busy()==True and go == 1: 
                                        eventlet.sleep(0.1)

                              play_localized_audio("story_19")

                              # ФИКС: фоновый поток — Serial не блокируется пока играет story.
                              # Бутылки и другие события обрабатываются мгновенно.
                              def _after_story_19():
                                  while channel3.get_busy() and go == 1:
                                      eventlet.sleep(0.1)
                                  send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks")
                                  socketio.emit('level', 'active_dog', to=None)
                                  socklist.append('active_dog')
                              socketio.start_background_task(_after_story_19)

                         if flag=="dog_sleep":
                              dog_growl.stop()
                              #----играем эффект 
                              play_effect(dog_sleep)

                         if flag=="dog_growl":
                              #----играем эффект 
                              play_effect(dog_growl, loops=-1)
                         if flag=="dog_lock":
                              # Защита от двойного срабатывания
                              if 'dog_end_processed' in socklist:
                                  logger.debug("Игнорируем повторный dog_lock")
                              else:
                                  socklist.append('dog_end_processed') # Ставим метку
                                  # FIX: Останавливаем зацикленный dog_growl перед эффектом победы
                                  dog_growl.stop()
                                  #----играем эффект 
                                  socketio.emit('level', 'dog',to=None)
                                  #-----добавили в историю
                                  socklist.append('dog')
                                  send_esp32_command(ESP32_API_TRAIN_URL, "key_finish")
                                  play_effect(dog_lock)

                                  #while effects_are_busy() and go == 1: 
                                  #    eventlet.sleep(0.1)

                                  play_localized_audio("story_21")

                         if flag=="story_20_a":
                              play_localized_audio("story_20_a")

                         if flag=="story_20_b":
                              play_localized_audio("story_20_b")

                         if flag=="story_20_c":
                              play_localized_audio("story_20_c")

                         if flag=="story_22_a":
                              # 1. Ждем, пока канал освободится
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              # 2. Воспроизводим историю
                              play_localized_audio("story_22_a")
                                  
                              # 3. Ждем, пока PLAY ЗАКОНЧИТСЯ
                              while channel3.get_busy()==True and go == 1:
                                  eventlet.sleep(0.1)
                              # 4. Отправляем подтверждение на Arduino
                              serial_write_queue.put('story_22_done')

                         if flag=="story_22_b":
                              # 1. Ждем, пока канал освободится
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              # 2. Воспроизводим историю
                              play_localized_audio("story_22_b")
                                 
                              # 3. Ждем, пока PLAY ЗАКОНЧИТСЯ
                              while channel3.get_busy()==True and go == 1:
                                  eventlet.sleep(0.1)
                              # 4. Отправляем подтверждение на Arduino
                              serial_write_queue.put('story_22_done')
                           
                         if flag=="story_22_c":
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_22_c")
                                  
                              # 3. Ждем, пока PLAY ЗАКОНЧИТСЯ
                              while channel3.get_busy()==True and go == 1:
                                  eventlet.sleep(0.1)
                              # 4. Отправляем подтверждение на Arduino
                              serial_write_queue.put('story_22_done')
                              
                         if flag=="cave_click":
                              #----играем эффект 
                              play_effect(cave_click)
                              socketio.emit('level', 'cave_click', to=None)
                              if 'cave_click' not in socklist: # (Опционально)
                                  socklist.append('cave_click')
                         if flag == "cave_repeat":
                              # Просто играем звук клика, никаких команд на пульт (UI) не отправляем!
                              play_effect(cave_click)
                         if flag=="cave_reset":
                              play_effect(cave_click)
                              socketio.emit('level', 'cave_reset', to=None)
                         if flag=="door_cave":
                              #----играем эффект 
                              play_effect(door_cave)
                              socketio.emit('level', 'door_cave', to=None)
                              # ИСПРАВЛЕНИЕ: Добавляем door_cave в socklist,
                              # чтобы при переподключении UI шкала Mine Door показывала 100%.
                              if 'door_cave' not in socklist:
                                  socklist.append('door_cave')
                              socketio.emit('level', 'active_troll',to=None)
                              socklist.append('active_troll')
                              socketio.emit('level', 'mine',to=None)
                              socklist.append('mine')
                              # Убираем ожидание завершения эффекта
                              # while effects_are_busy() and go == 1:
                              #     eventlet.sleep(0.1)
                              # Добавляем фиксированную задержку 2 секунды
                              eventlet.sleep(2.0)
                              play_localized_audio("story_26")

                              # ФИКС: фоновый поток — Serial не блокируется.
                              # Бутылки и другие события обрабатываются мгновенно.
                              def _after_story_26():
                                  while channel3.get_busy() and go == 1:
                                      eventlet.sleep(0.1)
                              socketio.start_background_task(_after_story_26)
                         if flag=="cave_search1":
                              #----играем эффект 
                              play_effect(cave_search)
                              socketio.emit('level', 'cave_search1', to=None)
                              socklist.append('cave_search1')
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_27_a")

                              # ФИКС: убрана блокировка Serial во время story_27_a
                              eventlet.sleep(0.1)
                         if flag=="cave_search2":
                              #----играем эффект 
                              play_effect(cave_search)
                              socketio.emit('level', 'cave_search2', to=None)
                              socklist.append('cave_search2')
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_27_b")

                              # ФИКС: убрана блокировка Serial во время story_27_b
                              eventlet.sleep(0.1)
                         if flag=="cave_search3":
                              #----играем эффект 
                              play_effect(cave_search)
                              socketio.emit('level', 'cave_search3', to=None)
                              socklist.append('cave_search3')
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_27_c")

                              # ФИКС: убрана блокировка Serial во время story_27_c
                              eventlet.sleep(0.1)
                         if flag=="cave_end":
                              if 'cave_end' in socklist:
                                  logger.debug("Игнорируем повторный cave_end")
                              else:
                                  #----играем эффект 
                                  socketio.emit('level', 'troll',to=None)
                                  socklist.append('troll')
                                  socketio.emit('level', 'cave_end', to=None)
                                  socklist.append('cave_end')
                                  play_effect(cave_end)
                                  send_esp32_command(ESP32_API_TRAIN_URL, "troll_finish")
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  play_localized_audio("story_30")
                                  # ФИКС: убрана блокировка — story_30 играет параллельно с Serial

                         if flag=="material_end":
                              #----играем эффект 
                              socketio.emit('level', 'active_open_bank_door',to=None)
                              socklist.append('active_open_bank_door')
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_4")
                         if flag=="miror":
                              socketio.emit('level', 'open_bank_door',to=None)
                              socklist.append('open_bank_door')
                              play_effect(door_bank)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_23")

                              while channel3.get_busy()==True and go == 1:
                                  eventlet.sleep(0.1)
                              eventlet.sleep(1.0)
                              serial_write_queue.put('open_bank')
                              # Этот блок немедленно отправит команду 'open_bank', 
                              # не дожидаясь завершения eventlet.sleep(5.0) и story_24.
                              while not serial_write_queue.empty():
                                  try:
                                      message_to_send = serial_write_queue.get_nowait()
                                      ser.write(str.encode(message_to_send + '\n'))
                                  except eventlet.queue.Empty:
                                      break # Очередь пуста
                                  eventlet.sleep(0.01) # Даем время на отправку
                              eventlet.sleep(5.0)
                              play_localized_audio("story_24")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)     
                         if flag=="safe_turn":
                              #----играем эффект 
                              play_effect(safe_turn)
                              socklist.append('safe_turn')
                         if flag=="safe_end":
                              socketio.emit('level', 'safe_end', to=None)
                              socklist.append('safe_end')
                              
                              # Вся остальная логика 'safe_end', которая у вас уже была
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_5")
                              play_effect(safe_end)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              serial_write_queue.put('open_safe')
                              
                              while not serial_write_queue.empty():
                                  try:
                                      message_to_send = serial_write_queue.get_nowait()
                                      ser.write(str.encode(message_to_send + '\n'))
                                  except eventlet.queue.Empty:
                                      break 
                                  eventlet.sleep(0.01) 
                                  
                              play_localized_audio("story_25")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)  
                              eventlet.sleep(1.1)
                              play_localized_audio("story_31")
                                  
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              play_effect(door_workshop)
                              serial_write_queue.put('open_workshop')
                              process_serial_queue()
                              eventlet.sleep(0.5)
                              serial_write_queue.put('open_workshop')
                              process_serial_queue()
                              play_background_music("fon9.mp3", loops=-1)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_32")
                              
                              socketio.emit('level', 'safe',to=None)
                              socklist.append('safe')
                         
                         # Список ВСЕХ команд, которые управляют шкалой сейфа
                         safe_commands_list = ['safe_step_1', 'safe_step_2', 'safe_step_3', 'safe_step_4', 'safe_end', 'safe_reset']

                         # Проверяем, пришел ли флаг, связанный с сейфом
                         if flag in safe_commands_list:
                             
                             # --- Логика ОЧИСТКИ ---
                             # (Теперь это будет работать, т.к. 'global socklist' объявлен вверху)
                             socklist = [item for item in socklist if item not in safe_commands_list]
                             
                             # --- Логика ДОБАВЛЕНИЯ ---
                             # Добавляем ТОЛЬКО ОДНУ, самую свежую команду
                             socklist.append(flag)
                             socketio.emit('level', flag, to=None)
                                 
                             # --- Логика ЭФФЕКТОВ/ДЕЙСТВИЙ (кроме 'safe_end') ---
                             if flag != 'safe_end':
                                 # Воспроизводим звук для 'reset' и 'step' ОДИН РАЗ
                                 play_effect(safe_fix)

                         if flag=="lib_door":
                              #----играем эффект 
                              play_effect(lib_door)
                              ser.write(str.encode('student_hide\n'))
                              eventlet.sleep(0.1)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_46")

                              send_esp32_command(ESP32_API_TRAIN_URL, "train_on")
                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              # Отправляем команду на Arduino, чтобы начать 5-секундное мерцание
                              serial_write_queue.put('library_flicker_start')
                              eventlet.sleep(0.1)
                              play_localized_audio("story_47")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)

                              play_background_music("fon15.mp3", loops=-1)
                              play_localized_audio("story_48")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              play_effect(door_top)
                              send_esp32_command(ESP32_API_WOLF_URL, "day_on")
                              send_esp32_command(ESP32_API_TRAIN_URL, "day_on")
                              send_esp32_command(ESP32_API_SUITCASE_URL, "day_on")
                              send_esp32_command(ESP32_API_SAFE_URL, "day_on") 
                              serial_write_queue.put('door_top')
                              eventlet.sleep(1)
                              socketio.emit('level', 'open_door_puzzle',to=None)
                              socklist.append('open_door_puzzle')
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_8")
                              play_localized_audio("story_49")

                         if flag=="door_basket":
                              if 'cup' in socklist:
                                  logger.debug("Игнорируем повторный door_basket")
                              else:
                                  send_esp32_command(ESP32_API_TRAIN_URL, "stage_9") 
                                  socketio.emit('level', 'cup',to=None)
                                  socklist.append('cup')
                                  #----играем эффект 
                                  play_effect(door_basket)
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  play_localized_audio("story_50")
                                  play_effect(lose1)
                                  socketio.emit('level', 'active_spell',to=None)
                                  socklist.append('active_spell')
                              
                         if flag == "swipe_r":
                              play_effect(swipe_r)
                         if flag == "swipe_l":
                              play_effect(swipe_l)
                                  
                         if flag=="door_spell":
                              socketio.emit('level', 'spell',to=None)
                              socklist.append('spell')
                              #----играем эффект 
                              play_effect(door_spell) 
                              socketio.emit('level', 'active_crystals',to=None)
                              socklist.append('active_crystals')
                         if flag=="spell_step_1":
                              socketio.emit('level', 'spell_step_1', to=None) 
                              socklist.append('spell_step_1')
                         if flag=="spell_step_2":
                              socketio.emit('level', 'spell_step_2', to=None) 
                              socklist.append('spell_step_2')
                         if flag=="spell_step_3":
                              socketio.emit('level', 'spell_step_3', to=None) 
                              socklist.append('spell_step_3')
                         if flag=="spell_step_4":
                              socketio.emit('level', 'spell_step_4', to=None) 
                              socklist.append('spell_step_4')
                         if flag=="spell_step_5":
                              socketio.emit('level', 'spell_step_5', to=None) 
                              socklist.append('spell_step_5')
                         if flag=="spell_reset":
                              socketio.emit('level', 'spell_reset', to=None) 
                              socklist.append('spell_reset')
                              
                         if flag=="cristal_up":
                              #----играем эффект 
                              play_effect(cristal_up)
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_10") 
                              socketio.emit('level', 'active_open_memory_stash',to=None)
                              socklist.append('active_open_memory_stash')
                              socketio.emit('level', 'crystals',to=None)
                              socklist.append('crystals')  
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              play_localized_audio("story_51")

                         if flag=="fire1":
                              #----играем эффект 
                              play_effect(fire1)
                              if fire1Flag == 0 and 'workshop' not in socklist:
                                  fire1Flag = 1
                                  play_localized_audio("story_32_a")

                         if flag=="fire2":
                              #----играем эффект 
                              play_effect(fire2)
                              if fire2Flag == 0 and 'workshop' not in socklist:
                                  fire2Flag = 1
                                  play_localized_audio("story_32_b")

                         if flag=="fire3":
                              #----играем эффект 
                              play_effect(fire3)
                         if flag=="fire0":
                              #----играем эффект 
                              play_effect(fire0)
                              if fire0Flag == 0 and 'workshop' not in socklist:
                                  fire0Flag = 1
                                  play_localized_audio("story_32_c")

                         # Teper' my lovim lyuboye soobshcheniye, nachinayushcheyesya s "item_find"
                         if flag.startswith("item_find"):
                              # flag (naprimer, "item_find:crystal") uzhe budet v logakh
                              # blagodarya "logger.info(f"[SERIAL_IN] {flag}")" vyshe.
                              
                              # 1. Vosproizvodim obshchiy zvuk
                              play_effect(item_find)
                              
                              # 2. Otpravlyayem obshchuyu komandu na ESP-kartu (train.ino)
                              #    (train.ino ostanovit svoyu pul'saciyu pri poluchenii "item_find")
                              send_esp32_command(ESP32_API_TRAIN_URL, "item_find")
                         if flag=="item_add":
                              #----играем эффект 
                              play_effect(item_add)
                         if flag=="broom":
                              #----играем эффект 
                              play_effect(craft_success)
                              socketio.emit('level', 'broom', to=None)
                              socklist.append('broom')
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)    
                              play_localized_audio("story_33")

                         if flag=="helmet":
                              #----играем эффект 
                              play_effect(craft_success)
                              socketio.emit('level', 'helmet', to=None)
                              socklist.append('helmet')
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)    
                              play_localized_audio("story_34")

                         if flag=="story_35":
                              # Если мы уже обработали эту историю, игнорируем дубли
                              if 'story_35_processed' in socklist:
                                  continue
                              socklist.append('story_35_processed')
                              
                              socketio.emit('level', 'workshop',to=None)
                              socklist.append('workshop')
                              send_esp32_command(ESP32_API_TRAIN_URL, "item_end") 
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_6") 
                              play_localized_audio("story_35")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              eventlet.sleep(1.0)
                              send_esp32_command(ESP32_API_WOLF_URL, "day_off")
                              send_esp32_command(ESP32_API_TRAIN_URL, "day_off")
                              send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
                              send_esp32_command(ESP32_API_SAFE_URL, "day_off")    
                              play_background_music("fon10.mp3", loops=-1)
                              play_localized_audio("story_36")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              send_command_with_confirmation('student_open', 'student_open_success')
                              eventlet.sleep(1.0)
                              play_localized_audio("story_37")

                         if flag=="h_clock":
                              socketio.emit('level', 'first_clock_2',to=None)
                              socklist.append('first_clock_2') 
                              #----играем эффект 
                              play_background_music("fon11.mp3", loops=-1)
                              play_effect(h_clock)
                              socketio.emit('level', 'active_second_clock_2',to=None)
                              socklist.append('active_second_clock_2')  
                         if flag=="uf_clock":
                              socketio.emit('level', 'second_clock_2',to=None)
                              socklist.append('second_clock_2')
                              #----играем эффект 
                              play_background_music("fon12.mp3", loops=-1)
                              play_effect(uf_clock)
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                              play_localized_audio("story_38")

                              socketio.emit('level', 'active_ghost',to=None)
                              socklist.append('active_ghost')         
                         if flag=="story_39":
                              #send_esp32_command(ESP32_API_WOLF_URL, "ghost_game")
                              play_localized_audio("story_39")

                         if flag=="story_40":
                              send_esp32_command(ESP32_API_WOLF_URL, "ghost_game")
                              socketio.emit('level', 'story_40', to=None)
                              socklist.append('story_40') # Добавляем флаг для UI
                              play_localized_audio("story_40")

                         if flag=="story_41":
                              send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                              socketio.emit('level', 'story_41', to=None)
                              socklist.append('story_41') # Добавляем флаг для UI
                              play_localized_audio("story_41")

                         if flag=="story_42":
                              socketio.emit('level', 'story_42', to=None)
                              socklist.append('story_42') # Добавляем флаг для UI
                              play_localized_audio("story_42")

                         if flag == "ghost_knock":
                              play_effect(knock_castle, loops=-1)
                         if flag=="punch":
                              channel2.stop()
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_7") 
                              socketio.emit('level', 'ghost',to=None)
                              socklist.append('ghost')
                              socketio.emit('level', 'punch', to=None)
                              socklist.append('punch') # Добавляем флаг для UI
                              play_localized_audio("story_43")

                              while channel3.get_busy()==True and go == 1: 
                                  eventlet.sleep(0.1)
                              serial_write_queue.put('open_library')
                              #send_esp32_command(ESP32_API_TRAIN_URL, #"ghost_game_end")
                              #send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
                              eventlet.sleep(2.0)
                              send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                              eventlet.sleep(1.0)
                              play_localized_audio("story_44")


                         if flag=="star_hint":
                              channel3.stop()
                              play_effect(star_hint)
                              send_esp32_command(ESP32_API_TRAIN_URL, "set_time")
                              socketio.emit('level', 'set_time', to=None)
                              socklist.append('set_time')
                         if flag=="fire":
                             # Воспроизводим, только если канал эффектов (channel2) свободен.
                             # Это гарантирует, что звук не запустится, если он еще играет.
                             if not channel2.get_busy():
                                 play_effect(fireplace)
                              
                         if flag=="mistake_crystal":
                              #----играем эффект
                              play_effect(mistake_crystal)
                              print("mistake_crystal")
                          #----раставили кристаллы на свои места    
                         if flag=="start_crystal":
                              #=----просто звуковой эффект
                              play_background_music("fon16.mp3", loops=-1)
                              play_effect(start_crystal)   
                              #----  правильно убрали кристал
                         if flag=="true_crystal":
                              play_effect(true_crystal) 
                         #----3 уровень     
                         if flag=="third_level":
                              #----отпраялем на клиента прогресс бар увеличиваем
                              socketio.emit('level', 'third_level',to=None)
                              #---добавляем в историю
                              socklist.append('third_level')
                              #-----играем эффект
                              play_effect(level_up)  
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              #------играем голос    
                              play_localized_audio("story_54")

                          #-----2 уровень        
                         if flag=="second_level":
                              socketio.emit('level', 'second_level',to=None)
                              socklist.append('second_level')
                              play_effect(level_up)  
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              #------играем голос    
                              play_localized_audio("story_53")

                         #----прошли 1 уровень      
                         if flag=="first_level":
                              socketio.emit('level', 'first_level',to=None)
                              socklist.append('first_level')
                              play_effect(level_up)  
                              while effects_are_busy() and go == 1: 
                                  eventlet.sleep(0.1)
                              #------играем голос    
                              play_localized_audio("story_52")

                         

                         # "boy_in_lesson" (с урока, level 18) -> Запустить интро
                         # 1. УРОК (Level 18): Вход
                         if flag == "boy_in_lesson":
                              # Единый фильтр от дребезга руки (2 секунды)
                              if time.time() - last_boy_in_time < 2.0:
                                  logger.debug("Игнорируем повторный boy_in_lesson (дребезг)")
                                  continue
                              last_boy_in_time = time.time()

                              if level_18_intro_played:
                                  logger.info("Возобновление урока (снятие с паузы)")
                                  pygame.mixer.music.unpause()
                                  if 'stop_players_rest' in socklist:
                                        socklist.remove('stop_players_rest')
                                  socketio.emit('level', 'start_players', to=None)
                              else:
                                  level_18_intro_played = True
                                  play_background_music("fon18.mp3", loops=-1)
                                  socketio.emit('level', 'start_players', to=None)
                                  socklist.append('start_players')
                                  
                                  lesson_intro_seq += 1
                                  current_seq = lesson_intro_seq
                                  
                                  def lesson_intro_task(seq):
                                      play_localized_audio("story_57")
                                      eventlet.sleep(0.3)
                                      while channel3.get_busy() == True and go == 1 and lesson_intro_seq == seq:
                                          eventlet.sleep(0.1)
                                      if lesson_intro_seq != seq or go != 1: return
                                      
                                      play_effect(applause)
                                      eventlet.sleep(2.0)
                                      if lesson_intro_seq != seq or go != 1: return
                                          
                                      serial_write_queue.put('start_lesson')
                                      serial_write_queue.put('start_lesson')
                                      
                                      # Включаем голос
                                      play_localized_audio("story_58")
                                      
                                      # --- ИСПРАВЛЕНИЕ: Ждем, пока story_58 ПОЛНОСТЬЮ закончится ---
                                      while channel3.get_busy() == True and go == 1 and lesson_intro_seq == seq:
                                          eventlet.sleep(0.1)
                                      if lesson_intro_seq != seq or go != 1: return
                                      
                                      # --- И ТОЛЬКО ТЕПЕРЬ пускаем мяч и гасим салют! ---
                                      serial_write_queue.put('start_game_basket')
                                      serial_write_queue.put('start_game_basket')
                                      logger.debug("SENT [Arduino]: start_game_basket (Salute stopped, ball started)")
                                  
                                  # Запуск сценария
                                  socketio.start_background_task(target=lesson_intro_task, seq=current_seq)

                         # 2. УРОК (Level 18): Выход (Пауза)
                         if flag == "boy_out_lesson":
                             # Если с момента входа прошло меньше 3 секунд, игнорируем выход.
                             if time.time() - last_boy_in_time < 3.0:
                                 logger.debug("Игнорируем boy_out_lesson (дребезг контактов)")
                             else:
                                 lesson_intro_seq += 1
                                 level_18_intro_played = False
                                 pygame.mixer.music.pause()
                                 try:
                                     play_effect(lose1)
                                     # Играем историю 69 (Мальчик ушел)
                                     play_localized_audio("story_69")
                                 except Exception as e:
                                     logger.error(f"Ошибка звука boy_out_lesson: {e}")

                                 
                                 socketio.emit('level', 'stop_players_rest', to=None)
                                 socklist.append('stop_players_rest')
                             
                         # 3. ИГРА (Level 19): Вход (Возобновление)
                         if flag == "boy_in_game":
                              if time.time() - last_boy_in_time < 2.0:
                                  logger.debug("Игнорируем повторный boy_in_game")
                              else:
                                  last_boy_in_time = time.time()
                                  play_effect(applause)
                                  pygame.mixer.music.unpause()
                                  # Играем историю 70 (Мальчик вернулся)
                                  play_localized_audio("story_70")

                                  
                                  # Удаляем флаг проигрыша, чтобы пауза снова работала
                                  if 'win_bot' in socklist:
                                       socklist.remove('win_bot')
                                       
                                  if 'stop_players_rest' in socklist:
                                        socklist.remove('stop_players_rest')
                                  socketio.emit('level', 'start_players', to=None)
                                  socklist.append('start_players')

                         # 4. ИГРА (Level 19): Выход (Пауза)
                         if flag == "boy_out_game":
                              # ПРОВЕРКА: Если только что выиграл бот, НЕ включаем паузу
                              # Мы проверяем историю событий (socklist) на наличие 'win_bot'
                              if 'win_bot' in socklist:
                                   logger.debug("boy_out_game: Игнорируем паузу, так как БОТ ВЫИГРАЛ.")
                                   # Ничего не делаем, пусть играет story_67
                                   # Но нужно удалить win_bot из списка, чтобы при СЛЕДУЮЩЕМ снятии пауза сработала?
                                   # Лучше удалить его при 'boy_in_game' (рестарте уровня).
                              else:
                                   if basket_timeout_timer:
                                       basket_timeout_timer.cancel()
                                       basket_timeout_timer = None
                                   # Стандартная логика паузы
                                   pygame.mixer.music.pause()
                                   try:
                                       play_effect(lose1)
                                       # Играем историю 69 (Мальчик ушел)
                                       play_localized_audio("story_69")

                                   except Exception as e:
                                       logger.error(f"Ошибка звука: {e}")

                                   
                                   socketio.emit('level', 'stop_players_rest', to=None)
                                   socklist.append('stop_players_rest')

                         if flag=="story_59":
                              play_localized_audio("story_59")

                         if flag=="story_55":
                              play_localized_audio("story_55")

                         if flag=="crime_end" or (flag=="boy_out" and 'active_crime' in socklist and 'crime' not in socklist):
                              socketio.emit('level', 'crime',to=None)
                              socklist.append('crime')
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_12") 
                              socketio.emit('level', 'active_basket',to=None)
                              socklist.append('active_basket')
                              play_background_music("fon17.mp3", loops=-1)
                              play_localized_audio("story_56")

                         if flag=="lesson_goal":
                              if time.time() - last_lesson_goal_time > 3.0: # <--- ЗАЩИТА ОТ ДВОЙНОГО ЗВУКА
                                  last_lesson_goal_time = time.time()
                                  play_effect(applause)
                                  play_localized_audio("story_61_a")

                         if flag=="flying_ball":
                              play_effect(flying_ball)
                              storyBasketFlag = 1
                         
                         # ОБЩАЯ ЛОГИКА ДЛЯ catch1 - catch4
                         if flag in ["catch1", "catch2", "catch3", "catch4"]:
                             if flag == "catch1": play_effect(catch1)
                             elif flag == "catch2": play_effect(catch2)
                             elif flag == "catch3": play_effect(catch3)
                             elif flag == "catch4": play_effect(catch4)
                            
                             if storyBasketFlag == 1:
                                 # story_60 играет РАНДОМНО при каждой поимке, без счётчика
                                 # Список всех вариантов story_60 (a..j = 10 штук)
                                 story60_variants = [
                                     "story_60_a", "story_60_b", "story_60_c", "story_60_d", "story_60_e",
                                     "story_60_f", "story_60_g", "story_60_h", "story_60_i", "story_60_j"
                                 ]
                                 chosen_story60 = random.choice(story60_variants)
                                 play_localized_audio(chosen_story60)
                                 logger.debug(f"BASKET: Ball caught → playing {chosen_story60}")
                                
                                 catchCount += 1
                                 logger.debug(f"BASKET: catchCount = {catchCount}")
                         
                         # ИСПРАВЛЕНИЕ: start_snitch — это аппаратный сигнал тайм-аута (15 сек) из basket3.ino.
                         if flag == "start_snitch":
                             logger.debug("BASKET: start_snitch received from basket3. Hardware timeout triggered.")
                             handle_basket_timeout()
                         
                         # --- Логика голов игрока с счетчиком ---
                         if flag=="goal_1_player" or flag=="goal_2_player" or flag=="goal_3_player" or flag=="goal_4_player":
                              # ЗАЩИТА ОТ ДВОЙНОГО СРАБАТЫВАНИЯ
                              if time.time() - last_lesson_goal_time < 3.0:
                                  continue
                              last_lesson_goal_time = time.time()
                              # --- ОТМЕНА ТАЙМЕРА ---
                              if basket_timeout_timer:
                                  basket_timeout_timer.cancel()
                                  basket_timeout_timer = None
                              # Добавляем инкрементальные флаги для UI
                              if flag == "goal_1_player":
                                  socklist.append('goal_1_player')
                                  socketio.emit('level', 'goal_1_player',to=None)
                              if flag == "goal_2_player":
                                  socklist.append('goal_2_player')
                                  socketio.emit('level', 'goal_2_player',to=None)
                              # 1. Выбираем случайный звук гола
                              chosen_goal_sound = random.choice(player_goal_sounds)
                              
                              # --- ТОЧНАЯ СИНХРОНИЗАЦИЯ С ЛЕНТОЙ ---
                              # ФИКС багF: threading.Timer несовместим с eventlet — может вызвать
                              # задержку или блокировку при записи в очередь из обычного треда.
                              # Заменено на eventlet.spawn_after — нативный eventlet-таймер.
                              effect_duration = chosen_goal_sound.get_length()
                              eventlet.spawn_after(effect_duration, lambda: serial_write_queue.put('goal_effect_end'))
                              # ---------------------------------------------
                              
                              play_effect(chosen_goal_sound)
                              
                              # 2. Воспроизвести историю по счетчику (используя базовый список)
                              if goalCount < len(player_goal_stories_base):
                                  base_name = player_goal_stories_base[goalCount]
                              else:
                                  # Если счетчик превысил кол-во историй, проигрываем последнюю
                                  base_name = player_goal_stories_base[-1]
                              
                              play_localized_audio(base_name)
                              
                              # 3. Увеличить счетчик для следующего гола
                              goalCount += 1

                         # Логика голов БОТА с счетчиком ---
                         if flag=="goal_1_bot" or flag=="goal_2_bot" or flag=="goal_3_bot" or flag=="goal_4_bot":
                              # Добавляем инкрементальные флаги для UI
                              if flag == "goal_1_bot":
                                  socketio.emit('level', 'goal_1_bot',to=None)
                                  socklist.append('goal_1_bot')
                              if flag == "goal_2_bot":
                                  socketio.emit('level', 'goal_2_bot',to=None)
                                  socklist.append('goal_2_bot')
                              # 1. Воспроизвести РАНДОМНЫЙ звук
                              play_effect(random.choice(enemy_goal_sounds))

                              # 2. Воспроизвести историю по счетчику
                              if enemyGoalCount < len(enemy_goal_stories_base):
                                  base_name = enemy_goal_stories_base[enemyGoalCount]
                              else:
                                  base_name = enemy_goal_stories_base[-1]
                              
                              play_localized_audio(base_name)
                              
                              # 3. Увеличить счетчик для следующего гола бота
                              enemyGoalCount += 1

                         if flag == "red_ball":
                              # Эффекты
                              enemyCatchCount += 1
                              # Логика эффектов (catch1...catch4) остается прежней
                              if enemyCatchCount == 1: play_effect(enemy_catch1)
                              elif enemyCatchCount == 2: play_effect(enemy_catch2)
                              elif enemyCatchCount == 3: play_effect(enemy_catch3)
                              elif enemyCatchCount == 4:
                                  play_effect(enemy_catch4)
                                  enemyCatchCount = 0   

                              # --- НОВАЯ ЛОГИКА СО СПИСКАМИ (Story 63) ---
                              if redSintchEnemyCatchCount < len(red_ball_active_stories_base):
                                  base_name = red_ball_active_stories_base[redSintchEnemyCatchCount]
                              else:
                                  base_name = red_ball_active_stories_base[-1]
                              
                              play_localized_audio(base_name)
 
                              redSintchEnemyCatchCount += 1
                              # Сброс счетчика, если прошли все истории (опционально)
                              if redSintchEnemyCatchCount >= len(red_ball_active_stories_base):
                                   redSintchEnemyCatchCount = 0
                            
                              print(enemyCatchCount)
                              
                         if flag == "enemy_catch1" or flag == "enemy_catch2" or flag == "enemy_catch3" or flag == "enemy_catch4":
                              # Определяем какой эффект играть
                              if flag == "enemy_catch1": play_effect(enemy_catch1)
                              if flag == "enemy_catch2": play_effect(enemy_catch2)
                              if flag == "enemy_catch3": play_effect(enemy_catch3)
                              if flag == "enemy_catch4": play_effect(enemy_catch4)

                              # --- ЛОГИКА СО СПИСКАМИ (Story 64) ---
                              if redClickSintchEnemyCatchCount < len(enemy_catch_stories_base):
                                  base_name = enemy_catch_stories_base[redClickSintchEnemyCatchCount]
                              else:
                                  base_name = enemy_catch_stories_base[-1]
                                  
                              play_localized_audio(base_name)
                              
                              redClickSintchEnemyCatchCount += 1
                              # Сброс счетчика
                              if redClickSintchEnemyCatchCount >= len(enemy_catch_stories_base):
                                  redClickSintchEnemyCatchCount = 0
                                  
                              print(redClickSintchEnemyCatchCount)
                         if flag=="win":
                              # --- ИГНОРИРУЕМ ДУБЛИКАТЫ (Защита от 3-х побед подряд) ---
                              if 'win_player' in socklist:
                                  continue
                              # --- ОТМЕНА ТАЙМЕРА ---
                              if basket_timeout_timer:
                                  basket_timeout_timer.cancel()
                                  basket_timeout_timer = None
                                  
                              play_background_music("fon19.mp3", loops=-1)    
                              play_localized_audio("story_66")

                              # Отправляем команды для UI
                              socketio.emit('level', 'win_player',to=None)
                              socklist.append('win_player')
                              
                              # Звуки голов
                              play_effect(random.choice(player_goal_sounds))
                              
                              # Цикл ожидания с обработкой очереди
                              start_wait = time.time()
                              while effects_are_busy() and go == 1:
                                  process_serial_queue() # <-- ПРОДОЛЖАЕМ ОТПРАВЛЯТЬ
                                  eventlet.sleep(0.1)
                                  if time.time() - start_wait > 8: break 
                              
                              play_effect(random.choice(player_goal_sounds))
                              
                              start_wait = time.time()
                              while effects_are_busy() and go == 1: 
                                  process_serial_queue() # <-- ПРОДОЛЖАЕМ ОТПРАВЛЯТЬ
                                  eventlet.sleep(0.1)
                                  if time.time() - start_wait > 8: break

                              # --- ФИНАЛ: ЗАПУСКАЕМ САЛЮТ И ЗВУК ПОБЕДЫ ОДНОВРЕМЕННО ---
                              serial_write_queue.put('play_win_salute') # Сигнал для Главной платы (Сменить волну на салют)
                              play_effect(win)                          # Звук победы (win.wav)
                              
                              # Отправляем салют на внешние ESP32 синхронно с аудио
                              send_esp32_command(ESP32_API_WOLF_URL, "firework")
                              send_esp32_command(ESP32_API_TRAIN_URL, "firework")
                              send_esp32_command(ESP32_API_SUITCASE_URL, "firework")
                              send_esp32_command(ESP32_API_SAFE_URL, "firework")
                         if flag=="win_robot":
                              # Если уже победил (есть в списке), ИГНОРИРУЕМ ПОВТОРЫ
                              if 'win_bot' not in socklist:
                                  socketio.emit('level', 'win_bot',to=None)
                                  socklist.append('win_bot')
                                  
                                  # --- ПЕРЕНЕСЕНО ВНУТРЬ (чтобы играло 1 раз) ---
                                  play_effect(enemy_goal1)
                                  while effects_are_busy() and go == 1: 
                                      eventlet.sleep(0.1)
                                  play_background_music("fon17.mp3", loops=-1)    
                                  play_localized_audio("story_67")
                                  # ---------------------------------------------
                        #-------прошли игру с кристалами
                         if flag=="memory_room_end":
                             if 'memory_room_end' in socklist:
                                 logger.debug("Игнорируем повторный memory_room_end")
                                 # Подтверждаем Arduino — останавливаем повторные отправки скипа
                                 if 'memory_room_skip_done' not in socklist:
                                     socklist.append('memory_room_skip_done')
                             else:
                                 # Arduino подтвердил — останавливаем повторные отправки скипа
                                 if 'memory_room_skip_done' not in socklist:
                                     socklist.append('memory_room_skip_done')
                                 #----отправили на клиента
                                 send_esp32_command(ESP32_API_TRAIN_URL, "stage_0") 
                                 socketio.emit('level', 'memory_room_end',to=None)
                                 #----добавили в историю
                                 socklist.append('memory_room_end')
                                 #------играем эффект
                                 play_effect(brain_end)
                                 #-----активируем последнюю игру
                                 socketio.emit('level', 'active_crime',to=None)
                                 socklist.append('active_crime')
                             
                                 
                            
      ###################################################                    #######################################################
                         
     #########################################################################
                        #------поставили пацана на место
                         if flag=="last_on":
                              #----отправили на клиента 
                              socketio.emit('level', 'last_on',to=None)
                              #-----добавили в историю
                              socklist.append('last_on')
                              #-----отправили рейтинг
                              socketio.emit('rate', rateTime,to=None)
                              socketio.emit('hintCount', str(hintCount),to=None)
                              rating = rating+hintCount
                              if rating<=75:
                                   star = 5
                              elif rating<=99 and rating>75:
                                   star = 4
                              elif rating <=123 and rating>99:
                                   star = 3
                              elif rating <=150 and rating>123:
                                   star = 2
                              elif rating>=180:
                                   star = 1                    
                              socketio.emit('rating', str(star),to=None)
                              #----играем фон       
                              #----меняем переменную
                              name = "story_12"  
                              #----удаляем старт из истории
                              socklist.remove('start_game')
                              #----переводим флаги в окончание игры
                              starts= 2
                              go = -1
                              eventlet.sleep(0.5)

                         # ОПТИМИЗИРОВАННАЯ ОБРАБОТКА ПОДСКАЗОК (HINTS)
                         # Мы просто берем имя флага (например, hint_2_b) и играем соответствующий файл
                         if flag.startswith("hint_"):
                             # Если канал историй (голос) сейчас занят, просто игнорируем подсказку
                             if channel3.get_busy():
                                 logger.debug(f"Игнорируем подсказку {flag}, так как звучит другая история/подсказка.")
                                 continue # Пропускаем дальнейшую обработку, в очередь не встает
                                 
                             hintCount += 1
                             play_localized_audio(flag)

              eventlet.sleep(0.1)
          except Exception as e:
              logger.error(f"CRASH IN SERIAL LOOP: {e}")
              # Пытаемся закрыть сломанный порт
              try:
                  ser.close()
              except:
                  pass
              
              logger.info("Попытка переподключения к Arduino через 2 секунды...")
              eventlet.sleep(2) # Даем системе время заново определить USB
              
              # Пытаемся открыть порт заново
              try:
                  ser = Serial('/dev/ttyUSB_MAIN', 115200, timeout=1)
                  logger.info("УСПЕХ: Связь с Arduino восстановлена!")
              except Exception as reconn_e:
                  logger.error(f"ОШИБКА: Не удалось переподключиться. Проверьте USB кабель! ({reconn_e})")
              
              eventlet.sleep(1)
   
#----метод таймера можно не трогать               
def timer():
    
    global flagtime
    global start
    global sec
    global language
    global mins
    global tensec
    global tenmins
    global h
    global sync
    global go
    global timeflag
    global starts
    global rateTime
    global rating
    timeflag = 0
    sec = 0
    tensec = 0
    mins = 0
    tenmins = 0
    h = 0
    sync = "0:00:00"
    flagtime = 0
    start = 0
    while True:
         # Если квест еще не запущен (ни Ready, ни Start),
         # мы просто спим 0.1 сек и пропускаем цикл.
         if (go != 2 and go != 3 and starts != 1):
             eventlet.sleep(0.1)
             continue
         eventlet.sleep(0)
              #print(i)
         if go == 2 or go == 3:# Сброс только при Restart или Ready
              sec = 0
              tensec = 0
              mins = 0
              tenmins = 0
              h = 0
              sync = "0:00:00"
              socketio.emit('timer', sync,to=None)
              eventlet.sleep(1)
         if starts==1:
               flagtime=1
               sec=sec+1
               if sec ==10:
                    sec=0
                    tensec=tensec+1
               if tensec >=6:
                    tensec=0
                    mins=mins+1
                    rating = rating+1
               if mins==10:
                    tenmins=tenmins+1
                    mins=0
               if tenmins>=6:
                    mins=0
                    tenmins=0
                    h=h+1
               eventlet.sleep(1)
               sync = str(h)+":"+str(tenmins)+str(mins)+":"+str(tensec)+str(sec)
               rateTime = sync
               socketio.emit('timer', sync,to=None)
               flagtime=0
               
def process_serial_queue():
    """Обрабатывает очередь отправки команд на Arduino."""
    try:
        message_to_send = serial_write_queue.get_nowait()
        description = EVENT_DESCRIPTIONS.get(message_to_send, '-')
        tag = get_device_tag(message_to_send)
        # Периодический heartbeat-запрос — на DEBUG, чтобы не засорял INFO-лог.
        if message_to_send == 'check_towers':
            logging.debug(f'SENT {tag}: {description} (RAW: {message_to_send})')
        else:
            logging.info(f'SENT {tag}: {description} (RAW: {message_to_send})')
        ser.write(str.encode(message_to_send + '\n'))
        eventlet.sleep(0.05)
    except eventlet.queue.Empty:
        pass

# --- Обработчик переключателя Bluetooth с пульта ---
@socketio.on('toggle_bluetooth')
def handle_bluetooth_toggle(is_checked):
    global starts # starts == 2 это режим RESTART
    
    # Разрешаем включать ТОЛЬКО в режиме Restart
    if starts == 2 and is_checked:
        set_bluetooth_state(True)
    else:
        # Во всех остальных случаях (или если сняли галочку) - выключаем
        set_bluetooth_state(False)
# ---------------------------------------------------------------

# --- Обработчик переключателя музыки Ready с пульта ---
@socketio.on('toggle_ready_music')
def handle_ready_music_toggle(is_checked):
    global go, play_ready_music
    
    # Гарантированно превращаем сигнал от пульта в True/False
    if isinstance(is_checked, str):
        play_ready_music = (is_checked.lower() == 'true')
    else:
        play_ready_music = bool(is_checked)
        
    # Надежно сохраняем выбор в файл по абсолютному пути
    try:
        with open('/home/pi/New/ready_music.txt', 'w') as f:
            f.write("1" if play_ready_music else "0")
    except Exception as e:
        logger.error(f"Error saving ready music state: {e}")
        
    # Включаем/выключаем на лету, если мы сейчас в режиме Ready (go == 3)
    if go == 3:
        if play_ready_music:
            play_background_music("fon1.mp3", loops=-1)
        else:
            pygame.mixer.music.stop()
# ---------------------------------------------------------------

# --- Обработчик переключателя Internet sharing с пульта ---
@socketio.on('toggle_internet_sharing')
def handle_internet_toggle(is_checked):
    global internet_sharing
    if isinstance(is_checked, str):
        internet_sharing = (is_checked.lower() == 'true')
    else:
        internet_sharing = bool(is_checked)
    try:
        with open(INTERNET_SHARE_FILE, 'w') as f:
            f.write("1" if internet_sharing else "0")
    except Exception as e:
        logger.error(f"Error saving internet sharing state: {e}")
    apply_internet_sharing(internet_sharing)
    logger.info(f"Internet sharing: {'ENABLED' if internet_sharing else 'DISABLED'}")
    socketio.emit('internet_state', internet_sharing, to=None)
# ---------------------------------------------------------------

# === СИСТЕМА АВТОРИЗАЦИИ (LOGIN) ===

# Красивая HTML страница входа (с галочкой)
LOGIN_PAGE_HTML = """
<!DOCTYPE html>
<html>
<head>
    <title>Вход в систему</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: sans-serif; display: flex; justify-content: center; align-items: center; height: 100vh; background: #222; color: #fff; }
        form { background: #333; padding: 20px; border-radius: 10px; text-align: center; box-shadow: 0 0 10px rgba(0,0,0,0.5); }
        input[type="password"] { padding: 10px; margin: 10px 0; width: 80%; border-radius: 5px; border: none; }
        button { padding: 10px 20px; background: #28a745; color: white; border: none; border-radius: 5px; cursor: pointer; font-size: 16px; margin-top: 10px;}
        button:hover { background: #218838; }
        .checkbox-container { margin: 10px 0; font-size: 0.9em; color: #ccc; }
        input[type="checkbox"] { transform: scale(1.2); margin-right: 5px; }
    </style>
</head>
<body>
    <form method="post">
        <h2>QUEST CONTROL</h2>
        <input type="password" name="password" placeholder="Password" required>
        <br>
        <div class="checkbox-container">
            <label>
                <input type="checkbox" name="remember" value="yes"> Remember me
            </label>
        </div>
        <button type="submit">Sign in</button>
    </form>
</body>
</html>
"""

# Проверка перед каждым запросом: вошел ли админ?
@app.before_request
def before_request():
    # Список эндпоинтов, которым НЕ нужен логин
    allowed_endpoints = ['login', 'static', 'api', 'api_log']
    
    # Также проверяем, не начинается ли путь с /api (на случай если эндпоинт не определен)
    if request.path.startswith('/api'):
        return None

    if not session.get('logged_in') and request.endpoint not in allowed_endpoints:
        return redirect(url_for('login'))

# Страница входа
@app.route('/login', methods=['GET', 'POST'])
def login():
    if request.method == 'POST':
        password = request.form.get('password')
        remember = request.form.get('remember') # Проверяем, нажата ли галочка
        
        if password == ADMIN_PASSWORD:
            session['logged_in'] = True
            
            if remember:
                # Если галочка стоит - запоминаем на 1 год (365 дней)
                session.permanent = True
                app.permanent_session_lifetime = timedelta(days=365)
            else:
                # Если не стоит - забываем при закрытии браузера
                session.permanent = False
                
            return redirect('/') 
        else:
            return "<h1>Неверный пароль!</h1><a href='/login'>Попробовать снова</a>"
            
    return render_template_string(LOGIN_PAGE_HTML)

# Возможность выйти (если понадобится)
@app.route('/logout')
def logout():
    session.pop('logged_in', None)
    return redirect(url_for('login'))

# === КОНЕЦ СИСТЕМЫ АВТОРИЗАЦИИ ===

#------наша основная программа крутиться здесь сам сервер порт можно измнить(при желании) методы таймер и сериал работают ассинхронно
if __name__ == '__main__':
    try:
        # логирование запуска ---
        # Проверка на некорректное завершение ---
        if os.path.exists(LATCH_FILE):
            # Файл существует с прошлого запуска. Это НЕКОРРЕКТНОЕ выключение.
            logger.warning(f"STARTUP: Обнаружен '{LATCH_FILE}'. Предыдущее завершение работы было некорректным.")
            show_improper_shutdown_warning = True # Флаг для отправки в UI
        else:
            # Файл не существует. Это был чистый запуск или корректное выключение.
            logger.debug(f"STARTUP: '{LATCH_FILE}' не найден. Запуск в штатном режиме.")
            show_improper_shutdown_warning = False

        # (Пере)создаем файл-метку для ТЕКУЩЕЙ сессии
        try:
            with open(LATCH_FILE, 'w') as f:
                f.write(f"Server started at {time.ctime()}")
            logger.debug(f"STARTUP: Файл '{LATCH_FILE}' создан для текущей сессии.")
        except Exception as e:
            logger.debug(f"STARTUP: Не удалось создать '{LATCH_FILE}': {e}")
        # Используем .debug(), чтобы это сообщение попало только в файл логов, а не в консоль.
        logger.debug("STARTUP: Сервер CastleServer.py успешно запущен.")
        
        # Фоновая функция для подключения к Wi-Fi без торможения квеста
        def startup_wifi_routine():
            logger.info("WI-FI: Ожидание инициализации USB-адаптера (12 сек)...")
            eventlet.sleep(12) # Ждем, пока свисток проснется после включения в розетку

            # Проверяем, был ли Wi-Fi включен в прошлый раз
            if not os.path.exists('/home/pi/wifi_enabled'):
                logger.info("WI-FI: Флаг wifi_enabled не найден — пропускаем автоподключение.")
                return

            text_to_wlan1_reset() # Включаем адаптер
            eventlet.sleep(3) # Даем время подхватить интерфейс

            # Перезапускаем dhcpcd — он управляет wpa_supplicant на Raspberry Pi
            subprocess.run("sudo systemctl restart dhcpcd", shell=True, check=False)
            logger.info("WI-FI: dhcpcd перезапущен, ждем подключения...")

            eventlet.sleep(5) # Даем dhcpcd время поднять wpa_supplicant

            # Проверяем подключение (до 30 секунд)
            for i in range(30):
                eventlet.sleep(1)
                try:
                    ssid_bytes = subprocess.check_output("iwgetid -r wlan1", shell=True)
                    current_ssid = ssid_bytes.decode('utf-8').strip()
                    if current_ssid:
                        ip_addr = "no IP"
                        try:
                            ip_cmd = "ip -4 addr show wlan1 | grep -oP '(?<=inet\\s)\\d+(\\.\\d+){3}'"
                            ip_bytes = subprocess.check_output(ip_cmd, shell=True)
                            ip_addr = ip_bytes.decode('utf-8').strip()
                        except:
                            pass
                        logger.info(f"WI-FI: Автоподключение успешно: {current_ssid} ({ip_addr})")
                        return
                except:
                    pass

            logger.warning("WI-FI: Автоподключение не удалось (таймаут 15 сек).")

        logger.info("Starting background tasks...")
        socketio.start_background_task(target=startup_wifi_routine) # Запускаем Wi-Fi в фоне
        socketio.start_background_task(target=serial)
        socketio.start_background_task(target=timer)
        socketio.start_background_task(target=system_status_loop) # Технический пульт: статусы устройств
        socketio.start_background_task(target=hostapd_bootstrap_watchdog)  # Авто-восстановление если hostapd встал кривой
        socketio.start_background_task(target=wlan1_watchdog)  # Авто-восстановление wlan1 если USB-донгл отвалился
        socketio.start_background_task(target=tailscale_watchdog)  # Авто-восстановление Tailscale (logged out / DNS fail)
        socketio.start_background_task(target=mega_boot_watchdog)  # Авто-DTR-reset если Mega молчит после cold-boot
        logger.info("Starting Flask-SocketIO server on http://0.0.0.0:3000")
        socketio.run(app, port=3000, host='0.0.0.0')
    except OSError as e:
        logger.critical(f"Could not start server. Error: {e}")
        logger.critical("HINT: The port 3000 might be in use by another application.")
    except Exception as e:
        logger.critical(f"An unexpected error occurred: {e}")
        
