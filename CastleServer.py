import eventlet
eventlet.monkey_patch()
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
#библиотека для работы с системой много крутых фишек тоже почитай
from subprocess import call
import os
import requests
from requests.exceptions import RequestException
# ИЗМЕНЕНИЕ: Добавляем очередь для безопасной работы с serial портом
import eventlet.queue
serial_write_queue = eventlet.queue.Queue()
# КОНЕЦ ИЗМЕНЕНИЯ
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
last_story_55_play_time = 0
is_processing_ready = False
log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)
pygame.mixer.pre_init(44100, -16, 2, 2048)
pygame.init()
pygame.mixer.init()
#------инициализация звуковых каналов
channel1 = pygame.mixer.Channel(0) 
channel2 = pygame.mixer.Channel(1)
channel3 = pygame.mixer.Channel(2)

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

story_45_en = pygame.mixer.Sound("story_45_en.wav")
story_45_ar = pygame.mixer.Sound("story_45_ar.wav")
story_45_ru = pygame.mixer.Sound("story_45_ru.wav")

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

#---------читаем файлы записываем в переменные 
f1 = open('1.txt','r')
a1=f1.read(4)
f1.close() 
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

# ИЗМЕНЕНИЕ: Убираем автопоиск и жёстко прописываем порт Arduino
try:
    # Указываем порт, найденный через утилиту serial.tools.list_ports
    ARDUINO_PORT = '/dev/ttyUSB0' 
    ser = serial.Serial(ARDUINO_PORT, 9600, timeout=1)
    print(f"INFO: Successfully connected to Arduino on port {ARDUINO_PORT}")
except serial.SerialException as e:
    print(f"FATAL: Could not open serial port {ARDUINO_PORT}. Error: {e}")
    print("HINT: Check connection and port name. Make sure you have permissions (sudo usermod -a -G dialout pi).")
    exit() # Завершаем работу, если не удалось подключиться
# КОНЕЦ ИЗМЕНЕНИЯ

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
    current_client_sid = request.sid
    socketio.emit('volume', str(phoneLevel))
    socketio.emit('volume1', str(effectLevel))
    socketio.emit('volume2', str(voiceLevel))

@socketio.on('disconnect')
def handle_disconnect():
    global current_client_sid
    if request.sid == current_client_sid:
        current_client_sid = None

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
     channel2.set_volume(float(eff),float(eff)) 
     socketio.emit('volume1', eff, to=None, include_self=False)
     
     #time.sleep(0)

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
             call("sudo shutdown -h now", shell=True) 
     if check == 'open_stash':
             serial_write_queue.put('open_stash')
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
             time.sleep(3)
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
             time.sleep(3) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_open_mansard_door',to=None)
             socklist.append('active_open_mansard_door')
        if check=='open_mansard_door':
             #-----отправка клиенту 
             socketio.emit('level', 'open_mansard_door',to=None)
             #-----добавить в историю
             socklist.append('galet_on')
             #----отправить на мегу
             serial_write_queue.put('open_mansard_door')
             name = "story_2"  
             #---ждем 3 секунды
             time.sleep(3) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_suitcase',to=None)
             socklist.append('active_suitcase')
             socketio.emit('level', 'active_animals',to=None)
             socklist.append('active_animals')  
             socketio.emit('level', 'active_wolf',to=None)
             socklist.append('active_wolf')   
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

             socketio.emit('level', 'active_owl',to=None)
             socklist.append('active_owl')
             socketio.emit('level', 'active_pedlock',to=None)
             socklist.append('active_pedlock')
             socketio.emit('level', 'active_cat',to=None)
             socklist.append('active_cat')
             socketio.emit('level', 'active_projector',to=None)
             socklist.append('active_projector')        
        if check == 'open_bank_door':
             #-----отправка клиенту 
             socketio.emit('level', 'open_bank_door',to=None)
             #-----добавить в историю
             socklist.append('open_bank_door')
             #----отправить на мегу
             serial_write_queue.put('open_bank_door')
             name = "story_2"     
             time.sleep(3) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_safe',to=None)
             socklist.append('active_safe')
        if check == 'pedlock':
             #-----отправка клиенту 
             socketio.emit('level', 'pedlock',to=None)
             #-----добавить в историю
             socklist.append('pedlock')
             #----отправить на мегу
             serial_write_queue.put('pedlock')
             name = "story_2"     
             time.sleep(3) 
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
             name = "story_2"
        if check == 'cat':
             #-----отправка клиенту 
             socketio.emit('level', 'cat',to=None)
             #-----добавить в историю
             socklist.append('cat')
             #----отправить на мегу
             serial_write_queue.put('cat')
             # --- НАЧАЛО ИЗМЕНЕНИЙ: Имитируем реакцию сервера на door_witch ---
             play_effect(door_witch) # 1. Воспроизводим звук открытия
             send_esp32_command(ESP32_API_TRAIN_URL, "fish_open") # 2. Гасим LED рыбы (24) на карте

             # 3. Воспроизводим историю
             if(language==1):
                 play_story(story_17_ru) #
             if(language==2):
                 play_story(story_17_en) #
             if(language==3):
                 play_story(story_17_ar) #

             # 4. Активируем следующий этап
             socketio.emit('level', 'active_open_potions_stash',to=None) #
             socklist.append('active_open_potions_stash') #
             # --- КОНЕЦ ИЗМЕНЕНИЙ ---

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
             time.sleep(3) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_owls',to=None)
             socklist.append('active_owls')
        if check == 'owls':
             #-----отправка клиенту 
             socketio.emit('level', 'owls',to=None)
             #-----добавить в историю
             socklist.append('owls')
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
        if check == 'projector':
             #-----отправка клиенту 
             socketio.emit('level', 'projector',to=None)
             #-----добавить в историю
             socklist.append('projector')
             #----отправить на мегу
             send_esp32_command(ESP32_API_TRAIN_URL, "projector")
             name = "story_2"     
             time.sleep(3) 
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
             socketio.emit('level', 'active_troll',to=None)
             socklist.append('active_troll')
        if check == 'troll':
             #-----отправка клиенту 
             socketio.emit('level', 'troll',to=None)
             #-----добавить в историю
             socklist.append('troll')
             #----отправить на мегу
             serial_write_queue.put('troll')
             name = "story_2" 
             socketio.emit('level', 'active_open_bank_door',to=None)
             socklist.append('active_open_bank_door')                        

        if check == 'safe':
             #-----отправка клиенту 
             socketio.emit('level', 'safe',to=None)
             #-----добавить в историю
             socklist.append('safe')
             #----отправить на мегу
             serial_write_queue.put('safe')
             name = "story_2"     
             time.sleep(5) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_workshop',to=None)
             socklist.append('active_workshop')
        if check == 'workshop':
             #-----отправка клиенту 
             socketio.emit('level', 'workshop',to=None)
             #-----добавить в историю
             socklist.append('workshop')
             #----отправить на мегу
             serial_write_queue.put('workshop')
             name = "story_2"     
             time.sleep(5) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_ghost',to=None)
             socklist.append('active_ghost')
        if check == 'ghost':
             #-----отправка клиенту 
             socketio.emit('level', 'ghost',to=None)
             #-----добавить в историю
             socklist.append('ghost')
             #----отправить на мегу
             serial_write_queue.put('ghost_skip')
             name = "story_2" 
        if check == 'cup':
             #-----отправка клиенту 
             socketio.emit('level', 'cup',to=None)
             #-----добавить в историю
             socklist.append('cup')
             #----отправить на мегу
             serial_write_queue.put('cup')
             name = "story_2"
             time.sleep(5) 
             #-----активируем блок с флагами
             socketio.emit('level', 'active_spell',to=None)
             socklist.append('active_spell')  
        if check == 'spell':
             #-----отправка клиенту 
             socketio.emit('level', 'spell',to=None)
             #-----добавить в историю
             socklist.append('spell')
             #----отправить на мегу
             serial_write_queue.put('spell')
             name = "story_2"
             time.sleep(5) 
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
             send_esp32_command(ESP32_API_TRAIN_URL, "stage_12") 
             #-----отправка клиенту 
             socketio.emit('level', 'first_level',to=None)
             #-----добавить в историю
             socklist.append('first_level')
             socketio.emit('level', 'second_level',to=None)
             #-----добавить в историю
             socklist.append('second_level')
             socketio.emit('level', 'third_level',to=None)
             #-----добавить в историю
             socklist.append('third_level')
             #----отправить на мегу
             serial_write_queue.put('memory_room_end')
                         
        if check == 'basket':
             #-----отправка клиенту 
             socketio.emit('level', 'basket',to=None)
             #-----добавить в историю
             socklist.append('basket')
             #----отправить на мегу
             serial_write_queue.put('basket')
             send_esp32_command(ESP32_API_WOLF_URL, "firework")
             send_esp32_command(ESP32_API_TRAIN_URL, "firework")
             send_esp32_command(ESP32_API_SUITCASE_URL, "firework")
             send_esp32_command(ESP32_API_SAFE_URL, "firework")
                 
        
     #------- обработка в режиме рестарта   
     elif go == 2: # --- ИЗМЕНЕНИЕ: Используем elif вместо if для режима рестарта ---
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
        if check=='ghost': 
             serial_write_queue.put('open_library_door')
        if check=='cup':
             serial_write_queue.put('open_high_tower_door')
        if check=='spell':
             serial_write_queue.put('open_low_tower_door')
        if check=='crystals':
             serial_write_queue.put('open_memory_door')
        if check=='basket':
             serial_write_queue.put('open_basket_door')


@app.route('/api', methods=['GET', 'POST'])
def handle_data():
    global mapClickHints
    global mapClickOut 
    if request.method == 'POST':
        data = request.get_json()
        print("Received from client:", data)
        if 'suitcase' in data and data['suitcase'] == 'end':
          print("suitcase")
          send_esp32_command(ESP32_API_TRAIN_URL, "case_finish")
          serial_write_queue.put('suitcase_end')
          socketio.emit('level', 'suitcase',to=None)
          socklist.append('suitcase')
          send_esp32_command(ESP32_API_SUITCASE_URL, "confirm_suitcase_end")
        if 'safe' in data and data['safe'] == 'end':
          print("safe")
          send_esp32_command(ESP32_API_TRAIN_URL, "safe_finish")
          serial_write_queue.put('safe_end')
          socketio.emit('level', 'animals',to=None)
          socklist.append('animals')
          send_esp32_command(ESP32_API_SAFE_URL, "confirm_safe_end")
        if 'wolf' in data and data['wolf'] == 'end':
          print("wolf")
          send_esp32_command(ESP32_API_TRAIN_URL, "wolf_finish")
          serial_write_queue.put('wolf_end')
          socketio.emit('level', 'wolf',to=None)
          socklist.append('wolf')
          send_esp32_command(ESP32_API_WOLF_URL, "confirm_wolf_end")
         
        # --- ИЗМЕНЕНИЕ: Добавляем логику для TRAIN (projector end) ---
        elif 'projector' in data and data['projector'] == 'end':
            print("projector")
            socketio.emit('level', 'projector',to=None)
            socklist.append('projector')
            # Активируем следующий этап (игра с поездом)
            socketio.emit('level', 'active_train',to=None)
            socklist.append('active_train')
        # --- КОНЕЦ ИЗМЕНЕНИЯ ---

        if 'map' in data and data['map'] == 'owl':
          print("owl")
          serial_write_queue.put('owl')
          time.sleep(1.0)
          play_effect(map_click)
          #while channel2.get_busy()==True: 
          #     time.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints += 1
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          if mapClickHints == 1:
               mapClickHints = -1
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)
          mapClickHints += 1

        if 'map' in data and data['map'] == 'fish':
          print("fish")
          serial_write_queue.put('fish')
          time.sleep(1.0)
          play_effect(map_click)
          #while channel2.get_busy()==True: 
          #     time.sleep(0.1) 
          if mapClickHints == 0:
               mapClickHints += 1
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          if mapClickHints == 1:
               mapClickHints = -1
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)
          mapClickHints += 1

        if 'map' in data and data['map'] == 'key':
          print("key")
          serial_write_queue.put('key')
          time.sleep(1.0)
          play_effect(map_click)
          #while channel2.get_busy()==True: 
          #     time.sleep(0.1) 
          if mapClickHints == 0:
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          if mapClickHints == 1:
               mapClickHints = -1
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar)
          mapClickHints += 1

        if 'map' in data and data['map'] == 'train':
          print("train")
          serial_write_queue.put('train')
          time.sleep(1.0) 
          play_effect(map_click)
          #while channel2.get_busy()==True: 
          #     time.sleep(0.1) 
          if mapClickHints == 0:
               if(language==1):
                   play_story(story_12_a_ru)  
               if(language==2):
                   play_story(story_12_a_en)
               if(language==3):
                   play_story(story_12_a_ar)
          if mapClickHints == 1:
               mapClickHints = -1
               if(language==1):
                   play_story(story_12_b_ru)  
               if(language==2):
                   play_story(story_12_b_en)
               if(language==3):
                   play_story(story_12_b_ar) 
          mapClickHints += 1          

        if 'train' in data and data['train'] == 'end':
            print("train_end")
            serial_write_queue.put('train_end')
            socketio.emit('level', 'train',to=None)
            socklist.append('train')
            socketio.emit('level', 'active_mine',to=None)
            socklist.append('active_mine')
            # Отправляем подтверждение обратно на ESP32, чтобы он перестал слать запросы
            send_esp32_command(ESP32_API_TRAIN_URL, "confirm_train_end")

        if 'projector' in data and data['projector'] == 'end':
            print("projector")
            socketio.emit('level', 'projector',to=None)
            socklist.append('projector')
            socketio.emit('level', 'active_train',to=None)
            socklist.append('active_train')

        if 'train' in data and data['train'] == 'skin':
            print("skin")
            serial_write_queue.put('skin')
            play_effect(item_find)    
  
        if 'map' in data and data['map'] == 'out':
          print("out")
          serial_write_queue.put('out')
          play_effect(map_out)
          while channel2.get_busy()==True: 
             time.sleep(0.1) 
          if mapClickOut == 0:
               if(language==1):
                   play_story(story_12_c_ru)  
               if(language==2):
                   play_story(story_12_c_en)
               if(language==3):
                   play_story(story_12_c_ar)
          if mapClickOut == 1:
               mapClickOut = -1
               if(language==1):
                   play_story(story_12_d_ru)  
               if(language==2):
                   play_story(story_12_d_en)
               if(language==3):
                   play_story(story_12_d_ar) 
          mapClickOut += 1

        if 'ghost' in data and data['ghost'] == 'end':
            print("ghost_map")
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
     global is_processing_ready
     print(res)
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
               channel2.pause() 
               go=-1
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
              channel2.unpause() 
              #----отправим на клиента
              socketio.emit('level', 'start_game',to=None)  
        #----если была в рестарте       
        if go == 3 and starts==3:
             print("start")
             #----очищаем историю 
             socklist.clear() 
             serial_write_queue.put('start')
             go=1
             starts = 1
        
     #----нажали на рестарт   
     if res =='restart':
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
         socketio.emit('level', 'rest',to=None)
         socklist.clear()
         socklist.append('rest')
         socklist.append('50')
         flag=""
         go=2
         channel3.stop() 
         channel2.stop() 
         pygame.mixer.music.stop()
         #pygame.mixer.music.unload()
         
         #ser.write('restart')
         go=2
          
     #----если нажало на демо    
     if res =='ready':
         if is_processing_ready:
             print("INFO: 'Ready' command is already being processed.")
             return # Выходим, если команда уже в обработке

         is_processing_ready = True
         socketio.emit('level', 'ready_processing') # Команда для UI, чтобы заблокировать кнопку
          #-----перейдет только если был в рестарте или просто запущен
         print("ready")
         if go == 2 or go == 0:
               if test_esp32() == True:
                    print("ready")
                    socklist.clear()
                    devices.clear()
                    serial_write_queue.put('ready')
                    send_esp32_command(ESP32_API_WOLF_URL, "ready")
                    send_esp32_command(ESP32_API_TRAIN_URL, "ready")
                    send_esp32_command(ESP32_API_SUITCASE_URL, "ready")
                    send_esp32_command(ESP32_API_SAFE_URL, "ready")
                    #----отправим на клиента
                    socketio.emit('level', 'ready',to=None)
                    go=3 
                    #---очистим список  
                    
                    #-----добавим демо
                    socklist.append('ready')
                    #----обновим значение флага
                    starts = 3
                    #----остановим музыку на всякий
                    channel3.stop() 
                    channel2.stop() 
                    pygame.mixer.music.stop()
                    play_background_music("fon1.mp3", loops=-1)
               else:
                    socklist.clear()
                    socketio.emit('level', 'start_error',to=None)
                    socklist.append('start_error')
                    final_string = ', '.join(str(device) for device in devices)
                    socklist.append(final_string)
                    socketio.emit('devices', final_string,to=None)
                    
         # --- НАЧАЛО ИЗМЕНЕНИЯ: Снимаем блокировку в конце ---
         socketio.emit('level', 'ready_finished') # Команда для UI, чтобы разблокировать кнопку
         is_processing_ready = False
         # --- КОНЕЦ ИЗМЕНЕНИЯ ---
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
     for i in socklist:
         if current_client_sid:
            socketio.emit('volume', str(phoneLevel), skip_sid=current_client_sid)
            socketio.emit('volume1', str(effectLevel), skip_sid=current_client_sid)
            socketio.emit('volume2', str(voiceLevel), skip_sid=current_client_sid)
         else:
            socketio.emit('volume', str(phoneLevel))
            socketio.emit('volume1', str(effectLevel))
            socketio.emit('volume2', str(voiceLevel))
         socketio.emit('level', i ,to=None)
         final_string = ', '.join(str(device) for device in devices)
         socketio.emit('devices', final_string,to=None)            
     
     
          
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
     # Получаем соответствующий аудиофайл
     if audio_file:
         channel3.play(audio_file, loops=loops)
         
         # Устанавливаем громкость
         with open(volume_file, 'r') as f:
             volume = float(f.read(4))
             channel3.set_volume(volume, volume)

def play_effect(audio_file, loops=0, volume_file='2.txt'):
    channel2.play(audio_file, loops=loops)
    
    # Читаем громкость из файла и устанавливаем
    with open(volume_file, 'r') as f:
        volume = float(f.read(4))
        channel2.set_volume(volume, volume)

def send_esp32_command(api_url, command, timeout=6, max_retries=4, retry_delay=1, async_mode=True):

    def _send_command():
        current_delay = retry_delay
        for attempt in range(max_retries):
            try:
                response = requests.post(api_url, json=command, timeout=timeout)
                response.raise_for_status()
                print(f"Команда {command} выполнена успешно")
                return response
            except RequestException as e:
                print(f"Попытка {attempt + 1}/{max_retries} не удалась: {e}")
                if attempt < max_retries - 1:
                    print(f"Повтор через {current_delay} секунд...")
                    time.sleep(current_delay)
                    current_delay *= 2
                else:
                    print(f"Все попытки отправки команды {command} на {api_url} не удались")
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
        except RequestException as e:
            devices.append(device_name)

    # Результат
    
    print(f"Итог: {success_count}/4 устройств доступно")
    print(devices)
    final_string = ', '.join(str(device) for device in devices)
    print(final_string)
    socketio.emit('devices',final_string ,to=None)
    if success_count == 4 and len(devices) == 0:
        socklist.clear()
        devices.clear()
    return success_count == 4 and len(devices) == 0 
                
def play_background_music(music_file, volume_file='1.txt', loops=-1):
    try:
        # Загружаем и воспроизводим музыку
        pygame.mixer.music.load(music_file)
        pygame.mixer.music.play(loops)
        
        # Устанавливаем громкость из файла
        with open(volume_file, 'r') as f:
            volume = float(f.read(4))
            pygame.mixer.music.set_volume(volume)
            
    except FileNotFoundError:
        print(f"Ошибка: файл {music_file} или {volume_file} не найден")
    except Exception as e:
        print(f"Ошибка при воспроизведении музыки: {e}")

#здесь уже обрабатываем все сообщения приходящие из меги и отображаем на пульте        
def serial():
     global flag 
     global mus
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
     global caveCounter
     global storyBasketFlag
     global catchCount
     global enemyCatchCount
     global sintchEnemyCatchCount
     global redSintchEnemyCatchCount
     global redClickSintchEnemyCatchCount
     global story13Flag 
     global fire0Flag
     global fire1Flag
     global fire2Flag
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
          # ИЗМЕНЕНИЕ: Добавляем блок для отправки сообщений из очереди
          try:
              message_to_send = serial_write_queue.get_nowait()
              ser.write(str.encode(message_to_send + '\n'))
          except eventlet.queue.Empty:
              pass # Если очередь пуста, ничего не делаем
          # КОНЕЦ ИЗМЕНЕНИЯ
          #---- иногда для ассинхрона нужно добавлять time.sleep(0)для переключения на другой метод
          time.sleep(0)
          if pygame.mixer.music.get_busy() == False:
               if nextTrack == 1:
                    play_background_music("fon8.mp3")
                    if(language==1):
                        play_story(story_11_ru)  
                    if(language==2):
                        play_story(story_11_en)
                    if(language==3):
                        play_story(story_11_ar)
                    nextTrack = 0

          if channel3.get_busy()==True:#------если канал с голосом играет
               a10 = phoneLevel
               if flagS == 0:#- если флаг равен 0
                   time.sleep(1)
                   serial_write_queue.put('soundon')#-----отправляем в сериал и череп перестает моргать если трек воспроизводится
                   flagS = 1 #--поднмаем флаг
                   time.sleep(1)
               while a10>0.01  and fs==0: # до тех пор пока звук не погаснет до значения 0.01 удавляем по 0.01
                    #a10=(a10*10-1)/10
                    a10 = round(a10,2)-round(0.01,2)
                    pygame.mixer.music.set_volume(round(a10,2))
                    time.sleep(0.05)
               fs = 1   
               a20 = 0.00  
               #после этого поднимаем флаг что бы прибавить до того состояния которое было записано в файле
          else :
               if flagS == 1:
                   if starts==1:
                        time.sleep(1) #--- работает только после того как стартанем
                        serial_write_queue.put('soundoff')
                        time.sleep(1)# если воспроизведение закончено начинаем моргать
                        flagS = 0
               #-----так же плавно поднимаем громкость
               while a20 != phoneLevel and fs==1:
                    a20 = round(a20,2)+0.01
                    #---постоянно читаем файл что бы не было конфликта защита от дурака
                    f1 = open('1.txt','r')
                    a6=f1.read(4)
                    f1.close() 
                    phoneLevel = float(a6)
                    pygame.mixer.music.set_volume(round(a20,2))
                    if round(a20,2)==phoneLevel:
                         fs = 0
                    time.sleep(0.01)
               fs = 0
          # аналог serial.available() rsstrip игнорирует всякие переходы на другую строку и перевод каретки     
          if ser.in_waiting > 0:
               line = ser.readline().decode('utf-8', errors='ignore').rstrip()
               flag = line
               time.sleep(0.1)
               print(flag)
               for i in socklist:
                    #----- постоянно обновляем данные по громкости синхроним 
                    socketio.emit('volume', str(phoneLevel))
                    #     phoneLeveltmp = phoneLevel
                    #     print(phoneLevel)
                    ##time.sleep(0.01)
                    #if effectLevel is not effectLeveltmp:
                    # ----постоянно обновляем данные по громкости синхроним 
                    socketio.emit('volume1', str(effectLevel))
                    #     effectLeveltmp = effectLevel
                    ##time.sleep(0.01)
                    #if voiceLevel is not voiceLeveltmp:
                    # ----постоянно обновляем данные по громкости синхроним 
                    socketio.emit('volume2', str(voiceLevel))
                    socketio.emit('level', i ,to=None)
                    final_string = ', '.join(str(device) for device in devices)
                    socketio.emit('devices', final_string,to=None)
               #проверяем если пришло значение в виде цифры отправляем на метод на клиенте volt
               if is_number(flag):
                    socketio.emit('volt', flag,to=None)
               #сперва ждем этого сообщения если оно придет тогда смело начинаем квест обработка аналогичная как и наверху это сообщение придет если все двери будут закрыты 
               if flag == "startgo":
                     #-----очистим историю
                     socklist.clear()
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
                     send_esp32_command(ESP32_API_SUITCASE_URL, "start")
                     send_esp32_command(ESP32_API_SAFE_URL, "start")
                     #----отправим на клиента старт
                     socketio.emit('level', 'start_game',to=None)
                     #-----играем фон
                     play_background_music("fon2.mp3")
                     time.sleep(8.0)
                     #-----играем историю
                     if(language==1):
                         play_story(story_1_ru)  
                     if(language==2):
                         play_story(story_1_en)
                     if(language==3):
                         play_story(story_1_ar)
                     pygame.mixer.music.set_volume(0.1)    
                     #-----меняем значение переменной
                     name = "start_story_1"    
                     
                     while channel3.get_busy()==True: 
                              time.sleep(0.1)
                     send_esp32_command(ESP32_API_TRAIN_URL, "train_light_off")
                     with open('1.txt', 'r') as f:
                         volume = float(f.read(4))
                     pygame.mixer.music.set_volume(volume)             
                     if(language==1):
                         play_story(story_2_a_ru)  
                     if(language==2):
                         play_story(story_2_a_en)
                     if(language==3):
                         play_story(story_2_a_ar)         
               #---режим для событий в ресте показывает что нужно вернуть на свои места
               if starts == 2:
                     if flag=="flag1_on":
                          if 'flag1_off' in socklist:
                                   socklist.remove('flag1_off')
                          socketio.emit('level', 'flag1_on',to=None)
                          socklist.append('flag1_on')
                          if 'Check Flags' not in devices:
                            devices.append('Check Flags')
                     if flag=="flag1_off":
                          if 'Check Flags' in devices:
                                   devices.remove('Check Flags')
                          if 'flag1_on' in socklist:
                                   socklist.remove('flag1_on')
                          socketio.emit('level', 'flag1_off',to=None)
                          socklist.append('flag1_off')
                     if flag=="flag2_on":
                          if 'flag2_off' in socklist:
                                   socklist.remove('flag2_off')
                          socketio.emit('level', 'flag2_on',to=None)
                          socklist.append('flag2_on')
                          if 'Check Flags' not in devices:
                            devices.append('Check Flags')
                     if flag=="flag2_off":
                          if 'Check Flags' in devices:
                                   devices.remove('Check Flags')
                          if 'flag2_on' in socklist:
                                   socklist.remove('flag2_on')
                          socketio.emit('level', 'flag2_off',to=None)
                          socklist.append('flag2_off') 

                     if flag=="flag3_on":
                          if 'flag3_off' in socklist:
                                   socklist.remove('flag3_off')
                          socketio.emit('level', 'flag3_on',to=None)
                          socklist.append('flag3_on')
                          if 'Check Flags' not in devices:
                            devices.append('Check Flags')
                     if flag=="flag3_off":
                          if 'Check Flags' in devices:
                                   devices.remove('Check Flags')
                          if 'flag3_on' in socklist:
                                   socklist.remove('flag3_on')
                          socketio.emit('level', 'flag3_off',to=None)
                          socklist.append('flag3_off') 
                     if flag=="flag4_on":
                          if 'flag4_off' in socklist:
                                   socklist.remove('flag4_off')
                          socketio.emit('level', 'flag4_on',to=None)
                          socklist.append('flag4_on')
                          if 'Check Flags' not in devices:
                            devices.append('Check Flags')
                     if flag=="flag4_off":
                          if 'Check Flags' in devices:
                                   devices.remove('Check Flags')
                          if 'flag4_on' in socklist:
                                   socklist.remove('flag4_on')
                          socketio.emit('level', 'flag4_off',to=None)
                          socklist.append('flag4_off') 
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
                     if flag=="galet_on":
                          if 'Check Galet' not in devices:
                            devices.append('Check Galet')
                          if 'galet_off' in socklist:
                                   socklist.remove('galet_off')
                          socketio.emit('level', 'galet_on',to=None)
                          socklist.append('galet_on')
                     if flag=="galet_off":
                          if 'Check Galet' in devices:
                                   devices.remove('Check Galet')
                          if 'galet_on' in socklist:
                                   socklist.remove('galet_on')
                          socketio.emit('level', 'galet_off',to=None)
                          socklist.append('galet_off')  

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
                          play_background_music("fon3.mp3")
                          #----играем эффект
                          play_effect(door_act)

                          while channel2.get_busy()==True: 
                              time.sleep(0.1) 
                          #----играем историю    
                          if(language==1):
                              play_story(story_3_ru)  
                          if(language==2):
                              play_story(story_3_en)
                          if(language==3):
                              play_story(story_3_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.1) 
                          serial_write_queue.put('kay_repeat') 
                          time.sleep(1.1)     
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1) 
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
                          play_background_music("fon4.mp3")
                          send_esp32_command(ESP32_API_TRAIN_URL, "train_uf_light_on")
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_knock")
                          socketio.emit('level', 'second_clock', to=None)
                          #----добавляем в историю
                          socklist.append('second_clock')
                          play_effect(uf_clock)
                          #-----ждем окончания эффекта
                          while channel2.get_busy()==True: 
                              time.sleep(0.1) 
                          #----играем историю    
                          if(language==1):
                              play_story(story_3_b_ru)  
                          if(language==2):
                              play_story(story_3_b_en)
                          if(language==3):
                              play_story(story_3_b_ar)  

                          while channel3.get_busy()==True: 
                              time.sleep(0.1)  

                          serial_write_queue.put('after_story_clock2')
                          time.sleep(1.0)     
                          #-----изменяем переменную
                          name = "story_1"  
                          #-----активируем блок с галетниками
                          socketio.emit('level', 'active_open_mansard_door',to=None)
                          socklist.append('active_open_mansard_door') 
                     if flag == "steps":
                          #----играем эффект
                          play_effect(steps)
                          #-----ждем окончания эффекта
                          while channel2.get_busy()==True: 
                              time.sleep(0.1) 

                          if(language==1):
                              play_story(story_3_c_ru)  
                          if(language==2):
                              play_story(story_3_c_en)
                          if(language==3):
                              play_story(story_3_c_ar)

                          while channel3.get_busy()==True: 
                              time.sleep(0.1)         

                          serial_write_queue.put('student_hide')
                          time.sleep(1.0)

                          send_esp32_command(ESP32_API_TRAIN_URL, "train_uf_light_off")
                          send_esp32_command(ESP32_API_TRAIN_URL, "train_light_on")
                          time.sleep(1.0)
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_1")
                          play_background_music("fon5.mp3")
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
                     #----прошли галетники     
                     if flag=="galet_on":
                          send_esp32_command(ESP32_API_WOLF_URL, "game")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "game")
                          send_esp32_command(ESP32_API_SAFE_URL, "game")
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_2")
                          #----отправляем на клиента
                          socketio.emit('level', 'open_mansard_door',to=None)
                          #-----добавили в историю
                          socklist.append('open_mansard_door')
                          #-----играем эффект
                          play_effect(door_attic)
                          #-----играем фон
                          play_background_music("fon6.mp3")
                           #---ждем окончания эффекта
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          #----играем историю    
                          if(language==1):
                              play_story(story_5_ru)  
                          if(language==2):
                              play_story(story_5_en)
                          if(language==3):
                              play_story(story_5_ar)

                          while channel3.get_busy()==True: 
                              time.sleep(0.1)

                          if(language==1):
                              play_story(story_6_ru)  
                          if(language==2):
                              play_story(story_6_en)
                          if(language==3):
                              play_story(story_6_ar)  
                          #----меняем переменную хранящую последнюю историю
                          name = "story_2"  
                          #----активируем блок с флагами
                          socketio.emit('level', 'active_suitcase',to=None)
                          socklist.append('active_suitcase')
                          socketio.emit('level', 'active_animals',to=None)
                          socklist.append('active_animals')
                          socketio.emit('level', 'active_swolf',to=None)
                          socklist.append('active_swolf')
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          play_background_music("fon7.mp3", loops=0) 
                          if(language==1):
                              play_story(story_10_ru)  
                          if(language==2):
                              play_story(story_10_en)
                          if(language==3):
                              play_story(story_10_ar)
                          nextTrack = 1
                          #----активируем игру с пером
                          socketio.emit('level', 'active_pedlock',to=None)
                          socklist.append('active_pedlock')
                          socketio.emit('level', 'active_owl',to=None)
                          socklist.append('active_owl')
                          socketio.emit('level', 'active_cat',to=None)
                          socklist.append('active_cat')
                          socketio.emit('level', 'active_projector',to=None)
                          socklist.append('active_projector')

                     if flag=="door_owl":
                          #----играем эффект 
                          play_effect(door_owl)
                          socketio.emit('level', 'owl',to=None)
                          #-----добавили в историю
                          socklist.append('owl')
                          send_esp32_command(ESP32_API_TRAIN_URL, "owl_open")
                          send_esp32_command(ESP32_API_TRAIN_URL, "map_disable_clicks") # Отключаем клики
                          #while channel2.get_busy()==True: 
                              #time.sleep(0.1)
                          time.sleep(2.0)
                          if story13Flag == 0:
                               story13Flag = 1
                               if(language==1):
                                    play_story(story_13_ru)  
                               if(language==2):
                                    play_story(story_13_en)
                               if(language==3):
                                    play_story(story_13_ar)
 
                               while channel3.get_busy()==True: 
                                    time.sleep(0.1)     
                               
 
                          if(language==1):
                              play_story(story_14_a_ru)  
                          if(language==2):
                              play_story(story_14_a_en)
                          if(language==3):
                              play_story(story_14_a_ar)
                          while channel3.get_busy()==True: time.sleep(0.1) # Ждем завершения story_14_a
                          send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks") # Включаем клики обратно
                          #----активируем игру с совами
                          socketio.emit('level', 'active_owls',to=None)
                          socklist.append('active_owls')

                     if flag=="owl_flew":
                          #----играем эффект 
                          play_effect(owl_flew)
                          #----активируем игру с совами

                     if flag=="owl_end":
                          #----играем эффект 
                          play_effect(owl_flew)
                          socketio.emit('level', 'owls',to=None)
                          #-----добавили в историю
                          socklist.append('owls')
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
                          #while channel2.get_busy()==True: 
                          #    time.sleep(0.1)
                          time.sleep(2.0)
                          if story13Flag == 0:
                               story13Flag = 1
                               if(language==1):
                                    play_story(story_13_ru)  
                               if(language==2):
                                    play_story(story_13_en)
                               if(language==3):
                                    play_story(story_13_ar)
 
                               while channel3.get_busy()==True: 
                                    time.sleep(0.1)     

                          if(language==1):
                              play_story(story_17_ru)  
                          if(language==2):
                              play_story(story_17_en)
                          if(language==3):
                              play_story(story_17_ar)
                          while channel3.get_busy()==True: time.sleep(0.1) # Ждем завершения story_17
                          send_esp32_command(ESP32_API_TRAIN_URL, "map_enable_clicks") # Включаем клики обратно
                          #----активируем игру с пером
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

                          while channel2.get_busy()==True: 
                              time.sleep(0.1)

                          #------играем голос    
                          if(language==1):
                              play_story(story_18_ru)  
                          if(language==2):
                              play_story(story_18_en)
                          if(language==3):
                              play_story(story_18_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)    
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
                          # while channel2.get_busy()==True: 
                          #    time.sleep(0.1)
                          
                          # Добавляем фиксированную задержку 2 секунды
                          time.sleep(2.0)
 
                          if story13Flag == 0:
                               story13Flag = 1
                               if(language==1):
                                    play_story(story_13_ru)  
                               if(language==2):
                                    play_story(story_13_en)
                               if(language==3):
                                    play_story(story_13_ar)
 
                               while channel3.get_busy()==True: 
                                    time.sleep(0.1)     

                          if(language==1):
                              play_story(story_19_ru)  
                          if(language==2):
                              play_story(story_19_en)
                          if(language==3):
                              play_story(story_19_ar)
                          while channel3.get_busy()==True: time.sleep(0.1) # Ждем завершения story_19
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
                          #----играем эффект 
                          socketio.emit('level', 'dog',to=None)
                          #-----добавили в историю
                          socklist.append('dog')
                          send_esp32_command(ESP32_API_TRAIN_URL, "key_finish")
                          play_effect(dog_lock)

                          #while channel2.get_busy()==True: 
                          #    time.sleep(0.1)

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
                          if(language==1):
                              play_story(story_22_a_ru)  
                          if(language==2):
                              play_story(story_22_a_en)
                          if(language==3):
                              play_story(story_22_a_ar)  

                     if flag=="story_22_b":
                          if(language==1):
                              play_story(story_22_b_ru)  
                          if(language==2):
                              play_story(story_22_b_en)
                          if(language==3):
                              play_story(story_22_b_ar)                  
                       
                     if flag=="story_22_c":
                          if(language==1):
                              play_story(story_22_c_ru)  
                          if(language==2):
                              play_story(story_22_c_en)
                          if(language==3):
                              play_story(story_22_c_ar)     
                                                         
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
                          # while channel2.get_busy()==True:
                          #     time.sleep(0.1)
                          # Добавляем фиксированную задержку 2 секунды
                          time.sleep(2.0)
                          if(language==1):
                              play_story(story_26_ru)  
                          if(language==2):
                              play_story(story_26_en)
                          if(language==3):
                              play_story(story_26_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)    
                     if flag=="cave_search1":
                          #----играем эффект 
                          play_effect(cave_search)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_27_a_ru)  
                          if(language==2):
                              play_story(story_27_a_en)
                          if(language==3):
                              play_story(story_27_a_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.1)        
                          serial_write_queue.put('cave_search1')    
                     if flag=="cave_search2":
                          #----играем эффект 
                          play_effect(cave_search)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_27_b_ru)  
                          if(language==2):
                              play_story(story_27_b_en)
                          if(language==3):
                              play_story(story_27_b_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.1)         
                          serial_write_queue.put('cave_search2')        
                     if flag=="cave_search3":
                          #----играем эффект 
                          play_effect(cave_search)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_27_c_ru)  
                          if(language==2):
                              play_story(story_27_c_en)
                          if(language==3):
                              play_story(story_27_c_ar) 
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.1)        
                          serial_write_queue.put('cave_search3')                          
                     if flag=="cave_end":
                          #----играем эффект 
                          socketio.emit('level', 'troll',to=None)
                          socklist.append('troll')
                          play_effect(cave_end)
                          send_esp32_command(ESP32_API_TRAIN_URL, "troll_finish")
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
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
                          #----играем эффект 
                          socketio.emit('level', 'active_safe',to=None)
                          socklist.append('active_safe')
                          socketio.emit('level', 'open_bank_door',to=None)
                          socklist.append('open_bank_door')
                          play_effect(door_bank)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_23_ru)  
                          if(language==2):
                              play_story(story_23_en)
                          if(language==3):
                              play_story(story_23_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.0)    
                          serial_write_queue.put('open_bank')    
                          time.sleep(5.0)      
                          if(language==1):
                              play_story(story_24_ru)  
                          if(language==2):
                              play_story(story_24_en)
                          if(language==3):
                              play_story(story_24_ar)

                          while channel3.get_busy()==True: 
                              time.sleep(0.1)     
                     if flag=="safe_turn":
                          #----играем эффект 
                          play_effect(safe_turn) 
                     if flag=="safe_fix":
                          #----играем эффект 
                          play_effect(safe_fix)      
                     if flag=="safe_end":
                          #----играем эффект 
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_5")
                          play_effect(safe_end)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_25_ru)  
                          if(language==2):
                              play_story(story_25_en)
                          if(language==3):
                              play_story(story_25_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)  
                          serial_write_queue.put('open_safe')
                          time.sleep(1.1)
                          if(language==1):
                              play_story(story_31_ru)  
                          if(language==2):
                              play_story(story_31_en)
                          if(language==3):
                              play_story(story_31_ar)
                              
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          serial_write_queue.put('open_workshop')
                          time.sleep(1.1)          
                          play_effect(door_workshop)
                          play_background_music("fon9.mp3", loops=0)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_32_ru)  
                          if(language==2):
                              play_story(story_32_en)
                          if(language==3):
                              play_story(story_32_ar)
                          socketio.emit('level', 'active_workshop',to=None)
                          socklist.append('active_workshop')
                          socketio.emit('level', 'safe',to=None)
                          socklist.append('safe')  

                     if flag=="lib_door":
                          #----играем эффект 
                          play_effect(lib_door)
                          socketio.emit('level', 'active_cup',to=None)
                          socklist.append('active_cup')
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_46_ru)  
                          if(language==2):
                              play_story(story_46_en)
                          if(language==3):
                              play_story(story_46_ar)    
                          send_esp32_command(ESP32_API_TRAIN_URL, "train_on")    
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.1)    
                          serial_write_queue.put('student_hide') 
                          time.sleep(1.1)    
                          if(language==1):
                              play_story(story_47_ru)  
                          if(language==2):
                              play_story(story_47_en)
                          if(language==3):
                              play_story(story_47_ar)

                          while channel3.get_busy()==True: 
                              time.sleep(0.1)        

                          #send_esp32_command(ESP32_API_TRAIN_URL, "train_on")

                          play_background_music("fon15.mp3")
                          if(language==1):
                              play_story(story_48_ru)  
                          if(language==2):
                              play_story(story_48_en)
                          if(language==3):
                              play_story(story_48_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          play_effect(door_top)
                          send_esp32_command(ESP32_API_WOLF_URL, "day_on")
                          send_esp32_command(ESP32_API_TRAIN_URL, "day_on")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "day_on")
                          send_esp32_command(ESP32_API_SAFE_URL, "day_on") 
                          serial_write_queue.put('door_top')
                          time.sleep(1)
                          socketio.emit('level', 'open_door_puzzle',to=None)
                          socklist.append('open_door_puzzle')
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_50_ru)  
                          if(language==2):
                              play_story(story_50_en)
                          if(language==3):
                              play_story(story_50_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)    
                          play_effect(lose1)
                          socketio.emit('level', 'active_spell',to=None)
                          socklist.append('active_spell')
                              
                     if flag=="door_spell":
                          socketio.emit('level', 'spell',to=None)
                          socklist.append('spell')
                          #----играем эффект 
                          play_effect(door_spell) 
                          socketio.emit('level', 'active_crystals',to=None)
                          socklist.append('active_crystals')              
                     if flag=="cristal_up":
                          #----играем эффект 
                          play_effect(cristal_up)
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_10") 
                          socketio.emit('level', 'active_open_memory_stash',to=None)
                          socklist.append('active_open_memory_stash')
                          socketio.emit('level', 'crystals',to=None)
                          socklist.append('crystals')  
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
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
                     if flag=="item_find":
                          #----играем эффект 
                          play_effect(item_find)
                          send_esp32_command(ESP32_API_TRAIN_URL, "item_find")
                     if flag=="item_add":
                          #----играем эффект 
                          play_effect(item_add)
                     if flag=="broom":
                          #----играем эффект 
                          play_effect(craft_success)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)    
                          if(language==1):
                              play_story(story_33_ru)  
                          if(language==2):
                              play_story(story_33_en)
                          if(language==3):
                              play_story(story_33_ar)                    
                     if flag=="helmet":
                          #----играем эффект 
                          play_effect(craft_success)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)    
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
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.0)
                          send_esp32_command(ESP32_API_WOLF_URL, "day_off")
                          send_esp32_command(ESP32_API_TRAIN_URL, "day_off")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "day_off")
                          send_esp32_command(ESP32_API_SAFE_URL, "day_off")    
                          play_background_music("fon10.mp3")
                          if(language==1):
                              play_story(story_36_ru)  
                          if(language==2):
                              play_story(story_36_en)
                          if(language==3):
                              play_story(story_36_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          serial_write_queue.put('student_open')
                          time.sleep(1.0)     
                          if(language==1):
                              play_story(story_37_ru)  
                          if(language==2):
                              play_story(story_37_en)
                          if(language==3):
                              play_story(story_37_ar) 
                          socketio.emit('level', 'active_ghost',to=None)
                          socklist.append('active_ghost')        

                     if flag=="h_clock":
                          #----играем эффект 
                          play_background_music("fon11.mp3")
                          play_effect(h_clock)
                     if flag=="uf_clock":
                          #----играем эффект 
                          play_background_music("fon12.mp3")
                          play_effect(uf_clock)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                          if(language==1):
                              play_story(story_38_ru)  
                          if(language==2):
                              play_story(story_38_en)
                          if(language==3):
                              play_story(story_38_ar)     
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
                          if(language==1):
                              play_story(story_40_ru)  
                          if(language==2):
                              play_story(story_40_en)
                          if(language==3):
                              play_story(story_40_ar)  
                     if flag=="story_41":
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game")
                          if(language==1):
                              play_story(story_41_ru)  
                          if(language==2):
                              play_story(story_41_en)
                          if(language==3):
                              play_story(story_41_ar)
                     if flag=="story_42":
                          if(language==1):
                              play_story(story_42_ru)  
                          if(language==2):
                              play_story(story_42_en)
                          if(language==3):
                              play_story(story_42_ar)         
                     if flag=="punch":
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_7") 
                          socketio.emit('level', 'ghost',to=None)
                          socklist.append('ghost') 
                          if(language==1):
                              play_story(story_43_ru)  
                          if(language==2):
                              play_story(story_43_en)
                          if(language==3):
                              play_story(story_43_ar) 
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          serial_write_queue.put('open_library')
                          #send_esp32_command(ESP32_API_TRAIN_URL, #"ghost_game_end")
                          #send_esp32_command(ESP32_API_WOLF_URL, "ghost_game_end")
                          time.sleep(2.0)
                          send_esp32_command(ESP32_API_TRAIN_URL, "ghost_game_end")
                          time.sleep(1.0)
                          if(language==1):
                              play_story(story_44_ru)  
                          if(language==2):
                              play_story(story_44_en)
                          if(language==3):
                              play_story(story_44_ar) 

                     if flag=="star_hint":
                          send_esp32_command(ESP32_API_TRAIN_URL, "stage_8") 
                          socketio.emit('level', 'active_cup',to=None)
                          socklist.append('active_cup')   
                          play_effect(star_hint)
                          send_esp32_command(ESP32_API_TRAIN_URL, "set_time")
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          if(language==1):
                              play_story(story_45_ru)  
                          if(language==2):
                              play_story(story_45_en)
                          if(language==3):
                              play_story(story_45_ar)        
                          
                     if flag=="mistake_crystal":
                          #----играем эффект
                          play_effect(mistake_crystal)
                          print("mistake_crystal")
                      #----раставили кристаллы на свои места    
                     if flag=="start_crystal":
                          #=----просто звуковой эффект
                          play_background_music("fon16.mp3")
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
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
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          #------играем голос    
                          if(language==1):
                              play_story(story_52_ru)  
                          if(language==2):
                              play_story(story_52_en)
                          if(language==3):
                              play_story(story_52_ar)  
                     if flag=="boy_in_game":
                          if 'stop_players_rest' in socklist:
                                socklist.remove('stop_players_rest')
                          socketio.emit('level', 'start_players',to=None)
                          socklist.append('start_players')

                     if flag=="boy_out_game":
                          if 'start_players' in socklist:
                                socklist.remove('start_players')
                          socketio.emit('level', 'stop_players_rest',to=None)
                          socklist.append('stop_players_rest')       

                     if flag=="boy_in":
                          play_background_music("fon18.mp3")
                          socketio.emit('level', 'start_players',to=None)
                          socklist.append('start_players') 
                          if(language==1):
                              play_story(story_57_ru)  
                          if(language==2):
                              play_story(story_57_en)
                          if(language==3):
                              play_story(story_57_ar) 
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          play_effect(applause)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)    
                          if(language==1):
                              play_story(story_58_ru)  
                          if(language==2):
                              play_story(story_58_en)
                          if(language==3):
                              play_story(story_58_ar)
                          while channel3.get_busy()==True: 
                              time.sleep(0.1)
                          time.sleep(1.0)    
                          serial_write_queue.put('start_game_basket') 
                          time.sleep(1.0)
                          socketio.emit('level', 'active_basket',to=None)
                          socklist.append('active_basket')     
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
                          play_background_music("fon17.mp3")
                          if(language==1):
                              play_story(story_56_ru)  
                          if(language==2):
                              play_story(story_56_en)
                          if(language==3):
                              play_story(story_56_ar)             
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
                     if flag=="goal_1_player":
                          play_effect(goal1)
                          if(language==1):
                              play_story(story_61_a_ru)  
                          if(language==2):
                              play_story(story_61_a_en)
                          if(language==3):
                              play_story(story_61_a_ar)    
                     if flag=="goal_2_player":
                          play_effect(goal2)
                          if(language==1):
                              play_story(story_61_b_ru)  
                          if(language==2):
                              play_story(story_61_b_en)
                          if(language==3):
                              play_story(story_61_b_ar)    
                     if flag=="goal_3_player":
                          play_effect(goal3)
                          if(language==1):
                              play_story(story_61_c_ru)  
                          if(language==2):
                              play_story(story_61_c_en)
                          if(language==3):
                              play_story(story_61_c_ar)    
                     if flag=="goal_4_player":
                          play_effect(goal4)
                          if(language==1):
                              play_story(story_61_d_ru)  
                          if(language==2):
                              play_story(story_61_d_en)
                          if(language==3):
                              play_story(story_61_d_ar)    

                     if flag=="goal_1_bot":
                          play_effect(enemy_goal1)
                          if(language==1):
                              play_story(story_65_a_ru)  
                          if(language==2):
                              play_story(story_65_a_en)
                          if(language==3):
                              play_story(story_65_a_ar)    
                     if flag=="goal_2_bot":
                          play_effect(enemy_goal2)
                          if(language==1):
                              play_story(story_65_b_ru)  
                          if(language==2):
                              play_story(story_65_b_en)
                          if(language==3):
                              play_story(story_65_b_ar)    
                     if flag=="goal_3_bot":
                          play_effect(enemy_goal3)
                          if(language==1):
                              play_story(story_65_c_ru)  
                          if(language==2):
                              play_story(story_65_c_en)
                          if(language==3):
                              play_story(story_65_c_ar)    
                     if flag=="goal_4_bot":
                          play_effect(enemy_goal4)
                          if(language==1):
                              play_story(story_65_d_ru)  
                          if(language==2):
                              play_story(story_65_d_en)
                          if(language==3):
                              play_story(story_65_d_ar)  

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
                          play_effect(win)
                          send_esp32_command(ESP32_API_WOLF_URL, "firework")
                          send_esp32_command(ESP32_API_TRAIN_URL, "firework")
                          send_esp32_command(ESP32_API_SUITCASE_URL, "firework")
                          send_esp32_command(ESP32_API_SAFE_URL, "firework")
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          play_background_music("fon19.mp3")    
                          if(language==1):
                              play_story(story_66_ru)  
                          if(language==2):
                              play_story(story_66_en)
                          if(language==3):
                              play_story(story_66_ar)
                     if flag=="win_robot":
                          play_effect(enemy_goal1)
                          while channel2.get_busy()==True: 
                              time.sleep(0.1)
                          play_background_music("fon17.mp3")    
                          if(language==1):
                              play_story(story_67_ru)  
                          if(language==2):
                              play_story(story_67_en)
                          if(language==3):
                              play_story(story_67_ar)         
                    #-------прошли игру с кристалами
                     if flag=="memory_room_end":
                         #----отправили на клиента
                         send_esp32_command(ESP32_API_TRAIN_URL, "stage_12") 
                         socketio.emit('level', 'memory_room_end',to=None)
                         #----добавили в историю
                         socklist.append('memory_room_end')
                         #------играем эффект
                         play_effect(brain_end)
                         #-----активируем последнюю игру
                         socketio.emit('level', 'active_basket',to=None)
                         socklist.append('active_basket') 
                         
                             
                        
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
                          time.sleep(0.5)
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
         time.sleep(0)
         for i in socklist:
              if current_client_sid:
                socketio.emit('volume', str(phoneLevel), skip_sid=current_client_sid)
                socketio.emit('volume1', str(effectLevel), skip_sid=current_client_sid)
                socketio.emit('volume2', str(voiceLevel), skip_sid=current_client_sid)
              else:
                socketio.emit('volume', str(phoneLevel))
                socketio.emit('volume1', str(effectLevel))
                socketio.emit('volume2', str(voiceLevel))


              socketio.emit('level', i ,to=None)
              final_string = ', '.join(str(device) for device in devices)
              socketio.emit('devices', final_string,to=None)
         
              #print(i)
         if go >=2:
              sec = 0
              tensec = 0
              mins = 0
              tenmins = 0
              h = 0
              sync = "0:00:00"
              socketio.emit('timer', sync,to=None)
              time.sleep(1)
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
               time.sleep(1)
               sync = str(h)+":"+str(tenmins)+str(mins)+":"+str(tensec)+str(sec)
               rateTime = sync
               socketio.emit('timer', sync,to=None)
               flagtime=0
               
#-----методы для повторения голосвых треков в случае смены языка(мноого)  чем больше треков тем больше логики              

#------наша основная программа крутиться здесь сам сервер порт можно измнить(при желании) методы таймер и сериал работают ассинхронно
if __name__ == '__main__':
    try:
        print("INFO: Starting background tasks...")
        socketio.start_background_task(target=serial)
        socketio.start_background_task(target=timer)
        print("INFO: Starting Flask-SocketIO server on http://0.0.0.0:3000")
        socketio.run(app, port=3000, host='0.0.0.0')
    except OSError as e:
        print(f"FATAL: Could not start server. Error: {e}")
        print("HINT: The port 3000 might be in use by another application.")
    except Exception as e:
        print(f"FATAL: An unexpected error occurred: {e}")

