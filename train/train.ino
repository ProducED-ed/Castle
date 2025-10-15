#include "GyverEncoder.h"
#include <FastLED.h>
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include <Adafruit_PCF8574.h>

HardwareSerial mySerial(1);  // Use UART1
#define NUM_LEDS 30
#define LED_PIN 17
#define MAX_BRIGHTNESS 255
#define NUM_LEDS1 4
#define DATA_PIN1 5
#define TUNNEL_LED 23
#define UF_LED 19


CRGB leds1[NUM_LEDS1];
CRGB ledMap[30];
#include "GyverButton.h"
int TimerLeds[] = { 1, 2, 3, 4, 5 };
unsigned long lightFlashTimer = 0;
bool up;
int light = 0;
unsigned long skinlightFlashTimer = 0;
bool skinup;
int skinlight = 0;

int DisableLeds[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int ActiveLeds[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int ClickLeds[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
int FutureLeds[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

bool newStateAvailable = false;
unsigned long _secondsTimer;
unsigned long _clockTimer;
bool _startTimer;
bool _timerEnabled;
int counter = 9;
int blinkLedNumber = -1;

int timerLedsCount = 0;
bool timerIsStarting = true;

long enc1Last = 0;  // Предыдущая позиция энкодера 1
long enc2Last = 0;  // Предыдущая позиция энкодера 2

bool fireworkActive = false;
unsigned long fireworkStartTime = 0;
const unsigned long FIREWORK_DURATION = 10000;  // 10 секунд салюта
CRGB fireworkColors[5] = { CRGB::Red, CRGB::Blue, CRGB::Green, CRGB::Purple, CRGB::Yellow };

long enc1Pos = 0, enc2Pos = 0;
int hue = 40;  // 0-21845 (красный-зелёный)
int hue1 = 0;
int brightness = 255;
unsigned long prevMillis = 0;
bool ledState = false;
int blinkInterval = 200;
bool isGreen = false;        // Флаг зелёного цвета
const int greenHue = 196;    // Зелёный в HSV
int targetBrightness = 255;  // Целевая яркость
int fadeSpeed = 5;           // Скорость изменения яркости
bool isLedOn = true;
int baseBrightness = 200;    // Базовая яркость
int flickerIntensity = 150;  // Сила мерцания (0-100)
unsigned long prevFlickerTime = 0;
unsigned long checkTrainTimer = 0;
uint8_t initialHue = 0;
long musicCounter = 0;
bool musicFlag1 = 0;
bool musicFlag2 = 0;
bool musicFlag3 = 0;
int language = 1;
int hint_counter = 0;
bool hintFlag = 1;

Adafruit_PCF8574 INPUTS;
Adafruit_PCF8574 OUTPUTS;

// --- Истории (7-42) ---
// Блок Story 15
const int TRACK_STORY_15_RU = 13;
const int TRACK_STORY_15_EN = 23;
const int TRACK_STORY_15_AR = 33;
const int TRACK_STORY_15_GE = 7;
const int TRACK_STORY_15_SP = 7;
const int TRACK_STORY_15_CH = 7;

// Блок Story 16
const int TRACK_STORY_16_RU = 14;
const int TRACK_STORY_16_EN = 24;
const int TRACK_STORY_16_AR = 34;
const int TRACK_STORY_16_GE = 8;
const int TRACK_STORY_16_SP = 8;
const int TRACK_STORY_16_CH = 8;

// --- Подсказки (9-42) ---
// Блок Hint 0
const int TRACK_HINT_0_RU = 15;
const int TRACK_HINT_0_EN = 25;
const int TRACK_HINT_0_AR = 35;
const int TRACK_HINT_0_GE = 9;
const int TRACK_HINT_0_SP = 9;
const int TRACK_HINT_0_CH = 9;

// Блок Hint 1
const int TRACK_HINT_1_RU = 16;
const int TRACK_HINT_1_EN = 26;
const int TRACK_HINT_1_AR = 36;
const int TRACK_HINT_1_GE = 10;
const int TRACK_HINT_1_SP = 10;
const int TRACK_HINT_1_CH = 10;

// Блок Hint 2
const int TRACK_HINT_2_RU = 17;
const int TRACK_HINT_2_EN = 27;
const int TRACK_HINT_2_AR = 37;
const int TRACK_HINT_2_GE = 11;
const int TRACK_HINT_2_SP = 11;
const int TRACK_HINT_2_CH = 11;

// Блок Hint 3
const int TRACK_HINT_3_RU = 18;
const int TRACK_HINT_3_EN = 28;
const int TRACK_HINT_3_AR = 38;
const int TRACK_HINT_3_GE = 12;
const int TRACK_HINT_3_SP = 12;
const int TRACK_HINT_3_CH = 12;

// Блок Hint 4
const int TRACK_HINT_4_RU = 19;
const int TRACK_HINT_4_EN = 29;
const int TRACK_HINT_4_AR = 39;
const int TRACK_HINT_4_GE = 12;
const int TRACK_HINT_4_SP = 12;
const int TRACK_HINT_4_CH = 12;

// Блок Hint 5
const int TRACK_HINT_5_RU = 20;
const int TRACK_HINT_5_EN = 30;
const int TRACK_HINT_5_AR = 40;
const int TRACK_HINT_5_GE = 12;
const int TRACK_HINT_5_SP = 12;
const int TRACK_HINT_5_CH = 12;

// Блок Hint 6
const int TRACK_HINT_6_RU = 21;
const int TRACK_HINT_6_EN = 31;
const int TRACK_HINT_6_AR = 41;
const int TRACK_HINT_6_GE = 12;
const int TRACK_HINT_6_SP = 12;
const int TRACK_HINT_6_CH = 12;

// Блок Hint 7
const int TRACK_HINT_7_RU = 22;
const int TRACK_HINT_7_EN = 32;
const int TRACK_HINT_7_AR = 42;
const int TRACK_HINT_7_GE = 12;
const int TRACK_HINT_7_SP = 12;
const int TRACK_HINT_7_CH = 12;

// --- Системные треки (1-6) ---
const int TRACK_TRAIN_ON = 1;
const int TRACK_ENGINE1 = 2;
const int TRACK_ENGINE2 = 3;
const int TRACK_KNOCK = 4;
const int TRACK_TRAIN = 5;
const int TRACK_GHOST = 6;
const int TRACK_ENGINE3 = 43;

bool isTrainClick = false;
bool isOwlClick = false;
bool isKeyClick = false;
bool isFishClick = false;
bool isStartTimer = false;
bool isStartTrain = false;
bool isTrainEnd = false;
bool isSkinPulsation;
bool ghostFlag;
bool isSendOut;
bool isTrollEnd = 0;
bool resumeEngineSound = false;

String mapState;

Encoder enc1(33, 25);
Encoder enc2(35, 32);
Encoder enc3(39, 34);
DFRobotDFPlayerMini myMP3;

//const char* ssid = "Castle";
//const char* password = "questquest";
const char* ssid = "ProducED";
const char* password = "32744965";

// Настройки статического IP
IPAddress local_IP(192, 168, 0, 202);

const char* externalApi = "http://192.168.0.120:3000/api";

WebServer server(80);

GButton ghost(36);
int state = 0;

void ResetTimer() {
  // Сбрасываем глобальные переменные таймера
  timerLedsCount = 0;
  timerIsStarting = true;

  // Гасим все светодиоды таймера
  for (int i = 0; i <= 10; i++) {
    ledMap[i] = CRGB(0, 0, 0);
  }

  // Гасим моргающий светодиод
  if (blinkLedNumber != -1) {
    ledMap[blinkLedNumber] = CRGB(0, 0, 0);
  }

  // Сбрасываем флаги
  isKeyClick = false;
  isFishClick = false;
  isOwlClick = false;
  isTrainClick = false;
  FastLED.show();
}


void startFirework() {
  fireworkActive = true;
  fireworkStartTime = millis();
  Serial.println("firework:started\r");
}

//Добавьте эту функцию для анимации фейерверка
void handleFirework() {
  if (!fireworkActive) return;

  static unsigned long lastFireworkTime = 0;
  static const int MAX_EXPLOSIONS = 3;
  static struct Explosion {
    int phase;
    CRGB color;
    int center;
    unsigned long startTime;
  } explosions[MAX_EXPLOSIONS];


  // Создаем новые взрывы
  if (millis() - lastFireworkTime >= 600) {
    lastFireworkTime = millis();

    // Ищем свободный слот для нового взрыва
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions[i].phase == 0) {
        // Красивые цвета салюта
        CRGB niceColors[] = {
          CRGB(255, 100, 50),   // Оранжевый
          CRGB(100, 255, 100),  // Светло-зеленый
          CRGB(100, 100, 255),  // Светло-синий
          CRGB(255, 255, 100),  // Светло-желтый
          CRGB(255, 100, 255),  // Розовый
          CRGB(100, 255, 255)   // Бирюзовый
        };

        explosions[i].color = niceColors[random(6)];
        explosions[i].center = random(6, 24);
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
        int radius = progress * 8;

        for (int i = 0; i < 30; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= radius) {
            float intensity = 1.0 - (float)distance / radius;
            ledMap[i] = explosions[e].color;
            ledMap[i].fadeToBlackBy(255 - (intensity * 255));
          }
        }
      } else if (elapsed < 1000) {
        // Фаза затухания
        float fadeProgress = (float)(elapsed - 500) / 500.0;
        uint8_t fadeAmount = fadeProgress * 255;

        for (int i = 0; i < 30; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= 8) {
            ledMap[i] = explosions[e].color;
            ledMap[i].fadeToBlackBy(fadeAmount);
          }
        }
      } else {
        // Завершаем взрыв
        explosions[e].phase = 0;
        for (int i = 0; i < 30; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= 8) {
            ledMap[i] = CRGB::Black;
          }
        }
      }
    }
  }

  // Плавное затухание всех светодиодов
  EVERY_N_MILLISECONDS(20) {
    for (int i = 0; i < 30; i++) {
      if (ledMap[i] != CRGB::Black) {
        ledMap[i].fadeToBlackBy(8);
      }
    }
  }

  FastLED.show();
}

void setup() {
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);
  FastLED.addLeds<WS2812B, DATA_PIN1, GRB>(leds1, NUM_LEDS1);
  FastLED.addLeds<WS2812B, 18, GRB>(ledMap, 30);

  for (int i = 0; i < 22; i++) {
    ActiveLeds[i] = -1;
    DisableLeds[i] = -1;
    ClickLeds[i] = -1;
    FutureLeds[i] = -1;
  }

  ghost.setDebounce(10);       // настройка антидребезга (по умолчанию 80 мс)
  ghost.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  ghost.setClickTimeout(100);  // настройка таймаута между кликами (по умолчанию 300 мс)
  ghost.setType(LOW_PULL);
  ghost.setDirection(NORM_OPEN);
  ghost.setTickMode(AUTO);

  pinMode(36, INPUT);

  enc1.setType(TYPE2);
  enc2.setType(TYPE2);
  enc3.setType(TYPE2);
  pinMode(TUNNEL_LED, OUTPUT);
  pinMode(UF_LED, OUTPUT);



  if (!INPUTS.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8574");
    //while (1);
  }
  for (uint8_t p = 0; p < 8; p++) {
    INPUTS.pinMode(p, INPUT_PULLUP);
  }

  if (!OUTPUTS.begin(0x21, &Wire)) {
    Serial.println("Couldn't find PCF8574");
  }
  for (uint8_t p = 0; p < 3; p++) {
    OUTPUTS.pinMode(p, OUTPUT);
  }

  if (!myMP3.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected!");
  }


  if (!WiFi.config(local_IP)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });

  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      if (body == "\"start\"") {
        OUTPUTS.digitalWrite(0, HIGH);
        myMP3.stop();
        delay(500);
        myMP3.playMp3Folder(TRACK_TRAIN);
        for (int i = 0; i <= 8; i++) {
          ledMap[i] = CRGB(0, 0, 0);
        }

        for (int i = 0; i < 22; i++) {
          ActiveLeds[i] = -1;
          ClickLeds[i] = -1;
          //FutureLeds[i] = -1;
          FutureLeds[i] = i + 9;
        }
        ActiveLeds[4] = 12;
        state = 0;
        FutureLeds[4] = -1;
        hintFlag = 1;
      }
      if (body == "\"language_1\"") {
        language = 1;
      }
      if (body == "\"language_2\"") {
        language = 2;
      }
      if (body == "\"language_3\"") {
        language = 3;
      }
      if (body == "\"language_4\"") {
        language = 4;
      }
      if (body == "\"language_5\"") {
        language = 5;
      }
      if (body == "\"language_6\"") {
        language = 6;
      }

      if (body == "\"ready\"") {
        OUTPUTS.digitalWrite(0, LOW);
        OUTPUTS.digitalWrite(1, LOW);
        for (int i = 0; i <= 3; i++) {
          leds1[i] = CRGB(0, 0, 0);
        }
        digitalWrite(TUNNEL_LED, LOW);
        digitalWrite(UF_LED, LOW);

        for (int i = 0; i < 22; i++) {
          ActiveLeds[i] = -1;
          DisableLeds[i] = -1;
          ClickLeds[i] = -1;
          FutureLeds[i] = -1;
        }
        state = 0;
        FastLED.show();
        hintFlag = 0;
      }

      if (body == "\"projector\"") {
        myMP3.playMp3Folder(TRACK_TRAIN_ON);
        ActiveLeds[0] = 9;
        ClickLeds[0] = -1;
        isStartTrain = 1;
        isSendOut = 1;
      }
      if (body == "\"ghost_knock\"") {
        myMP3.stop();
        delay(150);
        myMP3.playMp3Folder(TRACK_KNOCK);
      }
      if (body == "\"skip\"") {
        state = 3;
        SendData("{\"train\":\"end\"}");
        if (language == 1)
          myMP3.playMp3Folder(TRACK_STORY_16_RU);
        if (language == 2)
          myMP3.playMp3Folder(TRACK_STORY_16_EN);
        if (language == 3)
          myMP3.playMp3Folder(TRACK_STORY_16_AR);
        if (language == 4)
          myMP3.playMp3Folder(TRACK_STORY_16_GE);
        if (language == 5)
          myMP3.playMp3Folder(TRACK_STORY_16_SP);
        if (language == 6)
          myMP3.playMp3Folder(TRACK_STORY_16_CH);
      }

      if (body == "\"restart\"") {
        OUTPUTS.digitalWrite(0, HIGH);
        OUTPUTS.digitalWrite(1, HIGH);
        for (int i = 0; i <= 3; i++) {
          leds1[i] = CRGB(255, 255, 255);
        }
        isTrollEnd = 0;
        digitalWrite(TUNNEL_LED, HIGH);
        digitalWrite(UF_LED, HIGH);

        for (int i = 0; i <= 8; i++) {
          ledMap[i] = CRGB(255, 255, 255);
        }

        for (int i = 0; i < 22; i++) {
          ActiveLeds[i] = -1;
          ClickLeds[i] = -1;
          FutureLeds[i] = i + 8;
        }
        state = 0;
        hintFlag = 0;
        FastLED.show();
        enc1Pos = 0;
        enc2Pos = 0;
        hue = 40;  // 0-21845 (красный-зелёный)
        brightness = 255;
        prevMillis = 0;
        ledState = false;
        blinkInterval = 200;
        isGreen = false;         // Флаг зелёного цвета
        targetBrightness = 255;  // Целевая яркость
        fadeSpeed = 5;           // Скорость изменения яркости
        isLedOn = true;
        baseBrightness = 200;    // Базовая яркость
        flickerIntensity = 150;  // Сила мерцания (0-100)
        prevFlickerTime = 0;
        checkTrainTimer = 0;
        initialHue = 0;
        musicCounter = 0;
        musicFlag1 = 0;
        musicFlag2 = 0;
        musicFlag3 = 0;
        isTrainClick = false;
        isOwlClick = false;
        isKeyClick = false;
        isFishClick = false;
        isStartTimer = false;
        isStartTrain = false;
        isTrainEnd = false;
        isSkinPulsation = false;
        ghostFlag = false;
        isSendOut = false;
        mapState = "";
        newStateAvailable = false;
        _secondsTimer = 0;
        _clockTimer = 0;
        _startTimer = 0;
        _timerEnabled = false;
        counter = 9;
        blinkLedNumber = -1;
        timerLedsCount = 0;
        timerIsStarting = true;

        enc1Last = 0;  // Предыдущая позиция энкодера 1
        enc2Last = 0;  // Предыдущая позиция энкодера 2

        fireworkActive = false;
        fireworkStartTime = 0;
        lightFlashTimer = 0;
        up = 0;
        light = 0;
        skinlightFlashTimer = 0;
        skinup = 0;
        skinlight = 0;
      }

      if (body == "\"flag_on\"") {
        ActiveLeds[18] = -1;
        ActiveLeds[17] = -1;
        ActiveLeds[2] = -1;

        DisableLeds[18] = 27;
        DisableLeds[17] = 26;
        DisableLeds[2] = 11;

        FutureLeds[7] = -1;
        FutureLeds[5] = -1;
        FutureLeds[10] = -1;
        FutureLeds[14] = -1;

        DisableLeds[7] = -1;
        DisableLeds[5] = -1;
        DisableLeds[10] = -1;
        DisableLeds[14] = -1;

        ActiveLeds[7] = 16;
        ActiveLeds[5] = 14;
        ActiveLeds[10] = 19;
        ActiveLeds[14] = 23;
      }

      if (body == "\"flag_off\"") {
        FutureLeds[7] = 16;
        FutureLeds[5] = 14;
        FutureLeds[10] = 19;
        FutureLeds[14] = 23;

        ActiveLeds[7] = -1;
        ActiveLeds[5] = -1;
        ActiveLeds[10] = -1;
        ActiveLeds[14] = -1;
      }

      if (body == "\"stage_1\"") {

        FutureLeds[7] = -1;
        FutureLeds[5] = -1;
        FutureLeds[10] = -1;
        FutureLeds[14] = -1;
        FutureLeds[9] = -1;

        ActiveLeds[7] = 16;
        ActiveLeds[5] = 14;
        ActiveLeds[10] = 19;
        ActiveLeds[14] = 23;
        ActiveLeds[9] = 18;
      }


      if (body == "\"stage_2\"") {
        ActiveLeds[7] = -1;
        ActiveLeds[5] = -1;
        ActiveLeds[10] = -1;
        ActiveLeds[14] = -1;
        ActiveLeds[3] = -1;
        ActiveLeds[9] = -1;
        ActiveLeds[4] = -1;

        DisableLeds[7] = -1;
        DisableLeds[14] = -1;
        DisableLeds[5] = -1;
        DisableLeds[10] = -1;
        DisableLeds[3] = 12;
        DisableLeds[9] = 18;
        DisableLeds[4] = -1;

        FutureLeds[2] = -1;
        FutureLeds[17] = -1;
        FutureLeds[18] = -1;
        FutureLeds[4] = 12;

        FutureLeds[7] = 16;
        FutureLeds[5] = 14;
        FutureLeds[10] = 19;
        FutureLeds[14] = 23;

        ActiveLeds[2] = 11;
        ActiveLeds[17] = 26;
        ActiveLeds[18] = 27;
      }

      if (body == "\"wolf_finish\"") {
        ActiveLeds[2] = -1;
        DisableLeds[2] = 11;
      }

      if (body == "\"safe_finish\"") {
        ActiveLeds[17] = -1;
        DisableLeds[17] = 26;
      }

      if (body == "\"case_finish\"") {
        ActiveLeds[18] = -1;
        DisableLeds[18] = 27;
      }

      if (body == "\"stage_3\"") {
        // --- НОВЫЙ КОД: Принудительно убираем светодиоды из активных ---
        // Это гарантирует, что не будет конфликта с желтой пульсацией.
        // Мы очищаем те же ячейки, которые используются для этих светодиодов в других частях кода.
        ActiveLeds[0] = -1;  // Ячейка, иногда используемая для LED 9
        ActiveLeds[1] = -1;  // Ячейка, иногда используемая для LED 10
        ActiveLeds[15] = -1; // Ячейка, связанная с LED 24 (рыба)
        ActiveLeds[16] = -1; // Ячейка, связанная с LED 25 (ключ)

        // --- Старый код (остается без изменений) ---
        FutureLeds[0] = -1;
        FutureLeds[1] = -1;
        FutureLeds[16] = -1;
        FutureLeds[15] = -1;

        ClickLeds[0] = 9;
        ClickLeds[1] = 10;
        ClickLeds[16] = 25;
        ClickLeds[15] = 24;
        state = 1;
      }

      if (body == "\"owl_open\"") {
        DisableLeds[1] = 10;
        ActiveLeds[1] = -1;
        ClickLeds[1] = -1;
        isSendOut = 1;
        DisableLeds[10] = -1;
        ActiveLeds[10] = 19;
      }

      if (body == "\"key_open\"") {
        DisableLeds[16] = 25;
        ActiveLeds[16] = -1;
        ClickLeds[16] = -1;
        FutureLeds[16] = -1;
        isSendOut = 1;
        DisableLeds[7] = -1;
        ActiveLeds[7] = 16;
      }

      if (body == "\"fish_open\"") {
        DisableLeds[15] = 24;
        ActiveLeds[15] = -1;
        ClickLeds[15] = -1;
        FutureLeds[15] = -1;

        isSendOut = 1;
        FutureLeds[4] = -1;
        ActiveLeds[4] = 13;
      }
      if (body == "\"owl_finish\"") {

        DisableLeds[10] = 19;
        ActiveLeds[10] = -1;
      }
      if (body == "\"key_finish\"") {
        DisableLeds[7] = 16;
        ActiveLeds[7] = -1;
      }
      if (body == "\"fish_finish\"") {
        DisableLeds[4] = 13;
        ActiveLeds[4] = -1;
      }
      if (body == "\"troll_finish\"") {
        DisableLeds[13] = 22;
        ActiveLeds[13] = -1;
        isTrollEnd = 1;
      }
      if (body == "\"stage_4\"") {

        DisableLeds[13] = 22;
        ActiveLeds[13] = -1;
        ClickLeds[13] = -1;

        FutureLeds[6] = -1;
        ActiveLeds[6] = 15;
        state = 4;
      }

      if (body == "\"stage_5\"") {
        DisableLeds[6] = 15;
        ActiveLeds[6] = -1;
        FutureLeds[6] = -1;

        FutureLeds[5] = -1;
        DisableLeds[5] = -1;
        ActiveLeds[5] = 14;
      }

      if (body == "\"stage_6\"") {
        FutureLeds[3] = -1;
        DisableLeds[3] = -1;
        ActiveLeds[3] = 12;


        DisableLeds[5] = 14;
        ActiveLeds[5] = -1;
        FutureLeds[5] = -1;
      }

      if (body == "\"stage_7\"") {
        // --- НАЧАЛО ИСПРАВЛЕНИЯ ---

        // 1. Гасим предыдущий этап (Мастерская, LED 12)
        DisableLeds[3] = 12;
        ActiveLeds[3] = -1;

        // 2. Устанавливаем следующий этап (Башня Директора, LED 18) как БУДУЩИЙ (белый).
        FutureLeds[9] = 18;
        ActiveLeds[9] = -1; // Гарантированно убираем из активных на случай ошибки
        DisableLeds[9] = -1;

        // 3. Устанавливаем ТЕКУЩИЙ этап (Библиотека, LED 21) как АКТИВНЫЙ (желтый).
        ActiveLeds[12] = 21;
        FutureLeds[12] = -1; // Убираем из будущих
        DisableLeds[12] = -1;

        // 4. На всякий случай гасим ошибочный LED 20, который включался здесь ранее.
        DisableLeds[11] = 20;
        ActiveLeds[11] = -1;

        // --- КОНЕЦ ИСПРАВЛЕНИЯ ---
      }

      if (body == "\"stage_8\"") {
        DisableLeds[11] = 20;
        ActiveLeds[11] = -1;

        DisableLeds[8] = -1;
        FutureLeds[8] = -1;
        ActiveLeds[8] = 17;
      }

      if (body == "\"stage_9\"") {
        DisableLeds[14] = -1;
        FutureLeds[14] = -1;
        ActiveLeds[14] = 23;
      }

      if (body == "\"stage_10\"") {
        DisableLeds[12] = -1;
        FutureLeds[12] = -1;
        ActiveLeds[12] = 21;

        DisableLeds[14] = 23;
        FutureLeds[14] = -1;
        ActiveLeds[14] = -1;

        DisableLeds[8] = 17;
        FutureLeds[8] = -1;
        ActiveLeds[8] = -1;
      }

      if (body == "\"stage_11\"") {
        DisableLeds[12] = 21;
        FutureLeds[12] = -1;
        ActiveLeds[12] = -1;

        FutureLeds[3] = -1;
        DisableLeds[3] = -1;
        ActiveLeds[3] = 12;
      }

      if (body == "\"stage_12\"") {
        FutureLeds[3] = -1;
        DisableLeds[3] = 12;
        ActiveLeds[3] = -1;

        DisableLeds[14] = -1;
        FutureLeds[14] = -1;
        ActiveLeds[14] = 23;
      }

      if (body == "\"firework\"") {
        startFirework();
      }
      if (body == "\"train_light_off\"") {
        OUTPUTS.digitalWrite(0, LOW);
      }
      if (body == "\"train_light_on\"") {
        OUTPUTS.digitalWrite(0, HIGH);
      }
      if (body == "\"train_uf_light_on\"") {
        digitalWrite(UF_LED, HIGH);
        OUTPUTS.digitalWrite(0, LOW);
      }
      if (body == "\"train_uf_light_off\"") {
        digitalWrite(UF_LED, LOW);
      }
      if (body == "\"train_on\"") {
        myMP3.playMp3Folder(TRACK_TRAIN);
      }
      if (body == "\"item_find\"") {
        leds1[0] = CRGB(0, 128, 0);
        FastLED.show();
        isSkinPulsation = 0;
      }
      if (body == "\"item_end\"") {
        leds1[0] = CRGB(0, 0, 0);
        FastLED.show();
        state++;
        isSkinPulsation = 0;
      }

      if (body == "\"set_time\"") {
        OUTPUTS.digitalWrite(1, HIGH);
      }

      if (body == "\"day_on\"") {
        OUTPUTS.digitalWrite(0, HIGH);
        for (int i = 1; i <= 3; i++) {
          leds1[i] = CRGB(255, 255, 255);
        }
        digitalWrite(TUNNEL_LED, HIGH);
        FastLED.show();
      }

      if (body == "\"day_off\"") {
        OUTPUTS.digitalWrite(0, LOW);
        for (int i = 0; i <= 3; i++) {
          leds1[i] = CRGB(0, 0, 0);
        }
        digitalWrite(TUNNEL_LED, LOW);
        FastLED.show();
      }

      if (body == "\"ghost_game\"") {
        myMP3.stop();
        delay(50);
        myMP3.disableLoop();               // Сначала выключаем повтор
        myMP3.playMp3Folder(TRACK_GHOST);  // Запускаем трек
        myMP3.enableLoop();                // Включаем повтор для текущего трека
        ghostFlag = 1;
      }
      if (body == "\"ghost_game_end\"") {
        ghostFlag = 0;
        myMP3.stop();
      }
      Serial.println("Received POST: " + body);
      server.send(200, "application/json", "{\"status\":\"received\"}");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // обязательная функция отработки. Должна постоянно опрашиваться
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
  handlePlayerQueries();
  MapGerkon();
  MapLeds();
  StartTimer();

  if (!INPUTS.digitalRead(2)) {
    if (state == 0 && hintFlag) {
      myMP3.pause();  // Ставим фоновую музыку на паузу
	  resumeEngineSound = true;
      delay(50);
      if (language == 1) {
        myMP3.playMp3Folder(TRACK_HINT_0_RU);
      }
      if (language == 2) {
        myMP3.playMp3Folder(TRACK_HINT_0_EN);
      }
      if (language == 3) {
        myMP3.playMp3Folder(TRACK_HINT_0_AR);
      }
      if (language == 4) {
        myMP3.playMp3Folder(TRACK_HINT_0_GE);
      }
      if (language == 5) {
        myMP3.playMp3Folder(TRACK_HINT_0_SP);
      }
      if (language == 6) {
        myMP3.playMp3Folder(TRACK_HINT_0_CH);
      }
      hintFlag = 0;
    }
    if (state > 0 && state < 3 && hintFlag) {
      myMP3.pause();  // Ставим фоновую музыку на паузу
	  resumeEngineSound = true;
      delay(50);
      Serial.println(hint_counter);
      if (hint_counter == 0 && isStartTrain) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_1_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_1_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_1_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_1_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_1_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_1_CH);
        }
      }
      if (hint_counter == 1) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_2_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_2_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_2_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_2_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_2_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_2_CH);
        }
      }
      if (hint_counter == 2) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_3_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_3_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_3_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_3_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_3_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_3_CH);
        }
      }
      if (hint_counter == 3) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_4_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_4_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_4_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_4_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_4_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_4_CH);
        }
      }
      hintFlag = 0;
      hint_counter++;
      if (hint_counter > 2) {
        hint_counter = 0;
      }
    }
    if (isTrollEnd && hintFlag) {
      if (language == 1) {
        myMP3.playMp3Folder(TRACK_HINT_7_RU);
      }
      if (language == 2) {
        myMP3.playMp3Folder(TRACK_HINT_7_EN);
      }
      if (language == 3) {
        myMP3.playMp3Folder(TRACK_HINT_7_AR);
      }
      if (language == 4) {
        myMP3.playMp3Folder(TRACK_HINT_7_GE);
      }
      if (language == 5) {
        myMP3.playMp3Folder(TRACK_HINT_7_SP);
      }
      if (language == 6) {
        myMP3.playMp3Folder(TRACK_HINT_7_CH);
      }
      hintFlag = 0;
    }
    if (state > 3 && hintFlag && !isTrollEnd) {
      if (hint_counter == 0) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_5_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_5_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_5_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_5_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_5_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_5_CH);
        }
      }
      if (hint_counter == 1) {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_6_RU);
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_6_EN);
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_6_AR);
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_6_GE);
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_6_SP);
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_6_CH);
        }
      }
      hintFlag = 0;
      hint_counter++;
      if (hint_counter > 1) {
        hint_counter = 0;
      }
    }
  }

  if (fireworkActive) {
    handleFirework();
  } else {
    switch (state) {
      case 0:
        break;
      case 1:
        if (!INPUTS.digitalRead(1) && mapState == "train" && !isStartTrain) {
          Serial.println("trainclick");
          myMP3.playMp3Folder(TRACK_TRAIN_ON);
          SendData("{\"projector\":\"end\"}");
          ActiveLeds[0] = 9;
          ClickLeds[0] = -1;
          isStartTrain = 1;
          isSendOut = 1;
        }
        break;
      case 2:
        TrainGame();
        break;
      case 3:
        if (!isTrainEnd) {
          FutureLeds[13] = -1;
          ActiveLeds[13] = 22;
          DisableLeds[0] = 9;
          ActiveLeds[0] = -1;
          ClickLeds[0] = -1;
          digitalWrite(TUNNEL_LED, HIGH);
          for (int i = 1; i < 4; i++) {
            leds1[i] = CHSV(initialHue + (i * 7), 255, 255);
          }
          SendData("{\"train\":\"end\"}");
          FastLED.show();
          EVERY_N_MILLISECONDS(30) {
            FastLED.show();
            initialHue++;
          }
        } else {
          DisableLeds[0] = 9;
          ActiveLeds[0] = -1;
          ClickLeds[0] = -1;
          leds1[0] = CRGB(0, 128, 0);
          FastLED.show();

          FutureLeds[13] = -1;
          ActiveLeds[13] = 22;
          digitalWrite(TUNNEL_LED, HIGH);
          hint_counter = 0;
          state = 4;
        }
        break;
      case 4:
        WorkshopGame();
        break;
      case 5:
        GhostGame();
        break;
    }
  }
}



void MapGerkon() {

  bool keyLedActive = false;
  bool fishLedActive = false;
  bool owlLedActive = false;
  bool trainLedActive = false;

  for (int i = 0; i <= 20; i++) {
    if (ClickLeds[i] == 25) keyLedActive = true;   // Светодиод для ключа
    if (ClickLeds[i] == 24) fishLedActive = true;  // Светодиод для рыбы
    if (ClickLeds[i] == 10) owlLedActive = true;   // Светодиод для совы
    if (ClickLeds[i] == 9) trainLedActive = true;  // Светодиод для поезда
  }

  if (isStartTimer) {
    if (!INPUTS.digitalRead(6) && keyLedActive && !isKeyClick) {
      ResetTimer();
      isSendOut = 0;
      mapState = "key";
      isStartTimer = true;
      SendData("{\"map\":\"key\"}");
      Serial.println("key");
      isKeyClick = true;
      isFishClick = false;
      isOwlClick = false;
      isTrainClick = false;
      blinkLedNumber = 25;
    }

    if (!INPUTS.digitalRead(5) && fishLedActive && !isFishClick) {
      ResetTimer();
      isSendOut = 0;
      mapState = "fish";
      isStartTimer = true;
      SendData("{\"map\":\"fish\"}");
      isFishClick = true;
      isKeyClick = false;
      isOwlClick = false;
      isTrainClick = false;
      Serial.println("fish");
      blinkLedNumber = 24;
    }

    if (!INPUTS.digitalRead(3) && owlLedActive && !isOwlClick) {
      ResetTimer();
      isSendOut = 0;
      mapState = "owl";
      isStartTimer = true;
      SendData("{\"map\":\"owl\"}");
      isFishClick = false;
      isKeyClick = false;
      isTrainClick = false;
      isOwlClick = true;
      Serial.println("owl");
      blinkLedNumber = 10;
    }

    if (!INPUTS.digitalRead(4) && trainLedActive && !isTrainClick) {
      ResetTimer();
      isSendOut = 0;
      mapState = "train";
      isStartTimer = true;
      SendData("{\"map\":\"train\"}");
      isFishClick = false;
      isOwlClick = false;
      isKeyClick = false;
      isTrainClick = true;
      Serial.println("train");
      blinkLedNumber = 9;
    }
    return;  // Выходим, если таймер уже активен
  }

  // Если таймер не активен, проверяем нажатия только на АКТИВНЫЕ герконы
  if (!INPUTS.digitalRead(6) && keyLedActive) {
    if (!isKeyClick) {
      mapState = "key";
      isSendOut = 0;
      isStartTimer = true;
      SendData("{\"map\":\"key\"}");
      Serial.println("key");
      isKeyClick = true;
      isFishClick = false;
      isOwlClick = false;
      isTrainClick = false;
      blinkLedNumber = 25;
    }
  }

  if (!INPUTS.digitalRead(5) && fishLedActive) {
    if (!isFishClick) {
      mapState = "fish";
      isSendOut = 0;
      isStartTimer = true;
      SendData("{\"map\":\"fish\"}");
      isFishClick = true;
      isKeyClick = false;
      isOwlClick = false;
      isTrainClick = false;
      Serial.println("fish");
      blinkLedNumber = 24;
    }
  }

  if (!INPUTS.digitalRead(3) && owlLedActive) {
    if (!isOwlClick) {
      mapState = "owl";
      isSendOut = 0;
      isStartTimer = true;
      SendData("{\"map\":\"owl\"}");
      isFishClick = false;
      isKeyClick = false;
      isTrainClick = false;
      isOwlClick = true;
      Serial.println("owl");
      blinkLedNumber = 10;
    }
  }

  if (!INPUTS.digitalRead(4) && trainLedActive) {
    if (!isTrainClick) {
      mapState = "train";
      isSendOut = 0;
      isStartTimer = true;
      SendData("{\"map\":\"train\"}");
      isFishClick = false;
      isOwlClick = false;
      isKeyClick = false;
      isTrainClick = true;
      Serial.println("train");
      blinkLedNumber = 9;
    }
  }
}

// Новая функция для проверки, находится ли светодиод в списке активных
bool isLedActive(int ledNumber) {
  for (int i = 0; i < 22; i++) {
    if (ActiveLeds[i] == ledNumber) {
      return true; // Да, светодиод активен
    }
  }
  return false; // Нет, не активен
}

void MapLeds() {
  // --- Логика для пульсации ЖЕЛТЫХ светодиодов (ActiveLeds) ---
  static unsigned long lastPulsateTime = 0;
  static int pulsatingBrightness = 100;
  static bool fadeUp = true;
  if (millis() - lastPulsateTime > 15) {
    lastPulsateTime = millis();
    if (fadeUp) {
      pulsatingBrightness += 4;
      if (pulsatingBrightness >= 255) {
        pulsatingBrightness = 255;
        fadeUp = false;
      }
    } else {
      pulsatingBrightness -= 4;
      if (pulsatingBrightness <= 80) {
        pulsatingBrightness = 80;
        fadeUp = true;
      }
    }
  }

  // --- ВОЗВРАЩЕННАЯ ЛОГИКА для переливания цвета ClickLeds (фиолетовый-голубой) ---
  static unsigned long lightFlashTimer = 0;
  static bool up = false;
  static int light = 0;
  uint8_t r = 0, g = 0, b = 0;

  if (millis() - lightFlashTimer >= 6) {
    lightFlashTimer = millis();
    if (!up) {
      light++;
      if (light >= 255) up = true;
    } else {
      light--;
      if (light <= 0) up = false;
    }
    // Эти значения как раз и создают переход от фиолетового (128, 0, 255) к голубому (0, 128, 255)
    r = map(light, 0, 255, 128, 0);
    g = map(light, 0, 255, 0, 128);
    b = 255;
  }

  // --- Логика для пульсации ГОЛУБОГО светодиода при касании геркона ---
  static unsigned long blinkTimer = 0;
  static int fadeValue = 0;
  static bool fadeDirection = true;
  if (isStartTimer && blinkLedNumber != -1) {
    if (millis() - blinkTimer >= 5) {
      blinkTimer = millis();
      if (fadeDirection) {
        fadeValue += 8;
        if (fadeValue >= 255) fadeDirection = false;
      } else {
        fadeValue -= 8;
        if (fadeValue <= 0) fadeDirection = true;
      }
    }
  }

  // --- ОСНОВНОЙ ЦИКЛ ОТРИСОВКИ С ПРИОРИТЕТАМИ ---
  for (int i = 0; i <= 20; i++) {
    // 1. Устанавливаем базовые состояния
    if (FutureLeds[i] != -1) ledMap[FutureLeds[i]] = CRGB::White;
    if (DisableLeds[i] != -1) ledMap[DisableLeds[i]] = CRGB::Black;

    // 2. Устанавливаем ПЕРЕЛИВАНИЕ для кликабельных (ПРИОРИТЕТ НАД БЕЛЫМ)
    if (ClickLeds[i] != -1) {
      ledMap[ClickLeds[i]] = CRGB(r, g, b);
    }
    
    // 3. Устанавливаем ЖЕЛТУЮ пульсацию для активных (ПРИОРИТЕТ НАД ПЕРЕЛИВАНИЕМ)
    if (ActiveLeds[i] != -1) {
      ledMap[ActiveLeds[i]] = CHSV(55, 255, pulsatingBrightness);
    }
  }

  // 4. Устанавливаем ГОЛУБУЮ пульсацию для выбранного герконом (САМЫЙ ВЫСОКИЙ ПРИОРИТЕТ)
  if (isStartTimer && blinkLedNumber != -1) {
    uint8_t pulseG = map(fadeValue, 0, 255, 0, 128);
    uint8_t pulseB = map(fadeValue, 0, 255, 0, 255);
    ledMap[blinkLedNumber] = CRGB(0, pulseG, pulseB);
  }

  FastLED.show();
}

void TrainGame() {
  enc1.tick();
  enc2.tick();
  enc3.tick();

  if (enc1.isLeft()) {
    hue += 5;
  }
  if (enc1.isRight()) {
    hue -= 5;
  }
  if (enc2.isLeft()) {
    flickerIntensity += 5;
  }
  if (enc2.isRight()) {
    flickerIntensity -= 5;
  }

  if (enc3.isLeft()) musicCounter += 5;
  if (enc3.isRight()) musicCounter -= 5;

  if (flickerIntensity > 300) flickerIntensity = 0;
  else if (flickerIntensity < 0) flickerIntensity = 300;

  if (hue > 200) hue = 0;
  else if (hue < 0) hue = 200;

  if (hue > 0 && hue < 50) {
    hue1 = 96;
  }
  if (hue > 49 && hue < 100) {
    hue1 = 0;
  }
  if (hue > 99 && hue < 150) {
    hue1 = 160;
  }
  if (hue > 149 && hue < 200) {
    hue1 = greenHue;
  }


  if (musicCounter > 200) musicCounter = 0;
  else if (musicCounter < 0) musicCounter = 200;

  //checkGreen();

    // --- НОВЫЙ КОД С ВАШИМ РАСПРЕДЕЛЕНИЕМ ПРОЦЕНТОВ ---
  if (musicCounter >= 80 && musicCounter <= 119) {
      // Диапазон 1 (Центр, ~20%): Первый звук двигателя
      if (!musicFlag1) {
        musicFlag1 = true;
        musicFlag2 = false;
        musicFlag3 = false;
        myMP3.loop(TRACK_ENGINE1);
      }
  } else if (musicCounter >= 0 && musicCounter <= 59) {
      // Диапазон 2 (Начало, ~30%): Второй звук двигателя
      if (!musicFlag2) {
        musicFlag1 = false;
        musicFlag2 = true;
        musicFlag3 = false;
        myMP3.loop(TRACK_ENGINE2);
      }
  } else {
      // Диапазон 3 (Остальное, ~50%): Третий звук двигателя
      if (!musicFlag3) {
        musicFlag1 = false;
        musicFlag2 = false;
        musicFlag3 = true;
        myMP3.loop(TRACK_ENGINE3);
      }
  }

  if (millis() - prevFlickerTime > 30) {  // Обновляем каждые 30мс
    prevFlickerTime = millis();

    // Случайное изменение яркости вокруг базового уровня
    int flicker = random(-flickerIntensity, flickerIntensity);
    int currentBrightness = constrain(baseBrightness + flicker, 10, 255);

    //fill_solid(leds1, NUM_LEDS1, CHSV(hue, 255, 255));
    for (int i = 1; i < 4; i++) {
      leds1[i] = CHSV(hue1, 255, currentBrightness);
    }
    FastLED.show();
  }
  Serial.println(hue1 == greenHue);
  if ((hue1 == greenHue) && flickerIntensity <= 100 && musicCounter >= 80 && musicCounter <= 118) {
    if (millis() - checkTrainTimer >= 500) {
      state++;
      SendData("{\"train\":\"end\"}");
      if (language == 1)
        myMP3.playMp3Folder(TRACK_STORY_16_RU);
      if (language == 2)
        myMP3.playMp3Folder(TRACK_STORY_16_EN);
      if (language == 3)
        myMP3.playMp3Folder(TRACK_STORY_16_AR);
      if (language == 4)
        myMP3.playMp3Folder(TRACK_STORY_16_GE);
      if (language == 5)
        myMP3.playMp3Folder(TRACK_STORY_16_SP);
      if (language == 6)
        myMP3.playMp3Folder(TRACK_STORY_16_CH);
    }
  } else {
    checkTrainTimer = millis();
  }
}

void checkGreen() {
  isGreen = (abs(hue - greenHue) <= 30);
}

void handlePlayerQueries() {
  static bool flagTrack;
  static unsigned long trackTimer;
  if (millis() - trackTimer >= 2000) {
    flagTrack = 0;
  }
  if (myMP3.available()) {
    uint8_t type = myMP3.readType();
    Serial.println(type);
    if (type == 11) {  // должно быть 5
      int finishedTrack = myMP3.read();
      hintFlag = 1;
	  if (resumeEngineSound) {
        // Подсказка завершилась. Сбрасываем флаги, чтобы TrainGame() снова запустила звук.
        musicFlag1 = false;
        musicFlag2 = false;
        musicFlag3 = false;
        resumeEngineSound = false; // Сбрасываем наш флаг
      }
      Serial.print("Завершился трек: ");
      Serial.println(finishedTrack);
      delay(100);
      if (finishedTrack == TRACK_TRAIN_ON && !flagTrack) {
        if (language == 1)
          myMP3.playMp3Folder(TRACK_STORY_15_RU);
        if (language == 2)
          myMP3.playMp3Folder(TRACK_STORY_15_EN);
        if (language == 3)
          myMP3.playMp3Folder(TRACK_STORY_15_AR);
        if (language == 4)
          myMP3.playMp3Folder(TRACK_STORY_15_GE);
        if (language == 5)
          myMP3.playMp3Folder(TRACK_STORY_15_SP);
        if (language == 6)
          myMP3.playMp3Folder(TRACK_STORY_15_CH);
        trackTimer = millis();
        flagTrack = 1;
        hintFlag = 0;
      }
      if ((finishedTrack == TRACK_STORY_15_RU) || (finishedTrack == TRACK_STORY_15_EN) || (finishedTrack == TRACK_STORY_15_AR)) {
        state = 2;
        Serial.println("state");
      }

      if ((finishedTrack == TRACK_STORY_16_RU) || (finishedTrack == TRACK_STORY_16_EN) || (finishedTrack == TRACK_STORY_16_AR)) {
        trackTimer = millis();
        flagTrack = 1;
        isTrainEnd = 1;
        hintFlag = 1;
      }
    }
  }
}

void SendData(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(100);
    // Пример POST-запроса
    int httpCode = http.POST(payload);
    http.end();
  }
}

void StartTimer() {
  static int timerLeds[] = { 2, 3, 4, 5, 6, 7, 8, 1, 0 };

  if (isStartTimer) {
    if (timerIsStarting) {
      for (int i = 0; i <= 10; i++) {
        ledMap[i] = CRGB(3, 162, 255);
      }
      _secondsTimer = millis();
      timerIsStarting = false;
    }

    if (millis() - _secondsTimer >= 1000) {
      _secondsTimer = millis();
      if (timerLedsCount < 9) {
        ledMap[timerLeds[timerLedsCount]] = CRGB(0, 0, 0);
        timerLedsCount++;
      }
    }

    if (timerLedsCount >= 9) {
      timerLedsCount = 0;
      timerIsStarting = true;
      isStartTimer = false;

      isKeyClick = false;
      isFishClick = false;
      isOwlClick = false;
      isTrainClick = false;
      blinkLedNumber = -1;
      if (!isSendOut) {
        Serial.println("out");
        SendData("{\"map\":\"out\"}");
      }
    }
    FastLED.show();
  }
}
void SkinPulsation() {
  if (millis() - skinlightFlashTimer >= 18) {
    if (!skinup) {
      skinlight++;
      if (skinlight == 75)
        skinup = 1;
    } else {
      skinlight--;
      if (skinlight == 10)
        skinup = 0;
    }
    float brightness = skinlight / 75.0;  // коэффициент яркости от 0 до 1
    int green = 128 * brightness;         // Зеленая составляющая
    leds1[0] = CRGB(0, green, 0);
    FastLED.show();
    skinlightFlashTimer = millis();
  }
}

void WorkshopGame() {
  if (!INPUTS.digitalRead(0)) {
    if (!isSkinPulsation) {
      SendData("{\"train\":\"skin\"}");
      isSkinPulsation = 1;
    }
  }
  if (isSkinPulsation) {
    SkinPulsation();
  }
}

void GhostGame() {
  ghost.tick();
  if (ghostFlag == 1 and (ghost.isSingle() or ghost.isDouble() or ghost.isTriple())) {
    ghostFlag = 0;
    myMP3.stop();
    Serial.println("ghost_game_WIN");
    SendData("{\"ghost\":\"end\"}");
  }
}

void DayOn() {
}
void DayOff() {
}
