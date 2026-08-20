#include <Arduino.h>
#define BRIGHTNESS 255  // Яркость (0-255)

#define NUM_LEDS_FIRE 3  // Количество светодиодов для имитации огня
#define LED_PIN_FIRE 7   // Пин для NeoPixel

#define LOCK_PIN 21        // Пин, к которому подключен замок
#define LED_FLOOR1_PIN 23  // Пин для LED 1 этаж
#define LED_FLOOR2_PIN 12  // Пин для LED 2 этаж
#define ROOF_LIGHT_PIN 47  // Пин для подсветки окна крыши

#define REED_SWITCH_PIN 31       // Геркон печки
#define HINT_REED_SWITCH_PIN 29  // Геркон подсказки

// --- Пины для верстака ---
#define WORKBENCH_NEOPIXEL_PIN 9  // Пин для адресных светодиодов верстака (4 штуки)
#define REED_WORKBENCH1_PIN 44    // Геркон верстака 1
#define REED_WORKBENCH2_PIN 42    // Геркон верстака 2
#define REED_WORKBENCH3_PIN 40    // Геркон верстака 3
#define REED_WORKBENCH4_PIN 46    // Геркон верстака 4
#define NUM_LEDS_WORKBENCH 4      // Количество светодиодов верстака
// --- Конец пинов верстака ---

// --- Пины для сервоприводов ---
// Шлем и метла. На всех замках это светодиоды, и висят они на пинах 50/48.
// На CLC1 (Оман) вместо них стоят СЕРВОПРИВОДЫ, и подключены они к пинам 5/3.
// Это единственное отличие башни Мастерской у CLC1. Держим его развилкой в
// одном файле, а не отдельной копией: две копии уже разъезжались у главной
// платы, и каждая правка вносилась дважды.
// Сборка под Оман: arduino-cli compile ... \
//   --build-property compiler.cpp.extra_flags=-DWORKSHOP_OMAN_SERVOS=1
#ifndef WORKSHOP_OMAN_SERVOS
#define WORKSHOP_OMAN_SERVOS 0
#endif
#if WORKSHOP_OMAN_SERVOS
  #define HELMET_SERVO_PIN 5   // Сервопривод "шлем" (CLC1)
  #define BROOM_SERVO_PIN 3    // Сервопривод "метла" (CLC1)
#else
  #define HELMET_SERVO_PIN 50  // Светодиод "шлем"
  #define BROOM_SERVO_PIN 48   // Светодиод "метла"
#endif
// --- Конец пинов сервоприводов ---

#define GALET_PIN 30           // Галетник солнце
#define FLAG_IR_SENSOR_PIN 27  // Сенсор флаг

#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>
#if WORKSHOP_OMAN_SERVOS
#include <Servo.h>
Servo helmetServo;
Servo broomServo;
#endif

// Объект NeoPixel для огня
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS_FIRE, LED_PIN_FIRE, NEO_GRB + NEO_KHZ800);
// Новый объект NeoPixel для верстака
Adafruit_NeoPixel workbenchStrip = Adafruit_NeoPixel(NUM_LEDS_WORKBENCH, WORKBENCH_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

float heat = 0.0;
float coolingRate = 0.01;      // Скорость остывания
float heatingRate = 0.55;      // Скорость нагрева
float flickerIntensity = 0.1;  // Интенсивность мерцания
bool _restartGalet = 0;
bool _restartFlag = 0;

unsigned long previousMillis = 0;
const long interval = 40;  // Интервал обновления (мс) для эффекта мерцания

int fireStage = 0;                      // Текущая стадия огня (количество горящих светодиодов)
unsigned long fireTimer;                // Таймер для отслеживания 5-секундного интервала активности
const long FIRE_ACTIVE_TIMEOUT = 5000;  // 5 секунд ожидания активности геркона

unsigned long fadeOutTimer = 0;       // Таймер для затухания
const long FADE_OUT_INTERVAL = 5000;  // Интервал затухания для каждого светодиода (5 секунд)

unsigned long lastActivationTime = 0;
const long MIN_ACTIVATION_INTERVAL = 2000;

int ignitingLedIndex = -1;
unsigned long ignitingStartTime = 0;
const uint32_t IGNITING_COLOR_1 = strip.Color(255, 60, 0);
const uint32_t IGNITING_COLOR_2 = strip.Color(255, 165, 0);

bool lockOpen = false;
unsigned long lockOpenTime = 0;
const unsigned long lockOpenDuration = 500;

bool recurringLockActive = false;
unsigned long lastRecurringLockOpenTime = 0;
// 2026-07-31: было 10000 — БАГ. Keep-alive удержания был 8000 мс, то есть
// окно закрывалось РАНЬШЕ, чем наступал повторный импульс, и одиночный 500мс
// импульс так и не пробивал соленоид (жалоба клиента CLC2: дверь Workshop не
// открылась ни по игровой команде, ни с пульта — помогло только физически).
// В basket3.ino та же схема работает с интервалом 3000 при keep-alive 8000 —
// за окно умещается 2-3 импульса. Приводим к тому же соотношению.
const unsigned long RECURRING_LOCK_INTERVAL = 3000;

// 2026-06-04: manual-hold таймер для open_door/restart (тех-пульт). Аналог
// manualEMxUntil в basket3.ino (commit a2761a8). Одиночный 500мс импульс
// в open_door не пробивает соленоид без keep-alive (recurringLockActive
// сбрасывается в restart/ready и пульсирование останавливается). С этим
// таймером manageLock() пульсировал locker ещё 8 секунд после
// открытия с пульта. См. memory [[clc-basket-door-restart-fix-2026-05-28]].

bool floorLedsOn = false;

int workbenchMode = 0;
bool workbenchLedsInitialized = false;

bool workbench1PressedBeforeReady = false;
bool workbench2PressedBeforeReady = false;
bool workbench3PressedBeforeReady = false;
bool workbench4PressedBeforeReady = false;

int workbenchLedStates[NUM_LEDS_WORKBENCH] = { 0, 0, 0, 0 };

const uint32_t WORKBENCH_COLOR_GOLD = workbenchStrip.Color(255, 255, 0);
const uint32_t WORKBENCH_COLOR_POTION = workbenchStrip.Color(128, 0, 128);
const uint32_t WORKBENCH_COLOR_SKIN = workbenchStrip.Color(0, 128, 0);
const uint32_t WORKBENCH_COLOR_CRYSTAL = workbenchStrip.Color(255, 128, 0);
const uint32_t WORKBENCH_COLOR_METAL = workbenchStrip.Color(0, 128, 128);
const uint32_t WORKBENCH_COLOR_WHITE_50 = workbenchStrip.Color(127, 127, 127);

const uint32_t HELMET_COMBINATION[] = {
  workbenchStrip.Color(0, 128, 0),
  workbenchStrip.Color(255, 255, 0),
  workbenchStrip.Color(255, 128, 0),
  workbenchStrip.Color(255, 255, 0)
};
const uint32_t BROOM_COMBINATION[] = {
  workbenchStrip.Color(0, 128, 128),
  workbenchStrip.Color(0, 128, 0),
  workbenchStrip.Color(0, 128, 128),
  workbenchStrip.Color(128, 0, 128)
};

bool wbTestActive = false;
byte wbTestCombo = 0;                     // 0 живой тест, 1 метла, 2 шлем
byte wbTestPrev[NUM_LEDS_WORKBENCH] = { 0, 0, 0, 0 };
unsigned long wbTestChanged[NUM_LEDS_WORKBENCH] = { 0, 0, 0, 0 };
const unsigned long WB_TEST_DEBOUNCE = 60;
const int WB_REED_PINS[NUM_LEDS_WORKBENCH] = {
  REED_WORKBENCH1_PIN, REED_WORKBENCH2_PIN, REED_WORKBENCH3_PIN, REED_WORKBENCH4_PIN
};

bool helmetServoActivated = false;
bool broomServoActivated = false;

bool gameEnded = false;

bool isFirstFire1 = false;
bool isFirstFire2 = false;
bool isFirstFire0 = false;
bool hasSentReadyLog = false;

// НОВЫЕ ПЕРЕМЕННЫЕ для анимации
bool isCelebrationActive = false;
bool fireworkActive = false; // Флаг для фейерверка
unsigned long celebrationStartTime = 0;

GButton butt1(REED_SWITCH_PIN);
GButton hintButt(HINT_REED_SWITCH_PIN);
GButton workbenchButt1(REED_WORKBENCH1_PIN);
GButton workbenchButt2(REED_WORKBENCH2_PIN);
GButton workbenchButt3(REED_WORKBENCH3_PIN);
GButton workbenchButt4(REED_WORKBENCH4_PIN);
GButton galetButton(GALET_PIN);
GButton flagButton(FLAG_IR_SENSOR_PIN);

int state = 0;

void setup();
void loop();
void handleFireLogic();

void sendLog(String message) {
  Serial1.println("log:workshop:" + message);
}

void updateFireEffect(int stage);
void GenerateFire();
uint32_t calculateFireColor(float heatValue);
void handleCelebrationEffect();
void setWorkbenchLedColor(int ledIndex, uint32_t color);
void handleWorkbenchReedSwitches();
void wbTestReset();
bool handleWbTestCmd(const String &cmd);
void handleWorkbenchTest();
void updateWorkbenchLeds();
void activateHelmetServo();
void activateBroomServo();
void checkWorkbenchCombinations();
void checkGameEnd();
void handleUartCommands();
void openLock();
void manageLock();
void CheckState(bool force = false);
const unsigned long FLAG_RESEND_MS = 5000;
unsigned long lastFlagResend = 0;
void setup() {
  Serial1.begin(9600);
  Serial1.setTimeout(10);

  // Опознавательный баннер для тех-пульта — см. пояснение в owls.ino.
  // USB-Serial у этой башни больше ни для чего не используется: связь с главной
  // платой идёт по Serial1. Именно Workshop с Basket и путали местами на CLC3,
  // так что здесь баннер нужнее всего.
  Serial.begin(9600);
  for (uint8_t i = 0; i < 3; i++) {
    Serial.println(F("CLC-TOWER:workshop"));
    delay(80);
  }

  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  strip.show();

  workbenchStrip.begin();
  workbenchStrip.setBrightness(BRIGHTNESS);
  workbenchStrip.clear();
  workbenchStrip.show();

  pinMode(LOCK_PIN, OUTPUT);
  pinMode(LED_FLOOR1_PIN, OUTPUT);
  pinMode(LED_FLOOR2_PIN, OUTPUT);
  digitalWrite(LED_FLOOR2_PIN, LOW);
  pinMode(ROOF_LIGHT_PIN, OUTPUT);

#if !WORKSHOP_OMAN_SERVOS
  // Настраиваем пины вместо сервоприводов на выход для LED
  pinMode(HELMET_SERVO_PIN, OUTPUT); // Пин 5
  pinMode(BROOM_SERVO_PIN, OUTPUT);  // Пин 3
#endif

  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(LED_FLOOR1_PIN, LOW);
  digitalWrite(LED_FLOOR2_PIN, LOW);
  digitalWrite(ROOF_LIGHT_PIN, LOW);

#if WORKSHOP_OMAN_SERVOS
  // Сервоприводы в начальное положение (закрыто) и снимаем питание:
  // под нагрузкой удержания серво гудит и греется.
  helmetServo.attach(HELMET_SERVO_PIN);
  broomServo.attach(BROOM_SERVO_PIN);
  helmetServo.write(140);
  broomServo.write(140);
  helmetServo.detach();
  broomServo.detach();
#else
  // Гасим светодиоды шлема и метлы на старте
  digitalWrite(HELMET_SERVO_PIN, LOW);
  digitalWrite(BROOM_SERVO_PIN, LOW);
#endif

  butt1.setDebounce(50);
  butt1.setTimeout(200);
  butt1.setType(HIGH_PULL);
  butt1.setDirection(NORM_OPEN);

  hintButt.setDebounce(50);
  hintButt.setTimeout(200);
  hintButt.setType(HIGH_PULL);
  hintButt.setDirection(NORM_OPEN);

  galetButton.setDebounce(50);
  galetButton.setTimeout(300);
  galetButton.setClickTimeout(600);
  galetButton.setType(HIGH_PULL);
  galetButton.setDirection(NORM_OPEN);

  flagButton.setDebounce(50);
  flagButton.setTimeout(300);
  flagButton.setClickTimeout(600);
  flagButton.setType(LOW_PULL);
  flagButton.setDirection(NORM_OPEN);

  workbenchButt1.setDebounce(50);
  workbenchButt1.setTimeout(200);
  workbenchButt1.setType(HIGH_PULL);
  workbenchButt1.setDirection(NORM_OPEN);
  workbenchButt2.setDebounce(50);
  workbenchButt2.setTimeout(200);
  workbenchButt2.setType(HIGH_PULL);
  workbenchButt2.setDirection(NORM_OPEN);
  workbenchButt3.setDebounce(50);
  workbenchButt3.setTimeout(200);
  workbenchButt3.setType(HIGH_PULL);
  workbenchButt3.setDirection(NORM_OPEN);
  workbenchButt4.setDebounce(50);
  workbenchButt4.setTimeout(200);
  workbenchButt4.setType(HIGH_PULL);
  workbenchButt4.setDirection(NORM_OPEN);

  fireTimer = millis();
}

void loop() {
  monPoll();                 // монитор датчиков для /tech (только по команде)
  static int previousState = -1;
  if (state != previousState) {
    previousState = state;
  }

  butt1.tick();
  hintButt.tick();
  workbenchButt1.tick();
  workbenchButt2.tick();
  workbenchButt3.tick();
  workbenchButt4.tick();

  galetButton.tick();
  if (galetButton.isPress() && _restartGalet == 0) {
    Serial1.println("galet_on");
    _restartGalet = 1;
  }
  if (galetButton.isRelease() && _restartGalet == 1) {
    Serial1.println("galet_off");
    _restartGalet = 0;
  }

  flagButton.tick();
  if (flagButton.isPress()) {
    Serial1.println("flag1_on");
  }
  if (flagButton.isRelease()) {
    Serial1.println("flag1_off");
  }

  if (hintButt.isPress()) {
    Serial1.println("help");
  }

  // Проверяем команды В НАЧАЛЕ, чтобы установить/сбросить флаг
  handleUartCommands(); 

  // Логика для светодиодов верстака (workbenchStrip)
  if (wbTestActive) {
    // Тест с тех-пульта забирает ленту верстака себе — и только её.
    handleWorkbenchTest();
  } else if (fireworkActive) {
    // Фейерверк "перехватывает" управление лентой верстака
    handleFirework();
  } else {
    // Иначе - обычная логика верстака
    if (isCelebrationActive) { // 
      handleCelebrationEffect(); // 
    } else {
      handleWorkbenchReedSwitches(); // 
      updateWorkbenchLeds(); // 
    }
  }

  // Логика для светодиодов огня (strip) работает независимо
  switch (state) {
    case 0:
      handleFireLogic();
      break;
  }

  // Остальная логика игры
  checkWorkbenchCombinations();
  handleUartCommands();
  manageLock();
  checkGameEnd();
}

// --- Функции, связанные с логикой огня ---

void handleFireLogic() {
  if (butt1.isPress() && (millis() - lastActivationTime >= MIN_ACTIVATION_INTERVAL)) {
    if (fireStage < NUM_LEDS_FIRE) {
      int oldFireStage = fireStage;
      fireStage++;

      if (fireStage == 1) {
        if (isFirstFire1) {
          Serial1.println("fire1");
          isFirstFire1 = false;
          isFirstFire0 = true;
        }
        workbenchLedsInitialized = false;
        workbench1PressedBeforeReady = false;
        workbench2PressedBeforeReady = false;
        workbench3PressedBeforeReady = false;
        workbench4PressedBeforeReady = false;
      } else if (fireStage == 2) {
        if (isFirstFire2) {
          Serial1.println("fire2");
          isFirstFire2 = false;
        }
        if (oldFireStage < 2) {
          workbenchLedsInitialized = false;
          workbench1PressedBeforeReady = false;
          workbench2PressedBeforeReady = false;
          workbench3PressedBeforeReady = false;
          workbench4PressedBeforeReady = false;
        }
      } else if (fireStage == 3) {
        Serial1.println("fire3");
      }

      ignitingLedIndex = NUM_LEDS_FIRE - fireStage;
      ignitingStartTime = millis();
    }
    fireTimer = millis();
    fadeOutTimer = 0;
    lastActivationTime = millis();
  }

  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    updateFireEffect(fireStage);
    strip.show();
  }

  if (fireStage > 0 && millis() - fireTimer >= FIRE_ACTIVE_TIMEOUT) {
    if (fadeOutTimer == 0) {
      fadeOutTimer = millis();
      ignitingLedIndex = -1;
    } else if (millis() - fadeOutTimer >= FADE_OUT_INTERVAL) {
      int prevFireStage = fireStage;
      strip.setPixelColor(NUM_LEDS_FIRE - fireStage, 0);
      fireStage--;

      if (prevFireStage == 3 && fireStage == 2) {
        if (!isFirstFire2) {
          Serial1.println("fire2");
          isFirstFire2 = true;
        }
        if (fireStage < prevFireStage) {
          workbenchLedsInitialized = false;
          workbench1PressedBeforeReady = false;
          workbench2PressedBeforeReady = false;
          workbench3PressedBeforeReady = false;
          workbench4PressedBeforeReady = false;
        }
      } else if (prevFireStage == 2 && fireStage == 1) {
        if (!isFirstFire1) {
          Serial1.println("fire1");
          isFirstFire1 = true;
        }
        workbenchLedsInitialized = false;
        workbench1PressedBeforeReady = false;
        workbench2PressedBeforeReady = false;
        workbench3PressedBeforeReady = false;
        workbench4PressedBeforeReady = false;
      }

      fadeOutTimer = millis();
      if (fireStage == 0) {
        heat = 0.0;
        if (isFirstFire0) {
          Serial1.println("fire0");
          isFirstFire0 = false;
          isFirstFire1 = true;
          isFirstFire2 = true;
        }
        workbenchStrip.clear();
        workbenchStrip.show();
        workbenchLedsInitialized = false;
        for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
          workbenchLedStates[i] = 0;
        }
        workbench1PressedBeforeReady = false;
        workbench2PressedBeforeReady = false;
        workbench3PressedBeforeReady = false;
        workbench4PressedBeforeReady = false;
      }
    }
  }
}


void updateFireEffect(int stage) {
  for (int i = 0; i < NUM_LEDS_FIRE; i++) {
    int currentLedIndex = i;
    bool shouldBeOn = (NUM_LEDS_FIRE - 1 - currentLedIndex) < stage;
    if (shouldBeOn) {
      if (currentLedIndex == ignitingLedIndex && (millis() - ignitingStartTime < MIN_ACTIVATION_INTERVAL)) {
        if ((millis() / 100) % 2 == 0) {
          strip.setPixelColor(currentLedIndex, IGNITING_COLOR_1);
        } else {
          strip.setPixelColor(currentLedIndex, IGNITING_COLOR_2);
        }
      } else {
        GenerateFire();
        uint32_t color = calculateFireColor(heat);
        strip.setPixelColor(currentLedIndex, color);
      }
    } else {
      strip.setPixelColor(currentLedIndex, 0);
    }
  }

  if (ignitingLedIndex != -1 && (millis() - ignitingStartTime >= MIN_ACTIVATION_INTERVAL)) {
    ignitingLedIndex = -1;
  }
}

void GenerateFire() {
  if (random(100) < 15) {
    heat += random(10, 30) / 100.0;
  }
  heat -= coolingRate;
  heat += (random(0, 100) / 100.0 - 0.5) * flickerIntensity;
  heat = constrain(heat, 0.0, 1.0);
}

uint32_t calculateFireColor(float heatValue) {
  byte r, g, b;

  if (heatValue < 0.3) {
    r = 255 * (heatValue / 0.3);
    g = 0;
    b = 0;
  } else if (heatValue < 0.7) {
    r = 255;
    g = 255 * ((heatValue - 0.3) / 0.4);
    b = 0;
  } else {
    r = 255;
    g = 15;
    b = 0;
  }
  return strip.Color(r, g, b);
}

// --- Функции для управления верстаком, серво и т.д. ---

// НОВАЯ ФУНКЦИЯ для анимации
void handleCelebrationEffect() {
  if (!isCelebrationActive) {
    return;
  }

  unsigned long elapsedTime = millis() - celebrationStartTime;

  if (elapsedTime < 4000) {
    // Используем синусоиду для плавной пульсации.
    float angle = (elapsedTime / 1000.0) * (2.0 * PI);
    float brightness_factor = (sin(angle) + 1.0) / 2.0;  // от 0.0 до 1.0
    uint8_t brightness = brightness_factor * 255;
    uint32_t pulseColor = workbenchStrip.Color(brightness, brightness, brightness);

    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      workbenchStrip.setPixelColor(i, pulseColor);
    }
    workbenchStrip.show();
  } else {
    // Анимация завершена
    isCelebrationActive = false;

    // Сбрасываем светодиоды в белое состояние
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      workbenchStrip.setPixelColor(i, WORKBENCH_COLOR_WHITE_50);
      workbenchLedStates[i] = 1;  // Устанавливаем состояние "белый"
    }
    workbenchStrip.show();
  }
}


void setWorkbenchLedColor(int ledIndex, uint32_t color) {
  if (ledIndex >= 0 && ledIndex < NUM_LEDS_WORKBENCH) {
    workbenchStrip.setPixelColor(ledIndex, color);
    workbenchStrip.show();
    workbenchLedStates[ledIndex] = 2;
    Serial1.println("item_add");
  }
}

void handleWorkbenchReedSwitches() {
  const int REED_OPEN_STATE = HIGH;

  if ((fireStage == 2 || fireStage == 3) && workbenchMode != 0) {
    uint32_t activeColor = 0;
    switch (workbenchMode) {
      case 1: activeColor = WORKBENCH_COLOR_GOLD; break;
      case 2: activeColor = WORKBENCH_COLOR_POTION; break;
      case 3: activeColor = WORKBENCH_COLOR_SKIN; break;
      case 4: activeColor = WORKBENCH_COLOR_CRYSTAL; break;
      case 5: activeColor = WORKBENCH_COLOR_METAL; break;
      default: return;
    }

    if (workbenchButt1.isPress() && !workbench1PressedBeforeReady) {
      setWorkbenchLedColor(0, activeColor);
    } else if (workbenchButt1.state() == REED_OPEN_STATE) {
      workbench1PressedBeforeReady = false;
    }

    if (workbenchButt2.isPress() && !workbench2PressedBeforeReady) {
      setWorkbenchLedColor(1, activeColor);
    } else if (workbenchButt2.state() == REED_OPEN_STATE) {
      workbench2PressedBeforeReady = false;
    }

    if (workbenchButt3.isPress() && !workbench3PressedBeforeReady) {
      setWorkbenchLedColor(2, activeColor);
    } else if (workbenchButt3.state() == REED_OPEN_STATE) {
      workbench3PressedBeforeReady = false;
    }

    if (workbenchButt4.isPress() && !workbench4PressedBeforeReady) {
      setWorkbenchLedColor(3, activeColor);
    } else if (workbenchButt4.state() == REED_OPEN_STATE) {
      workbench4PressedBeforeReady = false;
    }

  } else {
    if (workbenchButt1.isPress()) workbench1PressedBeforeReady = true;
    if (workbenchButt2.isPress()) workbench2PressedBeforeReady = true;
    if (workbenchButt3.isPress()) workbench3PressedBeforeReady = true;
    if (workbenchButt4.isPress()) workbench4PressedBeforeReady = true;
  }
}


void updateWorkbenchLeds() {
  if (fireStage >= 2 && fireStage <= 3) {
    if (!workbenchLedsInitialized) {
      for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
        if (workbenchLedStates[i] == 0) {
          workbenchStrip.setPixelColor(i, WORKBENCH_COLOR_WHITE_50);
          workbenchLedStates[i] = 1;
        }
      }
      workbenchStrip.show();
      workbenchLedsInitialized = true;
      workbench1PressedBeforeReady = false;
      workbench2PressedBeforeReady = false;
      workbench3PressedBeforeReady = false;
      workbench4PressedBeforeReady = false;
    }
  } else {
    if (workbenchLedsInitialized || (workbenchLedStates[0] != 0 || workbenchLedStates[1] != 0 || workbenchLedStates[2] != 0 || workbenchLedStates[3] != 0)) {
      workbenchStrip.clear();
      workbenchStrip.show();
    }
    workbenchLedsInitialized = false;
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      workbenchLedStates[i] = 0;
    }
  }
}

void activateHelmetServo() {
  if (!helmetServoActivated) {
    // Запускаем анимацию
    if (!isCelebrationActive) {
      isCelebrationActive = true;
      celebrationStartTime = millis();
    }
#if WORKSHOP_OMAN_SERVOS
    // Поворачиваем сервопривод шлема (открыто)
    helmetServo.attach(HELMET_SERVO_PIN);
    delay(10);
    helmetServo.write(10);
    delay(500);
    helmetServo.detach();
#else
    // Включаем светодиод Шлема на пине 5
    digitalWrite(HELMET_SERVO_PIN, HIGH);
#endif
    helmetServoActivated = true;
    Serial1.println("helmet");
  }
}

void activateBroomServo() {
  if (!broomServoActivated) {
    // Запускаем анимацию
    if (!isCelebrationActive) {
      isCelebrationActive = true;
      celebrationStartTime = millis();
    }
#if WORKSHOP_OMAN_SERVOS
    // Поворачиваем сервопривод метлы (открыто)
    broomServo.attach(BROOM_SERVO_PIN);
    delay(10);
    broomServo.write(10);
    delay(500);
    broomServo.detach();
#else
    // Включаем светодиод Метлы на пине 3
    digitalWrite(BROOM_SERVO_PIN, HIGH);
#endif
    broomServoActivated = true;
    Serial1.println("broom");
  }
}

void checkWorkbenchCombinations() {
  if (fireStage >= 2 && fireStage <= 3) {
    bool helmetComboMatch = true;
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      if (workbenchStrip.getPixelColor(i) != HELMET_COMBINATION[i]) {
        helmetComboMatch = false;
        break;
      }
    }
    if (helmetComboMatch) {
      activateHelmetServo();
    }

    bool broomComboMatch = true;
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      if (workbenchStrip.getPixelColor(i) != BROOM_COMBINATION[i]) {
        broomComboMatch = false;
        break;
      }
    }
    if (broomComboMatch) {
      activateBroomServo();
    }
  }
}

void checkGameEnd() {
  if (helmetServoActivated && broomServoActivated && !gameEnded) {
    digitalWrite(LED_FLOOR1_PIN, LOW);
    // Включаем подсветку 2 этажа только в конце игры
    digitalWrite(LED_FLOOR2_PIN, HIGH);
    delay(500);
    // Логирование перед командой
    sendLog("Workshop game finished. Sending story_35. workshop_complete");
    delay(10);
    // КОНЕЦ
    Serial1.println("story_35");
    gameEnded = true;
  }
}

void CheckState(bool force = false) {
  // Проверяем состояние галетника (pin 30)
  if (!digitalRead(30)) { // Если галетник активен (LOW)
    if (!_restartGalet || force) {    
      delay(50);
      Serial1.println("galet_on");
      delay(20); // ВАЖНО: Пауза перед логом
      sendLog("Galet sensor is active (galet_on).");
      _restartGalet = 1;
    }
  } else {                   // Если галетник неактивен (HIGH)
    if (_restartGalet || force) {     
      delay(50);
      Serial1.println("galet_off");
      delay(20); // ВАЖНО
      sendLog("Galet sensor is inactive (galet_off).");
      _restartGalet = 0;    
    }
  }

  // Проверяем состояние флага (pin 27)
  if (digitalRead(27)) { // Если флаг на месте (HIGH)
    if (!_restartFlag || force) {    
      delay(50);
      Serial1.println("flag1_on");
      delay(20); // ВАЖНО
      sendLog("Flag sensor is active (flag1_on).");
      _restartFlag = 1;     
    }
  } else {                  // Если флага нет (LOW)
    if (_restartFlag || force) {     
      delay(50);
      Serial1.println("flag1_off");
      delay(20); // ВАЖНО
      sendLog("Flag sensor is inactive (flag1_off).");
      _restartFlag = 0;   
    }
  }
}

// ============ МОНИТОР ДАТЧИКОВ ДЛЯ ТЕХ-ПУЛЬТА ============
// Опрос входов по команде mon:1 от главной платы. Данные уходят ТЕМ ЖЕ каналом,
// что и игровые события — по UART в Main и дальше на сервер. Читать башню по USB
// было бы проще, но тогда монитор показывал бы исправность там, где игра датчик
// не видит: ровно так вышло со стартовым герконом, где обрыв был на плате.
//
// В игровую логику не вмешивается: только digitalRead и печать изменений.
// Пока monActive == false — ни одного лишнего действия за проход.
const uint8_t MON_PINS[] = {
  31, 29,          // геркон печки, геркон подсказки
  44, 42, 40, 46,  // герконы верстака 1-4
  30,              // галетник «солнце»
  27               // ИК-датчик «флаг»
};
const uint8_t MON_COUNT = sizeof(MON_PINS);
bool monActive = false;
uint8_t monPrev[MON_COUNT];
uint8_t monCnt[MON_COUNT];
unsigned long monTimer = 0;

void monReport(uint8_t i, uint8_t value) {
  Serial1.print(F("sens:tw"));
  Serial1.print(MON_PINS[i]);
  Serial1.print(':');
  Serial1.println(value ? 1 : 0);
}

bool handleMonCmd(const String &cmd) {
  if (!cmd.startsWith("mon:")) return false;
  bool wasActive = monActive;
  monActive = (cmd.indexOf('1') != -1);
  // Дамп только на переходе выключено -> включено. Главная плата
  // повторяет mon:1 каждые 5 секунд, чтобы башня, пережившая сброс или
  // перепрошивку, сама вернулась в монитор; полный снимок при каждом
  // повторе забил бы канал.
  if (monActive && !wasActive) {
    // Сразу отдаём состояние всех входов, чтобы пульт не ждал первого шевеления.
    for (uint8_t i = 0; i < MON_COUNT; i++) {
      monPrev[i] = digitalRead(MON_PINS[i]);
      monCnt[i] = 0;
      monReport(i, monPrev[i]);
    }
  }
  return true;
}

void monPoll() {
  if (!monActive) return;
  if (millis() - monTimer < 120) return;
  monTimer = millis();
  for (uint8_t i = 0; i < MON_COUNT; i++) {
    uint8_t v = digitalRead(MON_PINS[i]);
    if (v != monPrev[i]) {
      // Антидребезг: значение держится два опроса подряд (~240 мс).
      if (++monCnt[i] >= 2) { monPrev[i] = v; monCnt[i] = 0; monReport(i, v); }
    } else {
      monCnt[i] = 0;
    }
  }
}
// ============ /МОНИТОР ============

// ============ ТЕСТ ВЕРСТАКА ============
// На производстве светодиод и геркон верстака легко оказываются в разных
// гнёздах: в игре человек касается первого геркона, а загорается третий
// светодиод, и понять это можно только пройдя весь этап. Здесь пара
// «геркон → светодиод» проверяется напрямую: замкнул первый — светится первый.
// Не совпало — переставляют железо на верстаке, код тут ни при чём.
// Отдельно — показ комбинаций метлы и шлема теми же цветами, что в игре:
// так видно, что лента вообще передаёт нужные оттенки.
// Режим включается только с тех-пульта при остановленном квесте и трогает
// ТОЛЬКО ленту верстака: игровая логика не замораживается (после майской
// истории с зависанием квеста ни один режим диагностики её не перехватывает).

void wbTestReset() {
  workbenchStrip.clear();
  workbenchStrip.show();
  for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
    wbTestPrev[i] = 0;
    wbTestChanged[i] = 0;
  }
}

bool handleWbTestCmd(const String &cmd) {
  if (!cmd.startsWith("wbtest")) return false;

  if (cmd == "wbtest_off") {
    wbTestActive = false;
    wbTestCombo = 0;
    wbTestReset();
    // Возвращаем ленту игре: пусть переинициализирует её со своих состояний.
    workbenchLedsInitialized = false;
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) workbenchLedStates[i] = 0;
    Serial1.println(F("wbtest:off"));
    return true;
  }

  wbTestActive = true;
  wbTestCombo = (cmd == "wbtest_broom") ? 1 : (cmd == "wbtest_helmet") ? 2 : 0;
  wbTestReset();
  if (wbTestCombo) {
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
      workbenchStrip.setPixelColor(i, wbTestCombo == 1 ? BROOM_COMBINATION[i]
                                                       : HELMET_COMBINATION[i]);
    }
    workbenchStrip.show();
  }
  Serial1.println(wbTestCombo == 1 ? F("wbtest:broom")
                                   : wbTestCombo == 2 ? F("wbtest:helmet")
                                                      : F("wbtest:reeds"));
  return true;
}

void handleWorkbenchTest() {
  if (wbTestCombo) return;                // комбинация просто светит, опрос не нужен
  unsigned long now = millis();
  for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
    byte level = (digitalRead(WB_REED_PINS[i]) == LOW) ? 1 : 0;   // замкнут = LOW
    if (level == wbTestPrev[i]) continue;
    if (now - wbTestChanged[i] < WB_TEST_DEBOUNCE) continue;
    wbTestChanged[i] = now;
    wbTestPrev[i] = level;
    workbenchStrip.setPixelColor(i, level ? WORKBENCH_COLOR_WHITE_50 : 0);
    workbenchStrip.show();
    Serial1.print(F("wbtest:reed:"));
    Serial1.print(i + 1);
    Serial1.print(':');
    Serial1.println(level);
  }
}
// ============ /ТЕСТ ВЕРСТАКА ============

void handleUartCommands() {
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();
    
    if (command.endsWith("\r")) {
      command.remove(command.length() - 1);
    }

    if (handleMonCmd(command)) return;   // монитор датчиков тех-пульта
    if (handleWbTestCmd(command)) return;  // тест верстака с тех-пульта

    // Началась игра — тест верстака выключаем сами: забытый включённым, он
    // держал бы ленту верстака у себя весь этап Мастерской.
    if (wbTestActive && (command == "ready" || command == "start" || command == "restart")) {
      handleWbTestCmd("wbtest_off");
    }

    // Безопасный heartbeat от Main: отвечаем "pong" без побочек и без лога CMD.
    if (command == "ping_main") {
      Serial1.println("pong");
      return;
    }

    // Блокируем эхо только для команды restart
    if (command != "restart") {
      sendLog("Received command: " + command);
    }
    if (command == "check_state"){
      CheckState(true);     // Принудительная отправка
    }

    if (command == "workshop") {
      openLock();
      //Serial1.println("door_workshop");
    } else if (command == "open_workshop") {
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
      openLock();
    } else if (command == "ready" || command == "start") {
      if (command == "start") {
        hasSentReadyLog = false;
      }
      if (command == "ready") {
        if (!hasSentReadyLog) {
          hasSentReadyLog = true;
          digitalWrite(LED_FLOOR2_PIN, LOW);
        }
      }
      fireworkActive = false; // Сбрасываем фейерверк
      if (floorLedsOn) {
        digitalWrite(LED_FLOOR1_PIN, LOW);
        digitalWrite(LED_FLOOR2_PIN, LOW);
        floorLedsOn = false;
        recurringLockActive = false;
      }
#if WORKSHOP_OMAN_SERVOS
      // Сервоприводы в начальное положение (закрыто)
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
#else
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
#endif
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
      _restartGalet = 0;
      _restartFlag = 0;
      if (command == "ready") {
         Serial1.println("log:workshop:System Ready");
      }
    } else if (command == "restart") {
      hasSentReadyLog = false;
      fireworkActive = false; // Сбрасываем фейерверк
      //openLock();
      _restartGalet = 0;
      _restartFlag = 0;
      CheckState();
      // 2026-08-07: раз в FLAG_RESEND_MS шлём состояние флага принудительно.
      // Сообщения уходили только на фронтах, и потерянный или ложный фронт
      // разводил картину Mega с реальностью навсегда — клиент лечил это,
      // вынимая и вставляя флаг. Mega форвардит наверх только изменения.
      if (millis() - lastFlagResend >= FLAG_RESEND_MS) {
        lastFlagResend = millis();
        CheckState(true);
      }
      lockOpen = false;
      recurringLockActive = false;
      digitalWrite(LOCK_PIN, HIGH);
      // 2026-08-08: было delay(100) + keep-alive 8 сек. Правило проекта —
      // Restart открывает дверь ОДНИМ импульсом (эталон OpenAll() на Mega:
      // 500 мс и разоружение повторной пульсации). Сотка соленоид не пробивала,
      // и слабый импульс компенсировали удержанием, из-за которого дверь
      // толкало ещё 2-3 раза после рестарта. Доводим импульс до 500 мс.
      delay(500);
      digitalWrite(LOCK_PIN, LOW);
      digitalWrite(LED_FLOOR2_PIN, HIGH);
      workbenchMode = 0;
      workbenchStrip.clear();
      workbenchStrip.show();
      workbenchLedsInitialized = false;
      workbench1PressedBeforeReady = false;
      workbench2PressedBeforeReady = false;
      workbench3PressedBeforeReady = false;
      workbench4PressedBeforeReady = false;
      for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) {
        workbenchLedStates[i] = 0;
      }
#if WORKSHOP_OMAN_SERVOS
      // Сервоприводы в начальное положение (закрыто)
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
#else
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
#endif
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
    } else if (command == "servo") {
#if WORKSHOP_OMAN_SERVOS
      // Сервоприводы в начальное положение (закрыто)
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
#else
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
#endif
      helmetServoActivated = false;
      broomServoActivated = false;
    } else if (command == "helmet") {
      activateHelmetServo();
    } else if (command == "broom") {
      activateBroomServo();
    }

    else if (command == "open_door") {
      // 2026-08-08: удержание убрано — импульса 500 мс достаточно, см. рестарт.
      digitalWrite(LOCK_PIN, HIGH);
      delay(500);
      digitalWrite(LOCK_PIN, LOW);
      // 2026-07-31: выравниваем счётчик повторов от момента этой команды —
      // иначе первый повтор мог прийти в произвольный момент (или не прийти
      // вовсе, пока интервал был больше keep-alive).
      lastRecurringLockOpenTime = millis();
      lockOpen = false;
    } else if (command == "day_on") {
      digitalWrite(LED_FLOOR1_PIN, HIGH);
      digitalWrite(LED_FLOOR2_PIN, LOW);
    } else if (command == "day_off") {
      fireworkActive = false; // Сбрасываем фейерверк
      digitalWrite(LED_FLOOR1_PIN, LOW);
    } else if (command == "light_on") {
      digitalWrite(ROOF_LIGHT_PIN, HIGH);
    } else if (command == "light_off") {
      digitalWrite(ROOF_LIGHT_PIN, LOW);
    } else if (command == "skip") {
      if (!gameEnded) {
        activateHelmetServo();
        activateBroomServo();
      }
    } else if (command == "gold") {
      workbenchMode = 1;
    } else if (command == "potion") {
      workbenchMode = 2;
    } else if (command == "skin") {
      workbenchMode = 3;
    } else if (command == "crystal") {
      workbenchMode = 4;
    } else if (command == "metal") {
      workbenchMode = 5;
    }
    else if (command == "firework") {
      fireworkActive = true; // Включаем фейерверк
      workbenchStrip.clear(); // 
      workbenchStrip.show(); //
    }
  }
}

void openLock() {
  if (!lockOpen) {
    digitalWrite(LOCK_PIN, HIGH);
    lockOpen = true;
    lockOpenTime = millis();
    if (!floorLedsOn) {
      digitalWrite(LED_FLOOR1_PIN, HIGH);
      digitalWrite(LED_FLOOR2_PIN, LOW);
      floorLedsOn = true;
    }
    if (!recurringLockActive) {
      recurringLockActive = true;
      lastRecurringLockOpenTime = millis();
    }
  }
}

void manageLock() {
  if (lockOpen && (millis() - lockOpenTime >= lockOpenDuration)) {
    digitalWrite(LOCK_PIN, LOW);
    lockOpen = false;
  }

  // 2026-08-08: повторная пульсация остаётся только для ИГРЫ (recurringLockActive).
  // Manual-hold от open_door/restart убран: на рестарте дверь должна открыться
  // один раз, а не толкаться каждые 3 секунды.
  bool shouldRecur = recurringLockActive;
  if (shouldRecur && !lockOpen && (millis() - lastRecurringLockOpenTime >= RECURRING_LOCK_INTERVAL)) {
    digitalWrite(LOCK_PIN, HIGH);
    lockOpen = true;
    lockOpenTime = millis();
    lastRecurringLockOpenTime = millis();
  }
}

// Вспомогательная функция для имитации fadeToBlackBy (qsub8)
uint32_t fadeColor(uint32_t color, uint8_t fadeAmount) {
  uint8_t r = (color >> 16) & 0xFF;
  uint8_t g = (color >> 8) & 0xFF;
  uint8_t b = color & 0xFF;
  
  // Логика qsub8 (вычитание с насыщением)
  r = (r < fadeAmount) ? 0 : r - fadeAmount;
  g = (g < fadeAmount) ? 0 : g - fadeAmount;
  b = (b < fadeAmount) ? 0 : b - fadeAmount;
  
  return workbenchStrip.Color(r, g, b); // 
}


// Адаптированная функция фейерверка для workshop.ino (Adafruit_NeoPixel)
void handleFirework() {
  if (!fireworkActive) return;

  static unsigned long lastFireworkTime = 0;
  static const int MAX_EXPLOSIONS = 2; // Уменьшаем для 4 светодиодов
  static const int FIREWORK_RADIUS = 2; // Макс. радиус
  static struct Explosion {
    int phase;
    uint32_t color; // Используем uint32_t для Adafruit_NeoPixel
    int center;
    unsigned long startTime;
  } explosions[MAX_EXPLOSIONS];


  // Создаем новые взрывы (реже)
  if (millis() - lastFireworkTime >= 1000) {
    lastFireworkTime = millis();
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions[i].phase == 0) {
        // Цвета в формате Adafruit_NeoPixel
        uint32_t niceColors[] = {
          workbenchStrip.Color(255, 100, 50), // 
          workbenchStrip.Color(100, 255, 100),
          workbenchStrip.Color(100, 100, 255),
          workbenchStrip.Color(255, 255, 100),
          workbenchStrip.Color(255, 100, 255),
          workbenchStrip.Color(100, 255, 255)
        };
        explosions[i].color = niceColors[random(6)];
        explosions[i].center = random(NUM_LEDS_WORKBENCH); // [cite: 554]
        explosions[i].startTime = millis();
        explosions[i].phase = 1;
        break;
      }
    }
  }

  // Обрабатываем все активные взрывы
  for (int e = 0; e < MAX_EXPLOSIONS; e++) {
    if (explosions[e].phase > 0) {
      unsigned long elapsed = millis() - explosions[e].startTime;
      if (elapsed < 500) {
        // Фаза расширения
        float progress = (float)elapsed / 500.0;
        int radius = progress * FIREWORK_RADIUS;

        for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) { // [cite: 554]
          int distance = abs(i - explosions[e].center);
          if (distance <= radius) {
            float intensity = 1.0 - (float)distance / radius;
            
            // Масштабируем цвет по яркости (intensity)
            uint8_t r = (explosions[e].color >> 16) & 0xFF;
            uint8_t g = (explosions[e].color >> 8) & 0xFF;
            uint8_t b = explosions[e].color & 0xFF;
            
            r = r * intensity;
            g = g * intensity;
            b = b * intensity;
            
            workbenchStrip.setPixelColor(i, workbenchStrip.Color(r, g, b)); // 
          }
        }
      } else if (elapsed < 1000) {
        // Фаза затухания
        float fadeProgress = (float)(elapsed - 500) / 500.0;
        float intensity = 1.0 - fadeProgress; // 1.0 -> 0.0

        for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) { // [cite: 554]
          int distance = abs(i - explosions[e].center);
          if (distance <= FIREWORK_RADIUS) {
            // Масштабируем цвет по яркости (intensity)
            uint8_t r = (explosions[e].color >> 16) & 0xFF;
            uint8_t g = (explosions[e].color >> 8) & 0xFF;
            uint8_t b = explosions[e].color & 0xFF;

            r = r * intensity;
            g = g * intensity;
            b = b * intensity;
            
            workbenchStrip.setPixelColor(i, workbenchStrip.Color(r, g, b)); // 
          }
        }
      } else {
        // Завершаем взрыв
        explosions[e].phase = 0;
        for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) { // [cite: 554]
          int distance = abs(i - explosions[e].center);
          if (distance <= FIREWORK_RADIUS) {
            workbenchStrip.setPixelColor(i, 0); // 
          }
        }
      }
    }
  }

  // Плавное затухание (замена EVERY_N_MILLISECONDS)
  static unsigned long lastGlobalFade = 0;
  if (millis() - lastGlobalFade >= 20) {
    lastGlobalFade = millis();
    for (int i = 0; i < NUM_LEDS_WORKBENCH; i++) { // [cite: 554]
      uint32_t currentColor = workbenchStrip.getPixelColor(i); // 
      if (currentColor != 0) {
        // Используем нашу вспомогательную функцию затухания
        workbenchStrip.setPixelColor(i, fadeColor(currentColor, 8)); // 
      }
    }
  }

  workbenchStrip.show(); // 
}
