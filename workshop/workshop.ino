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
#define HELMET_SERVO_PIN 5  // Пин для сервопривода "шлем"
#define BROOM_SERVO_PIN 3   // Пин для сервопривода "метла"
// --- Конец пинов сервоприводов ---

#define GALET_PIN 30           // Галетник солнце
#define FLAG_IR_SENSOR_PIN 27  // Сенсор флаг

#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>
#include <Servo.h>  // Подключаем библиотеку для сервоприводов

// Объект NeoPixel для огня
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS_FIRE, LED_PIN_FIRE, NEO_GRB + NEO_KHZ800);
// Новый объект NeoPixel для верстака
Adafruit_NeoPixel workbenchStrip = Adafruit_NeoPixel(NUM_LEDS_WORKBENCH, WORKBENCH_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Объекты сервоприводов
Servo helmetServo;
Servo broomServo;
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
const unsigned long RECURRING_LOCK_INTERVAL = 10000;

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

bool helmetServoActivated = false;
bool broomServoActivated = false;

bool gameEnded = false;

bool isFirstFire1 = false;
bool isFirstFire2 = false;
bool isFirstFire0 = false;

// НОВЫЕ ПЕРЕМЕННЫЕ для анимации
bool isCelebrationActive = false;
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
void updateFireEffect(int stage);
void GenerateFire();
uint32_t calculateFireColor(float heatValue);
void handleCelebrationEffect();
void setWorkbenchLedColor(int ledIndex, uint32_t color);
void handleWorkbenchReedSwitches();
void updateWorkbenchLeds();
void activateHelmetServo();
void activateBroomServo();
void checkWorkbenchCombinations();
void checkGameEnd();
void handleUartCommands();
void openLock();
void manageLock();
void setup() {
  Serial1.begin(9600);
  Serial1.setTimeout(10);

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
  pinMode(ROOF_LIGHT_PIN, OUTPUT);

  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(LED_FLOOR1_PIN, LOW);
  digitalWrite(LED_FLOOR2_PIN, LOW);
  digitalWrite(ROOF_LIGHT_PIN, LOW);

  helmetServo.attach(HELMET_SERVO_PIN);
  broomServo.attach(BROOM_SERVO_PIN);
  helmetServo.write(140);
  broomServo.write(140);

  helmetServo.detach();
  broomServo.detach();

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
  butt1.tick();
  hintButt.tick();
  workbenchButt1.tick();
  workbenchButt2.tick();
  workbenchButt3.tick();
  workbenchButt4.tick();

  galetButton.tick();
  if (galetButton.isPress()) {
    Serial1.println("galet_on");
  }
  if (galetButton.isRelease()) {
    Serial1.println("galet_off");
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

  // Отдаем приоритет анимации. Если она активна, стандартная логика LED не выполняется.
  if (isCelebrationActive) {
    handleCelebrationEffect();
  } else {
    handleWorkbenchReedSwitches();
    updateWorkbenchLeds();
  }

  switch (state) {
    case 0:
      handleFireLogic();
      break;
  }

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
    helmetServo.attach(HELMET_SERVO_PIN);
    delay(10);
    helmetServo.write(10);
    delay(500);
    helmetServo.detach();
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
    broomServo.attach(BROOM_SERVO_PIN);
    delay(10);
    broomServo.write(10);
    delay(500);
    broomServo.detach();
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
    delay(500);
    Serial1.println("story_35");
    gameEnded = true;
  }
}


void handleUartCommands() {
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();

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
      if (floorLedsOn) {
        digitalWrite(LED_FLOOR1_PIN, LOW);
        digitalWrite(LED_FLOOR2_PIN, LOW);
        floorLedsOn = false;
        recurringLockActive = false;
      }
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
      _restartGalet = 0;
      _restartFlag = 0;
    } else if (command == "restart") {
      //openLock();
      if (!digitalRead(30) && !_restartGalet) {
        Serial1.println("galet_on");
        _restartGalet = 1;
        delay(500);
      }
      if (digitalRead(30) && _restartGalet) {
        Serial1.println("galet_off");
        _restartGalet = 0;
        delay(500);
      }

      if (digitalRead(27) && !_restartFlag) {
        Serial1.println("flag1_on");
        _restartFlag = 1;
        delay(500);
      }
      if (_restartFlag && !digitalRead(27)) {
        Serial1.println("flag1_off");
        delay(500);
        _restartFlag = 0;
      }
      lockOpen = false;
      recurringLockActive = false;
      digitalWrite(LOCK_PIN, HIGH);
      delay(100);
      digitalWrite(LOCK_PIN, LOW);
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
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
    } else if (command == "servo") {
      helmetServo.attach(HELMET_SERVO_PIN);
      broomServo.attach(BROOM_SERVO_PIN);
      delay(10);
      helmetServo.write(140);
      broomServo.write(140);
      delay(500);
      helmetServo.detach();
      broomServo.detach();
      helmetServoActivated = false;
      broomServoActivated = false;
    } else if (command == "helmet") {
      activateHelmetServo();
    } else if (command == "broom") {
      activateBroomServo();
    }

    else if (command == "open_door") {
      digitalWrite(LOCK_PIN, HIGH);
      delay(500);
      digitalWrite(LOCK_PIN, LOW);
    } else if (command == "day_on") {
      digitalWrite(LED_FLOOR1_PIN, HIGH);
      digitalWrite(LED_FLOOR2_PIN, HIGH);
    } else if (command == "day_off") {
      digitalWrite(LED_FLOOR1_PIN, LOW);
      digitalWrite(LED_FLOOR2_PIN, LOW);
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
      //workbenchMode = 5;
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
      digitalWrite(LED_FLOOR2_PIN, HIGH);
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

  if (recurringLockActive && !lockOpen && (millis() - lastRecurringLockOpenTime >= RECURRING_LOCK_INTERVAL)) {
    digitalWrite(LOCK_PIN, HIGH);
    lockOpen = true;
    lockOpenTime = millis();
    lastRecurringLockOpenTime = millis();
  }
}
