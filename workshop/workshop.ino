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
#define HELMET_SERVO_PIN 50  // Пин для сервопривода "шлем"
#define BROOM_SERVO_PIN 48   // Пин для сервопривода "метла"
// --- Конец пинов сервоприводов ---

#define GALET_PIN 30           // Галетник солнце
#define FLAG_IR_SENSOR_PIN 27  // Сенсор флаг

#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>

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
void updateWorkbenchLeds();
void activateHelmetServo();
void activateBroomServo();
void checkWorkbenchCombinations();
void checkGameEnd();
void handleUartCommands();
void openLock();
void manageLock();
void CheckState(bool force = false);
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
  digitalWrite(LED_FLOOR2_PIN, LOW);
  pinMode(ROOF_LIGHT_PIN, OUTPUT);

  // Настраиваем пины вместо сервоприводов на выход для LED
  pinMode(HELMET_SERVO_PIN, OUTPUT); // Пин 5
  pinMode(BROOM_SERVO_PIN, OUTPUT);  // Пин 3

  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(LED_FLOOR1_PIN, LOW);
  digitalWrite(LED_FLOOR2_PIN, LOW);
  digitalWrite(ROOF_LIGHT_PIN, LOW);

  // Гасим светодиоды шлема и метлы на старте
  digitalWrite(HELMET_SERVO_PIN, LOW);
  digitalWrite(BROOM_SERVO_PIN, LOW);

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
  static int previousState = -1;
  if (state != previousState) {
    String logMsg = "State changed to " + String(state);
    sendLog(logMsg);
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
    sendLog("Galet sensor activated (galet_on).");
    _restartGalet = 1;
  }
  if (galetButton.isRelease() && _restartGalet == 1) {
    Serial1.println("galet_off");
    sendLog("Galet sensor deactivated (galet_off).");
    _restartGalet = 0;
  }

  flagButton.tick();
  if (flagButton.isPress()) {
    Serial1.println("flag1_on");
    sendLog("Flag sensor activated (flag1_on).");
  }
  if (flagButton.isRelease()) {
    Serial1.println("flag1_off");
    sendLog("Flag sensor deactivated (flag1_off).");
  }

  if (hintButt.isPress()) {
    Serial1.println("help");
    sendLog("Hint button pressed.");
  }

  // Проверяем команды В НАЧАЛЕ, чтобы установить/сбросить флаг
  handleUartCommands(); 

  // Логика для светодиодов верстака (workbenchStrip)
  if (fireworkActive) {
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
          sendLog("Fire stage 1 reached.");
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
          sendLog("Fire stage 2 reached.");
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
        sendLog("Fire stage 3 reached.");
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
          sendLog("Fire stage decreased to 2.");
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
          sendLog("Fire stage decreased to 1.");
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
          sendLog("Fire extinguished.");
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
    sendLog("Item added to workbench.");
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
    // Включаем светодиод Шлема на пине 5
    digitalWrite(HELMET_SERVO_PIN, HIGH);
    helmetServoActivated = true;
    Serial1.println("helmet");
    sendLog("Helmet LED activated.");
  }
}

void activateBroomServo() {
  if (!broomServoActivated) {
    // Запускаем анимацию
    if (!isCelebrationActive) {
      isCelebrationActive = true;
      celebrationStartTime = millis();
    }
    // Включаем светодиод Метлы на пине 3
    digitalWrite(BROOM_SERVO_PIN, HIGH);
    broomServoActivated = true;
    Serial1.println("broom");
    sendLog("Broom LED activated.");
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
    sendLog("Game finished (story_35).");
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

void handleUartCommands() {
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');
    command.trim();
    sendLog("Received command: " + command);
    if (command.endsWith("\r")) {
      command.remove(command.length() - 1);
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
          sendLog("Checking initial sensor states.");
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
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
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
      lockOpen = false;
      recurringLockActive = false;
      digitalWrite(LOCK_PIN, HIGH);
      delay(100);
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
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
      helmetServoActivated = false;
      broomServoActivated = false;
      gameEnded = false;
      isFirstFire1 = true;
      isFirstFire2 = true;
      isFirstFire0 = true;
    } else if (command == "servo") {
      digitalWrite(HELMET_SERVO_PIN, LOW); // Выключаем LED шлема
      digitalWrite(BROOM_SERVO_PIN, LOW);  // Выключаем LED метлы
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

  if (recurringLockActive && !lockOpen && (millis() - lastRecurringLockOpenTime >= RECURRING_LOCK_INTERVAL)) {
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
