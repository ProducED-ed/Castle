# CLC Test Checklist — пошаговый тест квеста "Chronicles of the Living Castle"

> Этот чеклист построен по реальной state-machine `MAIN_BOARD_V5_COM5.ino` (~7500 строк) + server `CastleServer.py` + пульт `Front.html`. Каждый этап = одна `case` в `void loop()` (переменная `level`).
>
> **Цель теста** — проверить КАЖДЫЙ физический сенсор/выход в производственном комплекте за один проход квеста (~1 час).
>
> **Легенда:**
> - 🎯 — игровая цель этапа (что игрок должен сделать сюжетно)
> - 👆 — действие тестировщика
> - 👁️ — что должно произойти (визуально / audio / событие в логе)
> - 🔧 — компонент (пин Mega или expander, если есть)
> - 🏰 — задействована башня
> - 📡 — задействован ESP32
> - ⚠️ — рискованное место / нестабильное
>
> **Open Door / Skip кнопки на пульте** — у каждого этапа есть в `Front.html` кнопка «Skip» (имитация события) и «Open Door» (только открывает шериф без события). Использовать только если этап провален.

---

## 0. Подготовка (level=0 после boot, level=25 после restart)

### 0.1 Включение и boot
- [ ] Подать питание на Pi 4, ждать загрузки сервера (~60 c)
- [ ] 👁️ Прозвучал звук `on.wav` (`QUEST_SYSTEM_READY` от Mega → server играет on.wav) — если за 5 c не сыграл, **нажми физический reset на Mega** ([feedback_mega_post_avrdude_reset](memory)) или проверь USB-CDC от Pi
- [ ] Открыть `http://<pi-ip>:3000`, пароль `questquest` → загружается пульт оператора (`Front.html`)
- [ ] На вкладке «Settings» → Tech-pult → нажать **Check towers** → все 4 башни green (Workshop / Basket / Dog / Owls) — поле `tower_status:workshop=1,basket=1,dog=1,owls=1`
- [ ] ESP32: Train / Wolf / Safe / Suitcases — статус green (видны на пульте)

### 0.2 Restart — открытие всех 9 дверей
Нажать **Restart** на пульте — в коде: `OpenAll()` → импульс HIGH 500 мс на каждый из 9 пинов реле подряд → `RestOn()` (level=25).

- [ ] 🔧 **MansardDoor** (A6) — щёлкнул шериф мансарды
- [ ] 🔧 **PotionsRoomDoor** (A3) — щёлкнул шериф комнаты зельеварения
- [ ] 🔧 **LibraryDoor** (A11) — щёлкнул шериф библиотеки
- [ ] 🔧 **BankDoor** (A4) — щёлкнул шериф банка
- [ ] 🔧 **HightTowerDoor** (A9) — щёлкнул шериф нижней двери последней башни
- [ ] 🔧 **HightTowerDoor2** (30) — щёлкнул шериф верхней двери последней башни
- [ ] 🔧 **MemoryRoomDoor** (A2) — щёлкнул шериф комнаты воспоминаний
- [ ] 🔧 **CrimeDoor** (34) — щёлкнул шериф тюрьмы
- [ ] 🔧 **BankStashDoor** (A10) — щёлкнул шериф тайника в банке (сейф)
- [ ] 🏰 Башни тоже получили `restart` — Workshop / Basket / Dog / Owls открыли свои внутренние замки (workshop LOCK, dog DOOR_LOCK + CAGE_LOCK, basket SHERIF_EM1/EM2)

### 0.3 Свет «idle» (после RestOn level=25)
- [ ] 🔧 MansardLight (39) **HIGH** — мансарда подсвечена
- [ ] 🔧 LastTowerTopLight (36) **HIGH** — верх центральной башни горит
- [ ] 🔧 BankRoomLight (4) **HIGH** — бра в банке
- [ ] 🔧 TorchLight (7) **HIGH** — факела в последней башне
- [ ] 🔧 HallLight (37) **HIGH** — свет 1-й комнаты
- [ ] 🔧 UfHallLight (A5) **HIGH** — УФ-свет 1-й комнаты
- [ ] 🔧 LibraryLight (A7) **HIGH** — свет библиотеки
- [ ] 🔧 Fireworks (3) **LOW** — фейерверк выключен
- [ ] 👁️ Все NeoPixel-ленты светятся синим (`Color(0, 0, 200)`): GoldStrip(50), CandleStrip(51), CauldronStrip(48), CauldronRoomStrip(12), memory_Led(11)

### 0.4 Глобальные сенсоры (Tech-pult → Mega DIAG-tab)
На пульте включить **DIAG mode Mega** (`dg_on`). Snapshot 5×/c. Проверить каждый сенсор тыча магнитом / нажимая:

- [ ] 🔧 `startDoorPin` (41) — геркон стартовой двери, замкнуть → должна загореться точка в snapshot
- [ ] 🔧 `crimeDoorPin` (38) — геркон двери тюрьмы (он же `studentPin`)
- [ ] 🔧 `goldMaterialPin` (47) — кнопка золота у мастера
- [ ] 🔧 `directorPin` (43), `dragonPin` (42), `professorPin` (44), `witchPin` (45), `goblinPin` (A15) — кнопки фигур (если установлены)
- [ ] 🔧 `galetSwitchesPin` (40) — местный галетник (Main Board)
- [ ] 🔧 `WindowSens` (A13) — датчик дыхания у окна банка
- [ ] 🔧 `KnockSens` (A0) — пьезо стука в библиотеке (analogRead < 300 при ударе)
- [ ] 🔧 `firstCrystal` (27), `secondCrystal` (24), `thirdCrystal` (23), `fourCrystal` (25) — герконы 4 кристаллов в комнате воспоминаний
- [ ] 🔧 RFID OneWire pin 13 (бутылки в комнате зельеварения) — поднеси любую RFID-бутылку, в snapshot увидишь ID
- [ ] 🔧 Expander board1 (Y=21, addr=000): pin 0 (рыбка fish), pin 4 (потионы), pin 6 (UF clock), pin 7 (boy_out / crime)
- [ ] 🔧 Expander board2 (Y=20, addr=001): pin 0 (clock1 — 12 часов), pin 1 (crime_open / локер), pin 2 (clock2 — 42 минуты)
- [ ] 🔧 Expander board3 (Y=A12, addr=010): pin 0,1,2,3 (4 геркона свитков-сейфа в банке), pin 5 (lib_door)
- [ ] 🔧 Expander board4 (Y=33, addr=011): pin 0 (свайп left), pin 1 (свайп right), pin 2 (геркон кубка), pin 3 (геркон камина), pin 4 (cristal_up — финальный кристалл)

Выключить DIAG (`dg_off`) перед началом квеста.

### 0.5 Башни (опрос через `check_state` каждые 5–20 c автоматически)
- [ ] 🏰 Workshop — отвечает `pong` на `ping_main` (Serial1 → Mega RX18)
- [ ] 🏰 Basket — отвечает `pong` (Serial2)
- [ ] 🏰 Dog — отвечает `pong` (Serial3, USB-FTDI на ttyUSB через Pi для прошивки, см. [project_clc_dog_flash_silence_workflow])
- [ ] 🏰 Owls — отвечает `pong` (mySerial Software UART)

### 0.6 Аудио
- [ ] На пульте Tech → «Проверить аудио» → проверка наличия .wav (НЕ тест динамика! см. [reference_clc_audio_check_vs_speaker_test]). Динамик проверяется в ходе квеста.
- [ ] Слайдеры громкости Phone / Effect / Voice / Wolf / Platform / Suitcases / Safe — поработай каждый, см. колебания на пульте

---

## 1. Старт квеста: `StartDoor()` — level=1

🎯 Игрок входит в замок, открывает стартовую дверь. Кей (boy на серво) поворачивается.

**Вход:** игроки в комнате, оператор нажимает Start (или просто скрипт ждёт открытие двери).

**Код:** ждёт `startDoor.isRelease()` на пине 41.

- [ ] 👆 Замкнуть геркон стартовой двери `startDoorPin` (41)
- [ ] 👁️ Mega шлёт `open_door` → server играет соответствующее аудио
- [ ] 👁️ 🔧 HallLight (37) → **HIGH** (свет коридора)
- [ ] 👁️ 🔧 boyServo на пине 49 (Servo, 500–2500 us) — крутится с 0° → 170°, удерживается 3 c, отключается ⚠️ см. [project_clc_servo_pin49_test] — может не работать на CLC1, проверить!
- [ ] 👁️ Через ~2 мин (если игроки залипают) — повтор подсказки `dragon_crystal_repeat`
- [ ] Альтернатива: пульт → **Skip Start Door** → имитация всего вышеперечисленного

---

## 2. Часы — первая стрелка: `ClockGame()` — level=2

🎯 Поверни стрелку часов на «12»

**Код:** ждёт замыкания `!digitalReadExpander(0, board2)` (геркон 12 часов).

- [ ] 👆 Магнит к геркону 12 часов (board2, pin 0 → addr 001, Y=20)
- [ ] 👁️ Mega шлёт `clock1` → audio
- [ ] 👁️ 🔧 UfHallLight (A5) → **HIGH** (УФ-свет включается)
- [ ] 👁️ 🔧 HallLight (37) → **LOW** (обычный свет выключается)
- [ ] Пульт: индикатор **12 hours** становится зелёным
- [ ] Альтернатива: пульт → **First Clock Skip**

---

## 3. Часы — вторая стрелка: `Clock2Game()` — level=3

🎯 Поверни вторую стрелку на «42 минуты»

**Код:** ждёт `!digitalReadExpander(2, board2)`.

- [ ] 👆 Магнит к геркону 42 минут (board2, pin 2)
- [ ] 👁️ Mega шлёт `clock2` → audio
- [ ] 👁️ 🔧 UF-LED начинает мигать (`isUfBlinking`, 6 циклов — 3 ON + 3 OFF)
- [ ] 👁️ Через 6 c после старта мигания шлётся `steps` (что инициирует историю-переход)
- [ ] Пульт: индикатор **42 minutes** зелёный
- [ ] Альтернатива: **Second Clock Skip**

---

## 4. Игра «Галетники мансарды»: `GaletGame()` — level=4

🎯 Игроки в 4 башнях (Workshop, Basket, Dog, Owls) находят галетники-переключатели и переводят их в активное положение. + Один галетник на Main Board (`galetSwitchesPin` 40). Всего нужно 5.

**Код:** ждёт пока activeGalets == 5, при этом блокировка `galetGameReadyToWin` — если изначально все 5 уже на, не засчитывается, нужно сначала <5.

- [ ] 👁️ После открытия level 3 → server играет историю `story_4` (триггер `steps` отправлен Mega)
- [ ] 👆 Переключи галетник в **Workshop** (pin 30 на workshop.ino, GALET_PIN)
- [ ] 👁️ Mega принимает `galet_on` от Serial1 → шлёт `galetG2` → audio
- [ ] 👆 Переключи галетник в **Basket** (через Serial2, см. basket3.ino → `galet_on`)
- [ ] 👁️ `galetG3`
- [ ] 👆 Переключи галетник в **Dog** (`galetButton` = `ROSE_REED_PIN` A3 на dog.ino)
- [ ] 👁️ `galetG4`
- [ ] 👆 Переключи галетник в **Owls** (`PIN_HERKON_BOAT` D30 на owls.ino → `owls_galet_on`)
- [ ] 👁️ `galetG5`
- [ ] 👆 Переключи местный галетник Mega `galetSwitchesPin` (40)
- [ ] 👁️ `galetG1`
- [ ] 👁️ Когда все 5 активны → Mega шлёт `mansard_finish` → server открывает MansardDoor
- [ ] 🔧 MansardDoor (A6) реле → HIGH 300 мс → дверь мансарды откр.
- [ ] Альтернатива: **Open Mansard Door (Skip)** — открывает дверь без проверки галетников
- [ ] ⚠️ Если все галетники UP до старта — игра не засчитает, надо опустить хотя бы один и снова поднять

---

## 5. Три параллельные игры в мансарде: `ThreeGame()` — level=5

🎯 В мансарде 3 устройства: 📡 Чемодан (Suitcases ESP32), 📡 Сейф (Safe ESP32), 📡 Волк (Wolf ESP32). Все три должны быть пройдены.

**Код:** ждёт три флага одновременно `threeSuitcaseFlag && threeSafeFlag && threeWolfFlag`.

### 5.1 Suitcases (📡 ESP32)
- [ ] 👆 Нажми 4 кнопки на чемодане в правильном порядке (`butt1..butt4`), затем поднеси геркон `eyeSymbolGerkon` (или ждать кнопок-комбинации — см. chest.ino)
- [ ] 👁️ ESP32 шлёт `suitcase_end` → server → Mega `serial_write_queue.put('suitcase_end')`
- [ ] 👁️ 🔧 `insideLed` загорается (подсветка внутри чемодана), `SHERIF_EM2` импульс — замок чемодана открыт
- [ ] Альтернатива: пульт → **Suitcase Skip|Open**

### 5.2 Safe (📡 ESP32)
- [ ] 👆 Поверни 2 геркона `REED_SWITCH_1_PIN` (36), `REED_SWITCH_2_PIN` (32) на сейфе И положи мяч на `BALL_SENSOR_PIN` (39)
- [ ] 👁️ ESP32 шлёт `safe_end` → 🔧 `LOCKER_PIN` (18) импульс → откр.
- [ ] 👁️ 🔧 LED_PIN_1 (21), LED_PIN_2 (22) подсветка сейфа
- [ ] 👁️ DFPlayer Mini играет TRACK_STORY_28 (звук из сейфа)
- [ ] Альтернатива: пульт → **Golden Safe Skip|Open**

### 5.3 Wolf (📡 ESP32)
- [ ] 👆 Поверни луну, тыкай облака — `MoonGame` → `LeftCloudGame` → `CloudGame` → `WolfGame` (см. wolf.ino, последовательность игр)
- [ ] 👁️ Sensoros (PCF8574 INPUTS): moon(32), wolf(33), help(23), ghost(25), leftCloud(35), rightCloud1(36/39/34) — все читаются
- [ ] 👁️ Выходы PCF8574 OUTPUTS: SH1 (электрозамок волка), wolfEyeLed, leftCloud/rightCloud-LED, moonLed, three1/2/3Led
- [ ] 👁️ NeoPixel: wolfLed[10] (eye/туман, GPIO 19), threeLed[10] (троица, GPIO 18)
- [ ] 👁️ Финал: ESP32 шлёт `wolf_end` → Mega
- [ ] Альтернатива: пульт → **Wolf Skip|Open**

### 5.4 Завершение level 5
- [ ] 👁️ Когда все 3 флага установлены → Mega шлёт `three_game_end` → level++
- [ ] 👁️ Пульт: все три индикатора (Suitcase / Golden Safe / Wolf) зелёные

---

## 6. Игра «Флаги»: `Flags()` — level=6

🎯 В каждой из 4 башен (Workshop, Basket, Dog, Owls) есть IR-датчик флага. На каждый надо поднять флажок (бумажный с IR-отражателем).

**Код:** ждёт 4 флага `FirstFlag && SecondFlag && ThirdFlag && FourFlag`.

- [ ] 👆 Workshop 🏰: подними флажок к `FLAG_IR_SENSOR_PIN` (27) → шлёт `flag1_on`
- [ ] 👆 Basket 🏰: IR-датчик флага → шлёт `flag2_on`
- [ ] 👆 Dog 🏰: `FLAG_IR_SENSOR_PIN` (7) → шлёт `flag3_on`
- [ ] 👆 Owls 🏰: `PIN_IR_FLAG` (27) → шлёт `flag4_on`
- [ ] 👁️ Mega шлёт `flagsendmr` → audio → level++
- [ ] Пульт: индикатор **Flags** зелёный
- [ ] Альтернатива: пульт → **Open Mansard Stash (Skip)** (команда `m2lck`)

---

## 7. Большая «карта» — поиск 5 ингредиентов: `MapGame()` — level=7

🎯 Игроки получают карту со «слотами» (key/fish/owl/train) и должны положить нужный артефакт в каждый слот → запускается мини-игра в башне. **Все 5 ингредиентов:** Potion, Crystal (Dog), Owl (Owls), Train (Train ESP32), Troll/Metal (Basket).

### 7.1 Слот «Cat»: открытие комнаты зельеварения
- [ ] 👆 На карте «cat» рычаг → `!digitalReadExpander(0, board1)` (геркон 1 на board1, addr 000, Y=21) или пульт → **Cat Open Door**
- [ ] 👁️ Mega шлёт `door_witch` → 🔧 OpenLock(PotionsRoomDoor) → реле A3 импульс
- [ ] 👁️ 🔧 CauldronRoomStrip (NeoPixel, pin 12, 12 диодов) → плавное появление цвета `Color(255, 197, 143)` (255 шагов × 2 мс)

### 7.2 Игра «Бутылки» в комнате зельеварения: `Bottles()`
🎯 Поставить 4 RFID-бутылки в котёл в правильной последовательности.

Правильный порядок (по validHex[]):
1. **bottle с UID 0x1CA** (SecondBottle → шлёт `first_bottle`)
2. **bottle с UID 0x1C9** (FirstBottle → шлёт `second_bottle`)
3. **bottle с UID 0x1CB** (ThirdBottle → шлёт `third_bottle`)
4. **bottle с UID 0x1CC** (FourBottle → шлёт `four_bottle`)

- [ ] 👆 Поставь бутылку 1 в котёл (RFID OneWire pin 13)
- [ ] 👁️ 🔧 CauldronRoomStrip → зелёный, CauldronStrip — `CauldronTrueFire()` (10 диодов, плавный фиолетовый-зелёный)
- [ ] 👆 Убери → ставь бутылку 2 → CauldronRoomStrip → жёлто-зелёный
- [ ] 👆 Бутылка 3 → белый
- [ ] 👆 Бутылка 4 → шлёт `four_bottle` → isPotionEnd=true
- [ ] ⚠️ Ошибка: неправильная бутылка → CauldronMistakeFire() (красное мерцание, 10 диодов) + 2 c rfidCooldownEnd
- [ ] 👁️ После `isPotionEnd` → ждать `digitalReadExpander(4, board1)` (геркон финального предмета) → шлёт `item_find:potion` → потионная пульсация (GoldStrip + CauldronStrip)
- [ ] Альтернатива: **Bottles Open Potions Stash (Skip)**

### 7.3 Слот «Owl»: 🏰 Owls tower
- [ ] 👆 На карте — артефакт "Owl" в слот → server → Mega → mySerial `owl` → башня Owls активирует игру
- [ ] 👁️ Башня Owls: подсветка двери `PIN_LOKER_DOOR` (23) — открытие, светодиоды плиток `tileLeds[4]`
- [ ] 👆 Нажать `PIN_HERKON_OWA` (29) → owl button → башня шлёт `door_owl` → Mega шлёт `door_owl`
- [ ] 👆 Решить пазл с 4 плитками + 2 галетных переключателями (Left D46/44/42/40, Right D32/34/36/38)
- [ ] 👁️ Башня шлёт `owl_end` → Mega `light_off` всем башням → isOwlEnd=true
- [ ] Альтернатива: пульт → **Owl Open Door** или **Owls Skip**

### 7.4 Слот «Train»: 📡 Train ESP32
- [ ] 👆 На карте — артефакт "Train" → server → Train ESP32 → запускает MapLeds + поезд
- [ ] 👁️ Train: 30 NeoPixels (LED_PIN 17), 4 на DATA_PIN1 (5), TUNNEL_LED (23), UF_LED (19)
- [ ] 👆 Игрок крутит 3 энкодера, ждёт зелёного цвета на табло → жмёт «start»
- [ ] 👆 Knock-сенсор (vibration на pin 36) — детонирует «гром»
- [ ] 👁️ Train шлёт `train_active` → `train_end` → Mega шлёт Serial2 `start_troll` (будит тролля) → isTrainEnd=true
- [ ] Альтернатива: пульт → **Train Skip**, **Projector Skip**

### 7.5 Слот «Pedlock/Dog»: 🏰 Dog tower
- [ ] 👆 Игрок открывает навесной замок `PADLOCK_REED_PIN` (A5) → башня шлёт лог `Padlock open` → Mega `door_dog`
- [ ] 👆 Дальше — поверни колесо, найди кристалл, рыцарь и т.д. (см. resetQuestState/activateEndStage в dog.ino)
- [ ] 👁️ 🔧 VIBRO_MOTOR_PIN (6) — мотор-вибратор работает в pattern `VIBRO_ON_DURATIONS[]`
- [ ] 👁️ 🔧 LIGHTING_LED_PIN (9, PWM) плавно загорается
- [ ] 👁️ 🔧 CRYSTAL_LIGHT_PIN (3) плавно мигает кристалл
- [ ] 👁️ 🔧 LED_STRIP_PIN (4) — лента 12V через реле
- [ ] 👁️ 🔧 ROOF_LIGHTING_PIN (2)
- [ ] 👁️ 🔧 CAGE_LOCK_PIN (5) — реле замка клетки (импульс 500 мс)
- [ ] 👁️ 🔧 DOOR_LOCK_PIN (10) — реле двери
- [ ] 👁️ Финал: `END_REED_PIN` (A1) → лог `FINISHED` или `WIN: Dog locked` → Mega `dog_lock` → isDogEnd=true
- [ ] Альтернатива: пульт → **Pedlock Open Door**, **Dog Skip**

### 7.6 Слот «Mine/Troll»: 🏰 Basket tower (троль в пещере)
- [ ] 👆 Артефакт «Mine» на карту → Mega шлёт `Serial2 mine` → башня Basket открывает пещеру
- [ ] 👆 Игроки находят 3 куска руды (aluminium / bronze / copper)
- [ ] 👁️ Каждый кусок: башня шлёт `aluminium`/`bronze`/`copper` → Mega `cave_search1/2/3` → audio
- [ ] 👁️ Финал: башня шлёт `metal` → Mega рисует GoldStrip жёлтый + ответ всем башням `item_find`
- [ ] 👁️ Затем `cave_end` → isTrollEnd=true
- [ ] Альтернатива: пульт → **Mine Open Door**, **Troll Skip**

### 7.7 Завершение карты
- [ ] 👁️ Когда все 5 флагов `isPotionEnd && isDogEnd && isOwlEnd && isTrainEnd && isTrollEnd` → Mega шлёт `material_end` → level++ через 1 с

---

## 8. Открытие банка: `OpenBank()` — level=8

🎯 Игрок дует в окно (датчик пересечения IR-луча).

**Код:** ждёт `digitalRead(WindowSens)` (A13).

- [ ] 👆 Дунуть на IR-датчик WindowSens (A13) — луч прерывается
- [ ] 👁️ 🔧 CandleStrip → Color(0,0,0) (свеча гаснет)
- [ ] 👁️ Mega шлёт `miror` → server → audio + история
- [ ] 👁️ Каждые 30 c (если игроки залипают) — random подсказка `story_22_a/b/c`
- [ ] 👁️ scrollNumber=-1 (флаг для перехода в Scrolls)
- [ ] Альтернатива: пульт → **Open Bank Door**

---

## 9. Игра «Свитки/сейф в банке»: `Scrolls()` — level=9

🎯 Игрок поворачивает 4 рычажка (свитка) в правильной последовательности. На каждый свиток — отдельный геркон на board3.

**Правильная последовательность:**
1. ScrollOne: геркон 1 (board3 pin 2)
2. ScrollTwo: геркон 2 (board3 pin 0)
3. ScrollThree: геркон 3 (board3 pin 3)
4. ScrollFour: геркон 2 (board3 pin 0) — повторно
5. ScrollFive: геркон 4 (board3 pin 1)

- [ ] 👆 Сначала `open_bank` (от Mega приходит после miror) → 🔧 BankRoomLight (4) HIGH, 🔧 OpenLock(BankDoor A4)
- [ ] 👆 Шаг 1: магнит на board3 pin 2 → удерживай 2 c → шлёт `safe_turn` → `safe_fix` → `safe_step_1`
- [ ] 👆 Шаг 2: магнит на board3 pin 0 → `safe_step_2`
- [ ] 👆 Шаг 3: board3 pin 3 → `safe_step_3`
- [ ] 👆 Шаг 4: board3 pin 0 → `safe_step_4`
- [ ] 👆 Шаг 5: board3 pin 1 → `safe_end` → 🔧 GoldStrip жёлтый → scrollNumber=5
- [ ] 👁️ Через 4 c автомата → OpenLock(BankStashDoor A10) → level++ (Oven)
- [ ] ⚠️ Если перепутать порядок → `safe_reset` → начать сначала
- [ ] Альтернатива: пульт → **Safe Open**

---

## 10. Игра «Печь у мастера»: `Oven()` — level=10

🎯 Финальное собирание ингредиентов у мастера (Workshop). Игрок нажимает «золото» / поверяет ингредиенты.

- [ ] 👆 Нажать `goldButton` (47) — кнопка золота
- [ ] 👁️ Mega шлёт `gold` всем башням + `item_find:gold`
- [ ] 👁️ 🔧 CauldronStrip → фиолетовый, GoldPulsation() работает
- [ ] 👆 Альтернатива: геркон зелья на board1 pin 4 → шлёт `potion` всем → PotionPulsation
- [ ] 👆 Workshop 🏰: игрок взаимодействует с верстаком (4 геркона REED_WORKBENCH1-4 на pins 44/42/40/46), сервы шлем (HELMET 50), метла (BROOM 48)
- [ ] 👆 Workshop геркон печки `REED_SWITCH_PIN` (31) → fire1/2/3
- [ ] 👁️ 4 NeoPixel WORKBENCH_NEOPIXEL (9), стрипы 47 ROOF_LIGHT, LED_FLOOR1 (23), LED_FLOOR2 (12), LED_PIN_FIRE (7)
- [ ] 👁️ Финал: Workshop шлёт `story_35` → Mega: всё гасит (см. длинный блок digitalWrite) → HallLight HIGH → level++
- [ ] Альтернатива: пульт → **Workshop Skip**

---

## 11. Игра в библиотеке — призраки: `Library()` — level=11

🎯 Сначала 2 стрелки старинных часов, потом цепочка призраков (3 истории), потом стук в дверь.

**State machine `ghostState`:**
- case 0: ждёт board2 pin 0 (12 hours)
- case 1: ждёт board1 pin 6 (UF clock)
- case 2-4: ждёт команды `ghost` от пульта (3 истории: train/wolf/train → story_40/41/42)
- case 6: ждёт удара пьезо (`KnockSens` A0 < 300)

- [ ] 👆 Магнит board2 pin 0 → `h_clock` → UfHallLight HIGH, HallLight LOW
- [ ] 👆 Магнит board1 pin 6 → `uf_clock` → UF мерцание (6 циклов)
- [ ] 👁️ Через сервер кнопки: **Ghost (Train)** → story_40, **Ghost (Wolf)** → story_41, **Ghost (Train)** → story_42 + `ghost_knock`
- [ ] 👆 Ударь по `KnockSens` (пьезо A0) → analogRead < 300 → шлёт `punch` → 🔧 KnockSol (A8) выключается, 🔧 LibraryLight (A7) HIGH → level++
- [ ] Альтернатива: пульт → 5 кнопок Ghost Step 1..5 для пошагового скипа

---

## 12. Открытие двери в библиотеку: `LibraryGame()` — level=12

🎯 Игрок открывает физическую дверь в библиотеку.

**Код:** `digitalReadExpander(5, board3)`.

- [ ] 👆 Замкнуть геркон двери библиотеки (board3 pin 5)
- [ ] 👁️ Mega шлёт `lib_door` → 🔧 LibraryLight (A7) LOW → level++
- [ ] Альтернатива: пульт → **Door Puzzle Skip|Open**

---

## 13. Центральная башня — низ: `CentralTowerGame()` — level=13

🎯 Игрок берёт кубок, ставит у камина (геркон), потом возвращает на место (другой геркон).

**State machine `centralTowerState`:**
- case 0: ждёт геркон камина board4 pin 3 → шлёт `fire`. Когда отпускают → переход case 1.
- case 1: ждёт геркон кубка board4 pin 2 → `door_basket` + Serial2 `opent_basket` → fireFlag=1 → level++

- [ ] 👆 Замкнуть геркон камина (board4 pin 3) → `fire` audio
- [ ] 👆 Отпустить и замкнуть геркон кубка (board4 pin 2)
- [ ] 👁️ 🔧 OpenLock(BankStashDoor) реле, Mega шлёт `door_basket`
- [ ] 👁️ fireFlag=1 → потом каждый цикл вызывает `FireCup()` — strip2 pixel 65 случайный жёлтый
- [ ] Альтернатива: пульт → **Cup Skip|Open**

---

## 14. Центральная башня — верх (заклинание свайпами): `CentralTowerGameDown()` — level=14

🎯 Игрок водит магической палочкой влево-вправо вдоль 2 герконов. Последовательность: R-L-R-L-R (5 свайпов).

**Сенсоры:** board4 pin 0 (Left), board4 pin 1 (Right). Debounce 30 мс, swipe-timeout 1500 мс.

- [ ] 👆 Свайп вправо #1 (L→R) → `swipe_r` + `spell_step_1`
- [ ] 👆 Свайп влево #1 (R→L) → `swipe_l` + `spell_step_2`
- [ ] 👆 Свайп вправо #2 → `spell_step_3`
- [ ] 👆 Свайп влево #2 → `spell_step_4`
- [ ] 👆 Свайп вправо #3 → `spell_step_5` → 🔧 OpenLock(HightTowerDoor A9), 🔧 TorchLight (7) HIGH, шлёт `door_spell` → level++
- [ ] ⚠️ Неправильная последовательность → `swipe_wrong_sequence` + `spell_reset` → начать сначала
- [ ] Альтернатива: пульт → **Spell Open Door**

---

## 15. Финальный кристалл: `SealSpace()` — level=15

🎯 Игрок снимает кристалл с пьедестала (геркон отпускается = HIGH через INPUT_PULLUP).

**Код:** `digitalReadExpander(4, board4)` HIGH + debounce 500 мс.

- [ ] 👆 Снять кристалл (отжать геркон board4 pin 4)
- [ ] 👁️ Mega шлёт `cristal_up` → 🔧 OpenLock(MemoryRoomDoor A2) → level++
- [ ] Альтернатива: пульт → **Crystals Open Door**

---

## 16. Комната воспоминаний: `MemoryRoom()` — level=16

🎯 4 цветных кристалла + симона-подобная игра. State machine `_stages`:
- stage 0: `giftGame()` — ждёт ПОДНЯТИЯ всех 4 кристаллов (digitalRead = HIGH через INPUT_PULLUP). По мере поднятия LED групп i..i+3 на 41-диодной memory_Led гасятся.
- stage 1: `startGame()` — ждёт пока 4 кристалла НА МЕСТАХ → белая подсветка → переход в stage 2
- stage 2: `_presentation()` — показ последовательности цветных групп (i: 16-19, 20-23, 24-27 → 4 уровня)
- stage 3: `Game()` — игрок повторяет последовательность снимая/возвращая кристаллы

**Сенсоры:** firstCrystal (27), secondCrystal (24), thirdCrystal (23), fourCrystal (25). Все INPUT_PULLUP.
**Выход:** 🔧 memory_Led (NeoPixel, pin 11, 41 диод).

- [ ] 👆 Подними кристалл 1 (отжать геркон firstCrystal 27) → LED 0-3 гаснут
- [ ] 👆 Аналогично 2/3/4 — все 4 кристалла подняты → _stages=1
- [ ] 👆 Верни 4 кристалла обратно → белый свет → _stages=2 (presentation)
- [ ] 👁️ Smooth-fade groups + цветные паттерны (4 уровня: 250/250/0, 0/0/250 и т.д.)
- [ ] 👆 Повтори последовательность 4 раза (снимай-ставь правильные цвета)
- [ ] 👁️ Финал: server шлёт `memory_room_end` → Mega запускает 2-секундную радугу по memory_Led + затемнение → 🔧 LastTowerTopLight HIGH + 🔧 TorchLight HIGH + OpenLock(CrimeDoor 34) → level++
- [ ] Альтернатива: пульт → **Crystals Open Door** прыгает прямо в финал

---

## 17. Тюрьма — освобождение «преступника»: `CrimeHelp()` — level=17

🎯 Игрок открывает «локер» (замок камеры) и поднимает мальчика (геркон).

**Код:** ждёт геркон board1 pin 7 → HIGH (был LOW, стал HIGH = мальчик встал)

- [ ] 👆 Открыть локер → геркон board1 pin 7 (boy_out) сначала LOW (мальчик в клетке)
- [ ] 👆 Поднять мальчика → геркон стал HIGH → шлёт `crime_end` → level++
- [ ] 👁️ Каждые 20 c — подсказка `story_55`
- [ ] Альтернатива: пульт → **Crime Open Door**

---

## 18. Урок баскетбола: `BasketLesson()` — level=18

🎯 Мальчик возвращается в баскет-башню (учитель), демонстрирует игру (показ-имитация).

**Башня Basket 🏰** — `BasketLesson()` в basket3.ino. Mega получает от Serial2 короткие команды:
- `C` — confirm watchdog OK
- `I` — boy_in_lesson
- `O` — boy_out_lesson
- `G` — lesson_goal
- `D` — flying_ball → level++

- [ ] 👁️ Когда `isTrainBasket` (приходит после восстановления из карты) → шлёт `story_59`
- [ ] 👁️ В башне Basket: IR-датчик мальчика `boyButton` (28) → отправляет I/O
- [ ] 👁️ В башне Basket: IR-датчик корзины `BASKET_IR_PIN` (A10) — забил → G
- [ ] 👁️ 🔧 Fireworks (3) — салют включается при `lesson_goal` (disco-balls active)
- [ ] 👁️ Затем «летающий мяч» → `D` → level++
- [ ] Альтернатива: пульт — кнопки **start_lesson**, **start_game_basket**, **force_win_bask**

---

## 19. Финальная игра баскет: `Basket()` — level=19

🎯 Игрок vs бот, до 2 очков. Бот «крадёт» мяч (BotScore 1→2). Игрок должен забить 2 раза.

**Команды от basket3.ino:** I/O/1/2/4/5/3/6/S
- `I` — boy_in_game
- `O` — boy_out_game
- `1` — goal_1_player (игрок забил 1й гол)
- `2` — goal_2_player (победа)
- `4` — goal_1_bot
- `5` — goal_2_bot
- `3` — `win` событие → 🔧 Fireworks HIGH → level=20
- `S` — start_snitch (запуск мяча)
- `6` — win_robot (бот победил)

- [ ] 👆 Игрок занимает позицию (boyButton 28) → I
- [ ] 👆 Поймать мяч в корзину (BASKET_IR_PIN A10) — первый гол → `1` → 🔧 NeoPixel strip1/strip2 GreenWaveEffect, discoBallsActive
- [ ] 👆 Второй гол → `2` → … (зависит от логики basket3)
- [ ] 👁️ В коде Mega: `3` (win) → 🔧 Fireworks HIGH, level=20
- [ ] 👁️ После победы → FinalPresentation: MagicEffect (NeoPixel strip1/strip2 со звёздами), salute
- [ ] Альтернатива: пульт → **Basket Skip game** или **force_win_bask**

---

## 20. Финал и Ready/Restart: level=20 → level=26 → level=25

🎯 Победа, фейерверк, ожидание `ready` от оператора.

- [ ] 👁️ Mega играет `FinalPresentation()` — Fireworks HIGH + MagicEffect (рисует звёзды на 2 NeoPixel-полосах strip1/strip2)
- [ ] 👁️ NeoPixel-эффекты на всех 252 LED каждой полосы — `palette[]` 6 цветов (магическая палитра)
- [ ] 👆 Оператор жмёт **Ready** на пульте → Mega: рассылает `ready` всем 4 башням, тушит весь свет, level=26 (`ListenForReady` 3 c)
- [ ] 👁️ В режиме listen Mega шлёт snapshot статуса всех сенсоров (open_door / galet_on / cristal_up / boy_out / lib_door / crime_open / safe_open)
- [ ] 👆 Оператор жмёт **Restart** → Mega: SendRestartToAll + OpenAll (открыть 9 дверей) + RestOn → level=25
- [ ] 👁️ Готов к новой игре

---

## DIAG (Mega) — параллельный тест без квеста

Если нужно проверить выход (свет/дверь/сервер) без прохождения сценария: пульт → Tech-pult → **DIAG Mega ON** (`dg_on`). Активно ТОЛЬКО в level=0 (idle, ещё не нажали Start). Доступные команды:

- [ ] `dg_d1..dg_d9` — toggle каждая из 9 дверей (импульс 500 мс)
- [ ] `dg_fw` — toggle Fireworks (pin 3)
- [ ] `dg_sn` — request snapshot (вернёт `dgs:<csv>` с состоянием каждого пина)

⚠️ См. [project_clc_diag_pending_2026_05_23] — для башен (Workshop/Basket/Dog/Owls) DIAG пока не реализован, на 2026-05-24. Тестировать пины башен — через прохождение сценария.

---

## ESP32 устройства — отдельный DIAG (после квеста)

Каждое ESP32 имеет вкладку DIAG в Tech-пульте. Включи DIAG → snapshot 5×/c + кнопки на каждый выход. Чеклист физических компонентов:

### 📡 Train (ESP32)
- [ ] DIAG ON
- [ ] Toggle TUNNEL_LED (23) — лампа в туннеле горит/не горит
- [ ] Toggle UF_LED (19) — УФ
- [ ] NeoPixel LED_PIN (17, 30 диодов) — задать цвет
- [ ] NeoPixel DATA_PIN1 (5, 4 диода)
- [ ] Snapshot: knock-сенсор (vibration pin 36) — тыкни → vibCount++
- [ ] Encoders 1/2/3 (через PCF8574 INPUTS) — повернуть, цифры меняются
- [ ] OUTPUTS PCF8574 — каждый pin 0..7 toggle (если есть)

### 📡 Wolf (ESP32)
- [ ] DIAG ON, snapshot shows: moon(32), wolf(33), help(23), ghost(25), leftCloud(35), rightCloud1(36), rightCloud2(39), rightCloud3(34)
- [ ] Toggle `moon_led` (PCF8574 OUT pin 7)
- [ ] Toggle `left_cloud` (pin 3), `right_cloud` (pin 1), `wolf_eye` (pin 2)
- [ ] Toggle `three1`/`three2`/`three3` (pins 4/5/6)
- [ ] `lock_pulse` — 500 мс импульс на SH1 (pin 0) — замок волка щёлкает
- [ ] NeoPixel `wolf_strip` (GPIO 19, 10 диодов) — задать hex
- [ ] NeoPixel `three_strip` (GPIO 18, 10 диодов)
- [ ] DFPlayer `play` — проверь динамик каждым треком (TRACK_FON_WOLF, TRACK_CLOUD, TRACK_ghost)
- [ ] `volume` (0..30), `stop_audio`, `reset_vib`

### 📡 Safe (ESP32)
- [ ] DIAG ON, snapshot: REED_SWITCH_1 (36), REED_SWITCH_2 (32), BALL_SENSOR (39)
- [ ] Toggle LED_PIN_1 (21), LED_PIN_2 (22)
- [ ] `lock_pulse` LOCKER_PIN (18) — замок сейфа
- [ ] DFPlayer `play` (TRACK_STORY_28 etc) — проверь динамик

### 📡 Suitcases / Chest (ESP32)
- [ ] DIAG ON, snapshot кнопок (butt1..butt4), helpButton, eyeSymbolGerkon
- [ ] Toggle mistakeLed (красный)
- [ ] Toggle insideLed (подсветка внутри)
- [ ] `lock_pulse` SHERIF_EM2 — замок чемодана
- [ ] Toggle ledsSym[] (массив LED-символов) — каждый индивидуально
- [ ] DFPlayer `play` (TRACK_FON_SUITCASE, TRACK_STORY_8) — динамик

---

## Анти-чеклист (известные слабые места, обязательно проверить)

- [ ] ⚠️ **boyServo pin 49** — на CLC1 был «глухой» (см. [project_clc_servo_pin49_test]); проверить движение 0°→170° при открытии стартовой двери
- [ ] ⚠️ **USB hub** — на CLC3 был underpowered (см. [project_clc_usb_hub_underpowered]); если -110 errors → менять хаб на powered 5V/3A
- [ ] ⚠️ **Mega может не сбрасываться после avrdude** (см. [feedback_mega_post_avrdude_reset]); если on.wav не сыграл — физический reset
- [ ] ⚠️ **wlan1 (USB-WiFi для ESP32)** — может отвалиться, watchdog в сервере (см. [project_clc_wlan1_watchdog])
- [ ] ⚠️ **DIAG в level=0 only** — не пытаться включить во время квеста, иначе двери откроются и игра сломается
- [ ] ⚠️ **Галетники level=4** — если все 5 уже UP до старта, надо опустить и поднять (предохранитель `galetGameReadyToWin`)
- [ ] ⚠️ **Scrolls level=9** — правильный порядок герконов: pin2 → pin0 → pin3 → pin0 → pin1. Не путать!
- [ ] ⚠️ **Spell level=14** — последовательность R-L-R-L-R. Если водить только в одну сторону, не засчитается
- [ ] ⚠️ **Bottles level=7** — правильный порядок UID: 0x1CA → 0x1C9 → 0x1CB → 0x1CC. Неправильная → 2 c cooldown красным
- [ ] ⚠️ **Memory Room level=16** — 4 кристалла должны сначала ВСЕ быть подняты (giftGame), потом ВСЕ опущены (startGame), потом игра

---

## Итог теста

После полного прохождения нажми **Restart** → все 9 дверей открываются → пройди ещё раз быстро через скипы. Каждый «Skip» на пульте имитирует прохождение этапа без физического действия — это позволяет за 5–10 мин проверить что вся цепочка реле/audio/индикаторы работают, даже если какой-то сенсор тестировался отдельно через DIAG.

**Общее число этапов с физическими действиями игрока:** 20 (level 1..20).
**Общее число дверных реле:** 9 (Mega) + 4 (башни — workshop LOCK, basket SHERIF_EM1/EM2, dog DOOR/CAGE, owls PIN_LOKER) = 13 замков.
**Общее число NeoPixel-полос:** 5 Mega (GoldStrip, CandleStrip, CauldronStrip, CauldronRoomStrip, memory_Led) + strip1/strip2 (252 LED каждая, баскет-комета) + башенные (workshop verbench 4, dog crystal, owls tiles 4, train 30+4, wolf 10+10, three 10).
**Минимальное время полного теста:** ~60 мин при ровной работе, +30 мин если что-то ломается.
