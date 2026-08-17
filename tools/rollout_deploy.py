#!/usr/bin/env python3
"""Выложить мастер на замок: файлы сервера, пульты и прошивки.

Ничего не прошивает и не перезапускает — только копирует и сверяет md5.
Прошивка идёт кнопками с /tech, рестарт сервера — руками, когда квест свободен.

    python3 tools/rollout_deploy.py CLC3            # выложить всё
    python3 tools/rollout_deploy.py CLC3 --check    # только сверить, не копировать
    python3 tools/rollout_deploy.py CLC3 --texts    # ДОПОЛНИТЕЛЬНО заменить тексты реплик
    python3 tools/rollout_deploy.py CLC3 --restart  # и перезапустить сервер после выкладки

Замок ищется по подстроке имени в ~/.clc_castles.json (тот же конфиг, что у
rollout_status.py).

НЕ трогаются файлы, которые у каждого замка свои:
  * `castle_config.json` — калибровка серво, раскладка хаба, имя квеста;
  * `hint_texts.json` — тексты реплик. У клиентов формулировки могут
    отличаться, копия в репозитории — эталон для нового замка, а не хозяин.
    Заменить осознанно — флаг --texts.
Затереть их мастером — ровно то, ради чего всё это затевалось.
"""

import os
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from rollout_status import (BOARDS, CONFIG, PER_CASTLE_FILES, REMOTE_ROOT,  # noqa: E402
                            SERVER_FILES, collect, local_md5, ssh_run)

import json  # noqa: E402


def scp_files(castle, pairs, tries=3):
    """pairs: [(локальный путь, удалённый каталог)]. Возвращает список неудач."""
    import time
    failed = []
    for local, remote_dir in pairs:
        # -p обязателен: без него файл получает время копирования, и проверка
        # «прошита ли плата этим файлом» начинает врать на всех платах сразу —
        # даже там, где содержимое в плате уже лежит. Сохраняем дату сборки.
        base = ["scp", "-p", "-o", "ConnectTimeout=20",
                "-o", "UserKnownHostsFile=" + os.path.expanduser("~/.ssh/clc_known_hosts"),
                "-o", "StrictHostKeyChecking=accept-new"]
        if castle.get("auth") == "key":
            argv = base + ["-i", os.path.expanduser(castle["key"]), local,
                           f"{castle['user']}@{castle['host']}:{remote_dir}"]
        else:
            argv = (["sshpass", "-p", castle["password"]] + base
                    + [local, f"{castle['user']}@{castle['host']}:{remote_dir}"])
        ok = False
        for attempt in range(tries):
            r = subprocess.run(argv, capture_output=True, text=True, timeout=180)
            if r.returncode == 0:
                ok = True
                break
            if attempt < tries - 1:
                time.sleep(6)
        if not ok:
            failed.append(local)
    return failed


def restart_server(castle):
    """Перезапуск сервера отдельным вызовом, а не в цепочке.

    17.08.2026: рестарт был сцеплен через && с проверками, ssh порвался, повтор
    дал второй рестарт — и юнит завис в stop-sigterm. Причина: сервер не
    реагирует на SIGTERM, его перехватывает eventlet-хаб, systemd ждёт свои 90
    секунд. Поэтому здесь: одна команда на рестарт, потом проверка состояния, и
    если юнит застрял — добиваем SIGKILL. При Restart=always служба поднимается
    сама за пять секунд."""
    import time
    print("\n6. Перезапуск сервера")
    ssh_run(castle, "sudo systemctl restart castleserver", timeout=30, tries=1)
    time.sleep(20)
    state = (ssh_run(castle, "systemctl show castleserver -p SubState --value") or "").strip()
    if state in ("stop-sigterm", "stop", "final-sigterm"):
        print(f"   юнит застрял в {state} (SIGTERM игнорируется) — добиваю")
        ssh_run(castle, "sudo systemctl kill -s SIGKILL castleserver")
        time.sleep(12)
    active = (ssh_run(castle, "systemctl is-active castleserver") or "").strip()
    print(f"   служба: {active or 'НЕ ОТВЕЧАЕТ'}")
    out = ssh_run(castle, "tail -c 4000 /home/pi/New/logs/castle.log | strings | "
                          "grep -aE 'CONFIG: загружен|CHAT: тексты|MEGA BOOT WATCHDOG|Traceback' | tail -4")
    for line in (out or "").splitlines():
        print("   " + line.strip())
    http = ssh_run(castle, "curl -s -o /dev/null -m 5 -w '%{http_code}' http://localhost:3000/")
    print(f"   пульт отвечает: HTTP {(http or '—').strip()}")


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    needle = sys.argv[1].lower()
    check_only = "--check" in sys.argv
    with_texts = "--texts" in sys.argv
    with_restart = "--restart" in sys.argv

    castles = [c for c in json.load(open(CONFIG))["castles"] if needle in c["name"].lower()]
    if len(castles) != 1:
        sys.exit(f"Ожидал ровно один замок по «{sys.argv[1]}», нашёл {len(castles)}.")
    castle = castles[0]
    repo = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    print(f"Замок: {castle['name']} ({castle['host']})")

    if not check_only:
        # 1. Бэкап того, что там сейчас. Копируем на самом замке, чтобы не
        #    зависеть от канала: связь до клиентов рвётся регулярно.
        stamp = subprocess.run(["date", "+%Y%m%d_%H%M"], capture_output=True,
                               text=True).stdout.strip()
        backup = f"{REMOTE_ROOT}/backup_{stamp}"
        print(f"\n1. Бэкап на замке → {backup}")
        out = ssh_run(castle, (
            f"mkdir -p '{backup}/Sketches' && "
            f"cp -p {REMOTE_ROOT}/*.py {REMOTE_ROOT}/flash_stats.json '{backup}/' 2>/dev/null; "
            f"cp -p {REMOTE_ROOT}/templates/*.html {REMOTE_ROOT}/static/scripts.js '{backup}/' 2>/dev/null; "
            f"cp -rp {REMOTE_ROOT}/Sketches/* '{backup}/Sketches/' 2>/dev/null; "
            f"du -sh '{backup}'"))
        print("   " + (out.strip() if out else "НЕ УДАЛСЯ — дальше не иду"))
        if not out:
            sys.exit(1)

        # 2. Файлы сервера и пультов.
        print("\n2. Сервер и пульты")
        pairs = []
        for rel, remote in SERVER_FILES.items():
            pairs.append((os.path.join(repo, rel), f"{REMOTE_ROOT}/{remote}"))
        if with_texts:
            for rel, remote in PER_CASTLE_FILES.items():
                pairs.append((os.path.join(repo, rel), f"{REMOTE_ROOT}/{remote}"))
        failed = scp_files(castle, pairs)
        print("   " + ("выложено" if not failed else f"НЕ доехали: {failed}"))
        if with_texts:
            print("   + тексты реплик заменены эталонными (--texts)")
        else:
            print("   тексты реплик НЕ тронуты (нужно — флаг --texts)")

        # 3. Прошивки — .ino и бинарник ПАРОЙ, каждый файл отдельным вызовом.
        #    Маска вида «*.hex *.bin» на ESP32-папках раскрывалась в пустоту и
        #    рвала копирование всей папки (CLC2, 15.08) — поэтому явный список.
        print("\n3. Прошивки")
        pairs = []
        for board, (rel, remote) in BOARDS.items():
            src_dir = os.path.join(repo, os.path.dirname(rel))
            dst_dir = f"{REMOTE_ROOT}/{os.path.dirname(remote)}/"
            for name in sorted(os.listdir(src_dir)):
                if name.endswith((".ino", ".hex", ".bin")) and "baseline" not in name:
                    pairs.append((os.path.join(src_dir, name), dst_dir))
        failed = scp_files(castle, pairs)
        print(f"   файлов: {len(pairs)}, " + ("все доехали" if not failed else f"НЕ доехали: {failed}"))

    # 4. Сверка.
    print("\n4. Сверка md5 с мастером")
    snap = collect(castle)
    if snap is None:
        sys.exit("   замок не отвечает")
    bad = []
    for rel, remote in list(SERVER_FILES.items()) + [(r, rm) for r, rm in BOARDS.values()]:
        if local_md5(rel) != snap["md5"].get(remote):
            bad.append(rel)
    print("   " + ("✅ всё совпадает" if not bad else "❌ расходятся: " + ", ".join(bad)))
    for rel, remote in PER_CASTLE_FILES.items():
        same = local_md5(rel) == snap["md5"].get(remote)
        print(f"   📝 {rel}: " + ("совпадает с эталоном" if same else "своя версия замка"))

    # 5. Синтаксис прямо на замке: там свой python (3.9 против нашего 3.12).
    out = ssh_run(castle, f"cd {REMOTE_ROOT} && python3 -m py_compile "
                          f"CastleServer.py castle_config.py castle_setup.py hue_lights.py "
                          f"&& echo OK")
    print("\n5. Компиляция на замке: " + ("OK" if out and "OK" in out else "ОШИБКА"))

    if with_restart:
        restart_server(castle)
    else:
        print("\nДальше руками: sudo systemctl restart castleserver, потом прошивки с /tech.")
        print("Либо тот же вызов с флагом --restart.")


if __name__ == "__main__":
    main()
