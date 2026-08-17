#!/usr/bin/env python3
"""Выгрузить тексты подсказок из Google-таблицы в hint_texts.json.

Таблица — первоисточник по озвучке ([[clc-voice-lines-spreadsheet]]): Эдуард
правит текст там, и только потом идёт переозвучка. Локальной копии на замках
нет, поэтому чат гейммастера на игровом пульте берёт текст из этого файла.

    python3 tools/export_hint_texts.py          # перезаписать hint_texts.json

Файл едет на замки обычной раскаткой. Правили тексты в таблице — прогнать
заново и раскатать; ничего в коде менять не нужно.

Что внутри файла:
    texts    id → {ru, en, ar, ar_kw, fr, uk, pl}
    speakers префикс id → кто это говорит (для строки «Борис: hint_32_d»)
    esp32    устройство → список id по порядку «Playing Hint N» в его прошивке
"""

import json
import os
import subprocess
import sys

SHEET = "1cd-mAtyg0jatVvJhhAMhLVfhgawgvu7X2SwUum6e-iw"
REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(REPO, "hint_texts.json")

# Столбцы вкладки hints. Порядок ОТЛИЧАЕТСЯ от вкладки stories: там
# Polish идёт раньше Ukranian, здесь наоборот — проверено по шапке.
COLUMNS = ["ru", "en", "uk", "pl", "fr", "ar", "ar_kw"]

# Кто произносит подсказку. Ключ — префикс id до последнего подчёркивания.
# Группировка взята из прошивки главной платы (массивы *Hints[]) и из того,
# какому устройству принадлежит этап. Имена правятся здесь — это единственное
# место, откуда их берёт пульт.
SPEAKERS = {
    "dragon_crystal": "Дракон Сигнус",
    "hint_2": "Дракон Сигнус",
    "hint_5": "Дракон Сигнус",
    "hint_3": "Студент",
    "hint_37": "Студент",
    "hint_38": "Студент",
    "hint_44": "Студент",
    "hint_6": "Профессор",
    "hint_10": "Профессор",
    "hint_11": "Профессор",
    "hint_14": "Гном",
    "hint_17": "Ведьма",
    "hint_19": "Рыцарь",
    "hint_23": "Гоблин-банкир",
    "hint_26": "Тролль",
    "hint_32": "Борис (кузнец)",
    "hint_49": "Директор",
    "hint_50": "Директор",
    "hint_51": "Директор",
    "hint_56": "Директор",
    # Этапы на внешних ESP32
    "hint_15": "Поезд",
    "hint_16": "Поезд",
    "hint_9": "Волк",
    "hint_7": "Чемоданы",
    "hint_28": "Сейф",
}

# Соответствие «Playing Hint N» из логов ESP32 → id подсказки.
# В прошивке ESP32 знает только номер трека на своей SD-карте, а не id, поэтому
# связь восстановлена по числу подсказок у каждого устройства и по смыслу
# текстов: количество совпало у всех четырёх ровно.
# ПОРЯДОК внутри списка — единственное допущение этой таблицы. Если в чате
# у какого-то устройства текст поедет на одну подсказку, менять здесь.
ESP32_HINTS = {
    "train": ["hint_15_a", "hint_15_b", "hint_15_c", "hint_15_d", "hint_15_e",
              "hint_16_c", "hint_16_d", "hint_16_z"],
    "wolf": ["hint_9_a", "hint_9_b", "hint_9_c", "hint_9_d", "hint_9_e", "hint_9_f", "hint_9_z"],
    "chest": ["hint_7_a", "hint_7_b", "hint_7_c", "hint_7_d", "hint_7_z"],
    "safe": ["hint_28_a", "hint_28_b", "hint_28_c", "hint_28_z"],
}


def speaker_for(hint_id):
    if hint_id in SPEAKERS:
        return SPEAKERS[hint_id]
    prefix = hint_id.rsplit("_", 1)[0]
    return SPEAKERS.get(prefix, "—")


def main():
    try:
        import openpyxl
    except ImportError:
        sys.exit("Нужен openpyxl: pip3 install openpyxl")

    tmp = "/tmp/clc_voice_export.xlsx"
    url = f"https://docs.google.com/spreadsheets/d/{SHEET}/export?format=xlsx"
    print("Тяну таблицу…")
    r = subprocess.run(["curl", "-sL", "--max-time", "90", "-o", tmp, url])
    if r.returncode != 0 or not os.path.exists(tmp):
        sys.exit("Не скачалось. Таблица открыта по ссылке на чтение — проверь интернет.")

    ws = openpyxl.load_workbook(tmp)["hints"]
    header = [str(c or "").strip().lower() for c in next(ws.iter_rows(min_row=1, max_row=1,
                                                                     values_only=True))]
    if "russian" not in header[1]:
        sys.exit(f"Шапка вкладки hints изменилась: {header[:4]} — проверь порядок столбцов")

    texts, latin_fix = {}, []
    for row in ws.iter_rows(min_row=2, values_only=True):
        if not row or not row[0]:
            continue
        hid = str(row[0]).strip()
        # В таблице встречается id с кириллической «с» (hint_51_с). На Pi файл
        # называется латиницей, и прошивка просит латиницей — поэтому чиним,
        # иначе текст к такой подсказке не найдётся никогда.
        fixed = hid.replace("с", "c").replace("а", "a").replace("е", "e")
        if fixed != hid:
            latin_fix.append(hid)
            hid = fixed
        texts[hid] = {lang: (row[i + 1] or "").strip()
                      for i, lang in enumerate(COLUMNS) if i + 1 < len(row)}

    data = {
        "_комментарий": ("Выгружено из Google-таблицы скриптом tools/export_hint_texts.py. "
                         "Руками не править: правится таблица, потом прогон скрипта."),
        "speakers": {hid: speaker_for(hid) for hid in sorted(texts)},
        "esp32": ESP32_HINTS,
        "texts": texts,
    }
    with open(OUT, "w", encoding="utf-8") as f:
        json.dump(data, f, ensure_ascii=False, indent=1, sort_keys=True)

    print(f"Записано: {OUT}")
    print(f"  подсказок: {len(texts)}, размер: {os.path.getsize(OUT) // 1024} КБ")
    if latin_fix:
        print(f"  ⚠ id с кириллицей в таблице (починены при выгрузке): {latin_fix}")
    no_speaker = [h for h in texts if speaker_for(h) == "—"]
    if no_speaker:
        print(f"  ⚠ без персонажа (допиши в SPEAKERS): {no_speaker}")
    missing = [h for d in ESP32_HINTS.values() for h in d if h not in texts]
    if missing:
        print(f"  ⚠ в таблице нет текста для: {missing}")


if __name__ == "__main__":
    main()
