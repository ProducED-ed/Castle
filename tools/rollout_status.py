#!/usr/bin/env python3
"""Что на замках отстало от мастера — одной таблицей.

Зачем: правки рождаются на CLC4 в цеху, проверяются там же, а до клиентских
CLC2 и CLC3 доезжают «когда вспомним». Вспоминать — плохой механизм: за один
день 15.08.2026 накопилось четыре правки, и половина не доехала бы никуда.

Скрипт ТОЛЬКО ЧИТАЕТ: считает md5 файлов на каждом замке и сравнивает с тем,
что лежит в этом репозитории, плюс смотрит flash_stats.json — прошита ли плата
тем файлом, который на ней лежит. Ничего не копирует и не перезапускает.

Доступ к замкам берётся из ~/.clc_castles.json (в git не хранится — там пароли).
Там же живёт роль замка: какой из них сейчас мастер-стенд в цеху. Роль
переходящая — CLC4 уедет к клиенту в Кувейт, и стендом станет следующий
собираемый замок. В коде она не зашита нигде, правится одной строкой конфига.

Запуск:
    python3 tools/rollout_status.py            # все замки
    python3 tools/rollout_status.py CLC2       # только те, чьё имя содержит CLC2
"""

import json
import os
import subprocess
import sys

REPO = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONFIG = os.path.expanduser("~/.clc_castles.json")

# Файлы сервера: путь в репозитории → путь на замке.
SERVER_FILES = {
    "CastleServer.py": "CastleServer.py",
    "castle_config.py": "castle_config.py",
    "castle_setup.py": "castle_setup.py",
    "hue_lights.py": "hue_lights.py",
    "Tech.html": "templates/Tech.html",
    "Front.html": "templates/Front.html",
    "scripts.js": "static/scripts.js",
}

# Прошивки: плата → (путь в репозитории, путь на замке).
BOARDS = {
    "main": ("MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex",
             "Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex"),
    "owls": ("owls/owls.ino.hex", "Sketches/owls/owls.ino.hex"),
    "basket": ("basket3/basket3.ino.hex", "Sketches/basket3/basket3.ino.hex"),
    "workshop": ("workshop/workshop.ino.hex", "Sketches/workshop/workshop.ino.hex"),
    "dog": ("dog/dog.ino.hex", "Sketches/dog/dog.ino.hex"),
    "train": ("train/train.ino.bin", "Sketches/train/train.ino.bin"),
    "chest": ("chest/chest.ino.bin", "Sketches/chest/chest.ino.bin"),
    "safe": ("safe/safe.ino.bin", "Sketches/safe/safe.ino.bin"),
    "wolf": ("wolf/wolf.ino.bin", "Sketches/wolf/wolf.ino.bin"),
}

# Файлы, которые у каждого замка СВОИ и которые раскатка не трогает.
# Тексты реплик правятся под конкретного клиента (формулировки могут
# отличаться), поэтому копия в репозитории — не хозяин, а эталон для нового
# замка. Расхождение здесь не ошибка, и сверка показывает его отдельной
# строкой, а не в списке «отстало».
PER_CASTLE_FILES = {
    "hint_texts.json": "hint_texts.json",
}

REMOTE_ROOT = "/home/pi/New"


def local_md5(rel):
    path = os.path.join(REPO, rel)
    if not os.path.exists(path):
        return None
    out = subprocess.run(["md5sum", path], capture_output=True, text=True)
    return out.stdout.split()[0] if out.returncode == 0 else None


def ssh_run(castle, command, timeout=45, tries=3):
    """Одна ssh-команда. Возвращает stdout или None, если замок недоступен.

    Повторы обязательны: связь до замков идёт через Tailscale поверх сети
    клиента (а на производстве — через телефон), и одиночный отказ ничего не
    значит. Без повторов инструмент показывал бы «недоступен» через раз."""
    import time
    base = ["ssh", "-o", "ConnectTimeout=15", "-o", "BatchMode=no",
            "-o", "UserKnownHostsFile=" + os.path.expanduser("~/.ssh/clc_known_hosts"),
            "-o", "StrictHostKeyChecking=accept-new"]
    if castle.get("auth") == "key":
        base += ["-i", os.path.expanduser(castle["key"])]
        argv = base + [f"{castle['user']}@{castle['host']}", command]
    else:
        argv = (["sshpass", "-p", castle["password"]] + base
                + [f"{castle['user']}@{castle['host']}", command])
    for attempt in range(tries):
        try:
            out = subprocess.run(argv, capture_output=True, text=True, timeout=timeout)
            if out.returncode == 0:
                return out.stdout
        except subprocess.TimeoutExpired:
            pass
        if attempt < tries - 1:
            time.sleep(5)
    return None


def collect(castle):
    """Снимок замка: md5 всех интересных файлов + flash_stats + даты файлов прошивок."""
    files = (list(SERVER_FILES.values()) + list(PER_CASTLE_FILES.values())
             + [remote for _, remote in BOARDS.values()])
    quoted = " ".join(f"'{REMOTE_ROOT}/{f}'" for f in files)
    cmd = (f"md5sum {quoted} 2>/dev/null; echo '---FLASH---'; "
           f"cat '{REMOTE_ROOT}/flash_stats.json' 2>/dev/null; echo; echo '---MTIME---'; "
           f"stat -c '%n %Y' {quoted} 2>/dev/null; echo '---TZ---'; date +%z")
    raw = ssh_run(castle, cmd)
    if raw is None:
        return None
    md5s, flash, mtimes, tz = {}, {}, {}, 0
    section = "md5"
    for line in raw.splitlines():
        if line.startswith("---FLASH---"):
            section = "flash"; continue
        if line.startswith("---MTIME---"):
            section = "mtime"; continue
        if line.startswith("---TZ---"):
            section = "tz"; continue
        if section == "md5" and line.strip():
            h, _, path = line.partition("  ")
            md5s[path.replace(REMOTE_ROOT + "/", "")] = h
        elif section == "flash" and line.strip().startswith("{"):
            try:
                flash = json.loads(line)
            except ValueError:
                pass
        elif section == "mtime" and line.strip():
            path, _, ts = line.rpartition(" ")
            mtimes[path.replace(REMOTE_ROOT + "/", "")] = int(ts)
        elif section == "tz" and line.strip():
            # Вид +0300 → секунды. Часовой пояс замка нужен, чтобы разобрать
            # даты из flash_stats.json: они записаны по его местному времени.
            raw = line.strip()
            try:
                sign = -1 if raw[0] == "-" else 1
                tz = sign * (int(raw[1:3]) * 3600 + int(raw[3:5]) * 60)
            except (ValueError, IndexError):
                tz = 0
    return {"md5": md5s, "flash": flash, "mtime": mtimes, "tz": tz}


def flash_is_stale(flash_str, file_mtime, tz_offset_sec):
    """Файл прошивки новее того, что реально залито в плату?

    Две ловушки, обе стоили ложных тревог 17.08.2026:

    1. Время файла должно быть временем СБОРКИ, а не копирования — иначе после
       раскатки «не прошиты» показываются все платы сразу, включая те, чьё
       содержимое в плате уже лежит. За это отвечает `scp -p` в rollout_deploy.
    2. Дата в flash_stats.json записана по МЕСТНОМУ времени замка (замки в MSK),
       а время файла абсолютное. Наш VPS живёт по UTC, поэтому наивный разбор
       строки давал разъезд в три часа. Смещение спрашиваем у самого замка."""
    if not flash_str or not file_mtime:
        return None
    import calendar
    import datetime
    try:
        flashed = datetime.datetime.strptime(flash_str, "%d.%m.%Y %H:%M")
    except ValueError:
        return None
    flashed_epoch = calendar.timegm(flashed.timetuple()) - tz_offset_sec
    return flashed_epoch < file_mtime - 60  # минута форы на копирование


def report(castle, snap):
    print(f"\n=== {castle['name']} — {castle['host']} ({castle.get('role', '')})")
    if snap is None:
        print("    НЕДОСТУПЕН (Tailscale молчит)")
        return {"unreachable": True}

    behind_files, not_flashed, missing = [], [], []

    for rel, remote in SERVER_FILES.items():
        mine, theirs = local_md5(rel), snap["md5"].get(remote)
        if theirs is None:
            missing.append(rel)
        elif mine != theirs:
            behind_files.append(rel)

    for board, (rel, remote) in BOARDS.items():
        mine, theirs = local_md5(rel), snap["md5"].get(remote)
        if theirs is None:
            missing.append(remote)
            continue
        if mine != theirs:
            behind_files.append(f"прошивка {board}")
            continue
        # Файл тот же — но прошита ли им плата?
        if flash_is_stale(snap["flash"].get(board), snap["mtime"].get(remote), snap.get("tz", 0)):
            not_flashed.append(board)

    # Свои файлы замка — информационно. «Отличается» здесь означает «правили под
    # клиента», а не «забыли обновить».
    own = []
    for rel, remote in PER_CASTLE_FILES.items():
        theirs = snap["md5"].get(remote)
        if theirs is None:
            own.append(f"{rel}: НЕТ на замке")
        elif theirs != local_md5(rel):
            own.append(f"{rel}: своя версия (отличается от эталона)")
        else:
            own.append(f"{rel}: совпадает с эталоном")

    if not behind_files and not not_flashed and not missing:
        print("    ✅ всё совпадает с мастером и прошито")
    for line in own:
        print(f"    📝 {line}")
    if behind_files:
        print("    ⬇ НЕ ВЫЛОЖЕНО (файл у нас новее):")
        for f in behind_files:
            print(f"        {f}")
    if not_flashed:
        print("    ⚡ ВЫЛОЖЕНО, НО НЕ ПРОШИТО: " + ", ".join(not_flashed))
    if missing:
        print("    ? нет на замке: " + ", ".join(missing))
    return {"behind": behind_files, "not_flashed": not_flashed, "missing": missing}


def main():
    if not os.path.exists(CONFIG):
        sys.exit(f"Нет {CONFIG} — в нём доступы к замкам (в git не хранится).")
    with open(CONFIG) as f:
        castles = json.load(f)["castles"]
    if len(sys.argv) > 1:
        needle = sys.argv[1].lower()
        castles = [c for c in castles if needle in c["name"].lower()]

    print("Сверка замков с мастером. Читаю, ничего не меняю.")
    results = {}
    for c in castles:
        results[c["name"]] = report(c, collect(c))

    print("\n--- ИТОГО ---")
    for name, r in results.items():
        if r.get("unreachable"):
            print(f"  {name}: недоступен")
        elif r["behind"] or r["not_flashed"]:
            parts = []
            if r["behind"]:
                parts.append(f"{len(r['behind'])} файлов выложить")
            if r["not_flashed"]:
                parts.append(f"{len(r['not_flashed'])} плат прошить")
            print(f"  {name}: " + ", ".join(parts))
        else:
            print(f"  {name}: в порядке")


if __name__ == "__main__":
    main()
