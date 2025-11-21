import eventlet
eventlet.monkey_patch()

# ДОБАВЛЕНО: Словарь для расшифровки событий в логах
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
from flask import Flask, send_file, request, jsonify
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
            # --- Добавлены новые фильтры ---
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
            # --- Конец добавления ---
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
file_handler.setLevel(logging.DEBUG) # Файл пишет ВСЁ (DEBUG и выше)
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

serial_write_queue = eventlet.queue.Queue()
pending_commands = {} # Словарь для хранения последних команд
device_timers = {}    # Словарь для отслеживания активных таймеров
#----переменные 
ESP32_IP_WOLF = "192.168.0.201" 
ESP32_IP_TRAIN = "192.168.0.202" 
ESP32_IP_SUITCASE = "192.168.0.203" 
ESP32_IP_SAFE = "192.168.0.204" 
ESP32_API_WOLF_URL = f"http://{ESP32_IP_WOLF}/data"
ESP32_API_TRAIN_URL = f"http://{ESP32_IP_TRAIN}/data"
ESP32_API_SUITCASE_URL = f"http://{ESP32_IP_SUITCASE}/data"
ESP32_API_SAFE_URL = f"http://{ESP32_IP_SAFE}/data"
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
is_processing_ready = False
is_time_synced = False
mansard_galets = set()
last_mansard_count = 0
last_owl_flew_time = 0
last_boy_in_time = 0
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)
pygame.mixer.pre_init(44100, -16, 2, 2048)
pygame.init()
pygame.mixer.init()
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

#------эффекты в формате wav
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
# Списки для голов и историй ---
player_goal_sounds = [goal1,goal2, goal3, goal4, goal5, goal6, goal7]

#тут очень много историй и подсказок так что старайся если будешь добавлять файлы сжимать их предусмотрено много языков так что можно добавлять вплоть до китайского в нашей
# версии на последний момент было 3 языка просто замени названия

story_1_en = pygame.mixer.Sound("story_1_en.wav")
story_1_ar = pygame.mixer.Sound("story_1_ar.wav")
story_1_ru = pygame.mixer.Sound("story_1_ru.wav")

story_2_a_en = pygame.mixer.Sound("story_2_a_en.wav")
story_2_a_ar = pygame.mixer.Sound("story_2_a_ar.wav")
story_2_a_ru = pygame.mixer.Sound("story_2_a_ru.wav")

story_2_b_en = pygame.mixer.Sound("story_2_b_en.wav")
story_2_b_ar = pygame.mixer.Sound("story_2_b_ar.wav")
story_2_b_ru = pygame.mixer.Sound("story_2_b_ru.wav")

story_2_r_en = pygame.mixer.Sound("story_2_r_en.wav")
story_2_r_ar = pygame.mixer.Sound("story_2_r_ar.wav")
story_2_r_ru = pygame.mixer.Sound("story_2_r_ru.wav")

story_3_en = pygame.mixer.Sound("story_3_en.wav")
story_3_ar = pygame.mixer.Sound("story_3_ar.wav")
story_3_ru = pygame.mixer.Sound("story_3_ru.wav")

story_3_r_en = pygame.mixer.Sound("story_3_r_en.wav")
story_3_r_ar = pygame.mixer.Sound("story_3_r_ar.wav")
story_3_r_ru = pygame.mixer.Sound("story_3_r_ru.wav")

story_3_a_en = pygame.mixer.Sound("story_3_a_en.wav")
story_3_a_ar = pygame.mixer.Sound("story_3_a_ar.wav")
story_3_a_ru = pygame.mixer.Sound("story_3_a_ru.wav")

story_3_b_en = pygame.mixer.Sound("story_3_b_en.wav")
story_3_b_ar = pygame.mixer.Sound("story_3_b_ar.wav")
story_3_b_ru = pygame.mixer.Sound("story_3_b_ru.wav")

story_3_c_en = pygame.mixer.Sound("story_3_c_en.wav")
story_3_c_ar = pygame.mixer.Sound("story_3_c_ar.wav")
story_3_c_ru = pygame.mixer.Sound("story_3_c_ru.wav")

story_4_en = pygame.mixer.Sound("story_4_en.wav")
story_4_ar = pygame.mixer.Sound("story_4_ar.wav")
story_4_ru = pygame.mixer.Sound("story_4_ru.wav")

story_5_en = pygame.mixer.Sound("story_5_en.wav")
story_5_ar = pygame.mixer.Sound("story_5_ar.wav")
story_5_ru = pygame.mixer.Sound("story_5_ru.wav")

story_6_en = pygame.mixer.Sound("story_6_en.wav")
story_6_ar = pygame.mixer.Sound("story_6_ar.wav")
story_6_ru = pygame.mixer.Sound("story_6_ru.wav")

story_10_en = pygame.mixer.Sound("story_10_en.wav")
story_10_ar = pygame.mixer.Sound("story_10_ar.wav")
story_10_ru = pygame.mixer.Sound("story_10_ru.wav")

story_11_en = pygame.mixer.Sound("story_11_en.wav")
story_11_ar = pygame.mixer.Sound("story_11_ar.wav")
story_11_ru = pygame.mixer.Sound("story_11_ru.wav")

story_12_a_en = pygame.mixer.Sound("story_12_a_en.wav")
story_12_a_ar = pygame.mixer.Sound("story_12_a_ar.wav")
story_12_a_ru = pygame.mixer.Sound("story_12_a_ru.wav")

story_12_b_en = pygame.mixer.Sound("story_12_b_en.wav")
story_12_b_ar = pygame.mixer.Sound("story_12_b_ar.wav")
story_12_b_ru = pygame.mixer.Sound("story_12_b_ru.wav")

story_12_c_en = pygame.mixer.Sound("story_12_c_en.wav")
story_12_c_ar = pygame.mixer.Sound("story_12_c_ar.wav")
story_12_c_ru = pygame.mixer.Sound("story_12_c_ru.wav")

story_12_d_en = pygame.mixer.Sound("story_12_d_en.wav")
story_12_d_ar = pygame.mixer.Sound("story_12_d_ar.wav")
story_12_d_ru = pygame.mixer.Sound("story_12_d_ru.wav")

story_13_en = pygame.mixer.Sound("story_13_en.wav")
story_13_ar = pygame.mixer.Sound("story_13_ar.wav")
story_13_ru = pygame.mixer.Sound("story_13_ru.wav")

story_14_a_en = pygame.mixer.Sound("story_14_a_en.wav")
story_14_a_ar = pygame.mixer.Sound("story_14_a_ar.wav")
story_14_a_ru = pygame.mixer.Sound("story_14_a_ru.wav")

story_14_b_en = pygame.mixer.Sound("story_14_b_en.wav")
story_14_b_ar = pygame.mixer.Sound("story_14_b_ar.wav")
story_14_b_ru = pygame.mixer.Sound("story_14_b_ru.wav")

story_17_en = pygame.mixer.Sound("story_17_en.wav")
story_17_ar = pygame.mixer.Sound("story_17_ar.wav")
story_17_ru = pygame.mixer.Sound("story_17_ru.wav")

story_18_en = pygame.mixer.Sound("story_18_en.wav")
story_18_ar = pygame.mixer.Sound("story_18_ar.wav")
story_18_ru = pygame.mixer.Sound("story_18_ru.wav")

story_19_en = pygame.mixer.Sound("story_19_en.wav")
story_19_ar = pygame.mixer.Sound("story_19_ar.wav")
story_19_ru = pygame.mixer.Sound("story_19_ru.wav")

story_20_a_en = pygame.mixer.Sound("story_20_a_en.wav")
story_20_a_ar = pygame.mixer.Sound("story_20_a_ar.wav")
story_20_a_ru = pygame.mixer.Sound("story_20_a_ru.wav")

story_20_b_en = pygame.mixer.Sound("story_20_b_en.wav")
story_20_b_ar = pygame.mixer.Sound("story_20_b_ar.wav")
story_20_b_ru = pygame.mixer.Sound("story_20_b_ru.wav")

story_20_c_en = pygame.mixer.Sound("story_20_c_en.wav")
story_20_c_ar = pygame.mixer.Sound("story_20_c_ar.wav")
story_20_c_ru = pygame.mixer.Sound("story_20_c_ru.wav")

story_21_en = pygame.mixer.Sound("story_21_en.wav")
story_21_ar = pygame.mixer.Sound("story_21_ar.wav")
story_21_ru = pygame.mixer.Sound("story_21_ru.wav")

story_22_a_en = pygame.mixer.Sound("story_22_a_en.wav")
story_22_a_ar = pygame.mixer.Sound("story_22_a_ar.wav")
story_22_a_ru = pygame.mixer.Sound("story_22_a_ru.wav")

story_22_b_en = pygame.mixer.Sound("story_22_b_en.wav")
story_22_b_ar = pygame.mixer.Sound("story_22_b_ar.wav")
story_22_b_ru = pygame.mixer.Sound("story_22_b_ru.wav")

story_22_c_en = pygame.mixer.Sound("story_22_c_en.wav")
story_22_c_ar = pygame.mixer.Sound("story_22_c_ar.wav")
story_22_c_ru = pygame.mixer.Sound("story_22_c_ru.wav")

story_23_en = pygame.mixer.Sound("story_23_en.wav")
story_23_ar = pygame.mixer.Sound("story_23_ar.wav")
story_23_ru = pygame.mixer.Sound("story_23_ru.wav")

story_24_en = pygame.mixer.Sound("story_24_en.wav")
story_24_ar = pygame.mixer.Sound("story_24_ar.wav")
story_24_ru = pygame.mixer.Sound("story_24_ru.wav")

story_25_en = pygame.mixer.Sound("story_25_en.wav")
story_25_ar = pygame.mixer.Sound("story_25_ar.wav")
story_25_ru = pygame.mixer.Sound("story_25_ru.wav")

story_26_en = pygame.mixer.Sound("story_26_en.wav")
story_26_ar = pygame.mixer.Sound("story_26_ar.wav")
story_26_ru = pygame.mixer.Sound("story_26_ru.wav")

story_27_a_en = pygame.mixer.Sound("story_27_a_en.wav")
story_27_a_ar = pygame.mixer.Sound("story_27_a_ar.wav")
story_27_a_ru = pygame.mixer.Sound("story_27_a_ru.wav")

story_27_b_en = pygame.mixer.Sound("story_27_b_en.wav")
story_27_b_ar = pygame.mixer.Sound("story_27_b_ar.wav")
story_27_b_ru = pygame.mixer.Sound("story_27_b_ru.wav")

story_27_c_en = pygame.mixer.Sound("story_27_c_en.wav")
story_27_c_ar = pygame.mixer.Sound("story_27_c_ar.wav")
story_27_c_ru = pygame.mixer.Sound("story_27_c_ru.wav")

story_30_en = pygame.mixer.Sound("story_30_en.wav")
story_30_ar = pygame.mixer.Sound("story_30_ar.wav")
story_30_ru = pygame.mixer.Sound("story_30_ru.wav")

story_31_en = pygame.mixer.Sound("story_31_en.wav")
story_31_ar = pygame.mixer.Sound("story_31_ar.wav")
story_31_ru = pygame.mixer.Sound("story_31_ru.wav")

story_32_en = pygame.mixer.Sound("story_32_en.wav")
story_32_ar = pygame.mixer.Sound("story_32_ar.wav")
story_32_ru = pygame.mixer.Sound("story_32_ru.wav")

story_32_a_en = pygame.mixer.Sound("story_32_a_en.wav")
story_32_a_ar = pygame.mixer.Sound("story_32_a_ar.wav")
story_32_a_ru = pygame.mixer.Sound("story_32_a_ru.wav")

story_32_b_en = pygame.mixer.Sound("story_32_b_en.wav")
story_32_b_ar = pygame.mixer.Sound("story_32_b_ar.wav")
story_32_b_ru = pygame.mixer.Sound("story_32_b_ru.wav")

story_32_c_en = pygame.mixer.Sound("story_32_c_en.wav")
story_32_c_ar = pygame.mixer.Sound("story_32_c_ar.wav")
story_32_c_ru = pygame.mixer.Sound("story_32_c_ru.wav")

story_33_en = pygame.mixer.Sound("story_33_en.wav")
story_33_ar = pygame.mixer.Sound("story_33_ar.wav")
story_33_ru = pygame.mixer.Sound("story_33_ru.wav")

story_34_en = pygame.mixer.Sound("story_34_en.wav")
story_34_ar = pygame.mixer.Sound("story_34_ar.wav")
story_34_ru = pygame.mixer.Sound("story_34_ru.wav")

story_35_en = pygame.mixer.Sound("story_35_en.wav")
story_35_ar = pygame.mixer.Sound("story_35_ar.wav")
story_35_ru = pygame.mixer.Sound("story_35_ru.wav")

story_36_en = pygame.mixer.Sound("story_36_en.wav")
story_36_ar = pygame.mixer.Sound("story_36_ar.wav")
story_36_ru = pygame.mixer.Sound("story_36_ru.wav")

story_37_en = pygame.mixer.Sound("story_37_en.wav")
story_37_ar = pygame.mixer.Sound("story_37_ar.wav")
story_37_ru = pygame.mixer.Sound("story_37_ru.wav")

story_38_en = pygame.mixer.Sound("story_38_en.wav")
story_38_ar = pygame.mixer.Sound("story_38_ar.wav")
story_38_ru = pygame.mixer.Sound("story_38_ru.wav")

story_39_en = pygame.mixer.Sound("story_39_en.wav")
story_39_ar = pygame.mixer.Sound("story_39_ar.wav")
story_39_ru = pygame.mixer.Sound("story_39_ru.wav")

story_40_en = pygame.mixer.Sound("story_40_en.wav")
story_40_ar = pygame.mixer.Sound("story_40_ar.wav")
story_40_ru = pygame.mixer.Sound("story_40_ru.wav")

story_41_en = pygame.mixer.Sound("story_41_en.wav")
story_41_ar = pygame.mixer.Sound("story_41_ar.wav")
story_41_ru = pygame.mixer.Sound("story_41_ru.wav")

story_42_en = pygame.mixer.Sound("story_42_en.wav")
story_42_ar = pygame.mixer.Sound("story_42_ar.wav")
story_42_ru = pygame.mixer.Sound("story_42_ru.wav")

story_43_en = pygame.mixer.Sound("story_43_en.wav")
story_43_ar = pygame.mixer.Sound("story_43_ar.wav")
story_43_ru = pygame.mixer.Sound("story_43_ru.wav")

story_44_en = pygame.mixer.Sound("story_44_en.wav")
story_44_ar = pygame.mixer.Sound("story_44_ar.wav")
story_44_ru = pygame.mixer.Sound("story_44_ru.wav")

#story_45_en = pygame.mixer.Sound("story_45_en.wav")
#story_45_ar = pygame.mixer.Sound("story_45_ar.wav")
#story_45_ru = pygame.mixer.Sound("story_45_ru.wav")

story_46_en = pygame.mixer.Sound("story_46_en.wav")
story_46_ar = pygame.mixer.Sound("story_46_ar.wav")
story_46_ru = pygame.mixer.Sound("story_46_ru.wav")

story_47_en = pygame.mixer.Sound("story_47_en.wav")
story_47_ar = pygame.mixer.Sound("story_47_ar.wav")
story_47_ru = pygame.mixer.Sound("story_47_ru.wav")

story_48_en = pygame.mixer.Sound("story_48_en.wav")
story_48_ar = pygame.mixer.Sound("story_48_ar.wav")
story_48_ru = pygame.mixer.Sound("story_48_ru.wav")

story_49_en = pygame.mixer.Sound("story_49_en.wav")
story_49_ar = pygame.mixer.Sound("story_49_ar.wav")
story_49_ru = pygame.mixer.Sound("story_49_ru.wav")

story_50_en = pygame.mixer.Sound("story_50_en.wav")
story_50_ar = pygame.mixer.Sound("story_50_ar.wav")
story_50_ru = pygame.mixer.Sound("story_50_ru.wav")

story_51_en = pygame.mixer.Sound("story_51_en.wav")
story_51_ar = pygame.mixer.Sound("story_51_ar.wav")
story_51_ru = pygame.mixer.Sound("story_51_ru.wav")

story_52_en = pygame.mixer.Sound("story_52_en.wav")
story_52_ar = pygame.mixer.Sound("story_52_ar.wav")
story_52_ru = pygame.mixer.Sound("story_52_ru.wav")

story_53_en = pygame.mixer.Sound("story_53_en.wav")
story_53_ar = pygame.mixer.Sound("story_53_ar.wav")
story_53_ru = pygame.mixer.Sound("story_53_ru.wav")

story_54_en = pygame.mixer.Sound("story_54_en.wav")
story_54_ar = pygame.mixer.Sound("story_54_ar.wav")
story_54_ru = pygame.mixer.Sound("story_54_ru.wav")

story_55_en = pygame.mixer.Sound("story_55_en.wav")
story_55_ar = pygame.mixer.Sound("story_55_ar.wav")
story_55_ru = pygame.mixer.Sound("story_55_ru.wav")

story_56_en = pygame.mixer.Sound("story_56_en.wav")
story_56_ar = pygame.mixer.Sound("story_56_ar.wav")
story_56_ru = pygame.mixer.Sound("story_56_ru.wav")

story_57_en = pygame.mixer.Sound("story_57_en.wav")
story_57_ar = pygame.mixer.Sound("story_57_ar.wav")
story_57_ru = pygame.mixer.Sound("story_57_ru.wav")

story_58_en = pygame.mixer.Sound("story_58_en.wav")
story_58_ar = pygame.mixer.Sound("story_58_ar.wav")
story_58_ru = pygame.mixer.Sound("story_58_ru.wav")

story_59_en = pygame.mixer.Sound("story_59_en.wav")
story_59_ar = pygame.mixer.Sound("story_59_ar.wav")
story_59_ru = pygame.mixer.Sound("story_59_ru.wav")

story_60_a_en = pygame.mixer.Sound("story_60_a_en.wav")
story_60_a_ar = pygame.mixer.Sound("story_60_a_ar.wav")
story_60_a_ru = pygame.mixer.Sound("story_60_a_ru.wav")

story_60_b_en = pygame.mixer.Sound("story_60_b_en.wav")
story_60_b_ar = pygame.mixer.Sound("story_60_b_ar.wav")
story_60_b_ru = pygame.mixer.Sound("story_60_b_ru.wav")

story_60_c_en = pygame.mixer.Sound("story_60_c_en.wav")
story_60_c_ar = pygame.mixer.Sound("story_60_c_ar.wav")
story_60_c_ru = pygame.mixer.Sound("story_60_c_ru.wav")

story_60_d_en = pygame.mixer.Sound("story_60_d_en.wav")
story_60_d_ar = pygame.mixer.Sound("story_60_d_ar.wav")
story_60_d_ru = pygame.mixer.Sound("story_60_d_ru.wav")

story_60_e_en = pygame.mixer.Sound("story_60_e_en.wav")
story_60_e_ar = pygame.mixer.Sound("story_60_e_ar.wav")
story_60_e_ru = pygame.mixer.Sound("story_60_e_ru.wav")

story_60_f_en = pygame.mixer.Sound("story_60_f_en.wav")
story_60_f_ar = pygame.mixer.Sound("story_60_f_ar.wav")
story_60_f_ru = pygame.mixer.Sound("story_60_f_ru.wav")

story_60_g_en = pygame.mixer.Sound("story_60_g_en.wav")
story_60_g_ar = pygame.mixer.Sound("story_60_g_ar.wav")
story_60_g_ru = pygame.mixer.Sound("story_60_g_ru.wav")

story_60_h_en = pygame.mixer.Sound("story_60_h_en.wav")
story_60_h_ar = pygame.mixer.Sound("story_60_h_ar.wav")
story_60_h_ru = pygame.mixer.Sound("story_60_h_ru.wav")

story_60_i_en = pygame.mixer.Sound("story_60_i_en.wav")
story_60_i_ar = pygame.mixer.Sound("story_60_i_ar.wav")
story_60_i_ru = pygame.mixer.Sound("story_60_i_ru.wav")

story_60_j_en = pygame.mixer.Sound("story_60_j_en.wav")
story_60_j_ar = pygame.mixer.Sound("story_60_j_ar.wav")
story_60_j_ru = pygame.mixer.Sound("story_60_j_ru.wav")


story_61_a_en = pygame.mixer.Sound("story_61_a_en.wav")
story_61_a_ar = pygame.mixer.Sound("story_61_a_ar.wav")
story_61_a_ru = pygame.mixer.Sound("story_61_a_ru.wav")

story_61_b_en = pygame.mixer.Sound("story_61_b_en.wav")
story_61_b_ar = pygame.mixer.Sound("story_61_b_ar.wav")
story_61_b_ru = pygame.mixer.Sound("story_61_b_ru.wav")

story_61_d_en = pygame.mixer.Sound("story_61_d_en.wav")
story_61_d_ar = pygame.mixer.Sound("story_61_d_ar.wav")
story_61_d_ru = pygame.mixer.Sound("story_61_d_ru.wav")

story_61_e_en = pygame.mixer.Sound("story_61_e_en.wav")
story_61_e_ar = pygame.mixer.Sound("story_61_e_ar.wav")
story_61_e_ru = pygame.mixer.Sound("story_61_e_ru.wav")

story_61_f_en = pygame.mixer.Sound("story_61_f_en.wav")
story_61_f_ar = pygame.mixer.Sound("story_61_f_ar.wav")
story_61_f_ru = pygame.mixer.Sound("story_61_f_ru.wav")

story_61_g_en = pygame.mixer.Sound("story_61_g_en.wav")
story_61_g_ar = pygame.mixer.Sound("story_61_g_ar.wav")
story_61_g_ru = pygame.mixer.Sound("story_61_g_ru.wav")

story_61_h_en = pygame.mixer.Sound("story_61_h_en.wav")
story_61_h_ar = pygame.mixer.Sound("story_61_h_ar.wav")
story_61_h_ru = pygame.mixer.Sound("story_61_h_ru.wav")

story_61_i_en = pygame.mixer.Sound("story_61_i_en.wav")
story_61_i_ar = pygame.mixer.Sound("story_61_i_ar.wav")
story_61_i_ru = pygame.mixer.Sound("story_61_i_ru.wav")

story_61_j_en = pygame.mixer.Sound("story_61_j_en.wav")
story_61_j_ar = pygame.mixer.Sound("story_61_j_ar.wav")
story_61_j_ru = pygame.mixer.Sound("story_61_j_ru.wav")

story_61_c_en = pygame.mixer.Sound("story_61_c_en.wav")
story_61_c_ar = pygame.mixer.Sound("story_61_c_ar.wav")
story_61_c_ru = pygame.mixer.Sound("story_61_c_ru.wav")

story_62_a_en = pygame.mixer.Sound("story_62_a_en.wav")
story_62_a_ar = pygame.mixer.Sound("story_62_a_ar.wav")
story_62_a_ru = pygame.mixer.Sound("story_62_a_ru.wav")

story_62_b_en = pygame.mixer.Sound("story_62_b_en.wav")
story_62_b_ar = pygame.mixer.Sound("story_62_b_ar.wav")
story_62_b_ru = pygame.mixer.Sound("story_62_b_ru.wav")

story_62_c_en = pygame.mixer.Sound("story_62_c_en.wav")
story_62_c_ar = pygame.mixer.Sound("story_62_c_ar.wav")
story_62_c_ru = pygame.mixer.Sound("story_62_c_ru.wav")

story_62_d_en = pygame.mixer.Sound("story_62_d_en.wav")
story_62_d_ar = pygame.mixer.Sound("story_62_d_ar.wav")
story_62_d_ru = pygame.mixer.Sound("story_62_d_ru.wav")

story_62_e_en = pygame.mixer.Sound("story_62_e_en.wav")
story_62_e_ar = pygame.mixer.Sound("story_62_e_ar.wav")
story_62_e_ru = pygame.mixer.Sound("story_62_e_ru.wav")

story_63_a_en = pygame.mixer.Sound("story_63_a_en.wav")
story_63_a_ar = pygame.mixer.Sound("story_63_a_ar.wav")
story_63_a_ru = pygame.mixer.Sound("story_63_a_ru.wav")

story_63_b_en = pygame.mixer.Sound("story_63_b_en.wav")
story_63_b_ar = pygame.mixer.Sound("story_63_b_ar.wav")
story_63_b_ru = pygame.mixer.Sound("story_63_b_ru.wav")

story_63_c_en = pygame.mixer.Sound("story_63_c_en.wav")
story_63_c_ar = pygame.mixer.Sound("story_63_c_ar.wav")
story_63_c_ru = pygame.mixer.Sound("story_63_c_ru.wav")

story_63_d_en = pygame.mixer.Sound("story_63_d_en.wav")
story_63_d_ar = pygame.mixer.Sound("story_63_d_ar.wav")
story_63_d_ru = pygame.mixer.Sound("story_63_d_ru.wav")

story_63_e_en = pygame.mixer.Sound("story_63_e_en.wav")
story_63_e_ar = pygame.mixer.Sound("story_63_e_ar.wav")
story_63_e_ru = pygame.mixer.Sound("story_63_e_ru.wav")

story_63_f_en = pygame.mixer.Sound("story_63_f_en.wav")
story_63_f_ar = pygame.mixer.Sound("story_63_f_ar.wav")
story_63_f_ru = pygame.mixer.Sound("story_63_f_ru.wav")

story_63_g_en = pygame.mixer.Sound("story_63_g_en.wav")
story_63_g_ar = pygame.mixer.Sound("story_63_g_ar.wav")
story_63_g_ru = pygame.mixer.Sound("story_63_g_ru.wav")

story_63_h_en = pygame.mixer.Sound("story_63_h_en.wav")
story_63_h_ar = pygame.mixer.Sound("story_63_h_ar.wav")
story_63_h_ru = pygame.mixer.Sound("story_63_h_ru.wav")

story_63_i_en = pygame.mixer.Sound("story_63_i_en.wav")
story_63_i_ar = pygame.mixer.Sound("story_63_i_ar.wav")
story_63_i_ru = pygame.mixer.Sound("story_63_i_ru.wav")

story_63_j_en = pygame.mixer.Sound("story_63_j_en.wav")
story_63_j_ar = pygame.mixer.Sound("story_63_j_ar.wav")
story_63_j_ru = pygame.mixer.Sound("story_63_j_ru.wav")

story_64_a_en = pygame.mixer.Sound("story_64_a_en.wav")
story_64_a_ar = pygame.mixer.Sound("story_64_a_ar.wav")
story_64_a_ru = pygame.mixer.Sound("story_64_a_ru.wav")

story_64_b_en = pygame.mixer.Sound("story_64_b_en.wav")
story_64_b_ar = pygame.mixer.Sound("story_64_b_ar.wav")
story_64_b_ru = pygame.mixer.Sound("story_64_b_ru.wav")

story_65_a_en = pygame.mixer.Sound("story_65_a_en.wav")
story_65_a_ar = pygame.mixer.Sound("story_65_a_ar.wav")
story_65_a_ru = pygame.mixer.Sound("story_65_a_ru.wav")

story_65_b_en = pygame.mixer.Sound("story_65_b_en.wav")
story_65_b_ar = pygame.mixer.Sound("story_65_b_ar.wav")
story_65_b_ru = pygame.mixer.Sound("story_65_b_ru.wav")

story_65_c_en = pygame.mixer.Sound("story_65_c_en.wav")
story_65_c_ar = pygame.mixer.Sound("story_65_c_ar.wav")
story_65_c_ru = pygame.mixer.Sound("story_65_c_ru.wav")

story_65_d_en = pygame.mixer.Sound("story_65_d_en.wav")
story_65_d_ar = pygame.mixer.Sound("story_65_d_ar.wav")
story_65_d_ru = pygame.mixer.Sound("story_65_d_ru.wav")

story_65_e_en = pygame.mixer.Sound("story_65_e_en.wav")
story_65_e_ar = pygame.mixer.Sound("story_65_e_ar.wav")
story_65_e_ru = pygame.mixer.Sound("story_65_e_ru.wav")

story_65_f_en = pygame.mixer.Sound("story_65_f_en.wav")
story_65_f_ar = pygame.mixer.Sound("story_65_f_ar.wav")
story_65_f_ru = pygame.mixer.Sound("story_65_f_ru.wav")

story_65_g_en = pygame.mixer.Sound("story_65_g_en.wav")
story_65_g_ar = pygame.mixer.Sound("story_65_g_ar.wav")
story_65_g_ru = pygame.mixer.Sound("story_65_g_ru.wav")

story_65_h_en = pygame.mixer.Sound("story_65_h_en.wav")
story_65_h_ar = pygame.mixer.Sound("story_65_h_ar.wav")
story_65_h_ru = pygame.mixer.Sound("story_65_h_ru.wav")

story_65_i_en = pygame.mixer.Sound("story_65_i_en.wav")
story_65_i_ar = pygame.mixer.Sound("story_65_i_ar.wav")
story_65_i_ru = pygame.mixer.Sound("story_65_i_ru.wav")

story_65_j_en = pygame.mixer.Sound("story_65_j_en.wav")
story_65_j_ar = pygame.mixer.Sound("story_65_j_ar.wav")
story_65_j_ru = pygame.mixer.Sound("story_65_j_ru.wav")

story_66_en = pygame.mixer.Sound("story_66_en.wav")
story_66_ar = pygame.mixer.Sound("story_66_ar.wav")
story_66_ru = pygame.mixer.Sound("story_66_ru.wav")

story_67_en = pygame.mixer.Sound("story_67_en.wav")
story_67_ar = pygame.mixer.Sound("story_67_ar.wav")
story_67_ru = pygame.mixer.Sound("story_67_ru.wav")

story_68_en = pygame.mixer.Sound("story_68_en.wav")
story_68_ar = pygame.mixer.Sound("story_68_ar.wav")
story_68_ru = pygame.mixer.Sound("story_68_ru.wav")

story_69_en = pygame.mixer.Sound("story_69_en.wav")
story_69_ar = pygame.mixer.Sound("story_69_ar.wav")
story_69_ru = pygame.mixer.Sound("story_69_ru.wav")

story_70_en = pygame.mixer.Sound("story_70_en.wav")
story_70_ar = pygame.mixer.Sound("story_70_ar.wav")
story_70_ru = pygame.mixer.Sound("story_70_ru.wav")

#дракон
hint_2_b_en = pygame.mixer.Sound("hint_2_b_en.wav")
hint_2_b_ar = pygame.mixer.Sound("hint_2_b_ar.wav")
hint_2_b_ru = pygame.mixer.Sound("hint_2_b_ru.wav")

hint_2_c_en = pygame.mixer.Sound("hint_2_c_en.wav")
hint_2_c_ar = pygame.mixer.Sound("hint_2_c_ar.wav")
hint_2_c_ru = pygame.mixer.Sound("hint_2_c_ru.wav")

hint_2_z_en = pygame.mixer.Sound("hint_2_z_en.wav")
hint_2_z_ar = pygame.mixer.Sound("hint_2_z_ar.wav")
hint_2_z_ru = pygame.mixer.Sound("hint_2_z_ru.wav")

#мальчик
hint_3_b_en = pygame.mixer.Sound("hint_3_b_en.wav")
hint_3_b_ar = pygame.mixer.Sound("hint_3_b_ar.wav")
hint_3_b_ru = pygame.mixer.Sound("hint_3_b_ru.wav")

hint_3_c_en = pygame.mixer.Sound("hint_3_c_en.wav")
hint_3_c_ar = pygame.mixer.Sound("hint_3_c_ar.wav")
hint_3_c_ru = pygame.mixer.Sound("hint_3_c_ru.wav")

hint_3_z_en = pygame.mixer.Sound("hint_3_z_en.wav")
hint_3_z_ar = pygame.mixer.Sound("hint_3_z_ar.wav")
hint_3_z_ru = pygame.mixer.Sound("hint_3_z_ru.wav")

#дракон
hint_5_b_en = pygame.mixer.Sound("hint_5_b_en.wav")
hint_5_b_ar = pygame.mixer.Sound("hint_5_b_ar.wav")
hint_5_b_ru = pygame.mixer.Sound("hint_5_b_ru.wav")

hint_5_c_en = pygame.mixer.Sound("hint_5_c_en.wav")
hint_5_c_ar = pygame.mixer.Sound("hint_5_c_ar.wav")
hint_5_c_ru = pygame.mixer.Sound("hint_5_c_ru.wav")

#макгонагл
hint_6_b_en = pygame.mixer.Sound("hint_6_b_en.wav")
hint_6_b_ar = pygame.mixer.Sound("hint_6_b_ar.wav")
hint_6_b_ru = pygame.mixer.Sound("hint_6_b_ru.wav")

hint_6_c_en = pygame.mixer.Sound("hint_6_c_en.wav")
hint_6_c_ar = pygame.mixer.Sound("hint_6_c_ar.wav")
hint_6_c_ru = pygame.mixer.Sound("hint_6_c_ru.wav")

#макгонагл
hint_10_b_en = pygame.mixer.Sound("hint_10_b_en.wav")
hint_10_b_ar = pygame.mixer.Sound("hint_10_b_ar.wav")
hint_10_b_ru = pygame.mixer.Sound("hint_10_b_ru.wav")

hint_10_c_en = pygame.mixer.Sound("hint_10_c_en.wav")
hint_10_c_ar = pygame.mixer.Sound("hint_10_c_ar.wav")
hint_10_c_ru = pygame.mixer.Sound("hint_10_c_ru.wav")

hint_11_b_en = pygame.mixer.Sound("hint_11_b_en.wav")
hint_11_b_ar = pygame.mixer.Sound("hint_11_b_ar.wav")
hint_11_b_ru = pygame.mixer.Sound("hint_11_b_ru.wav")

hint_11_c_en = pygame.mixer.Sound("hint_11_c_en.wav")
hint_11_c_ar = pygame.mixer.Sound("hint_11_c_ar.wav")
hint_11_c_ru = pygame.mixer.Sound("hint_11_c_ru.wav")

hint_11_z_en = pygame.mixer.Sound("hint_11_z_en.wav")
hint_11_z_ar = pygame.mixer.Sound("hint_11_z_ar.wav")
hint_11_z_ru = pygame.mixer.Sound("hint_11_z_ru.wav")

#гном
hint_14_b_en = pygame.mixer.Sound("hint_14_b_en.wav")
hint_14_b_ar = pygame.mixer.Sound("hint_14_b_ar.wav")
hint_14_b_ru = pygame.mixer.Sound("hint_14_b_ru.wav")

hint_14_c_en = pygame.mixer.Sound("hint_14_c_en.wav")
hint_14_c_ar = pygame.mixer.Sound("hint_14_c_ar.wav")
hint_14_c_ru = pygame.mixer.Sound("hint_14_c_ru.wav")

hint_14_z_en = pygame.mixer.Sound("hint_14_z_en.wav")
hint_14_z_ar = pygame.mixer.Sound("hint_14_z_ar.wav")
hint_14_z_ru = pygame.mixer.Sound("hint_14_z_ru.wav")

#ведьма
hint_17_b_en = pygame.mixer.Sound("hint_17_b_en.wav")
hint_17_b_ar = pygame.mixer.Sound("hint_17_b_ar.wav")
hint_17_b_ru = pygame.mixer.Sound("hint_17_b_ru.wav")

hint_17_c_en = pygame.mixer.Sound("hint_17_c_en.wav")
hint_17_c_ar = pygame.mixer.Sound("hint_17_c_ar.wav")
hint_17_c_ru = pygame.mixer.Sound("hint_17_c_ru.wav")

hint_17_z_en = pygame.mixer.Sound("hint_17_z_en.wav")
hint_17_z_ar = pygame.mixer.Sound("hint_17_z_ar.wav")
hint_17_z_ru = pygame.mixer.Sound("hint_17_z_ru.wav")

#рыцарь
hint_19_b_en = pygame.mixer.Sound("hint_19_b_en.wav")
hint_19_b_ar = pygame.mixer.Sound("hint_19_b_ar.wav")
hint_19_b_ru = pygame.mixer.Sound("hint_19_b_ru.wav")

hint_19_c_en = pygame.mixer.Sound("hint_19_c_en.wav")
hint_19_c_ar = pygame.mixer.Sound("hint_19_c_ar.wav")
hint_19_c_ru = pygame.mixer.Sound("hint_19_c_ru.wav")

hint_19_z_en = pygame.mixer.Sound("hint_19_z_en.wav")
hint_19_z_ar = pygame.mixer.Sound("hint_19_z_ar.wav")
hint_19_z_ru = pygame.mixer.Sound("hint_19_z_ru.wav")

#гоблин
hint_23_b_en = pygame.mixer.Sound("hint_23_b_en.wav")
hint_23_b_ar = pygame.mixer.Sound("hint_23_b_ar.wav")
hint_23_b_ru = pygame.mixer.Sound("hint_23_b_ru.wav")

hint_23_c_en = pygame.mixer.Sound("hint_23_c_en.wav")
hint_23_c_ar = pygame.mixer.Sound("hint_23_c_ar.wav")
hint_23_c_ru = pygame.mixer.Sound("hint_23_c_ru.wav")

hint_23_z_en = pygame.mixer.Sound("hint_23_z_en.wav")
hint_23_z_ar = pygame.mixer.Sound("hint_23_z_ar.wav")
hint_23_z_ru = pygame.mixer.Sound("hint_23_z_ru.wav")

#троль
hint_26_b_en = pygame.mixer.Sound("hint_26_b_en.wav")
hint_26_b_ar = pygame.mixer.Sound("hint_26_b_ar.wav")
hint_26_b_ru = pygame.mixer.Sound("hint_26_b_ru.wav")

hint_26_c_en = pygame.mixer.Sound("hint_26_c_en.wav")
hint_26_c_ar = pygame.mixer.Sound("hint_26_c_ar.wav")
hint_26_c_ru = pygame.mixer.Sound("hint_26_c_ru.wav")

hint_26_z_en = pygame.mixer.Sound("hint_26_z_en.wav")
hint_26_z_ar = pygame.mixer.Sound("hint_26_z_ar.wav")
hint_26_z_ru = pygame.mixer.Sound("hint_26_z_ru.wav")

#плотник
hint_32_b_en = pygame.mixer.Sound("hint_32_b_en.wav")
hint_32_b_ar = pygame.mixer.Sound("hint_32_b_ar.wav")
hint_32_b_ru = pygame.mixer.Sound("hint_32_b_ru.wav")

hint_32_c_en = pygame.mixer.Sound("hint_32_c_en.wav")
hint_32_c_ar = pygame.mixer.Sound("hint_32_c_ar.wav")
hint_32_c_ru = pygame.mixer.Sound("hint_32_c_ru.wav")

hint_32_d_en = pygame.mixer.Sound("hint_32_d_en.wav")
hint_32_d_ar = pygame.mixer.Sound("hint_32_d_ar.wav")
hint_32_d_ru = pygame.mixer.Sound("hint_32_d_ru.wav")

hint_32_e_en = pygame.mixer.Sound("hint_32_e_en.wav")
hint_32_e_ar = pygame.mixer.Sound("hint_32_e_ar.wav")
hint_32_e_ru = pygame.mixer.Sound("hint_32_e_ru.wav")

hint_32_z_en = pygame.mixer.Sound("hint_32_z_en.wav")
hint_32_z_ar = pygame.mixer.Sound("hint_32_z_ar.wav")
hint_32_z_ru = pygame.mixer.Sound("hint_32_z_ru.wav")

#студент
hint_37_b_en = pygame.mixer.Sound("hint_37_b_en.wav")
hint_37_b_ar = pygame.mixer.Sound("hint_37_b_ar.wav")
hint_37_b_ru = pygame.mixer.Sound("hint_37_b_ru.wav")

hint_37_c_en = pygame.mixer.Sound("hint_37_c_en.wav")
hint_37_c_ar = pygame.mixer.Sound("hint_37_c_ar.wav")
hint_37_c_ru = pygame.mixer.Sound("hint_37_c_ru.wav")

hint_38_b_en = pygame.mixer.Sound("hint_38_b_en.wav")
hint_38_b_ar = pygame.mixer.Sound("hint_38_b_ar.wav")
hint_38_b_ru = pygame.mixer.Sound("hint_38_b_ru.wav")

#студент
hint_44_b_en = pygame.mixer.Sound("hint_44_b_en.wav")
hint_44_b_ar = pygame.mixer.Sound("hint_44_b_ar.wav")
hint_44_b_ru = pygame.mixer.Sound("hint_44_b_ru.wav")

hint_44_c_en = pygame.mixer.Sound("hint_44_c_en.wav")
hint_44_c_ar = pygame.mixer.Sound("hint_44_c_ar.wav")
hint_44_c_ru = pygame.mixer.Sound("hint_44_c_ru.wav")

#пугачева
hint_49_b_en = pygame.mixer.Sound("hint_49_b_en.wav")
hint_49_b_ar = pygame.mixer.Sound("hint_49_b_ar.wav")
hint_49_b_ru = pygame.mixer.Sound("hint_49_b_ru.wav")

hint_49_c_en = pygame.mixer.Sound("hint_49_c_en.wav")
hint_49_c_ar = pygame.mixer.Sound("hint_49_c_ar.wav")
hint_49_c_ru = pygame.mixer.Sound("hint_49_c_ru.wav")

#пугачева
hint_50_b_en = pygame.mixer.Sound("hint_50_b_en.wav")
hint_50_b_ar = pygame.mixer.Sound("hint_50_b_ar.wav")
hint_50_b_ru = pygame.mixer.Sound("hint_50_b_ru.wav")

hint_50_c_en = pygame.mixer.Sound("hint_50_c_en.wav")
hint_50_c_ar = pygame.mixer.Sound("hint_50_c_ar.wav")
hint_50_c_ru = pygame.mixer.Sound("hint_50_c_ru.wav")

#пугчева
hint_51_b_en = pygame.mixer.Sound("hint_51_b_en.wav")
hint_51_b_ar = pygame.mixer.Sound("hint_51_b_ar.wav")
hint_51_b_ru = pygame.mixer.Sound("hint_51_b_ru.wav")

hint_51_c_en = pygame.mixer.Sound("hint_51_c_en.wav")
hint_51_c_ar = pygame.mixer.Sound("hint_51_c_ar.wav")
hint_51_c_ru = pygame.mixer.Sound("hint_51_c_ru.wav")

#снова студент
hint_56_b_en = pygame.mixer.Sound("hint_56_b_en.wav")
hint_56_b_ar = pygame.mixer.Sound("hint_56_b_ar.wav")
hint_56_b_ru = pygame.mixer.Sound("hint_56_b_ru.wav")

# --- ДОБАВЛЕНО: Карта для логирования имен звуковых файлов ---
# Эта карта сопоставляет объект pygame.mixer.Sound с его именем файла (или переменной)
# для удобочитаемого лога.
try:
    _SOUND_NAME_MAP = {
        # Эффекты
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

        # Истории
        story_1_en: "story_1_en.wav",
        story_1_ar: "story_1_ar.wav",
        story_1_ru: "story_1_ru.wav",
        story_2_a_en: "story_2_a_en.wav",
        story_2_a_ar: "story_2_a_ar.wav",
        story_2_a_ru: "story_2_a_ru.wav",
        story_2_b_en: "story_2_b_en.wav",
        story_2_b_ar: "story_2_b_ar.wav",
        story_2_b_ru: "story_2_b_ru.wav",
        story_2_r_en: "story_2_r_en.wav",
        story_2_r_ar: "story_2_r_ar.wav",
        story_2_r_ru: "story_2_r_ru.wav",
        story_3_en: "story_3_en.wav",
        story_3_ar: "story_3_ar.wav",
        story_3_ru: "story_3_ru.wav",
        story_3_r_en: "story_3_r_en.wav",
        story_3_r_ar: "story_3_r_ar.wav",
        story_3_r_ru: "story_3_r_ru.wav",
        story_3_a_en: "story_3_a_en.wav",
        story_3_a_ar: "story_3_a_ar.wav",
        story_3_a_ru: "story_3_a_ru.wav",
        story_3_b_en: "story_3_b_en.wav",
        story_3_b_ar: "story_3_b_ar.wav",
        story_3_b_ru: "story_3_b_ru.wav",
        story_3_c_en: "story_3_c_en.wav",
        story_3_c_ar: "story_3_c_ar.wav",
        story_3_c_ru: "story_3_c_ru.wav",
        story_4_en: "story_4_en.wav",
        story_4_ar: "story_4_ar.wav",
        story_4_ru: "story_4_ru.wav",
        story_5_en: "story_5_en.wav",
        story_5_ar: "story_5_ar.wav",
        story_5_ru: "story_5_ru.wav",
        story_6_en: "story_6_en.wav",
        story_6_ar: "story_6_ar.wav",
        story_6_ru: "story_6_ru.wav",
        story_10_en: "story_10_en.wav",
        story_10_ar: "story_10_ar.wav",
        story_10_ru: "story_10_ru.wav",
        story_11_en: "story_11_en.wav",
        story_11_ar: "story_11_ar.wav",
        story_11_ru: "story_11_ru.wav",
        story_12_a_en: "story_12_a_en.wav",
        story_12_a_ar: "story_12_a_ar.wav",
        story_12_a_ru: "story_12_a_ru.wav",
        story_12_b_en: "story_12_b_en.wav",
        story_12_b_ar: "story_12_b_ar.wav",
        story_12_b_ru: "story_12_b_ru.wav",
        story_12_c_en: "story_12_c_en.wav",
        story_12_c_ar: "story_12_c_ar.wav",
        story_12_c_ru: "story_12_c_ru.wav",
        story_12_d_en: "story_12_d_en.wav",
        story_12_d_ar: "story_12_d_ar.wav",
        story_12_d_ru: "story_12_d_ru.wav",
        story_13_en: "story_13_en.wav",
        story_13_ar: "story_13_ar.wav",
        story_13_ru: "story_13_ru.wav",
        story_14_a_en: "story_14_a_en.wav",
        story_14_a_ar: "story_14_a_ar.wav",
        story_14_a_ru: "story_14_a_ru.wav",
        story_14_b_en: "story_14_b_en.wav",
        story_14_b_ar: "story_14_b_ar.wav",
        story_14_b_ru: "story_14_b_ru.wav",
        story_17_en: "story_17_en.wav",
        story_17_ar: "story_17_ar.wav",
        story_17_ru: "story_17_ru.wav",
        story_18_en: "story_18_en.wav",
        story_18_ar: "story_18_ar.wav",
        story_18_ru: "story_18_ru.wav",
        story_19_en: "story_19_en.wav",
        story_19_ar: "story_19_ar.wav",
        story_19_ru: "story_19_ru.wav",
        story_20_a_en: "story_20_a_en.wav",
        story_20_a_ar: "story_20_a_ar.wav",
        story_20_a_ru: "story_20_a_ru.wav",
        story_20_b_en: "story_20_b_en.wav",
        story_20_b_ar: "story_20_b_ar.wav",
        story_20_b_ru: "story_20_b_ru.wav",
        story_20_c_en: "story_20_c_en.wav",
        story_20_c_ar: "story_20_c_ar.wav",
        story_20_c_ru: "story_20_c_ru.wav",
        story_21_en: "story_21_en.wav",
        story_21_ar: "story_21_ar.wav",
        story_21_ru: "story_21_ru.wav",
        story_22_a_en: "story_22_a_en.wav",
        story_22_a_ar: "story_22_a_ar.wav",
        story_22_a_ru: "story_22_a_ru.wav",
        story_22_b_en: "story_22_b_en.wav",
        story_22_b_ar: "story_22_b_ar.wav",
        story_22_b_ru: "story_22_b_ru.wav",
        story_22_c_en: "story_22_c_en.wav",
        story_22_c_ar: "story_22_c_ar.wav",
        story_22_c_ru: "story_22_c_ru.wav",
        story_23_en: "story_23_en.wav",
        story_23_ar: "story_23_ar.wav",
        story_23_ru: "story_23_ru.wav",
        story_24_en: "story_24_en.wav",
        story_24_ar: "story_24_ar.wav",
        story_24_ru: "story_24_ru.wav",
        story_25_en: "story_25_en.wav",
        story_25_ar: "story_25_ar.wav",
        story_25_ru: "story_25_ru.wav",
        story_26_en: "story_26_en.wav",
        story_26_ar: "story_26_ar.wav",
        story_26_ru: "story_26_ru.wav",
        story_27_a_en: "story_27_a_en.wav",
        story_27_a_ar: "story_27_a_ar.wav",
        story_27_a_ru: "story_27_a_ru.wav",
        story_27_b_en: "story_27_b_en.wav",
        story_27_b_ar: "story_27_b_ar.wav",
        story_27_b_ru: "story_27_b_ru.wav",
        story_27_c_en: "story_27_c_en.wav",
        story_27_c_ar: "story_27_c_ar.wav",
        story_27_c_ru: "story_27_c_ru.wav",
        story_30_en: "story_30_en.wav",
        story_30_ar: "story_30_ar.wav",
        story_30_ru: "story_30_ru.wav",
        story_31_en: "story_31_en.wav",
        story_31_ar: "story_31_ar.wav",
        story_31_ru: "story_31_ru.wav",
        story_32_en: "story_32_en.wav",
        story_32_ar: "story_32_ar.wav",
        story_32_ru: "story_32_ru.wav",
        story_32_a_en: "story_32_a_en.wav",
        story_32_a_ar: "story_32_a_ar.wav",
        story_32_a_ru: "story_32_a_ru.wav",
        story_32_b_en: "story_32_b_en.wav",
        story_32_b_ar: "story_32_b_ar.wav",
        story_32_b_ru: "story_32_b_ru.wav",
        story_32_c_en: "story_32_c_en.wav",
        story_32_c_ar: "story_32_c_ar.wav",
        story_32_c_ru: "story_32_c_ru.wav",
        story_33_en: "story_33_en.wav",
        story_33_ar: "story_33_ar.wav",
        story_33_ru: "story_33_ru.wav",
        story_34_en: "story_34_en.wav",
        story_34_ar: "story_34_ar.wav",
        story_34_ru: "story_34_ru.wav",
        story_35_en: "story_35_en.wav",
        story_35_ar: "story_35_ar.wav",
        story_35_ru: "story_35_ru.wav",
        story_36_en: "story_36_en.wav",
        story_36_ar: "story_36_ar.wav",
        story_36_ru: "story_36_ru.wav",
        story_37_en: "story_37_en.wav",
        story_37_ar: "story_37_ar.wav",
        story_37_ru: "story_37_ru.wav",
        story_38_en: "story_38_en.wav",
        story_38_ar: "story_38_ar.wav",
        story_38_ru: "story_38_ru.wav",
        story_39_en: "story_39_en.wav",
        story_39_ar: "story_39_ar.wav",
        story_39_ru: "story_39_ru.wav",
        story_40_en: "story_40_en.wav",
        story_40_ar: "story_40_ar.wav",
        story_40_ru: "story_40_ru.wav",
        story_41_en: "story_41_en.wav",
        story_41_ar: "story_41_ar.wav",
        story_41_ru: "story_41_ru.wav",
        story_42_en: "story_42_en.wav",
        story_42_ar: "story_42_ar.wav",
        story_42_ru: "story_42_ru.wav",
        story_43_en: "story_43_en.wav",
        story_43_ar: "story_43_ar.wav",
        story_43_ru: "story_43_ru.wav",
        story_44_en: "story_44_en.wav",
        story_44_ar: "story_44_ar.wav",
        story_44_ru: "story_44_ru.wav",
        story_46_en: "story_46_en.wav",
        story_46_ar: "story_46_ar.wav",
        story_46_ru: "story_46_ru.wav",
        story_47_en: "story_47_en.wav",
        story_47_ar: "story_47_ar.wav",
        story_47_ru: "story_47_ru.wav",
        story_48_en: "story_48_en.wav",
        story_48_ar: "story_48_ar.wav",
        story_48_ru: "story_48_ru.wav",
        story_49_en: "story_49_en.wav",
        story_49_ar: "story_49_ar.wav",
        story_49_ru: "story_49_ru.wav",
        story_50_en: "story_50_en.wav",
        story_50_ar: "story_50_ar.wav",
        story_50_ru: "story_50_ru.wav",
        story_51_en: "story_51_en.wav",
        story_51_ar: "story_51_ar.wav",
        story_51_ru: "story_51_ru.wav",
        story_52_en: "story_52_en.wav",
        story_52_ar: "story_52_ar.wav",
        story_52_ru: "story_52_ru.wav",
        story_53_en: "story_53_en.wav",
        story_53_ar: "story_53_ar.wav",
        story_53_ru: "story_53_ru.wav",
        story_54_en: "story_54_en.wav",
        story_54_ar: "story_54_ar.wav",
        story_54_ru: "story_54_ru.wav",
        story_55_en: "story_55_en.wav",
        story_55_ar: "story_55_ar.wav",
        story_55_ru: "story_55_ru.wav",
        story_56_en: "story_56_en.wav",
        story_56_ar: "story_56_ar.wav",
        story_56_ru: "story_56_ru.wav",
        story_57_en: "story_57_en.wav",
        story_57_ar: "story_57_ar.wav",
        story_57_ru: "story_57_ru.wav",
        story_58_en: "story_58_en.wav",
        story_58_ar: "story_58_ar.wav",
        story_58_ru: "story_58_ru.wav",
        story_59_en: "story_59_en.wav",
        story_59_ar: "story_59_ar.wav",
        story_59_ru: "story_59_ru.wav",
        story_60_a_en: "story_60_a_en.wav",
        story_60_a_ar: "story_60_a_ar.wav",
        story_60_a_ru: "story_60_a_ru.wav",
        story_60_b_en: "story_60_b_en.wav",
        story_60_b_ar: "story_60_b_ar.wav",
        story_60_b_ru: "story_60_b_ru.wav",
        story_60_c_en: "story_60_c_en.wav",
        story_60_c_ar: "story_60_c_ar.wav",
        story_60_c_ru: "story_60_c_ru.wav",
        story_60_d_en: "story_60_d_en.wav",
        story_60_d_ar: "story_60_d_ar.wav",
        story_60_d_ru: "story_60_d_ru.wav",
        story_60_e_en: "story_60_e_en.wav",
        story_60_e_ar: "story_60_e_ar.wav",
        story_60_e_ru: "story_60_e_ru.wav",
        story_60_f_en: "story_60_f_en.wav",
        story_60_f_ar: "story_60_f_ar.wav",
        story_60_f_ru: "story_60_f_ru.wav",
        story_60_g_en: "story_60_g_en.wav",
        story_60_g_ar: "story_60_g_ar.wav",
        story_60_g_ru: "story_60_g_ru.wav",
        story_60_h_en: "story_60_h_en.wav",
        story_60_h_ar: "story_60_h_ar.wav",
        story_60_h_ru: "story_60_h_ru.wav",
        story_60_i_en: "story_60_i_en.wav",
        story_60_i_ar: "story_60_i_ar.wav",
        story_60_i_ru: "story_60_i_ru.wav",
        story_60_j_en: "story_60_j_en.wav",
        story_60_j_ar: "story_60_j_ar.wav",
        story_60_j_ru: "story_60_j_ru.wav",
        story_61_a_en: "story_61_a_en.wav",
        story_61_a_ar: "story_61_a_ar.wav",
        story_61_a_ru: "story_61_a_ru.wav",
        story_61_b_en: "story_61_b_en.wav",
        story_61_b_ar: "story_61_b_ar.wav",
        story_61_b_ru: "story_61_b_ru.wav",
        story_61_d_en: "story_61_d_en.wav",
        story_61_d_ar: "story_61_d_ar.wav",
        story_61_d_ru: "story_61_d_ru.wav",
        story_61_e_en: "story_61_e_en.wav",
        story_61_e_ar: "story_61_e_ar.wav",
        story_61_e_ru: "story_61_e_ru.wav",
        story_61_f_en: "story_61_f_en.wav",
        story_61_f_ar: "story_61_f_ar.wav",
        story_61_f_ru: "story_61_f_ru.wav",
        story_61_g_en: "story_61_g_en.wav",
        story_61_g_ar: "story_61_g_ar.wav",
        story_61_g_ru: "story_61_g_ru.wav",
        story_61_h_en: "story_61_h_en.wav",
        story_61_h_ar: "story_61_h_ar.wav",
        story_61_h_ru: "story_61_h_ru.wav",
        story_61_i_en: "story_61_i_en.wav",
        story_61_i_ar: "story_61_i_ar.wav",
        story_61_i_ru: "story_61_i_ru.wav",
        story_61_j_en: "story_61_j_en.wav",
        story_61_j_ar: "story_61_j_ar.wav",
        story_61_j_ru: "story_61_j_ru.wav",
        story_61_c_en: "story_61_c_en.wav",
        story_61_c_ar: "story_61_c_ar.wav",
        story_61_c_ru: "story_61_c_ru.wav",
        story_62_a_en: "story_62_a_en.wav",
        story_62_a_ar: "story_62_a_ar.wav",
        story_62_a_ru: "story_62_a_ru.wav",
        story_62_b_en: "story_62_b_en.wav",
        story_62_b_ar: "story_62_b_ar.wav",
        story_62_b_ru: "story_62_b_ru.wav",
        story_62_c_en: "story_62_c_en.wav",
        story_62_c_ar: "story_62_c_ar.wav",
        story_62_c_ru: "story_62_c_ru.wav",
        story_62_d_en: "story_62_d_en.wav",
        story_62_d_ar: "story_62_d_ar.wav",
        story_62_d_ru: "story_62_d_ru.wav",
        story_62_e_en: "story_62_e_en.wav",
        story_62_e_ar: "story_62_e_ar.wav",
        story_62_e_ru: "story_62_e_ru.wav",
        story_63_a_en: "story_63_a_en.wav",
        story_63_a_ar: "story_63_a_ar.wav",
        story_63_a_ru: "story_63_a_ru.wav",
        story_63_b_en: "story_63_b_en.wav",
        story_63_b_ar: "story_63_b_ar.wav",
        story_63_b_ru: "story_63_b_ru.wav",
        story_63_c_en: "story_63_c_en.wav",
        story_63_c_ar: "story_63_c_ar.wav",
        story_63_c_ru: "story_63_c_ru.wav",
        story_63_d_en: "story_63_d_en.wav",
        story_63_d_ar: "story_63_d_ar.wav",
        story_63_d_ru: "story_63_d_ru.wav",
        story_63_e_en: "story_63_e_en.wav",
        story_63_e_ar: "story_63_e_ar.wav",
        story_63_e_ru: "story_63_e_ru.wav",
        story_63_f_en: "story_63_f_en.wav",
        story_63_f_ar: "story_63_f_ar.wav",
        story_63_f_ru: "story_63_f_ru.wav",
        story_63_g_en: "story_63_g_en.wav",
        story_63_g_ar: "story_63_g_ar.wav",
        story_63_g_ru: "story_63_g_ru.wav",
        story_63_h_en: "story_63_h_en.wav",
        story_63_h_ar: "story_63_h_ar.wav",
        story_63_h_ru: "story_63_h_ru.wav",
        story_63_i_en: "story_63_i_en.wav",
        story_63_i_ar: "story_63_i_ar.wav",
        story_63_i_ru: "story_63_i_ru.wav",
        story_63_j_en: "story_63_j_en.wav",
        story_63_j_ar: "story_63_j_ar.wav",
        story_63_j_ru: "story_63_j_ru.wav",
        story_64_a_en: "story_64_a_en.wav",
        story_64_a_ar: "story_64_a_ar.wav",
        story_64_a_ru: "story_64_a_ru.wav",
        story_64_b_en: "story_64_b_en.wav",
        story_64_b_ar: "story_64_b_ar.wav",
        story_64_b_ru: "story_64_b_ru.wav",
        story_65_a_en: "story_65_a_en.wav",
        story_65_a_ar: "story_65_a_ar.wav",
        story_65_a_ru: "story_65_a_ru.wav",
        story_65_b_en: "story_65_b_en.wav",
        story_65_b_ar: "story_65_b_ar.wav",
        story_65_b_ru: "story_65_b_ru.wav",
        story_65_c_en: "story_65_c_en.wav",
        story_65_c_ar: "story_65_c_ar.wav",
        story_65_c_ru: "story_65_c_ru.wav",
        story_65_d_en: "story_65_d_en.wav",
        story_65_d_ar: "story_65_d_ar.wav",
        story_65_d_ru: "story_65_d_ru.wav",
        story_65_e_en: "story_65_e_en.wav",
        story_65_e_ar: "story_65_e_ar.wav",
        story_65_e_ru: "story_65_e_ru.wav",
        story_65_f_en: "story_65_f_en.wav",
        story_65_f_ar: "story_65_f_ar.wav",
        story_65_f_ru: "story_65_f_ru.wav",
        story_65_g_en: "story_65_g_en.wav",
        story_65_g_ar: "story_65_g_ar.wav",
        story_65_g_ru: "story_65_g_ru.wav",
        story_65_h_en: "story_65_h_en.wav",
        story_65_h_ar: "story_65_h_ar.wav",
        story_65_h_ru: "story_65_h_ru.wav",
        story_65_i_en: "story_65_i_en.wav",
        story_65_i_ar: "story_65_i_ar.wav",
        story_65_i_ru: "story_65_i_ru.wav",
        story_65_j_en: "story_65_j_en.wav",
        story_65_j_ar: "story_65_j_ar.wav",
        story_65_j_ru: "story_65_j_ru.wav",
        story_66_en: "story_66_en.wav",
        story_66_ar: "story_66_ar.wav",
        story_66_ru: "story_66_ru.wav",
        story_67_en: "story_67_en.wav",
        story_67_ar: "story_67_ar.wav",
        story_67_ru: "story_67_ru.wav",
        story_68_en: "story_68_en.wav",
        story_68_ar: "story_68_ar.wav",
        story_68_ru: "story_68_ru.wav",
        story_69_en: "story_69_en.wav",
        story_69_ar: "story_69_ar.wav",
        story_69_ru: "story_69_ru.wav",
        story_70_en: "story_70_en.wav",
        story_70_ar: "story_70_ar.wav",
        story_70_ru: "story_70_ru.wav",

        # Подсказки
        hint_2_b_en: "hint_2_b_en.wav",
        hint_2_b_ar: "hint_2_b_ar.wav",
        hint_2_b_ru: "hint_2_b_ru.wav",
        hint_2_c_en: "hint_2_c_en.wav",
        hint_2_c_ar: "hint_2_c_ar.wav",
        hint_2_c_ru: "hint_2_c_ru.wav",
        hint_2_z_en: "hint_2_z_en.wav",
        hint_2_z_ar: "hint_2_z_ar.wav",
        hint_2_z_ru: "hint_2_z_ru.wav",
        hint_3_b_en: "hint_3_b_en.wav",
        hint_3_b_ar: "hint_3_b_ar.wav",
        hint_3_b_ru: "hint_3_b_ru.wav",
        hint_3_c_en: "hint_3_c_en.wav",
        hint_3_c_ar: "hint_3_c_ar.wav",
        hint_3_c_ru: "hint_3_c_ru.wav",
        hint_3_z_en: "hint_3_z_en.wav",
        hint_3_z_ar: "hint_3_z_ar.wav",
        hint_3_z_ru: "hint_3_z_ru.wav",
        hint_5_b_en: "hint_5_b_en.wav",
        hint_5_b_ar: "hint_5_b_ar.wav",
        hint_5_b_ru: "hint_5_b_ru.wav",
        hint_5_c_en: "hint_5_c_en.wav",
        hint_5_c_ar: "hint_5_c_ar.wav",
        hint_5_c_ru: "hint_5_c_ru.wav",
        hint_6_b_en: "hint_6_b_en.wav",
        hint_6_b_ar: "hint_6_b_ar.wav",
        hint_6_b_ru: "hint_6_b_ru.wav",
        hint_6_c_en: "hint_6_c_en.wav",
        hint_6_c_ar: "hint_6_c_ar.wav",
        hint_6_c_ru: "hint_6_c_ru.wav",
        hint_10_b_en: "hint_10_b_en.wav",
        hint_10_b_ar: "hint_10_b_ar.wav",
        hint_10_b_ru: "hint_10_b_ru.wav",
        hint_10_c_en: "hint_10_c_en.wav",
        hint_10_c_ar: "hint_10_c_ar.wav",
        hint_10_c_ru: "hint_10_c_ru.wav",
        hint_11_b_en: "hint_11_b_en.wav",
        hint_11_b_ar: "hint_11_b_ar.wav",
        hint_11_b_ru: "hint_11_b_ru.wav",
        hint_11_c_en: "hint_11_c_en.wav",
        hint_11_c_ar: "hint_11_c_ar.wav",
        hint_11_c_ru: "hint_11_c_ru.wav",
        hint_11_z_en: "hint_11_z_en.wav",
        hint_11_z_ar: "hint_11_z_ar.wav",
        hint_11_z_ru: "hint_11_z_ru.wav",
        hint_14_b_en: "hint_14_b_en.wav",
        hint_14_b_ar: "hint_14_b_ar.wav",
        hint_14_b_ru: "hint_14_b_ru.wav",
        hint_14_c_en: "hint_14_c_en.wav",
        hint_14_c_ar: "hint_14_c_ar.wav",
        hint_14_c_ru: "hint_14_c_ru.wav",
        hint_14_z_en: "hint_14_z_en.wav",
        hint_14_z_ar: "hint_14_z_ar.wav",
        hint_14_z_ru: "hint_14_z_ru.wav",
        hint_17_b_en: "hint_17_b_en.wav",
        hint_17_b_ar: "hint_17_b_ar.wav",
        hint_17_b_ru: "hint_17_b_ru.wav",
        hint_17_c_en: "hint_17_c_en.wav",
        hint_17_c_ar: "hint_17_c_ar.wav",
        hint_17_c_ru: "hint_17_c_ru.wav",
        hint_17_z_en: "hint_17_z_en.wav",
        hint_17_z_ar: "hint_17_z_ar.wav",
        hint_17_z_ru: "hint_17_z_ru.wav",
        hint_19_b_en: "hint_19_b_en.wav",
        hint_19_b_ar: "hint_19_b_ar.wav",
        hint_19_b_ru: "hint_19_b_ru.wav",
        hint_19_c_en: "hint_19_c_en.wav",
        hint_19_c_ar: "hint_19_c_ar.wav",
        hint_19_c_ru: "hint_19_c_ru.wav",
        hint_19_z_en: "hint_19_z_en.wav",
        hint_19_z_ar: "hint_19_z_ar.wav",
        hint_19_z_ru: "hint_19_z_ru.wav",
        hint_23_b_en: "hint_23_b_en.wav",
        hint_23_b_ar: "hint_23_b_ar.wav",
        hint_23_b_ru: "hint_23_b_ru.wav",
        hint_23_c_en: "hint_23_c_en.wav",
        hint_23_c_ar: "hint_23_c_ar.wav",
        hint_23_c_ru: "hint_23_c_ru.wav",
        hint_23_z_en: "hint_23_z_en.wav",
        hint_23_z_ar: "hint_23_z_ar.wav",
        hint_23_z_ru: "hint_23_z_ru.wav",
        hint_26_b_en: "hint_26_b_en.wav",
        hint_26_b_ar: "hint_26_b_ar.wav",
        hint_26_b_ru: "hint_26_b_ru.wav",
        hint_26_c_en: "hint_26_c_en.wav",
        hint_26_c_ar: "hint_26_c_ar.wav",
        hint_26_c_ru: "hint_26_c_ru.wav",
        hint_26_z_en: "hint_26_z_en.wav",
        hint_26_z_ar: "hint_26_z_ar.wav",
        hint_26_z_ru: "hint_26_z_ru.wav",
        hint_32_b_en: "hint_32_b_en.wav",
        hint_32_b_ar: "hint_32_b_ar.wav",
        hint_32_b_ru: "hint_32_b_ru.wav",
        hint_32_c_en: "hint_32_c_en.wav",
        hint_32_c_ar: "hint_32_c_ar.wav",
        hint_32_c_ru: "hint_32_c_ru.wav",
        hint_32_d_en: "hint_32_d_en.wav",
        hint_32_d_ar: "hint_32_d_ar.wav",
        hint_32_d_ru: "hint_32_d_ru.wav",
        hint_32_e_en: "hint_32_e_en.wav",
        hint_32_e_ar: "hint_32_e_ar.wav",
        hint_32_e_ru: "hint_32_e_ru.wav",
        hint_32_z_en: "hint_32_z_en.wav",
        hint_32_z_ar: "hint_32_z_ar.wav",
        hint_32_z_ru: "hint_32_z_ru.wav",
        hint_37_b_en: "hint_37_b_en.wav",
        hint_37_b_ar: "hint_37_b_ar.wav",
        hint_37_b_ru: "hint_37_b_ru.wav",
        hint_37_c_en: "hint_37_c_en.wav",
        hint_37_c_ar: "hint_37_c_ar.wav",
        hint_37_c_ru: "hint_37_c_ru.wav",
        hint_38_b_en: "hint_38_b_en.wav",
        hint_38_b_ar: "hint_38_b_ar.wav",
        hint_38_b_ru: "hint_38_b_ru.wav",
        hint_44_b_en: "hint_44_b_en.wav",
        hint_44_b_ar: "hint_44_b_ar.wav",
        hint_44_b_ru: "hint_44_b_ru.wav",
        hint_44_c_en: "hint_44_c_en.wav",
        hint_44_c_ar: "hint_44_c_ar.wav",
        hint_44_c_ru: "hint_44_c_ru.wav",
        hint_49_b_en: "hint_49_b_en.wav",
        hint_49_b_ar: "hint_49_b_ar.wav",
        hint_49_b_ru: "hint_49_b_ru.wav",
        hint_49_c_en: "hint_49_c_en.wav",
        hint_49_c_ar: "hint_49_c_ar.wav",
        hint_49_c_ru: "hint_49_c_ru.wav",
        hint_50_b_en: "hint_50_b_en.wav",
        hint_50_b_ar: "hint_50_b_ar.wav",
        hint_50_b_ru: "hint_50_b_ru.wav",
        hint_50_c_en: "hint_50_c_en.wav",
        hint_50_c_ar: "hint_50_c_ar.wav",
        hint_50_c_ru: "hint_50_c_ru.wav",
        hint_51_b_en: "hint_51_b_en.wav",
        hint_51_b_ar: "hint_51_b_ar.wav",
        hint_51_b_ru: "hint_51_b_ru.wav",
        hint_51_c_en: "hint_51_c_en.wav",
        hint_51_c_ar: "hint_51_c_ar.wav",
        hint_51_c_ru: "hint_51_c_ru.wav",
        hint_56_b_en: "hint_56_b_en.wav",
        hint_56_b_ar: "hint_56_b_ar.wav",
        hint_56_b_ru: "hint_56_b_ru.wav",
    }
except NameError:
    # Это может случиться, если скрипт запущен не полностью,
    # но в рабочей среде все переменные должны быть определены.
    _SOUND_NAME_MAP = {}
    logger.warning("NameError during _SOUND_NAME_MAP creation. Some sounds may not be defined yet.")

player_goal_sounds = [goal2, goal3, goal4, goal5, goal6, goal7]

# Списки историй по языкам (начиная с 'b')
player_goal_stories_ru = [
    story_61_b_ru, story_61_c_ru, story_61_d_ru, story_61_e_ru, story_61_f_ru, 
    story_61_g_ru, story_61_h_ru, story_61_i_ru, story_61_j_ru
]
player_goal_stories_en = [
    story_61_b_en, story_61_c_en, story_61_d_en, story_61_e_en, story_61_f_en, 
    story_61_g_en, story_61_h_en, story_61_i_en, story_61_j_en
]
player_goal_stories_ar = [
    story_61_b_ar, story_61_c_ar, story_61_d_ar, story_61_e_ar, story_61_f_ar, 
    story_61_g_ar, story_61_h_ar, story_61_i_ar, story_61_j_ru
]

enemy_goal_sounds = [enemy_goal1, enemy_goal2, enemy_goal3, enemy_goal4]

# Списки для историй бота
enemy_goal_stories_ru = [
    story_65_a_ru, story_65_b_ru, story_65_c_ru, story_65_d_ru, story_65_e_ru, 
    story_65_f_ru, story_65_g_ru, story_65_h_ru, story_65_i_ru, story_65_j_ru
]
enemy_goal_stories_en = [
    story_65_a_en, story_65_b_en, story_65_c_en, story_65_d_en, story_65_e_en, 
    story_65_f_en, story_65_g_en, story_65_h_en, story_65_i_en, story_65_j_en
]
enemy_goal_stories_ar = [
    story_65_a_ar, story_65_b_ar, story_65_c_ar, story_65_d_ar, story_65_e_ar, 
    story_65_f_ar, story_65_g_ar, story_65_h_ar, story_65_i_ar, story_65_j_ar
]

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
    ARDUINO_PORT = '/dev/ttyUSB0' 
    ser = serial.Serial(ARDUINO_PORT, 115200, timeout=1)
    logger.info(f"Successfully connected to Arduino on port {ARDUINO_PORT}")
except serial.SerialException as e:
    logger.critical(f"Could not open serial port {ARDUINO_PORT}. Error: {e}")
    logger.critical("HINT: Check connection and port name. Make sure you have permissions (sudo usermod -a -G dialout pi).")
    exit() # Завершаем работу, если не удалось подключиться

#---конфиг сервера
Payload.max_decode_packets = 200
#async_mode = None  
app = Flask('feedback')
app.config['SECRET_KEY'] = 'secret!'
app.static_folder = 'static'
socketio = SocketIO(app, cors_allowed_origins="*", allow_unsafe_werkzeug=True)

#основной декоратор срабатывает при запросе браузера страницы отправляет наш файл с интерфейсом и все необходимые дополняющие css js icon
#html храниться в папке templates все остальное в папке static
@app.route('/')
def index():
     return send_file('templates/Front.html')

#декоратор работы socket отвечает за настройки wifi
@socketio.on('WLAN')
def WLAN(ssid):
     s = ssid # здесь будет записана строка на которую хотим заменить значение мы задаем ее на клиенте
     pos = 2 #строка которую поменяем
     #открытие файла для чтения в этом файле хранятся настройки для работы с wifi
     f = open('/etc/hostapd/hostapd.conf', 'r')
     L = f.readlines()
     if (pos >= 0) and (pos < len(L)):
         if (pos==len(L)-1):
             L[pos] = s
         else:
             L[pos] = s + '\n'
     f.close()
     # обязательно после чтения закрыть файл освободить ресурсы
     f = open('/etc/hostapd/hostapd.conf', 'w')
     #запись в файл нового значения
     for line in L:
         f.write(line)
     f.close()  
     #перезагружаемся после всего
     os.system("sudo reboot")


@socketio.on('connect')
def handle_connect():
    global current_client_sid
    global socklist
    # Отправка предупреждения о некорректном выключении ---
    global show_improper_shutdown_warning
    if show_improper_shutdown_warning:
        # Отправляем команду на показ модального окна ТОЛЬКО этому клиенту
        socketio.emit('show_shutdown_warning', to=request.sid)
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
    
    # Отправка всей истории новому клиенту ---
    # Отправляем один раз при подключении, чтобы 'синхронизировать' клиента
    # Мы отправляем только этому 'sid', чтобы не спамить других
    logger.debug(f"New client connected ({request.sid}). Sending full state history ({len(socklist)} items).")
    for i in socklist:
        socketio.emit('level', i, to=request.sid)

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
          ph = round(phone/100,2)
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
          socketio.emit('volume', ph, to=None, include_self=False)
     
#все тоже самое для других звуковых каналов             
@socketio.on('Voice')
def Voice(voice):
     global voiceLevel
     voi = round(voice/100,2)
     f1 = open('3.txt','w')
     f1.write(str(voi))
     f1.close()   
     voiceLevel = float(voi)
     channel3.set_volume(float(voi),float(voi))  
     socketio.emit('volume2', voi, to=None, include_self=False)    
@socketio.on('Effects')
def Effects(effects):
     global effectLevel
     eff = round(effects/100,2)
     f1 = open('2.txt','w')
     f1.write(str(eff))
     f1.close()   
     effectLevel = float(eff)
     
     # --- Применяем громкость ко ВСЕМ каналам эффектов ---
     for ch in effects_pool:
         ch.set_volume(float(eff), float(eff))
     # ---------------------------------------------------------------
     
     socketio.emit('volume1', eff, to=None, include_self=False)

@socketio.on('WolfUp')
def WolfSound(wolfsound):
     global wolfLevel
     f1 = open('5.txt','w')
     f1.write(str(wolfsound))
     f1.close()   
     send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{wolfsound}", debounce=True)
     wolfLevel = int(wolfsound)
     socketio.emit('wolf', wolfLevel, to=None, include_self=False)

@socketio.on('WolfDown')
def WolfSound(wolfsound):
     global wolfLevel
     f1 = open('5.txt','w')
     f1.write(str(wolfsound))
     f1.close()   
     send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{wolfsound}", debounce=True)
     wolfLevel = int(wolfsound)
     socketio.emit('wolf', wolfLevel, to=None, include_self=False) 

@socketio.on('PlatformUp')
def TrainSound(platformsound):
     global trainLevel
     f1 = open('6.txt','w')
     f1.write(str(platformsound))
     f1.close()   
     send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{platformsound}", debounce=True)
     trainLevel = int(platformsound)
     socketio.emit('platform', trainLevel, to=None, include_self=False)

@socketio.on('PlatformDown')
def TrainSound(platformsound):
     global trainLevel
     f1 = open('6.txt','w')
     f1.write(str(platformsound))
     f1.close()   
     send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{platformsound}", debounce=True)
     trainLevel = int(platformsound)
     socketio.emit('platform', trainLevel, to=None, include_self=False)

@socketio.on('SuitcasesUp')
def SuitcasesSound(suitcasessound):
     global suitcaseLevel
     f1 = open('7.txt','w')
     f1.write(str(suitcasessound))
     f1.close()   
     send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{suitcasessound}", debounce=True)
     suitcaseLevel = int(suitcasessound)
     socketio.emit('suitcases', suitcaseLevel, to=None, include_self=False)

@socketio.on('SuitcasesDown')
def SuitcasesSound(suitcasessound):
     global suitcaseLevel
     f1 = open('7.txt','w')
     f1.write(str(suitcasessound))
     f1.close()   
     send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{suitcasessound}", debounce=True)
     suitcaseLevel = int(suitcasessound)
     socketio.emit('suitcases', suitcaseLevel, to=None, include_self=False)     

@socketio.on('SafeUp')
def SafeSound(safesound):
     global safeLevel
     f1 = open('8.txt','w')
     f1.write(str(safesound))
     f1.close()   
     send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{safesound}", debounce=True)
     safeLevel = int(safesound)
     socketio.emit('safe', safeLevel, to=None, include_self=False)

@socketio.on('SafeDown')
def SafeSound(safesound):
     global safeLevel
     f1 = open('8.txt','w')
     f1.write(str(safesound))
     f1.close()   
     send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{safesound}", debounce=True)
     safeLevel = int(safesound)
     socketio.emit('safe', safeLevel, to=None, include_self=False)  

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
       send_esp32_command(ESP32_API_WOLF_URL, "language_1")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_1")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_1")
       send_esp32_command(ESP32_API_SAFE_URL, "language_1")
       #------если какая то история или подсказка воспроизводится в методе langRu озвучиваем ту же дорожку только на русском
       #if channel3.get_busy()==True:
         # langRu()
      #-------иначе просто воспроизведем ее еще раз    
       #else:
          #langRuRepeat(name)

     if check == 'english':
       f4 = open('4.txt','w')
       f4.write('2')
       f4.close() 
       language = 2 
       send_esp32_command(ESP32_API_WOLF_URL, "language_2")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_2")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_2")
       send_esp32_command(ESP32_API_SAFE_URL, "language_2")
       #if channel3.get_busy()==True:
          #langEn()
       #else:
          #langEnRepeat(name)
          
     if check == 'arabian':
       f4 = open('4.txt','w')
       f4.write('3')
       f4.close() 
       language = 3 
       send_esp32_command(ESP32_API_WOLF_URL, "language_3")
       send_esp32_command(ESP32_API_TRAIN_URL, "language_3")
       send_esp32_command(ESP32_API_SUITCASE_URL, "language_3")
       send_esp32_command(ESP32_API_SAFE_URL, "language_3")
       #if channel3.get_busy()==True:
          #langFr()
       #else:
          #langFrRepeat(name)           

    #----нажали выключить
     if check == 'off':
             # Добавлено удаление файла-метки и остановка музыки ---
             
             # 1. Логируем событие
             logger.debug("SHUTDOWN: Получена команда 'off' от пульта...")
             
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
          #------нажали на пропуск игры с тумблером
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
        #----нажали пропустить игру с галетниками     
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
             #-----активируем блок с флагами
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
             
             # 3. Воспроизводим истории
             if(language==1):
                 play_story(story_5_ru)  
             if(language==2):
                 play_story(story_5_en)
             if(language==3):
                 play_story(story_5_ar)

             while channel3.get_busy()==True and go == 1: 
                 eventlet.sleep(0.1)
             
             if(language==1):
                 play_story(story_6_ru)  
             if(language==2):
                 play_story(story_6_en)
             if(language==3):
                 play_story(story_6_ar)

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
             #-----активируем блок с флагами
             socketio.emit('level', 'active_dog',to=None)
             socklist.append('active_dog')
        if check == 'dog':
             #-----отправка клиенту 
             socketio.emit('level', 'dog',to=None)
             #-----добавить в историю
             socklist.append('dog')
             #----отправить на мегу
             serial_write_queue.put('dog')
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
             # 3. Воспроизводим историю
             if(language==1):
                 play_story(story_17_ru) #
             if(language==2):
                 play_story(story_17_en) #
             if(language==3):
                 play_story(story_17_ar) #

             # 4. Активируем следующий этап
             socketio.emit('level', 'active_open_potions_stash',to=None) #
             socklist.append('active_open_potions_stash')

             #-----активируем блок с флагами
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
             #-----отправка клиенту 
             socketio.emit('level', 'owl',to=None)
             #-----добавить в историю
             socklist.append('owl')
             #send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
             #----отправить на мегу
             serial_write_queue.put('owl_door')
             name = "story_2"     
             eventlet.sleep(1) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_owls',to=None)
             socklist.append('active_owls')
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
             send_esp32_command(ESP32_API_TRAIN_URL, "owl_finish")
             if(language==1):
                 play_story(story_14_b_ru) #
             if(language==2):
                 play_story(story_14_b_en) #
             if(language==3):
                 play_story(story_14_b_ar) #
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
             #-----активируем блок с флагами
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
             #-----активируем блок с флагами
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
             #-----активируем блок с флагами
             #socketio.emit('level', 'active_first_clock_2',to=None)
             #socklist.append('active_first_clock_2')
        if check == 'first_clock_2':
             #-----отправка клиенту 
             socketio.emit('level', 'first_clock_2',to=None)
             #-----добавить в историю
             socklist.append('first_clock_2')
             #----отправить на мегу
             serial_write_queue.put('first_clock_2')
             name = "story_2"     
             eventlet.sleep(5) 
             #-----активируем блок с флагами
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
             #-----активируем блок с флагами
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
        if check == 'cup':
             #-----отправка клиенту 
             socketio.emit('level', 'cup',to=None)
             #-----добавить в историю
             socklist.append('cup')
             #----отправить на мегу
             serial_write_queue.put('cup')
             name = "story_2"
             eventlet.sleep(5) 
             #-----активируем блок с флагами
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
             #-----активируем блок с флагами
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
             socklist.append('memory_room_end')
             #----отправить на мегу
             serial_write_queue.put('memory_room_end')
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
             #-----добавить в историю
             socklist.append('basket')
             socketio.emit('level', 'win_player',to=None)
             #-----добавить в историю
             socklist.append('win_player')
             #----отправить на мегу
             serial_write_queue.put('basket')
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
        # Добавляем обработку простых логов от ESP32
        if 'log' in data:
             # Это сообщение попадет и в файл, и в консоль (если уровень INFO)
             logging.debug(f"RECEIVED [ESP32 Log]: {data['log']}")
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

        if 'suitcase' in data and data['suitcase'] == 'end':
          logger.debug("'suitcase: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "case_finish")
          serial_write_queue.put('suitcase_end')
          socketio.emit('level', 'suitcase',to=None)
          socklist.append('suitcase')
          send_esp32_command(ESP32_API_SUITCASE_URL, "confirm_suitcase_end")
        if 'safe' in data and data['safe'] == 'end':
          logger.debug("'safe: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "safe_finish")
          serial_write_queue.put('safe_end')
          socketio.emit('level', 'animals',to=None)
          socklist.append('animals')
          send_esp32_command(ESP32_API_SAFE_URL, "confirm_safe_end")
        if 'wolf' in data and data['wolf'] == 'end':
          logger.debug("'wolf: end' logic triggered.")
          send_esp32_command(ESP32_API_TRAIN_URL, "wolf_finish")
          serial_write_queue.put('wolf_end')
          socketio.emit('level', 'wolf',to=None)
          socklist.append('wolf')
          send_esp32_command(ESP32_API_WOLF_URL, "confirm_wolf_end")
         
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
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          elif mapClickHints == 1:
               mapClickHints = 0
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)

        if 'map' in data and data['map'] == 'fish':
          logger.debug("'map: fish' logic triggered.")
          serial_write_queue.put('fish')
          eventlet.sleep(1.0)
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          elif mapClickHints == 1:
               mapClickHints = 0
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)

        if 'map' in data and data['map'] == 'key':
          logger.debug("'map: key' logic triggered.")
          serial_write_queue.put('key')
          eventlet.sleep(1.0)
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          elif mapClickHints == 1:
               mapClickHints = 0
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)

        if 'map' in data and data['map'] == 'train':
          logger.debug("'map: train' logic triggered.")
          serial_write_queue.put('train')
          eventlet.sleep(1.0) 
          play_effect(map_click)
          #while effects_are_busy() and go == 1: 
          #     eventlet.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints = 1
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          elif mapClickHints == 1:
               mapClickHints = 0
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)

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
               if(language==1):
                   play_story(story_12_c_ru)  
               if(language==2):
                   play_story(story_12_c_en)
               if(language==3):
                   play_story(story_12_c_ar)
          elif mapClickOut == 1:
               mapClickOut = 0
               if(language==1):
                   play_story(story_12_d_ru)  
               if(language==2):
                   play_story(story_12_d_en)
               if(language==3):
                   play_story(story_12_d_ar)

        if 'ghost' in data and data['ghost'] == 'end':
            logger.debug("'ghost: end' logic triggered.")
            serial_write_queue.put('ghost')
                 

        # Можно отправить данные на ESP32
        try:
            #response = requests.post(ESP32_API_URL, json=data, timeout=3)
            return jsonify({"status": "success", "esp32_response": response.json()})
        except Exception as e:
            return jsonify({"status": "error", "message": str(e)}), 500
            
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
          #channel2.play(sound2, loops = -1) 
        #
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
             send_esp32_command(ESP32_API_WOLF_URL, f"set_level_{wolfLevel}")
             send_esp32_command(ESP32_API_TRAIN_URL, f"set_level_{trainLevel}")
             send_esp32_command(ESP32_API_SUITCASE_URL, f"set_level_{suitcaseLevel}")
             send_esp32_command(ESP32_API_SAFE_URL, f"set_level_{safeLevel}")
             logger.info("Start command received")
             #----очищаем историю 
             socklist.clear() 
             serial_write_queue.put('start')
             go=1
             starts = 1
             logger.debug("State changed: Game started from 'ready' state.")
        
     #----нажали на рестарт   
     if res =='restart':
         logger.debug("State changed: Game restarted.")
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
         devices.clear()
         if is_processing_ready:
             print("INFO: 'Ready' command is already being processed.")
             return # Выходим, если команда уже в обработке

         is_processing_ready = True
         socketio.emit('level', 'ready_processing') # Команда для UI, чтобы заблокировать кнопку
          #-----перейдет только если был в рестарте или просто запущен
         logger.info("Ready command received")
         if go == 2 or go == 0:
               # --- Очищаем список ошибок ПЕРЕД началом проверки ---
               # global devices
               # devices.clear()
               
               # --- Отправляем команду на очистку UI перед проверкой ---
               socketio.emit('clear_check_flags', to=None)
               
               # 1. Проверяем ESP (эта функция очищает 'devices' в начале)
               test_esp32()
               
               # 2. ОТПРАВЛЯЕМ 'ready' на Arduino.
               # Arduino НЕМЕДЛЕННО сбросит свои флаги и начнет 
               # проверку состояния (start_door, safe_open и т.д.)
               serial_write_queue.put('ready')

               # 3. Даем 4 секунд, используя НЕБЛОКИРУЮЩИЙ eventlet.sleep().
               #    Это позволяет потоку serial() работать и ПОЛУЧАТЬ ошибки от Arduino.
               eventlet.sleep(4.0) # <-- БЫЛО eventlet.sleep(1.0)
               
               # 4. Теперь проверяем 'devices'. К этому моменту он
               #    содержит И ошибки ESP, И ошибки Arduino.
               if len(devices) == 0:
                    # --- ВСЕ В ПОРЯДКЕ ---
                    logger.info("Ready: OK")
                    socklist.clear()
                    # Принудительно закрываем модальное окно с ошибкой (если оно было)
                    socketio.emit('level', 'modal_end', to=None)
                    socketio.emit('level', 'ready',to=None)
                    
                    # Отправляем команды ESP (теперь, когда уверены, что все хорошо)
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
                    play_background_music("fon1.mp3", loops=-1)
                    logger.debug("State changed: System is ready for game start.")
               else:
                    # --- НАЙДЕНЫ ОШИБКИ (Либо ESP, либо Arduino) ---
                    logger.warning(f"Ready: FAILED. Devices: {devices}")
                    socklist.clear() # Очищаем socklist от старых данных
                    
                    # --- Просто отправляем клиенту строку с ошибками ---
                    final_string = ', '.join(str(device) for device in set(devices))
                    socketio.emit('devices', final_string, to=None)
                    
         # --- Снимаем блокировку в конце ---
         socketio.emit('level', 'ready_finished') # Команда для UI, чтобы разблокировать кнопку
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

def play_story(audio_file, loops=0, volume_file='3.txt'):
    # --- Улучшено логирование звуков ---
    try:
        # Пытаемся найти имя файла в карте, чтобы лог был читаемым.
        audio_name = _SOUND_NAME_MAP.get(audio_file, "Неизвестный аудиофайл истории")
        logging.info(f"PLAY [История]: {audio_name}")
    except Exception as e:
        # На случай, если что-то пойдет не так с логированием.
        logging.error(f"Ошибка логирования имени истории: {e}")
    # Воспроизводит историю/подсказку, АВТОМАТИЧЕСКИ приглушая фоновую музыку.
    global story_fade_active, phoneLevel, go
    
    # СТРАХОВКА: Если канал историй молчит, значит фейд точно не активен.
    # Сбрасываем флаг, чтобы гарантировать приглушение для новой истории.
    if not channel3.get_busy():
        story_fade_active = False
    
    # 1. Приглушаем фоновую музыку (только если она еще не приглушена)
    if not story_fade_active and go == 1:
        story_fade_active = True
        serial_write_queue.put('soundon')
        
        # Получаем текущий phoneLevel
        try:
            with open('1.txt', 'r') as f:
                phoneLevel = float(f.read(4))
        except Exception as e:
            logger.error(f"Ошибка чтения файла громкости 1.txt (в play_story): {e}")
            pass # Используем старое значение

        temp_vol = phoneLevel 
        target_vol = 0.1 # Целевая громкость 10%
        
        while temp_vol > target_vol and go == 1:
            temp_vol = round(temp_vol, 2) - 0.01
            if temp_vol < target_vol: temp_vol = target_vol # Гарантия, что не уйдем ниже
            pygame.mixer.music.set_volume(round(temp_vol, 2))
            eventlet.sleep(0.05) # Плавное затухание
        # Даем микшеру "вздохнуть" после цикла изменения громкости,
        # чтобы предотвратить "заикание" при немедленном
        # запуске нового звука.
        eventlet.sleep(0.1)
    
    # 2. Воспроизводим саму историю
    if audio_file:
        channel3.play(audio_file, loops=loops)
        
        # Устанавливаем громкость истории
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
            channel3.set_volume(volume, volume)
            
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

    # 3. Воспроизводим
    selected_channel.play(audio_file, loops=loops)
    
    # 4. Устанавливаем громкость (нужно прочитать файл и применить к выбранному каналу)
    try:
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
            selected_channel.set_volume(volume, volume)
    except Exception as e:
        logger.error(f"Ошибка установки громкости эффекта: {e}")
        
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
        # Загружаем и воспроизводим музыку
        pygame.mixer.music.load(music_file)
        pygame.mixer.music.play(loops)
        
        # Устанавливаем громкость из файла
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
            pygame.mixer.music.set_volume(volume)
            
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

#здесь уже обрабатываем все сообщения приходящие из меги и отображаем на пульте        
def serial():
     global flag
     global mus
     global mansard_galets, last_mansard_count
     global socklist 
     global devices

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
     global last_owl_flew_time
     global last_boy_in_time
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
     level_18_intro_played = False
     #алгоритм на понижение громкости работает хитро сорян за имена переменных лучше его не трогай намучаешься капец сам делал долго связано в округлением данных float и урпавлением во время эффекта
     #если нужно быстрее или медленне измени значения sleep
     while True:
          check_story_and_fade_up() # Проверяем, не закончила ли история играть
          process_serial_queue()
          # Добавляем блок для отправки сообщений из очереди
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
                    if(language==1):
                        play_story(story_11_ru)  
                    if(language==2):
                        play_story(story_11_en)
                    if(language==3):
                        play_story(story_11_ar)
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

          # аналог serial.available() rsstrip игнорирует всякие переходы на другую строку и перевод каретки     
          if ser.in_waiting > 0:
               line = ser.readline().decode('utf-8', errors='ignore').rstrip()
               flag = line
               # --------------------------------------------------------
               flag_on_commands = ["workshop_flag1_on", "dog_flag3_on", "owls_flag4_on"]
               flag_off_commands = ["workshop_flag1_off", "dog_flag3_off", "owls_flag4_off"]

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
               eventlet.sleep(0.1)
               # ИЗМЕНЕНО: Улучшенное логирование входящих сообщений от Arduino
               description = EVENT_DESCRIPTIONS.get(flag, '-')
               tag = get_device_tag(flag)
               logging.info(f'RECEIVED {tag}: {description} (RAW: {flag})')
               # Логирование смены уровня ---
               if flag.startswith("level_"):
                   try:
                       # Извлекаем номер уровня (например, "level_5" -> "5")
                       level_number = flag.split('_')[1]
                       # Логируем на уровне DEBUG (только в файл)
                       logger.debug(f"ARDUINO LEVEL: Main board transitioned to level {level_number}")
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
                     #----добавим в истори старт
                     socklist.append('50')
                     socklist.append('start_game')
                     socketio.emit('level', 'start_game',to=None) 
                     starts = 1
                     send_esp32_command(ESP32_API_WOLF_URL, "start")
                     send_esp32_command(ESP32_API_TRAIN_URL, "start")
                     send_esp32_command(ESP32_API_TRAIN_URL, "set_level_15")
                     send_esp32_command(ESP32_API_SUITCASE_URL, "start")
                     send_esp32_command(ESP32_API_SAFE_URL, "start")
                     #----отправим на клиента старт
                     socketio.emit('level', 'start_game',to=None)
                     #-----играем фон
                     play_background_music("fon2.mp3", loops=-1)
                     eventlet.sleep(8.0)
                     #-----играем историю
                     if(language==1):
                         play_story(story_1_ru)  
                     if(language==2):
                         play_story(story_1_en)
                     if(language==3):
                         play_story(story_1_ar)
                     #-----меняем значение переменной
                     name = "start_story_1"    
                     
                     while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                     check_story_and_fade_up()
                     send_esp32_command(ESP32_API_TRAIN_URL, "train_light_off")
                     if(language==1):
                         play_story(story_2_a_ru)  
                     if(language==2):
                         play_story(story_2_a_en)
                     if(language==3):
                         play_story(story_2_a_ar)         
               #---режим для событий в ресте показывает что нужно вернуть на свои места
               if starts == 2 or starts == 0 or starts == 3:
                     

                     if "flag1_on" in socklist or "flag2_on" in socklist or "flag3_on" in socklist or "flag4_on" in socklist:
                        print('Check Flags Add') 
                        if 'Check Flags' not in devices:
                            devices.append('Check Flags')
                     else:
                        logger.debug('Check Flags remove')
                        if 'Check Flags' in devices:
                                   devices.remove('Check Flags')  
                                   logger.debug('Check Flags remove')       
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
                         # 1. Воспроизводим звук
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
                          #----играем историю    
                          if(language==1):
                              play_story(story_2_b_ru)  
                          if(language==2):
                              play_story(story_2_b_en)
                          if(language==3):
                              play_story(story_2_b_ar)
                     if flag == "dragon_crystal_repeat":
                          #----играем историю    
                          if(language==1):
                              play_story(story_2_r_ru)  
                          if(language==2):
                              play_story(story_2_r_en)
                          if(language==3):
                              play_story(story_2_r_ar)         
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
                          #----играем историю    
                          if(language==1):
                              play_story(story_3_ru)  
                          if(language==2):
                              play_story(story_3_en)
                          if(language==3):
                              play_story(story_3_ar)
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
                          #----играем историю    
                          if(language==1):
                              play_story(story_3_a_ru)  
                          if(language==2):
                              play_story(story_3_a_en)
                          if(language==3):
                              play_story(story_3_a_ar)              
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
                          #----играем историю    
                          if(language==1):
                              play_story(story_3_b_ru)  
                          if(language==2):
                              play_story(story_3_b_en)
                          if(language==3):
                              play_story(story_3_b_ar)  

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

                          if(language==1):
                              play_story(story_3_c_ru)  
                          if(language==2):
                              play_story(story_3_c_en)
                          if(language==3):
                              play_story(story_3_c_ar)

                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)         

                          serial_write_queue.put('student_hide')
                          # --- ЗАЩИТА: Проверяем, не активен ли уже 2 этап ---
                          if not train_stage_2_active:
                              send_esp32_command(ESP32_API_TRAIN_URL, "stage_1")
                          else:
                              logger.info("SKIP stage_1 because stage_2 is already active.")
                          eventlet.sleep(1.0)

                          send_esp32_command(ESP32_API_TRAIN_URL, "train_uf_light_off")
                          send_esp32_command(ESP32_API_TRAIN_URL, "train_light_on")
                          play_background_music("fon5.mp3", loops=-1)
                          if(language==1):
                              play_story(story_4_ru)  
                          if(language==2):
                              play_story(story_4_en)
                          if(language==3):
                              play_story(story_4_ar)
                          #-----изменяем переменную
                     if flag == "kay_repeat":
                          if(language==1):
                              play_story(story_3_r_ru)  
                          if(language==2):
                              play_story(story_3_r_en)
                          if(language==3):
                              play_story(story_3_r_ar)
                     # --- Логика для Прогресс-бара Mansard Game (5 галетников) ---
                          
                     # 1. Определяем сигналы (согласно MAIN_BOARD_V5_COM5.ino)
                     galet_signals = {
                         "galet1": "g1", "galet2": "g2", "galet3": "g3", "galet4": "g4", "galet5": "g5"
                     }
                     galet_off_signals = {
                         "galet1_off": "g1", "galet2_off": "g2", "galet3_off": "g3", "galet4_off": "g4", "galet5_off": "g5"
                     }
                  
                     changed = False
                  
                     # 2. Проверяем, пришел ли сигнал ВКЛ
                     if flag in galet_signals:
                         # Добавляем, только если его еще не было
                         if galet_signals[flag] not in mansard_galets:
                             mansard_galets.add(galet_signals[flag])
                             changed = True
                  
                     # 3. Проверяем, пришел ли сигнал ВЫКЛ
                     if flag in galet_off_signals:
                         # Удаляем, только если он там был
                         if galet_off_signals[flag] in mansard_galets:
                             mansard_galets.discard(galet_off_signals[flag])
                             changed = True

                     # 4. Если состояние изменилось, обновляем UI
                     if changed:
                         current_count = len(mansard_galets)
                      
                         # Проверяем, отличается ли новое значение от старого (чтобы не спамить)
                         if current_count != last_mansard_count:
                             percent = current_count * 20
                             event_name = f"mansard_progress_{percent}" # например, "mansard_progress_40"
                          
                             # 4.1. Отправляем в SocketIO
                             socketio.emit('level', event_name, to=None)
                          
                             # 4.2. Обновляем историю (socklist)
                             # Удаляем старое значение прогресса из истории
                             old_percent = last_mansard_count * 20
                             old_event_name = f"mansard_progress_{old_percent}"
                          
                             # Используем цикл while, чтобы удалить ВСЕ старые вхождения
                             while old_event_name in socklist:
                                 try:
                                     socklist.remove(old_event_name)
                                 except ValueError:
                                     pass # На случай, если что-то пошло не так
                                  
                             socklist.append(event_name)
                          
                             # 4.3. Обновляем последнее известное значение
                             last_mansard_count = current_count
                             logger.debug(f"Mansard progress updated: {current_count} galets ({percent}%)")
                     # --- Конец Логики Прогресс-бара Mansard Game ---
                     #----прошли галетники     
                     if flag=="galet_on":
                          # --- Фиксируем, что мы перешли на этап 2 ---
                          train_stage_2_active = True
                          #-----играем фон
                          play_background_music("fon6.mp3", loops=-1)
                          #---ждем окончания эффекта
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          #----играем историю    
                          if(language==1):
                              play_story(story_5_ru)  
                          if(language==2):
                              play_story(story_5_en)
                          if(language==3):
                              play_story(story_5_ar)

                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          #----отправляем на клиента
                          socketio.emit('level', 'open_mansard_door',to=None)
                          #-----добавили в историю
                          socklist.append('open_mansard_door')
                          #-----играем эффект
                          play_effect(door_attic)
                          if(language==1):
                              play_story(story_6_ru)  
                          if(language==2):
                              play_story(story_6_en)
                          if(language==3):
                              play_story(story_6_ar)
                              
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          send_esp32_command(ESP32_API_WOLF_URL, "game")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "game")
                          send_esp32_command(ESP32_API_SAFE_URL, "game")
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_2") # <-- КОМАНДА ВЫКЛЮЧЕНИЯ

                     if flag=="three_game_end":
                          send_esp32_command(ESP32_API_TRAIN_URL, "flag_on")
                          socketio.emit('level', 'active_open_mansard_stash',to=None)
                          socklist.append('active_open_mansard_stash')
                          #channel3.stop() 
                          #channel2.stop() 
                          #pygame.mixer.music.stop()
                             
                    #---если пришло сообщение что поставили красный флаг проверяем не было ли в истории сообщения что флаг сняли если было удаляем из истории
                     if flag=="flag1_on":
                          if 'flag1_off' in socklist:
                                   socklist.remove('flag1_off')
                         #----отправляем на клиента
                          socketio.emit('level', 'flag1_on',to=None)
                          #----добавили в историю
                          socklist.append('flag1_on')
                     if flag=="flag2_on":
                          if 'flag2_off' in socklist:
                                   socklist.remove('flag2_off')
                          socketio.emit('level', 'flag2_on',to=None)
                          socklist.append('flag2_on')
                     if flag=="flag3_on":
                          if 'flag3_off' in socklist:
                                   socklist.remove('flag3_off')
                          socketio.emit('level', 'flag3_on',to=None)
                          socklist.append('flag3_on')
                     if flag=="flag4_on":
                          if 'flag4_off' in socklist:
                                   socklist.remove('flag4_off')
                          socketio.emit('level', 'flag4_on',to=None)
                          socklist.append('flag4_on') 

                     if flag=="flag1_off":
                          if 'flag1_on' in socklist:
                                   socklist.remove('flag1_on')
                          socketio.emit('level', 'flag1_off',to=None)
                          socklist.append('flag1_off')
                     if flag=="flag2_off":
                          if 'flag2_on' in socklist:
                                   socklist.remove('flag2_on')
                          socketio.emit('level', 'flag2_off',to=None)
                          socklist.append('flag2_off')
                     if flag=="flag3_off":
                          if 'flag3_on' in socklist:
                                   socklist.remove('flag3_on')
                          socketio.emit('level', 'flag3_off',to=None)
                          socklist.append('flag3_off')
                     if flag=="flag4_off":
                          if 'flag4_on' in socklist:
                                   socklist.remove('flag4_on')
                          socketio.emit('level', 'flag4_off',to=None)
                          socklist.append('flag4_off')   
                    #-------закончили игру с флагами
                     if flag=="flagsendmr":
                          #----играем эффект 
                          pygame.mixer.music.stop()
                          play_effect(flags)
                          send_esp32_command(ESP32_API_TRAIN_URL, "flag_off")
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_3")
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          play_background_music("fon7.mp3", loops=0) 
                          if(language==1):
                              play_story(story_10_ru)  
                          if(language==2):
                              play_story(story_10_en)
                          if(language==3):
                              play_story(story_10_ar)
                          nextTrack = 1
                          

                     if flag=="door_owl":
                          # [FIX] Проверка на повтор: если 'owl' уже есть в истории, игнорируем
                          if 'owl' in socklist:
                              logger.debug("Игнорируем повторный door_owl")
                          else:
                              #----играем эффект 
                              play_effect(door_owl)
                              socketio.emit('level', 'owl',to=None)
                              #-----добавили в историю
                              socklist.append('owl')
                              send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
                              send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks") # Отключаем клики
                              #while effects_are_busy() and go == 1: 
                                  #eventlet.sleep(0.1)
                              eventlet.sleep(2.0)
                              if story13Flag == 0:
                                   story13Flag = 1
                                   if(language==1):
                                        play_story(story_13_ru)  
                                   if(language==2):
                                        play_story(story_13_en)
                                   if(language==3):
                                        play_story(story_13_ar)
     
                                   while channel3.get_busy()==True and go == 1: 
                                        eventlet.sleep(0.1)
                                   
     
                              if(language==1):
                                  play_story(story_14_a_ru)  
                              if(language==2):
                                  play_story(story_14_a_en)
                              if(language==3):
                                  play_story(story_14_a_ar)
                              while channel3.get_busy()==True and go == 1: eventlet.sleep(0.1) # Ждем завершения story_14_a
                              send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks") # Включаем клики обратно
                              #----активируем игру с совами
                              socketio.emit('level', 'active_owls',to=None)
                              socklist.append('active_owls')

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
                          #----играем эффект 
                          play_effect(owl_flew)
                          socketio.emit('level', 'owls',to=None)
                          #-----добавили в историю
                          socklist.append('owls')
                          owlFlewCount = 4 # Гарантируем 100%
                          socketio.emit('level', 'owl_flew_4', to=None)
                          socklist.append(f'owl_flew_4')
                          send_esp32_command(ESP32_API_TRAIN_URL, "owl_finish")
                          if(language==1):
                              play_story(story_14_b_ru)  
                          if(language==2):
                              play_story(story_14_b_en)
                          if(language==3):
                              play_story(story_14_b_ar)

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
                               if(language==1):
                                    play_story(story_13_ru)  
                               if(language==2):
                                    play_story(story_13_en)
                               if(language==3):
                                    play_story(story_13_ar)
 
                               while channel3.get_busy()==True and go == 1: 
                                    eventlet.sleep(0.1)     

                          if(language==1):
                              play_story(story_17_ru)  
                          if(language==2):
                              play_story(story_17_en)
                          if(language==3):
                              play_story(story_17_ar)
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

                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)

                          #------играем голос    
                          if(language==1):
                              play_story(story_18_ru)  
                          if(language==2):
                              play_story(story_18_en)
                          if(language==3):
                              play_story(story_18_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)    
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
                               if(language==1):
                                    play_story(story_13_ru)  
                               if(language==2):
                                    play_story(story_13_en)
                               if(language==3):
                                    play_story(story_13_ar)
 
                               while channel3.get_busy()==True and go == 1: 
                                    eventlet.sleep(0.1)

                          if(language==1):
                              play_story(story_19_ru)  
                          if(language==2):
                              play_story(story_19_en)
                          if(language==3):
                              play_story(story_19_ar)
                          while channel3.get_busy()==True and go == 1: eventlet.sleep(0.1) # Ждем завершения story_19
                          send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks") # Включаем клики обратно
                          #----активируем игру с собакой
                          socketio.emit('level', 'active_dog',to=None)
                          socklist.append('active_dog')

                     if flag=="dog_sleep":
                          #----играем эффект 
                          play_effect(dog_sleep)

                     if flag=="dog_growl":
                          #----играем эффект 
                          play_effect(dog_growl) 
                     if flag=="dog_lock":
                          # Защита от двойного срабатывания
                          if 'dog_end_processed' in socklist:
                              logger.debug("Игнорируем повторный dog_lock")
                          else:
                              socklist.append('dog_end_processed') # Ставим метку
                              #----играем эффект 
                              socketio.emit('level', 'dog',to=None)
                              #-----добавили в историю
                              socklist.append('dog')
                              send_esp32_command(ESP32_API_TRAIN_URL, "key_finish")
                              play_effect(dog_lock)

                              #while effects_are_busy() and go == 1: 
                              #    eventlet.sleep(0.1)

                              if(language==1):
                                  play_story(story_21_ru)  
                              if(language==2):
                                  play_story(story_21_en)
                              if(language==3):
                                  play_story(story_21_ar)

                     if flag=="story_20_a":
                          if(language==1):
                              play_story(story_20_a_ru)  
                          if(language==2):
                              play_story(story_20_a_en)
                          if(language==3):
                              play_story(story_20_a_ar)

                     if flag=="story_20_b":
                          if(language==1):
                              play_story(story_20_b_ru)  
                          if(language==2):
                              play_story(story_20_b_en)
                          if(language==3):
                              play_story(story_20_b_ar) 
                     if flag=="story_20_c":
                          if(language==1):
                              play_story(story_20_c_ru)  
                          if(language==2):
                              play_story(story_20_c_en)
                          if(language==3):
                              play_story(story_20_c_ar)         

                     if flag=="story_22_a":
                          # 1. Ждем, пока канал освободится
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          # 2. Воспроизводим историю
                          if(language==1):
                              play_story(story_22_a_ru)  
                          if(language==2):
                              play_story(story_22_a_en)
                          if(language==3):
                              play_story(story_22_a_ar)
                              
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
                          if(language==1):
                              play_story(story_22_b_ru)  
                          if(language==2):
                              play_story(story_22_b_en)
                          if(language==3):
                              play_story(story_22_b_ar)
                             
                          # 3. Ждем, пока PLAY ЗАКОНЧИТСЯ
                          while channel3.get_busy()==True and go == 1:
                              eventlet.sleep(0.1)
                          # 4. Отправляем подтверждение на Arduino
                          serial_write_queue.put('story_22_done')
                       
                     if flag=="story_22_c":
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_22_c_ru)  
                          if(language==2):
                              play_story(story_22_c_en)
                          if(language==3):
                              play_story(story_22_c_ar)
                              
                          # 3. Ждем, пока PLAY ЗАКОНЧИТСЯ
                          while channel3.get_busy()==True and go == 1:
                              eventlet.sleep(0.1)
                          # 4. Отправляем подтверждение на Arduino
                          serial_write_queue.put('story_22_done')
                          
                     if flag=="cave_click":
                          #----играем эффект 
                          play_effect(cave_click)
                     if flag=="door_cave":
                          #----играем эффект 
                          play_effect(door_cave)
                          socketio.emit('level', 'active_troll',to=None)
                          socklist.append('active_troll')
                          socketio.emit('level', 'mine',to=None)
                          socklist.append('mine')
                          # Убираем ожидание завершения эффекта
                          # while effects_are_busy() and go == 1:
                          #     eventlet.sleep(0.1)
                          # Добавляем фиксированную задержку 2 секунды
                          eventlet.sleep(2.0)
                          if(language==1):
                              play_story(story_26_ru)  
                          if(language==2):
                              play_story(story_26_en)
                          if(language==3):
                              play_story(story_26_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)    
                     if flag=="cave_search1":
                          #----играем эффект 
                          play_effect(cave_search)
                          socketio.emit('level', 'cave_search1', to=None)
                          socklist.append('cave_search1')
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_27_a_ru)  
                          if(language==2):
                              play_story(story_27_a_en)
                          if(language==3):
                              play_story(story_27_a_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          eventlet.sleep(1.1)        
                          serial_write_queue.put('cave_search1')    
                     if flag=="cave_search2":
                          #----играем эффект 
                          play_effect(cave_search)
                          socketio.emit('level', 'cave_search2', to=None)
                          socklist.append('cave_search2')
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_27_b_ru)  
                          if(language==2):
                              play_story(story_27_b_en)
                          if(language==3):
                              play_story(story_27_b_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          eventlet.sleep(1.1)         
                          serial_write_queue.put('cave_search2')        
                     if flag=="cave_search3":
                          #----играем эффект 
                          play_effect(cave_search)
                          socketio.emit('level', 'cave_search3', to=None)
                          socklist.append('cave_search3')
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_27_c_ru)  
                          if(language==2):
                              play_story(story_27_c_en)
                          if(language==3):
                              play_story(story_27_c_ar) 
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          eventlet.sleep(1.1)        
                          serial_write_queue.put('cave_search3')                          
                     if flag=="cave_end":
                          #----играем эффект 
                          socketio.emit('level', 'troll',to=None)
                          socklist.append('troll')
                          socketio.emit('level', 'cave_end', to=None)
                          socklist.append('cave_end')
                          play_effect(cave_end)
                          send_esp32_command(ESP32_API_TRAIN_URL, "troll_finish")
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_30_ru)  
                          if(language==2):
                              play_story(story_30_en)
                          if(language==3):
                              play_story(story_30_ar)    
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
                          if(language==1):
                              play_story(story_23_ru)  
                          if(language==2):
                              play_story(story_23_en)
                          if(language==3):
                              play_story(story_23_ar)
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
                          if(language==1):
                              play_story(story_24_ru)  
                          if(language==2):
                              play_story(story_24_en)
                          if(language==3):
                              play_story(story_24_ar)

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
                              
                          if(language==1):
                              play_story(story_25_ru)  
                          if(language==2):
                              play_story(story_25_en)
                          if(language==3):
                              play_story(story_25_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)  
                          eventlet.sleep(1.1)
                          if(language==1):
                              play_story(story_31_ru)  
                          if(language==2):
                              play_story(story_31_en)
                          if(language==3):
                              play_story(story_31_ar)
                              
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          serial_write_queue.put('open_workshop')
                          eventlet.sleep(1.1)          
                          play_effect(door_workshop)
                          play_background_music("fon9.mp3", loops=-1)
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_32_ru)  
                          if(language==2):
                              play_story(story_32_en)
                          if(language==3):
                              play_story(story_32_ar)
                          
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
                          if(language==1):
                              play_story(story_46_ru)  
                          if(language==2):
                              play_story(story_46_en)
                          if(language==3):
                              play_story(story_46_ar)    
                          send_esp32_command(ESP32_API_TRAIN_URL, "train_on")
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          # Отправляем команду на Arduino, чтобы начать 5-секундное мерцание
                          serial_write_queue.put('library_flicker_start')
                          eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_47_ru)  
                          if(language==2):
                              play_story(story_47_en)
                          if(language==3):
                              play_story(story_47_ar)

                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)

                          play_background_music("fon15.mp3", loops=-1)
                          if(language==1):
                              play_story(story_48_ru)  
                          if(language==2):
                              play_story(story_48_en)
                          if(language==3):
                              play_story(story_48_ar)
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
                          if(language==1):
                              play_story(story_49_ru)  
                          if(language==2):
                              play_story(story_49_en)
                          if(language==3):
                              play_story(story_49_ar)
                     if flag=="door_basket":
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_9") 
                          socketio.emit('level', 'cup',to=None)
                          socklist.append('cup')
                          #----играем эффект 
                          play_effect(door_basket)
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          if(language==1):
                              play_story(story_50_ru)  
                          if(language==2):
                              play_story(story_50_en)
                          if(language==3):
                              play_story(story_50_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)    
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
                          if(language==1):
                              play_story(story_51_ru)  
                          if(language==2):
                              play_story(story_51_en)
                          if(language==3):
                              play_story(story_51_ar)

                     if flag=="fire1":
                          #----играем эффект 
                          play_effect(fire1)
                          if fire1Flag == 0:
                              fire1Flag = 1
                              if(language==1):
                                  play_story(story_32_a_ru)  
                              if(language==2):
                                  play_story(story_32_a_en)
                              if(language==3):
                                  play_story(story_32_a_ar)
                     if flag=="fire2":
                          #----играем эффект 
                          play_effect(fire2)
                          if fire2Flag == 0:
                              fire2Flag = 1
                              if(language==1):
                                  play_story(story_32_b_ru)  
                              if(language==2):
                                  play_story(story_32_b_en)
                              if(language==3):
                                  play_story(story_32_b_ar)
                     if flag=="fire3":
                          #----играем эффект 
                          play_effect(fire3)
                     if flag=="fire0":
                          #----играем эффект 
                          play_effect(fire0)
                          if fire0Flag == 0:
                              fire0Flag = 1
                              if(language==1):
                                  play_story(story_32_c_ru)  
                              if(language==2):
                                  play_story(story_32_c_en)
                              if(language==3):
                                  play_story(story_32_c_ar)
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
                          socklist.append('broom')
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)    
                          if(language==1):
                              play_story(story_33_ru)  
                          if(language==2):
                              play_story(story_33_en)
                          if(language==3):
                              play_story(story_33_ar)                    
                     if flag=="helmet":
                          #----играем эффект 
                          play_effect(craft_success)
                          socklist.append('helmet')
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)    
                          if(language==1):
                              play_story(story_34_ru)  
                          if(language==2):
                              play_story(story_34_en)
                          if(language==3):
                              play_story(story_34_ar)
                     if flag=="story_35":
                          socketio.emit('level', 'workshop',to=None)
                          socklist.append('workshop') 
                          send_esp32_command(ESP32_API_TRAIN_URL, "item_end") 
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_6") 
                          if(language==1):
                              play_story(story_35_ru)  
                          if(language==2):
                              play_story(story_35_en)
                          if(language==3):
                              play_story(story_35_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          eventlet.sleep(1.0)
                          send_esp32_command(ESP32_API_WOLF_URL, "day_off")
                          send_esp32_command(ESP32_API_TRAIN_URL, "day_off")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
                          send_esp32_command(ESP32_API_SAFE_URL, "day_off")    
                          play_background_music("fon10.mp3", loops=-1)
                          if(language==1):
                              play_story(story_36_ru)  
                          if(language==2):
                              play_story(story_36_en)
                          if(language==3):
                              play_story(story_36_ar)
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          serial_write_queue.put('student_open')
                          eventlet.sleep(1.0)     
                          if(language==1):
                              play_story(story_37_ru)  
                          if(language==2):
                              play_story(story_37_en)
                          if(language==3):
                              play_story(story_37_ar)

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
                          if(language==1):
                              play_story(story_38_ru)  
                          if(language==2):
                              play_story(story_38_en)
                          if(language==3):
                              play_story(story_38_ar)
                          socketio.emit('level', 'active_ghost',to=None)
                          socklist.append('active_ghost')         
                     if flag=="story_39":
                          #send_esp32_command(ESP32_API_WOLF_URL, "ghost_game")
                          if(language==1):
                              play_story(story_39_ru)  
                          if(language==2):
                              play_story(story_39_en)
                          if(language==3):
                              play_story(story_39_ar) 
                     if flag=="story_40":
                          send_esp32_command(ESP32_API_WOLF_URL, "ghost_game")
                          socketio.emit('level', 'story_40', to=None)
                          socklist.append('story_40') # Добавляем флаг для UI
                          if(language==1):
                              play_story(story_40_ru)  
                          if(language==2):
                              play_story(story_40_en)
                          if(language==3):
                              play_story(story_40_ar)  
                     if flag=="story_41":
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                          socketio.emit('level', 'story_41', to=None)
                          socklist.append('story_41') # Добавляем флаг для UI
                          if(language==1):
                              play_story(story_41_ru)  
                          if(language==2):
                              play_story(story_41_en)
                          if(language==3):
                              play_story(story_41_ar)
                     if flag=="story_42":
                          socketio.emit('level', 'story_42', to=None)
                          socklist.append('story_42') # Добавляем флаг для UI
                          if(language==1):
                              play_story(story_42_ru)  
                          if(language==2):
                              play_story(story_42_en)
                          if(language==3):
                              play_story(story_42_ar)
                     if flag == "ghost_knock":
                          play_effect(knock_castle, loops=-1)
                     if flag=="punch":
                          channel2.stop()
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_7") 
                          socketio.emit('level', 'ghost',to=None)
                          socklist.append('ghost')
                          socketio.emit('level', 'punch', to=None)
                          socklist.append('punch') # Добавляем флаг для UI
                          if(language==1):
                              play_story(story_43_ru)  
                          if(language==2):
                              play_story(story_43_en)
                          if(language==3):
                              play_story(story_43_ar) 
                          while channel3.get_busy()==True and go == 1: 
                              eventlet.sleep(0.1)
                          serial_write_queue.put('open_library')
                          #send_esp32_command(ESP32_API_TRAIN_URL, #"ghost_game_end")
                          #send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
                          eventlet.sleep(2.0)
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                          eventlet.sleep(1.0)
                          if(language==1):
                              play_story(story_44_ru)  
                          if(language==2):
                              play_story(story_44_en)
                          if(language==3):
                              play_story(story_44_ar) 

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
                          if(language==1):
                              play_story(story_54_ru)  
                          if(language==2):
                              play_story(story_54_en)
                          if(language==3):
                              play_story(story_54_ar)
                      #-----2 уровень        
                     if flag=="second_level":
                          socketio.emit('level', 'second_level',to=None)
                          socklist.append('second_level')
                          play_effect(level_up)  
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          #------играем голос    
                          if(language==1):
                              play_story(story_53_ru)  
                          if(language==2):
                              play_story(story_53_en)
                          if(language==3):
                              play_story(story_53_ar)
                     #----прошли 1 уровень      
                     if flag=="first_level":
                          socketio.emit('level', 'first_level',to=None)
                          socklist.append('first_level')
                          play_effect(level_up)  
                          while effects_are_busy() and go == 1: 
                              eventlet.sleep(0.1)
                          #------играем голос    
                          if(language==1):
                              play_story(story_52_ru)  
                          if(language==2):
                              play_story(story_52_en)
                          if(language==3):
                              play_story(story_52_ar)  
                     

                     # "boy_in_lesson" (с урока, level 18) -> Запустить интро
                     # 1. УРОК (Level 18): Вход
                     if flag == "boy_in_lesson":
                          # Если интро уже сыграно (урок идет), и мы вернулись с паузы
                          if level_18_intro_played:
                              # [FIX] Снимаем с паузы
                              logger.info("Возобновление урока (снятие с паузы)")
                              pygame.mixer.music.unpause()
                              if 'stop_players_rest' in socklist:
                                    socklist.remove('stop_players_rest')
                              socketio.emit('level', 'start_players', to=None)
                          # Если это первый запуск
                          elif time.time() - last_boy_in_time < 2.0:
                              logger.debug("Игнорируем повторный boy_in_lesson (дребезг при установке)")
                          else:
                              level_18_intro_played = True
                              # Запоминаем время нового стабильного входа
                              last_boy_in_time = time.time()
                              play_background_music("fon18.mp3", loops=-1)
                              socketio.emit('level', 'start_players', to=None)
                              socklist.append('start_players')
                              
                              if(language==1):
                                  play_story(story_57_ru)
                              if(language==2):
                                  play_story(story_57_en)
                              if(language==3):
                                  play_story(story_57_ar)
                              
                              # Ждем завершения истории
                              while channel3.get_busy() == True and go == 1:
                                  eventlet.sleep(0.1)
                              
                              play_effect(applause)
                              
                              while channel2.get_busy() == True and go == 1:
                                  eventlet.sleep(0.1)
                                  
                              if(language==1):
                                  play_story(story_58_ru)
                              if(language==2):
                                  play_story(story_58_en)
                              if(language==3):
                                  play_story(story_58_ar)
                              
                              while channel3.get_busy() == True and go == 1:
                                  eventlet.sleep(0.1)
                              
                              eventlet.sleep(1.0)

                          # --- ЗАЩИТНЫЙ МЕХАНИЗМ: ДВОЙНАЯ ОТПРАВКА ---
                          
                          # 1. Взводим защиту (дублируем)
                          serial_write_queue.put('start_lesson')
                          logger.debug("SENT [Arduino]: start_lesson (1/2)")
                          eventlet.sleep(0.2) # Короткая пауза
                          serial_write_queue.put('start_lesson')
                          logger.debug("SENT [Arduino]: start_lesson (2/2)")
                          
                          # Ждем, чтобы башня точно успела обработать
                          eventlet.sleep(1.0) 
                          
                          # 2. Запускаем мяч (ТРОЙНОЙ УДАР)
                          serial_write_queue.put('start_game_basket')
                          logger.debug("SENT [Arduino]: start_game_basket (1/3)")
                          eventlet.sleep(0.5) 
                          serial_write_queue.put('start_game_basket')
                          logger.debug("SENT [Arduino]: start_game_basket (2/3)")
                          eventlet.sleep(0.5) 
                          serial_write_queue.put('start_game_basket')
                          logger.debug("SENT [Arduino]: start_game_basket (3/3)")
                          # -------------------------------------------
                          
                          eventlet.sleep(1.0)
                          socketio.emit('level', 'active_basket', to=None)
                          socklist.append('active_basket')

                     # 2. УРОК (Level 18): Выход (Пауза)
                     if flag == "boy_out_lesson":
                         # Если с момента входа прошло меньше 3 секунд, игнорируем выход.
                         if time.time() - last_boy_in_time < 3.0:
                             logger.debug("Игнорируем boy_out_lesson (дребезг контактов)")
                         else:
                             level_18_intro_played = False
                             pygame.mixer.music.pause()
                             try:
                                 play_effect(lose1)
                                 # Играем историю 69 (Мальчик ушел)
                                 if(language==1): play_story(story_69_ru)
                                 if(language==2): play_story(story_69_en)
                                 if(language==3): play_story(story_69_ar)
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
                              if(language==1): play_story(story_70_ru)
                              if(language==2): play_story(story_70_en)
                              if(language==3): play_story(story_70_ar)
                              
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
                               # Стандартная логика паузы
                               pygame.mixer.music.pause()
                               try:
                                   play_effect(lose1)
                                   # Играем историю 69 (Мальчик ушел)
                                   if(language==1): play_story(story_69_ru)
                                   if(language==2): play_story(story_69_en)
                                   if(language==3): play_story(story_69_ar)
                               except Exception as e:
                                   logger.error(f"Ошибка звука: {e}")

                               
                               socketio.emit('level', 'stop_players_rest', to=None)
                               socklist.append('stop_players_rest')

                     if flag=="story_59":
                          if(language==1):
                              play_story(story_59_ru)  
                          if(language==2):
                              play_story(story_59_en)
                          if(language==3):
                              play_story(story_59_ar)
                     if flag=="story_55":
                          if(language==1):
                              play_story(story_55_ru)  
                          if(language==2):
                              play_story(story_55_en)
                          if(language==3):
                              play_story(story_55_ar)
                     if flag=="crime_end":
                          socketio.emit('level', 'crime',to=None)
                          socklist.append('crime')
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_12") 
                          socketio.emit('level', 'active_basket',to=None)
                          socklist.append('active_basket')
                          play_background_music("fon17.mp3", loops=-1)
                          if(language==1):
                              play_story(story_56_ru)  
                          if(language==2):
                              play_story(story_56_en)
                          if(language==3):
                              play_story(story_56_ar)
                     if flag=="lesson_goal":
                          # 1. Воспроизводим звук аплодисментов (Эффект, Канал 2)
                          play_effect(applause)
                          
                          # 2. Воспроизводим story_61_a (История, Канал 3)
                          # Эта история будет автоматически прервана, если поступит другая команда play_story()
                          if(language==1):
                              play_story(story_61_a_ru)  
                          if(language==2):
                              play_story(story_61_a_en)
                          if(language==3):
                              play_story(story_61_a_ar)
                     if flag=="flying_ball":
                          play_effect(flying_ball)
                          storyBasketFlag = 1
                     if flag=="catch1":
                          play_effect(catch1)
                          if storyBasketFlag == 1:
                              catchCount += 1
                              if catchCount == 1:
                                   if(language==1):
                                        play_story(story_60_a_ru)  
                                   if(language==2):
                                        play_story(story_60_a_en)
                                   if(language==3):
                                        play_story(story_60_a_ar)
                              if catchCount == 2:
                                   if(language==1):
                                        play_story(story_60_b_ru)  
                                   if(language==2):
                                        play_story(story_60_b_en)
                                   if(language==3):
                                        play_story(story_60_b_ar)
                              if catchCount == 3:
                                   if(language==1):
                                        play_story(story_60_c_ru)  
                                   if(language==2):
                                        play_story(story_60_c_en)
                                   if(language==3):
                                        play_story(story_60_c_ar)   
                              if catchCount == 4:
                                   if(language==1):
                                        play_story(story_60_d_ru)  
                                   if(language==2):
                                        play_story(story_60_d_en)
                                   if(language==3):
                                        play_story(story_60_d_ar)
                              if catchCount == 5:
                                   if(language==1):
                                        play_story(story_60_e_ru)  
                                   if(language==2):
                                        play_story(story_60_e_en)
                                   if(language==3):
                                        play_story(story_60_e_ar)
                              if catchCount == 6:
                                   if(language==1):
                                        play_story(story_60_f_ru)  
                                   if(language==2):
                                        play_story(story_60_f_en)
                                   if(language==3):
                                        play_story(story_60_f_ar)
                              if catchCount == 7:
                                   if(language==1):
                                        play_story(story_60_g_ru)  
                                   if(language==2):
                                        play_story(story_60_g_en)
                                   if(language==3):
                                        play_story(story_60_g_ar)
                              if catchCount == 8:
                                   if(language==1):
                                        play_story(story_60_h_ru)  
                                   if(language==2):
                                        play_story(story_60_h_en)
                                   if(language==3):
                                        play_story(story_60_h_ar)
                              if catchCount == 9:
                                   if(language==1):
                                        play_story(story_60_i_ru)  
                                   if(language==2):
                                        play_story(story_60_i_en)
                                   if(language==3):
                                        play_story(story_60_i_ar)
                              if catchCount == 10:
                                   if(language==1):
                                        play_story(story_60_j_ru)  
                                   if(language==2):
                                        play_story(story_60_j_en)
                                   if(language==3):
                                        play_story(story_60_j_ar)          
                              print(catchCount)
                     if flag=="catch2":
                          play_effect(catch2)
                          if storyBasketFlag == 1:
                              catchCount += 1
                              if catchCount == 1:
                                   if(language==1):
                                        play_story(story_60_a_ru)  
                                   if(language==2):
                                        play_story(story_60_a_en)
                                   if(language==3):
                                        play_story(story_60_a_ar)
                              if catchCount == 2:
                                   if(language==1):
                                        play_story(story_60_b_ru)  
                                   if(language==2):
                                        play_story(story_60_b_en)
                                   if(language==3):
                                        play_story(story_60_b_ar)
                              if catchCount == 3:
                                   if(language==1):
                                        play_story(story_60_c_ru)  
                                   if(language==2):
                                        play_story(story_60_c_en)
                                   if(language==3):
                                        play_story(story_60_c_ar)   
                              if catchCount == 4:
                                   if(language==1):
                                        play_story(story_60_d_ru)  
                                   if(language==2):
                                        play_story(story_60_d_en)
                                   if(language==3):
                                        play_story(story_60_d_ar)
                              if catchCount == 5:
                                   if(language==1):
                                        play_story(story_60_e_ru)  
                                   if(language==2):
                                        play_story(story_60_e_en)
                                   if(language==3):
                                        play_story(story_60_e_ar)
                              if catchCount == 6:
                                   if(language==1):
                                        play_story(story_60_f_ru)  
                                   if(language==2):
                                        play_story(story_60_f_en)
                                   if(language==3):
                                        play_story(story_60_f_ar)
                              if catchCount == 7:
                                   if(language==1):
                                        play_story(story_60_g_ru)  
                                   if(language==2):
                                        play_story(story_60_g_en)
                                   if(language==3):
                                        play_story(story_60_g_ar)
                              if catchCount == 8:
                                   if(language==1):
                                        play_story(story_60_h_ru)  
                                   if(language==2):
                                        play_story(story_60_h_en)
                                   if(language==3):
                                        play_story(story_60_h_ar)
                              if catchCount == 9:
                                   if(language==1):
                                        play_story(story_60_i_ru)  
                                   if(language==2):
                                        play_story(story_60_i_en)
                                   if(language==3):
                                        play_story(story_60_i_ar)
                              if catchCount == 10:
                                   if(language==1):
                                        play_story(story_60_j_ru)  
                                   if(language==2):
                                        play_story(story_60_j_en)
                                   if(language==3):
                                        play_story(story_60_j_ar)
                              print(catchCount) 
                     if flag=="catch3":
                          play_effect(catch3)
                          if storyBasketFlag == 1:
                              catchCount += 1
                              if catchCount == 1:
                                   if(language==1):
                                        play_story(story_60_a_ru)  
                                   if(language==2):
                                        play_story(story_60_a_en)
                                   if(language==3):
                                        play_story(story_60_a_ar)
                              if catchCount == 2:
                                   if(language==1):
                                        play_story(story_60_b_ru)  
                                   if(language==2):
                                        play_story(story_60_b_en)
                                   if(language==3):
                                        play_story(story_60_b_ar)
                              if catchCount == 3:
                                   if(language==1):
                                        play_story(story_60_c_ru)  
                                   if(language==2):
                                        play_story(story_60_c_en)
                                   if(language==3):
                                        play_story(story_60_c_ar)   
                              if catchCount == 4:
                                   if(language==1):
                                        play_story(story_60_d_ru)  
                                   if(language==2):
                                        play_story(story_60_d_en)
                                   if(language==3):
                                        play_story(story_60_d_ar)
                              if catchCount == 5:
                                   if(language==1):
                                        play_story(story_60_e_ru)  
                                   if(language==2):
                                        play_story(story_60_e_en)
                                   if(language==3):
                                        play_story(story_60_e_ar)
                              if catchCount == 6:
                                   if(language==1):
                                        play_story(story_60_f_ru)  
                                   if(language==2):
                                        play_story(story_60_f_en)
                                   if(language==3):
                                        play_story(story_60_f_ar)
                              if catchCount == 7:
                                   if(language==1):
                                        play_story(story_60_g_ru)  
                                   if(language==2):
                                        play_story(story_60_g_en)
                                   if(language==3):
                                        play_story(story_60_g_ar)
                              if catchCount == 8:
                                   if(language==1):
                                        play_story(story_60_h_ru)  
                                   if(language==2):
                                        play_story(story_60_h_en)
                                   if(language==3):
                                        play_story(story_60_h_ar)
                              if catchCount == 9:
                                   if(language==1):
                                        play_story(story_60_i_ru)  
                                   if(language==2):
                                        play_story(story_60_i_en)
                                   if(language==3):
                                        play_story(story_60_i_ar)
                              if catchCount == 10:
                                   if(language==1):
                                        play_story(story_60_j_ru)  
                                   if(language==2):
                                        play_story(story_60_j_en)
                                   if(language==3):
                                        play_story(story_60_j_ar)
                              print(catchCount)
                     if flag=="catch4":
                          play_effect(catch4)
                          if storyBasketFlag == 1:
                              catchCount += 1
                              if catchCount == 1:
                                   if(language==1):
                                        play_story(story_60_a_ru)  
                                   if(language==2):
                                        play_story(story_60_a_en)
                                   if(language==3):
                                        play_story(story_60_a_ar)
                              if catchCount == 2:
                                   if(language==1):
                                        play_story(story_60_b_ru)  
                                   if(language==2):
                                        play_story(story_60_b_en)
                                   if(language==3):
                                        play_story(story_60_b_ar)
                              if catchCount == 3:
                                   if(language==1):
                                        play_story(story_60_c_ru)  
                                   if(language==2):
                                        play_story(story_60_c_en)
                                   if(language==3):
                                        play_story(story_60_c_ar)   
                              if catchCount == 4:
                                   if(language==1):
                                        play_story(story_60_d_ru)  
                                   if(language==2):
                                        play_story(story_60_d_en)
                                   if(language==3):
                                        play_story(story_60_d_ar)
                              if catchCount == 5:
                                   if(language==1):
                                        play_story(story_60_e_ru)  
                                   if(language==2):
                                        play_story(story_60_e_en)
                                   if(language==3):
                                        play_story(story_60_e_ar)
                              if catchCount == 6:
                                   if(language==1):
                                        play_story(story_60_f_ru)  
                                   if(language==2):
                                        play_story(story_60_f_en)
                                   if(language==3):
                                        play_story(story_60_f_ar)
                              if catchCount == 7:
                                   if(language==1):
                                        play_story(story_60_g_ru)  
                                   if(language==2):
                                        play_story(story_60_g_en)
                                   if(language==3):
                                        play_story(story_60_g_ar)
                              if catchCount == 8:
                                   if(language==1):
                                        play_story(story_60_h_ru)  
                                   if(language==2):
                                        play_story(story_60_h_en)
                                   if(language==3):
                                        play_story(story_60_h_ar)
                              if catchCount == 9:
                                   if(language==1):
                                        play_story(story_60_i_ru)  
                                   if(language==2):
                                        play_story(story_60_i_en)
                                   if(language==3):
                                        play_story(story_60_i_ar)
                              if catchCount == 10:
                                   if(language==1):
                                        play_story(story_60_j_ru)  
                                   if(language==2):
                                        play_story(story_60_j_en)
                                   if(language==3):
                                        play_story(story_60_j_ar) 
                              print(catchCount)      
                     # --- Логика голов игрока с счетчиком ---
                     if flag=="goal_1_player" or flag=="goal_2_player" or flag=="goal_3_player" or flag=="goal_4_player":
                          # Добавляем инкрементальные флаги для UI
                          if flag == "goal_1_player":
                              socklist.append('goal_1_player')
                              socketio.emit('level', 'goal_1_player',to=None)
                          if flag == "goal_2_player":
                              socklist.append('goal_2_player')
                              socketio.emit('level', 'goal_2_player',to=None)
                          # 1. Воспроизвести случайный звук гола (goal2-goal7)
                          play_effect(random.choice(player_goal_sounds))
                          
                          # 2. Выбрать правильный список историй по языку
                          current_story_list = []
                          if language == 1:
                              current_story_list = player_goal_stories_ru
                          elif language == 2:
                              current_story_list = player_goal_stories_en
                          elif language == 3:
                              current_story_list = player_goal_stories_ar
                          
                          # 3. Воспроизвести историю по счетчику (начиная с b, c, d...)
                          if goalCount < len(current_story_list):
                              play_story(current_story_list[goalCount])
                          else:
                              # Если счетчик превысил кол-во историй, проигрываем последнюю
                              play_story(current_story_list[-1]) 
                          
                          # 4. Увеличить счетчик для следующего гола
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

                          # 2. Выбрать правильный список историй по языку
                          current_enemy_story_list = []
                          if language == 1:
                              current_enemy_story_list = enemy_goal_stories_ru
                          elif language == 2:
                              current_enemy_story_list = enemy_goal_stories_en
                          elif language == 3:
                              current_enemy_story_list = enemy_goal_stories_ar
                          
                          # 3. Воспроизвести историю по счетчику (начиная с a, b, c...)
                          if enemyGoalCount < len(current_enemy_story_list):
                              play_story(current_enemy_story_list[enemyGoalCount])
                          else:
                              # Если счетчик превысил кол-во историй, проигрываем последнюю
                              play_story(current_enemy_story_list[-1]) 
                          
                          # 4. Увеличить счетчик для следующего гола бота
                          enemyGoalCount += 1

                     if flag=="start_snitch":
                          #----играем эффект 
                          enemyCatchCount += 1
                          if enemyCatchCount == 1:
                              play_effect(enemy_catch1)
                          if enemyCatchCount == 2:
                              play_effect(enemy_catch2)
                          if enemyCatchCount == 3:
                              play_effect(enemy_catch3)
                          if enemyCatchCount == 4:
                              play_effect(enemy_catch4)
                              enemyCatchCount = 0   
                          sintchEnemyCatchCount += 1
                          if sintchEnemyCatchCount == 1:
                               if(language==1):
                                    play_story(story_62_a_ru)  
                               if(language==2):
                                    play_story(story_62_a_en)
                               if(language==3):
                                    play_story(story_62_a_ar)
                          if sintchEnemyCatchCount == 2:
                               if(language==1):
                                    play_story(story_62_b_ru)  
                               if(language==2):
                                    play_story(story_62_b_en)
                               if(language==3):
                                    play_story(story_62_b_ar)
                          if sintchEnemyCatchCount == 3:
                               if(language==1):
                                    play_story(story_62_c_ru)  
                               if(language==2):
                                    play_story(story_62_c_en)
                               if(language==3):
                                    play_story(story_62_c_ar)   
                          if sintchEnemyCatchCount == 4:
                               if(language==1):
                                    play_story(story_62_d_ru)  
                               if(language==2):
                                    play_story(story_62_d_en)
                               if(language==3):
                                    play_story(story_62_d_ar)
                          if sintchEnemyCatchCount == 5:
                               sintchEnemyCatchCount = 0
                               if(language==1):
                                    play_story(story_62_e_ru)  
                               if(language==2):
                                    play_story(story_62_e_en)
                               if(language==3):
                                    play_story(story_62_e_ar)    
                          print(enemyCatchCount)                

                     if flag=="red_ball":
                          #----играем эффект 
                          enemyCatchCount += 1
                          if enemyCatchCount == 1:
                              play_effect(enemy_catch1)
                          if enemyCatchCount == 2:
                              play_effect(enemy_catch2)
                          if enemyCatchCount == 3:
                              play_effect(enemy_catch3)
                          if enemyCatchCount == 4:
                              play_effect(enemy_catch4)
                              enemyCatchCount = 0   
                          redSintchEnemyCatchCount += 1
                          if redSintchEnemyCatchCount == 1:
                               if(language==1):
                                    play_story(story_63_a_ru)  
                               if(language==2):
                                    play_story(story_63_a_en)
                               if(language==3):
                                    play_story(story_63_a_ar)
                          if redSintchEnemyCatchCount == 2:
                               if(language==1):
                                    play_story(story_63_b_ru)  
                               if(language==2):
                                    play_story(story_63_b_en)
                               if(language==3):
                                    play_story(story_63_b_ar)
                          if redSintchEnemyCatchCount == 3:
                               if(language==1):
                                    play_story(story_63_c_ru)  
                               if(language==2):
                                    play_story(story_63_c_en)
                               if(language==3):
                                    play_story(story_63_c_ar)   
                          if redSintchEnemyCatchCount == 4:
                               if(language==1):
                                    play_story(story_63_d_ru)  
                               if(language==2):
                                    play_story(story_63_d_en)
                               if(language==3):
                                    play_story(story_63_d_ar)
                          if redSintchEnemyCatchCount == 5:
                               if(language==1):
                                    play_story(story_63_e_ru)  
                               if(language==2):
                                    play_story(story_63_e_en)
                               if(language==3):
                                    play_story(story_63_e_ar)
                          if redSintchEnemyCatchCount == 6:
                               if(language==1):
                                    play_story(story_63_f_ru)  
                               if(language==2):
                                    play_story(story_63_f_en)
                               if(language==3):
                                    play_story(story_63_f_ar)
                          if redSintchEnemyCatchCount == 7:
                               if(language==1):
                                    play_story(story_63_g_ru)  
                               if(language==2):
                                    play_story(story_63_g_en)
                               if(language==3):
                                    play_story(story_63_g_ar)
                          if redSintchEnemyCatchCount == 8:
                               if(language==1):
                                    play_story(story_63_h_ru)  
                               if(language==2):
                                    play_story(story_63_h_en)
                               if(language==3):
                                    play_story(story_63_h_ar)
                          if redSintchEnemyCatchCount == 9:
                               if(language==1):
                                    play_story(story_63_i_ru)  
                               if(language==2):
                                    play_story(story_63_i_en)
                               if(language==3):
                                    play_story(story_63_i_ar)
                          if redSintchEnemyCatchCount == 10:
                               redSintchEnemyCatchCount = 0
                               if(language==1):
                                    play_story(story_63_j_ru)  
                               if(language==2):
                                    play_story(story_63_j_en)
                               if(language==3):
                                    play_story(story_63_j_ar)     
                          print(enemyCatchCount)     
                     if flag=="enemy_catch1":
                          play_effect(enemy_catch1)
                          redClickSintchEnemyCatchCount += 1
                          if redClickSintchEnemyCatchCount == 1:
                               if(language==1):
                                    play_story(story_64_a_ru)  
                               if(language==2):
                                    play_story(story_64_a_en)
                               if(language==3):
                                    play_story(story_64_a_ar)
                          if redClickSintchEnemyCatchCount == 2:
                               redClickSintchEnemyCatchCount = 0
                               if(language==1):
                                    play_story(story_64_b_ru)  
                               if(language==2):
                                    play_story(story_64_b_en)
                               if(language==3):
                                    play_story(story_64_b_ar)          
                          print(redClickSintchEnemyCatchCount)
                     if flag=="enemy_catch2":
                          play_effect(enemy_catch2)
                          redClickSintchEnemyCatchCount += 1
                          if redClickSintchEnemyCatchCount == 1:
                               if(language==1):
                                    play_story(story_64_a_ru)  
                               if(language==2):
                                    play_story(story_64_a_en)
                               if(language==3):
                                    play_story(story_64_a_ar)
                          if redClickSintchEnemyCatchCount == 2:
                               redClickSintchEnemyCatchCount = 0
                               if(language==1):
                                    play_story(story_64_b_ru)  
                               if(language==2):
                                    play_story(story_64_b_en)
                               if(language==3):
                                    play_story(story_64_b_ar)
                     if flag=="enemy_catch3":
                          play_effect(enemy_catch3)
                          redClickSintchEnemyCatchCount += 1
                          if redClickSintchEnemyCatchCount == 1:
                               redClickSintchEnemyCatchCount = 0
                               if(language==1):
                                    play_story(story_64_a_ru)  
                               if(language==2):
                                    play_story(story_64_a_en)
                               if(language==3):
                                    play_story(story_64_a_ar)
                          if redClickSintchEnemyCatchCount == 2:
                               if(language==1):
                                    play_story(story_64_b_ru)  
                               if(language==2):
                                    play_story(story_64_b_en)
                               if(language==3):
                                    play_story(story_64_b_ar)
                     if flag=="enemy_catch4":
                          play_effect(enemy_catch4)
                          redClickSintchEnemyCatchCount += 1
                          if redClickSintchEnemyCatchCount == 1:
                               if(language==1):
                                    play_story(story_64_a_ru)  
                               if(language==2):
                                    play_story(story_64_a_en)
                               if(language==3):
                                    play_story(story_64_a_ar)
                          if redClickSintchEnemyCatchCount == 2:
                               redClickSintchEnemyCatchCount =0
                               if(language==1):
                                    play_story(story_64_b_ru)  
                               if(language==2):
                                    play_story(story_64_b_en)
                               if(language==3):
                                    play_story(story_64_b_ar)
                     if flag=="win":
                          play_background_music("fon19.mp3", loops=-1)    
                          if(language==1): play_story(story_66_ru)  
                          if(language==2): play_story(story_66_en)
                          if(language==3): play_story(story_66_ar)

                          # Отправляем команды
                          socketio.emit('level', 'win_player',to=None)
                          socklist.append('win_player')
                          
                          # [FIX] Сначала ставим в очередь, потом сразу пытаемся отправить
                          serial_write_queue.put('basket') 
                          process_serial_queue() # <-- ПРИНУДИТЕЛЬНАЯ ОТПРАВКА

                          send_esp32_command(ESP32_API_WOLF_URL, "firework")
                          send_esp32_command(ESP32_API_TRAIN_URL, "firework")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "firework")
                          send_esp32_command(ESP32_API_SAFE_URL, "firework")
                          
                          # Звуки голов
                          play_effect(random.choice(player_goal_sounds))
                          
                          # [FIX] Цикл ожидания с обработкой очереди
                          start_wait = time.time()
                          while effects_are_busy() and go == 1:
                              process_serial_queue() # <-- ПРОДОЛЖАЕМ ОТПРАВЛЯТЬ
                              eventlet.sleep(0.1)
                              # Защита от вечного цикла (макс 3 сек на звук)
                              if time.time() - start_wait > 3: break 
                          
                          play_effect(random.choice(player_goal_sounds))
                          
                          start_wait = time.time()
                          while effects_are_busy() and go == 1: 
                              process_serial_queue() # <-- ПРОДОЛЖАЕМ ОТПРАВЛЯТЬ
                              eventlet.sleep(0.1)
                              if time.time() - start_wait > 3: break

                          play_effect(win)
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
                              if(language==1):
                                  play_story(story_67_ru)  
                              if(language==2):
                                  play_story(story_67_en)
                              if(language==3):
                                  play_story(story_67_ar)
                              # ---------------------------------------------
                    #-------прошли игру с кристалами
                     if flag=="memory_room_end":
                         #----отправили на клиента
                         send_esp32_command(ESP32_API_TRAIN_URL, "stage_0") 
                         socketio.emit('level', 'memory_room_end',to=None)
                         #----добавили в историю
                         socklist.append('memory_room_end')
                         #------играем эффект
                         play_effect(brain_end)
                         #-----активируем последнюю игру
                         #socketio.emit('level', 'active_basket',to=None)
                         #socklist.append('active_basket') 
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
                     if flag=="hint_2_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_2_b_ru)
                         if(language==2):
                             play_story(hint_2_b_en)
                         if(language==3):
                             play_story(hint_2_b_ar)
                     if flag=="hint_2_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_2_c_ru)
                         if(language==2):
                             play_story(hint_2_c_en)
                         if(language==3):
                             play_story(hint_2_c_ar)
                     if flag=="hint_2_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_2_z_ru)
                         if(language==2):
                             play_story(hint_2_z_en)
                         if(language==3):
                             play_story(hint_2_z_ar)
                     if flag=="hint_3_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_3_b_ru)
                         if(language==2):
                             play_story(hint_3_b_en)
                         if(language==3):
                             play_story(hint_3_b_ar)
                     if flag=="hint_3_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_3_c_ru)
                         if(language==2):
                             play_story(hint_3_c_en)
                         if(language==3):
                             play_story(hint_3_c_ar)
                     if flag=="hint_3_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_3_z_ru)
                         if(language==2):
                             play_story(hint_3_z_en)
                         if(language==3):
                             play_story(hint_3_z_ar)
                     if flag=="hint_5_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_5_b_ru)
                         if(language==2):
                             play_story(hint_5_b_en)
                         if(language==3):
                             play_story(hint_5_b_ar)
                     if flag=="hint_5_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_5_c_ru)
                         if(language==2):
                             play_story(hint_5_c_en)
                         if(language==3):
                             play_story(hint_5_c_ar)
                     if flag=="hint_11_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_11_b_ru)
                         if(language==2):
                             play_story(hint_11_b_en)
                         if(language==3):
                             play_story(hint_11_b_ar)
                     if flag=="hint_11_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_11_c_ru)
                         if(language==2):
                             play_story(hint_11_c_en)
                         if(language==3):
                             play_story(hint_11_c_ar)
                     if flag=="hint_11_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_11_z_ru)
                         if(language==2):
                             play_story(hint_11_z_en)
                         if(language==3):
                             play_story(hint_11_z_ar)
                     if flag=="hint_6_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_6_b_ru)
                         if(language==2):
                             play_story(hint_6_b_en)
                         if(language==3):
                             play_story(hint_6_b_ar)
                     if flag=="hint_6_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_6_c_ru)
                         if(language==2):
                             play_story(hint_6_c_en)
                         if(language==3):
                             play_story(hint_6_c_ar)
                     if flag=="hint_10_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_10_b_ru)
                         if(language==2):
                             play_story(hint_10_b_en)
                         if(language==3):
                             play_story(hint_10_b_ar)
                     if flag=="hint_10_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_10_c_ru)
                         if(language==2):
                             play_story(hint_10_c_en)
                         if(language==3):
                             play_story(hint_10_c_ar)
                     if flag=="hint_14_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_14_b_ru)
                         if(language==2):
                             play_story(hint_14_b_en)
                         if(language==3):
                             play_story(hint_14_b_ar)
                     if flag=="hint_14_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_14_c_ru)
                         if(language==2):
                             play_story(hint_14_c_en)
                         if(language==3):
                             play_story(hint_14_c_ar)
                     if flag=="hint_14_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_14_z_ru)
                         if(language==2):
                             play_story(hint_14_z_en)
                         if(language==3):
                             play_story(hint_14_z_ar)
                     if flag=="hint_17_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_17_b_ru)
                         if(language==2):
                             play_story(hint_17_b_en)
                         if(language==3):
                             play_story(hint_17_b_ar)
                     if flag=="hint_17_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_17_c_ru)
                         if(language==2):
                             play_story(hint_17_c_en)
                         if(language==3):
                             play_story(hint_17_c_ar)
                     if flag=="hint_17_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_17_z_ru)
                         if(language==2):
                             play_story(hint_17_z_en)
                         if(language==3):
                             play_story(hint_17_z_ar)
                     if flag=="hint_19_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_19_b_ru)
                         if(language==2):
                             play_story(hint_19_b_en)
                         if(language==3):
                             play_story(hint_19_b_ar)
                     if flag=="hint_19_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_19_c_ru)
                         if(language==2):
                             play_story(hint_19_c_en)
                         if(language==3):
                             play_story(hint_19_c_ar)
                     if flag=="hint_19_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_19_z_ru)
                         if(language==2):
                             play_story(hint_19_z_en)
                         if(language==3):
                             play_story(hint_19_z_ar)
                     if flag=="hint_23_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_23_b_ru)
                         if(language==2):
                             play_story(hint_23_b_en)
                         if(language==3):
                             play_story(hint_23_b_ar)
                     if flag=="hint_23_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_23_c_ru)
                         if(language==2):
                             play_story(hint_23_c_en)
                         if(language==3):
                             play_story(hint_23_c_ar)
                     if flag=="hint_23_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_23_z_ru)
                         if(language==2):
                             play_story(hint_23_z_en)
                         if(language==3):
                             play_story(hint_23_z_ar)
                     if flag=="hint_26_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_26_b_ru)
                         if(language==2):
                             play_story(hint_26_b_en)
                         if(language==3):
                             play_story(hint_26_b_ar)
                     if flag=="hint_26_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_26_c_ru)
                         if(language==2):
                             play_story(hint_26_c_en)
                         if(language==3):
                             play_story(hint_26_c_ar)
                     if flag=="hint_26_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_26_z_ru)
                         if(language==2):
                             play_story(hint_26_z_en)
                         if(language==3):
                             play_story(hint_26_z_ar)
                     if flag=="hint_32_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_32_b_ru)
                         if(language==2):
                             play_story(hint_32_b_en)
                         if(language==3):
                             play_story(hint_32_b_ar)
                     if flag=="hint_32_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_32_c_ru)
                         if(language==2):
                             play_story(hint_32_c_en)
                         if(language==3):
                             play_story(hint_32_c_ar)
                     if flag=="hint_32_d":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_32_d_ru)
                         if(language==2):
                             play_story(hint_32_d_en)
                         if(language==3):
                             play_story(hint_32_d_ar)
                     if flag=="hint_32_e":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_32_e_ru)
                         if(language==2):
                             play_story(hint_32_e_en)
                         if(language==3):
                             play_story(hint_32_e_ar)
                     if flag=="hint_32_z":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_32_z_ru)
                         if(language==2):
                             play_story(hint_32_z_en)
                         if(language==3):
                             play_story(hint_32_z_ar)
                     if flag=="hint_37_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_37_b_ru)
                         if(language==2):
                             play_story(hint_37_b_en)
                         if(language==3):
                             play_story(hint_37_b_ar)
                     if flag=="hint_37_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_37_c_ru)
                         if(language==2):
                             play_story(hint_37_c_en)
                         if(language==3):
                             play_story(hint_37_c_ar)
                     if flag=="hint_38_b":
                         hintCount+=1
                         while channel3.get_busy()==True and go == 1: 
                             eventlet.sleep(0.1)
                         
                         if(language==1):
                             play_story(hint_38_b_ru)
                         if(language==2):
                             play_story(hint_38_b_en)
                         if(language==3):
                             play_story(hint_38_b_ar)
                     if flag=="hint_44_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_44_b_ru)
                         if(language==2):
                             play_story(hint_44_b_en)
                         if(language==3):
                             play_story(hint_44_b_ar)
                     if flag=="hint_44_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_44_c_ru)
                         if(language==2):
                             play_story(hint_44_c_en)
                         if(language==3):
                             play_story(hint_44_c_ar)
                     if flag=="hint_49_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_49_b_ru)
                         if(language==2):
                             play_story(hint_49_b_en)
                         if(language==3):
                             play_story(hint_49_b_ar)
                     if flag=="hint_49_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_49_c_ru)
                         if(language==2):
                             play_story(hint_49_c_en)
                         if(language==3):
                             play_story(hint_49_c_ar)
                     if flag=="hint_50_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_50_b_ru)
                         if(language==2):
                             play_story(hint_50_b_en)
                         if(language==3):
                             play_story(hint_50_b_ar)
                     if flag=="hint_50_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_50_c_ru)
                         if(language==2):
                             play_story(hint_50_c_en)
                         if(language==3):
                             play_story(hint_50_c_ar)
                     if flag=="hint_51_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_51_b_ru)
                         if(language==2):
                             play_story(hint_51_b_en)
                         if(language==3):
                             play_story(hint_51_b_ar)
                     if flag=="hint_51_c":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_51_c_ru)
                         if(language==2):
                             play_story(hint_51_c_en)
                         if(language==3):
                             play_story(hint_51_c_ar)
                     if flag=="hint_56_b":
                         hintCount+=1
                         if(language==1):
                             play_story(hint_56_b_ru)
                         if(language==2):
                             play_story(hint_56_b_en)
                         if(language==3):
                             play_story(hint_56_b_ar)
  
   
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
        logging.info(f'SENT {tag}: {description} (RAW: {message_to_send})')
        ser.write(str.encode(message_to_send + '\n'))
        eventlet.sleep(0.05)
    except eventlet.queue.Empty:
        pass

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
        logger.info("Starting background tasks...")
        socketio.start_background_task(target=serial)
        socketio.start_background_task(target=timer)
        logger.info("Starting Flask-SocketIO server on http://0.0.0.0:3000")
        socketio.run(app, port=3000, host='0.0.0.0')
    except OSError as e:
        logger.critical(f"Could not start server. Error: {e}")
        logger.critical("HINT: The port 3000 might be in use by another application.")
    except Exception as e:
        logger.critical(f"An unexpected error occurred: {e}")

