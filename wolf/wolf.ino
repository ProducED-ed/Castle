#include <FastLED.h>
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WebServer.h>
#include <HTTPClient.h>
#include "GyverButton.h"
#include <Adafruit_PCF8574.h>
#include <WiFi.h>
#include <Arduino.h>
HardwareSerial mySerial(1);

Adafruit_PCF8574 OUTPUTS;

CRGB wolfLed[10];
CRGB threeLed[10];

GButton moonGerk(32);
GButton wolfGerk(33);
GButton rightCloud1Gerk(36);
GButton rightCloud2Gerk(39);
GButton rightCloud3Gerk(34);
GButton leftCloudGerk(35);
GButton helpButton(23);
GButton ghost(25);

byte moonLed = 7;
byte leftCloudLed = 3;
byte wolfEyeLed = 2;
byte rightCloudLed = 1;

byte three1Led = 4;
byte three2Led = 5;
byte three3Led = 6;

byte SH1 = 0;

uint8_t speed = 20;           // Скорость волны
uint8_t blueIntensity = 150;  // Макс. синий
uint8_t flickerAmount = 1;    // Уровень мерцания
uint8_t hueShift = 0;         // Сдвиг оттенка (0-255)

unsigned long prevTime = 0;
unsigned long wolfTimer = 0;
unsigned long doorTimer = 0;
unsigned long repeatDoorTimer = 0;
bool doorRepeatActive = false;
const uint16_t updateInterval = 30;  // Частота обновления (мс)

unsigned long cloudFiStartTime = 0;
const unsigned long CLOUD_FI_DURATION = 8000;

unsigned long lockStartTime = 0;
bool lockActive = false;
byte lockPin = 0;
const unsigned long LOCK_DURATION = 500; // 500ms = 0.5 секунды
const unsigned long LOCK_INTERVAL = 8000; // 8000ms = 8 секунд


static unsigned long delayStartTime = 0;
static bool delayActive = false;

unsigned long startTime = 0;
int value = 30;

int state = 0;
bool lightCircut1;
bool lightCircut2;
bool lightCircut3;
int language = 1;
bool cloudFiPlaying = false;
bool hintFlag = 1;

// --- Системные треки (1-4) ---
const int TRACK_FON_WOLF = 1;
const int TRACK_CLOUD = 2;
const int TRACK_ghost = 3;
const int TRACK_CLOUD_FI = 4;

// --- Истории (5-34) ---
// Блок Story A
const int TRACK_STORY_9_A_RU = 12;
const int TRACK_STORY_9_A_EN = 22;
const int TRACK_STORY_9_A_AR = 32;
const int TRACK_STORY_9_A_GE = 5;
const int TRACK_STORY_9_A_SP = 5;
const int TRACK_STORY_9_A_CH = 5;

// Блок Story B
const int TRACK_STORY_9_B_RU = 13;
const int TRACK_STORY_9_B_EN = 23;
const int TRACK_STORY_9_B_AR = 33;
const int TRACK_STORY_9_B_GE = 6;
const int TRACK_STORY_9_B_SP = 6;
const int TRACK_STORY_9_B_CH = 6;

// Блок Story C
const int TRACK_STORY_9_C_RU = 14;
const int TRACK_STORY_9_C_EN = 24;
const int TRACK_STORY_9_C_AR = 34;
const int TRACK_STORY_9_C_GE = 7;
const int TRACK_STORY_9_C_SP = 7;
const int TRACK_STORY_9_C_CH = 7;

// --- Подсказки (8-41) ---
// Блок Hint 1
const int TRACK_HINT_1_RU = 16;
const int TRACK_HINT_1_EN = 26;
const int TRACK_HINT_1_AR = 36;
const int TRACK_HINT_1_GE = 9;
const int TRACK_HINT_1_SP = 9;
const int TRACK_HINT_1_CH = 9;

// Блок Hint 2
const int TRACK_HINT_2_RU = 17;
const int TRACK_HINT_2_EN = 27;
const int TRACK_HINT_2_AR = 37;
const int TRACK_HINT_2_GE = 10;
const int TRACK_HINT_2_SP = 10;
const int TRACK_HINT_2_CH = 10;

// Блок Hint 3
const int TRACK_HINT_3_RU = 18;
const int TRACK_HINT_3_EN = 28;
const int TRACK_HINT_3_AR = 38;
const int TRACK_HINT_3_GE = 11;
const int TRACK_HINT_3_SP = 11;
const int TRACK_HINT_3_CH = 11;

// Блок Hint 4
const int TRACK_HINT_4_RU = 19;
const int TRACK_HINT_4_EN = 29;
const int TRACK_HINT_4_AR = 39;
const int TRACK_HINT_4_GE = 11;
const int TRACK_HINT_4_SP = 11;
const int TRACK_HINT_4_CH = 11;

// Блок Hint 5
const int TRACK_HINT_5_RU = 20;
const int TRACK_HINT_5_EN = 30;
const int TRACK_HINT_5_AR = 40;
const int TRACK_HINT_5_GE = 11;
const int TRACK_HINT_5_SP = 11;
const int TRACK_HINT_5_CH = 11;

// Блок Hint 6
const int TRACK_HINT_6_RU = 21;
const int TRACK_HINT_6_EN = 31;
const int TRACK_HINT_6_AR = 41;
const int TRACK_HINT_6_GE = 11;
const int TRACK_HINT_6_SP = 11;
const int TRACK_HINT_6_CH = 11;

// Блок Hint 0
const int TRACK_HINT_0_RU = 15;
const int TRACK_HINT_0_EN = 25;
const int TRACK_HINT_0_AR = 35;
const int TRACK_HINT_0_GE = 8;
const int TRACK_HINT_0_SP = 8;
const int TRACK_HINT_0_CH = 8;

bool storyFlag1;
bool storyFlag2;
bool storyFlag3;
bool ghostFlag = 0;
bool CloudFlag = 0;
bool MP3Flag = 1;
bool TRACK_Flag = 1;

int hint_counter = 0;
bool wolfEndConfirmed = false;      // Флаг подтверждения от сервера
unsigned long wolfEndSendTimer = 0; // Таймер для периодической отправки

// --- Переменные для фонового затухания звука ---
bool isFadingOut = false;
unsigned long fadeStartTime = 0;
const unsigned long FADE_DURATION = 2000; // 2 секунды
const int INITIAL_VOLUME = 30;

// Фейерверк
bool fireworkActive = false;
unsigned long fireworkStartTime = 0;
const unsigned long FIREWORK_DURATION = 10000;  // 10 секунд салюта
CRGB fireworkColors[6] = { 
  CRGB(255, 100, 50),   // Оранжевый
  CRGB(100, 255, 100),  // Светло-зеленый
  CRGB(100, 100, 255),  // Светло-синий
  CRGB(255, 255, 100),  // Светло-желтый
  CRGB(255, 100, 255),  // Розовый
  CRGB(100, 255, 255)   // Бирюзовый
};

const char* ssid = "Castle";
const char* password = "questquest";
//const char* ssid = "ProducED";
//const char* password = "32744965";

// Настройки статического IP
IPAddress local_IP(192, 168, 0, 201);

const char* externalApi = "http://192.168.0.100:3000/api";

WebServer server(80);

DFRobotDFPlayerMini myMP3;


void WolfSendData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    // Пример POST-запроса
    String payload = "{\"wolf\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}


void GhostSendData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");

    // Пример POST-запроса
    String payload = "{\"ghost\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}

void setup() {
  delay(2000);
  Serial.begin(115200);
  delay(100);
  if (!OUTPUTS.begin(0x20, &Wire)) {
    Serial.println("Couldn't find PCF8574");
  }
  for (uint8_t p = 0; p < 8; p++) {
    OUTPUTS.pinMode(p, OUTPUT);
    OUTPUTS.digitalWrite(p, LOW);
  }

  FastLED.addLeds<WS2812B, 18, GRB>(threeLed, 10);
  FastLED.addLeds<WS2812B, 19, GRB>(wolfLed, 10);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  wolfGerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  wolfGerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  wolfGerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  wolfGerk.setType(HIGH_PULL);
  wolfGerk.setDirection(NORM_OPEN);
  wolfGerk.setTickMode(AUTO);

  moonGerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  moonGerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  moonGerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  moonGerk.setType(HIGH_PULL);
  moonGerk.setDirection(NORM_OPEN);
  moonGerk.setTickMode(AUTO);

  rightCloud1Gerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  rightCloud1Gerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  rightCloud1Gerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  rightCloud1Gerk.setType(HIGH_PULL);
  rightCloud1Gerk.setDirection(NORM_OPEN);
  rightCloud1Gerk.setTickMode(AUTO);


  rightCloud2Gerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  rightCloud2Gerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  rightCloud2Gerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  rightCloud2Gerk.setType(HIGH_PULL);
  rightCloud2Gerk.setDirection(NORM_OPEN);
  rightCloud2Gerk.setTickMode(AUTO);

  rightCloud3Gerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  rightCloud3Gerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  rightCloud3Gerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  rightCloud3Gerk.setType(HIGH_PULL);
  rightCloud3Gerk.setDirection(NORM_OPEN);
  rightCloud3Gerk.setTickMode(AUTO);

  leftCloudGerk.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  leftCloudGerk.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  leftCloudGerk.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  leftCloudGerk.setType(HIGH_PULL);
  leftCloudGerk.setDirection(NORM_OPEN);
  leftCloudGerk.setTickMode(AUTO);

  helpButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  helpButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  helpButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  helpButton.setType(HIGH_PULL);
  helpButton.setDirection(NORM_OPEN);

  ghost.setDebounce(10);       // настройка антидребезга (по умолчанию 80 мс)
  ghost.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  ghost.setClickTimeout(100);  // настройка таймаута между кликами (по умолчанию 300 мс)
  ghost.setType(LOW_PULL);
  ghost.setDirection(NORM_OPEN);
  ghost.setTickMode(AUTO);


  Serial.println("1");
  if (!myMP3.begin(mySerial, true, true)) {
    Serial.println("DFPlayer Mini not detected!");
  }
  myMP3.volume(value);
  myMP3.stop();

  Serial.println("2");
   if (!WiFi.config(local_IP)) {
     Serial.println("STA Failed to configure");
   }
  Serial.println("3");
  WiFi.begin(ssid, password);
  Serial.println("4");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  server.on("/", HTTP_POST, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });

  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      if (body == "\"game\"") {
        state = 1;
        doorRepeatActive = false;
        myMP3.playMp3Folder(TRACK_FON_WOLF);
        lightCircut1 = 0;
        storyFlag1 = 0;
        lightCircut2 = 0;
        lightCircut3 = 0;
        hint_counter = 0;
        storyFlag2 = 0;
        storyFlag3 = 0;
        ghostFlag = 0;
        CloudFlag = 0;
        MP3Flag = 1;
        TRACK_Flag = 1;
        cloudFiPlaying = false;
      }
      if (body == "\"restart\"") {
        state = 7;
        doorRepeatActive = false;
        hintFlag = 0;
        myMP3.stop();
        OUTPUTS.digitalWrite(moonLed, HIGH);
        OUTPUTS.digitalWrite(leftCloudLed, HIGH);
        OUTPUTS.digitalWrite(wolfEyeLed, HIGH);
        OUTPUTS.digitalWrite(rightCloudLed, HIGH);
        OUTPUTS.digitalWrite(three1Led, HIGH);
        OUTPUTS.digitalWrite(three2Led, HIGH);
        OUTPUTS.digitalWrite(three3Led, HIGH);
        fill_solid(wolfLed, 10, CRGB(255, 255, 255));
        fill_solid(threeLed, 10, CRGB(255, 255, 255));
        FastLED.show();
        OpenLock(SH1);
        storyFlag1 = 0;
        storyFlag2 = 0;
        storyFlag3 = 0;
        ghostFlag = 0;
        CloudFlag = 0;
        MP3Flag = 1;
        TRACK_Flag = 1;
        cloudFiPlaying = false;
        fireworkActive = false;
		wolfEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
      }
      if (body == "\"start\"") {
        state = 0;
        doorRepeatActive = false;
        hintFlag = 1;
        myMP3.stop();
		wolfEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
      }
      if (body == "\"ready\"") {
        state = 0;
        doorRepeatActive = false;
        myMP3.stop();
		wolfEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
      }
      if (body == "\"volume_up\"") {
        value = value + 1;
        if (value >= 30) value = 30;
        myMP3.volume(value);
        Serial.println(value);
      }

      if (body == "\"volume_down\"") {
        value = value - 1;
        if (value <= 0) value = 0;
        myMP3.volume(value);
        Serial.println(value);
      }
      if (body.startsWith("\"set_level_")) {
    // Извлекаем число из строки "set_level_25"
    int startPos = body.indexOf('_', 6) + 1; // Находим позицию после второго '_'
    int endPos = body.lastIndexOf('"'); // Находим позицию закрывающей кавычки
    
    if (startPos > 0 && endPos > startPos) {
        String levelStr = body.substring(startPos, endPos);
        int newLevel = levelStr.toInt();
        
        // Проверяем корректность уровня (например, от 0 до 30)
        if (newLevel >= 0 && newLevel <= 30) {
            value = newLevel;
            myMP3.volume(value);
            Serial.print("Установлена громкость: ");
            Serial.println(value);
        } else {
            Serial.println("Некорректный уровень громкости");
        }
    }
}
	  if (body == "\"confirm_wolf_end\"") {
        wolfEndConfirmed = true;
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
      if (body == "\"open_door\"") {
        OpenLock(SH1);
        state = 6;
      }
      if (body == "\"skip\"") {
        myMP3.stop();
        OUTPUTS.digitalWrite(moonLed, HIGH);
        OUTPUTS.digitalWrite(leftCloudLed, HIGH);
        OUTPUTS.digitalWrite(wolfEyeLed, HIGH);
        OUTPUTS.digitalWrite(rightCloudLed, HIGH);
        OUTPUTS.digitalWrite(three1Led, HIGH);
        OUTPUTS.digitalWrite(three2Led, HIGH);
        OUTPUTS.digitalWrite(three3Led, HIGH);
        // state = 6; // Убираем немедленный переход
        state = 5; // Переходим в состояние отправки
        wolfEndSendTimer = millis(); // Готовимся к отправке
        fill_solid(wolfLed, 10, CRGB(255, 255, 255));
        fill_solid(threeLed, 10, CRGB(255, 255, 255));
        FastLED.show();
        delay(50);
        if (language == 1)
          myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
        if (language == 2)
          myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
        if (language == 3)
          myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
        if (language == 4)
          myMP3.playMp3Folder(TRACK_STORY_9_C_GE);
        if (language == 5)
          myMP3.playMp3Folder(TRACK_STORY_9_C_SP);
        if (language == 6)
          myMP3.playMp3Folder(TRACK_STORY_9_C_CH);
        OpenLock(SH1);
        doorRepeatActive = true;
        repeatDoorTimer = millis();
      }

      if (body == "\"day_on\"") {
        myMP3.stop();
        doorRepeatActive = false;
        state = 99;
        OUTPUTS.digitalWrite(moonLed, HIGH);
        OUTPUTS.digitalWrite(leftCloudLed, HIGH);
        OUTPUTS.digitalWrite(wolfEyeLed, HIGH);
        OUTPUTS.digitalWrite(rightCloudLed, HIGH);
        OUTPUTS.digitalWrite(three1Led, HIGH);
        OUTPUTS.digitalWrite(three2Led, HIGH);
        OUTPUTS.digitalWrite(three3Led, HIGH);
        fill_solid(wolfLed, 10, CRGB(255, 255, 255));
        fill_solid(threeLed, 10, CRGB(255, 255, 255));
        FastLED.show();
      }

      if (body == "\"day_off\"") {
        state = 0;
        doorRepeatActive = false;
      }

      if (body == "\"ghost_game\"") {
        myMP3.stop();
        delay(50);
        myMP3.disableLoop();               // Сначала выключаем повтор
        myMP3.playMp3Folder(TRACK_ghost);  // Запускаем трек
        myMP3.enableLoop();                // Включаем повтор для текущего трек
        Serial.println(TRACK_ghost);
        ghostFlag = 1;
      }
      if (body == "\"ghost_game_end\"") {
        ghostFlag = 0;
        myMP3.stop();
      }

      // Добавь эту команду для запуска салюта
      if (body == "\"firework\"") {
        startFirework();
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
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
  
  handlePlayerQueries();
  handleFadeOut();

  // Обработка салюта (добавь эту строку)
  if (fireworkActive) {
    handleFirework();
    return;  // Пропускаем остальную логику во время салюта
  }

  helpButton.tick();
  if (helpButton.isPress() && ghostFlag == 0) {
    if (state == 0 && hintFlag) {
      myMP3.pause();  // Ставим фоновую музыку на паузу
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
      Serial.println("1");
      myMP3.pause();  // Ставим фоновую музыку на паузу
      delay(50);
      if (hint_counter == 0) {
        Serial.println("2");
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
      if(hint_counter == 1)
       {
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
      if(hint_counter == 2)
       {
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
      if(hint_counter == 3)
       {
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
      if(hint_counter == 4)
       {
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
      hint_counter++;
      if(hint_counter>4){
        hint_counter=0;
      }
      hintFlag=0;
    }
    if (state > 4 && hintFlag) {
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
      hintFlag = 0;
    }
  }


  if (ghostFlag == 1 and (ghost.isSingle() or ghost.isDouble() or ghost.isTriple())) {
    ghostFlag = 0;
    myMP3.stop();
    Serial.println("ghost_game_WIN");
    GhostSendData();
  }


  switch (state) {
    case 0:
      OUTPUTS.digitalWrite(moonLed, LOW);
      OUTPUTS.digitalWrite(leftCloudLed, LOW);
      OUTPUTS.digitalWrite(wolfEyeLed, LOW);
      OUTPUTS.digitalWrite(rightCloudLed, LOW);
      OUTPUTS.digitalWrite(three1Led, LOW);
      OUTPUTS.digitalWrite(three2Led, LOW);
      OUTPUTS.digitalWrite(three3Led, LOW);
      FastLED.clear();
      FastLED.show();
      break;
    case 1:
      MoonGame();
      break;
    case 2:
      CloudGame();
      break;
    case 3:
      LeftCloudGame();
      break;
    case 4:
      WolfGame();
      break;
	case 5:
      // Новое состояние для надежной отправки данных
      if (!wolfEndConfirmed) {
        if (millis() - wolfEndSendTimer > 1000) {
          WolfSendData();
          wolfEndSendTimer = millis();
        }
      } else {
        // Как только получили подтверждение, открываем замок и переходим дальше
        OpenLock(SH1);
        state = 6;
        doorRepeatActive = true;
        repeatDoorTimer = millis();
      }
      auroraEffect(); // Продолжаем анимацию
      FastLED.show();
      break;
    case 6:
      if (doorRepeatActive) {
        if (millis() - repeatDoorTimer >= 10000) {
          OpenLock(SH1);
          repeatDoorTimer = millis();
        }
      }
      if (millis() - prevTime >= updateInterval) {
        prevTime = millis();
        auroraEffect();
        FastLED.show();
      }
      break;
    case 7:
      break;
  }
}

void MoonGame() {
  //static bool
  //Serial.println(moonGerk.isHold());
  FastLED.clear();
  FastLED.show();
  OUTPUTS.digitalWrite(moonLed, LOW);
  OUTPUTS.digitalWrite(leftCloudLed, LOW);
  OUTPUTS.digitalWrite(wolfEyeLed, LOW);
  OUTPUTS.digitalWrite(rightCloudLed, LOW);
  OUTPUTS.digitalWrite(three1Led, LOW);
  OUTPUTS.digitalWrite(three2Led, LOW);
  OUTPUTS.digitalWrite(three3Led, LOW);
  OUTPUTS.digitalWrite(wolfEyeLed, LOW);
  if (moonGerk.isHold()) {
    OUTPUTS.digitalWrite(moonLed, HIGH);
    OUTPUTS.digitalWrite(rightCloudLed, HIGH);
    if (!storyFlag1) {
      //myMP3.pause(); // Ставим фоновую музыку на паузу
      delay(150);
      if (language == 1)
        myMP3.playMp3Folder(TRACK_STORY_9_A_RU);
      if (language == 2)
        myMP3.playMp3Folder(TRACK_STORY_9_A_EN);
      if (language == 3)
        myMP3.playMp3Folder(TRACK_STORY_9_A_AR);
      if (language == 4)
        myMP3.playMp3Folder(TRACK_STORY_9_A_GE);
      if (language == 5)
        myMP3.playMp3Folder(TRACK_STORY_9_A_SP);
      if (language == 6)
        myMP3.playMp3Folder(TRACK_STORY_9_A_CH);
      storyFlag1 = 1;
    }
    state++;
  } else {
    OUTPUTS.digitalWrite(moonLed, LOW);
    OUTPUTS.digitalWrite(rightCloudLed, LOW);
    OUTPUTS.digitalWrite(leftCloudLed, LOW);
    OUTPUTS.digitalWrite(wolfEyeLed, LOW);
    OUTPUTS.digitalWrite(three1Led, LOW);
    OUTPUTS.digitalWrite(three2Led, LOW);
    OUTPUTS.digitalWrite(three3Led, LOW);
  }
}

void CloudGame() {
  FastLED.clear();
  FastLED.show();

  if (!moonGerk.isHold()) {
    if (!delayActive) {
      OUTPUTS.digitalWrite(moonLed, LOW);
      // Начинаем отсчет 2 секунд
      delayStartTime = millis();
      delayActive = true;
      Serial.println("Moon released - 2s delay started");
    }

    // Проверяем прошло ли 2 секунды
    if (delayActive && millis() - delayStartTime >= 2000) {
      OUTPUTS.digitalWrite(moonLed, HIGH);
      OUTPUTS.digitalWrite(rightCloudLed, HIGH);
      state = 1;
      delayActive = false;
      Serial.println("2s delay completed - state = 1");
    }
  } else {
    // MoonGerk снова нажат - сбрасываем таймер
    delayActive = false;
    OUTPUTS.digitalWrite(moonLed, HIGH);
  }

  if (rightCloud1Gerk.isHold()) {
    OUTPUTS.digitalWrite(three1Led, HIGH);
  } else {
    OUTPUTS.digitalWrite(three1Led, LOW);
  }

  if (rightCloud2Gerk.isHold()) {
    OUTPUTS.digitalWrite(three2Led, HIGH);
  } else {
    OUTPUTS.digitalWrite(three2Led, LOW);
  }

  if (rightCloud3Gerk.isHold()) {
    OUTPUTS.digitalWrite(three3Led, HIGH);
    OUTPUTS.digitalWrite(leftCloudLed, HIGH);
    state++;
    delayActive = false;  // Сбрасываем таймер
  } else {
    OUTPUTS.digitalWrite(three3Led, LOW);
    OUTPUTS.digitalWrite(leftCloudLed, LOW);
  }
}

void LeftCloudGame() {
  FastLED.clear();
  FastLED.show();
  if (!rightCloud3Gerk.isHold()) {
    state = 2;
  }
  if (!moonGerk.isHold()) {
    if (!delayActive) {
      OUTPUTS.digitalWrite(moonLed, LOW);
      // Начинаем отсчет 2 секунд
      delayStartTime = millis();
      delayActive = true;
      Serial.println("Moon released - 2s delay started");
    }

    // Проверяем прошло ли 2 секунды
    if (delayActive && millis() - delayStartTime >= 2000) {
      OUTPUTS.digitalWrite(moonLed, HIGH);
      OUTPUTS.digitalWrite(rightCloudLed, HIGH);
      state = 1;
      delayActive = false;
      Serial.println("2s delay completed - state = 1");
    }
  } else {
    // MoonGerk снова нажат - сбрасываем таймер
    delayActive = false;
    OUTPUTS.digitalWrite(moonLed, HIGH);
  }



  if (leftCloudGerk.isHold() and !moonGerk.isHold()) {

    if (!CloudFlag) {
      state = 4;
      CloudFlag = 1;
      Serial.println("оаоаоа");
      OUTPUTS.digitalWrite(wolfEyeLed, HIGH);
      if (!TRACK_Flag) myMP3.playMp3Folder(TRACK_CLOUD);
    }
  } else {
    CloudFlag = 0;
  }
}

// Добавьте глобальную переменную

void WolfGame() {
  if (TRACK_Flag) {
    Serial.println("Story B started");
    myMP3.stop();
    delay(50);
    if (language == 1) myMP3.playMp3Folder(TRACK_STORY_9_B_RU);
    if (language == 2) myMP3.playMp3Folder(TRACK_STORY_9_B_EN);
    if (language == 3) myMP3.playMp3Folder(TRACK_STORY_9_B_AR);
    if (language == 4) myMP3.playMp3Folder(TRACK_STORY_9_B_GE);
    if (language == 5) myMP3.playMp3Folder(TRACK_STORY_9_B_SP);
    if (language == 6) myMP3.playMp3Folder(TRACK_STORY_9_B_CH);
    
    TRACK_Flag = 0;
    cloudFiPlaying = true;
  }

  OUTPUTS.digitalWrite(wolfEyeLed, HIGH);

  // Проверяем геркон ВОЛКА немедленно ---
  // Это позволяет выиграть "в первой фазе", прервав story_B.
  if (wolfGerk.isHold()) {
    auroraEffect(); // Включить анимацию
    FastLED.show();
    myMP3.stop();   // Прервать story_B (или любой другой трек)
    delay(50);
    
    // Воспроизвести историю победы story_C
    if (language == 1) myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
    if (language == 2) myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
    if (language == 3) myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
    if (language == 4) myMP3.playMp3Folder(TRACK_STORY_9_C_GE);
    if (language == 5) myMP3.playMp3Folder(TRACK_STORY_9_C_SP);
    if (language == 6) myMP3.playMp3Folder(TRACK_STORY_9_C_CH);
    
    state = 5; // Переходим в состояние отправки данных о победе 
    wolfEndSendTimer = millis(); // Готовимся к отправке
    
    // Сбрасываем флаги, чтобы не мешать следующему состоянию
    cloudFiPlaying = false; 
    TRACK_Flag = false; 
    
    return; // Выходим из WolfGame(), так как мы победили
  }

  if (cloudFiPlaying) {
    return;
  }
  
  lightCircut1 = !rightCloud3Gerk.isHold();
  lightCircut2 = !leftCloudGerk.isHold();

  if ((lightCircut1 || lightCircut2)) {
    state = 1; // <- МОМЕНТАЛЬНЫЙ СБРОС СОСТОЯНИЯ
    
    // --- ЗАПУСКАЕМ ФОНОВОЕ ЗАТУХАНИЕ ВМЕСТО СТАРОГО ЦИКЛА ---
    if (!isFadingOut) { // Проверяем, что затухание еще не запущено
        isFadingOut = true;
        fadeStartTime = millis();
        Serial.println("Starting background fade out...");
    }
    // Старый блокирующий цикл for() полностью удален
  }

  if (wolfGerk.isHold()) {
    // state++; // Убираем немедленный переход
    doorTimer = millis();
    auroraEffect();
    FastLED.show();
    myMP3.stop();
    delay(50);
    if (language == 1) myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
    if (language == 2) myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
    if (language == 3) myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
    if (language == 4) myMP3.playMp3Folder(TRACK_STORY_9_C_GE);
    if (language == 5) myMP3.playMp3Folder(TRACK_STORY_9_C_SP);
    if (language == 6) myMP3.playMp3Folder(TRACK_STORY_9_C_CH);
	state = 5; // Переходим в новое состояние отправки
    wolfEndSendTimer = millis(); // Готовимся к отправке
  }
}

void OpenDoor() {
  if (millis() - doorTimer >= 8000) {
    Serial.println("WolfWIN");
    WolfSendData();
    OpenLock(SH1);
    state++;
    doorRepeatActive = true;
    repeatDoorTimer = millis();
  }
}

void OpenLock(byte num) {
  OUTPUTS.digitalWrite(num, HIGH);
  delay(500);
  OUTPUTS.digitalWrite(num, LOW);
}

void auroraEffect() {
  hueShift++;  // Плавный сдвиг оттенка

  for (int i = 0; i < 10; i++) {
    // Основа: синий + белый с волнами
    uint8_t wave1 = beatsin8(speed, 50, blueIntensity, 0, i * 10);
    uint8_t wave2 = beatsin8(speed + 5, 50, 200, 0, i * 10 + 128);

    // Добавляем мерцание
    wave1 += random8(-flickerAmount, flickerAmount);
    wave2 += random8(-flickerAmount, flickerAmount);

    // Ограничиваем и смешиваем цвета
    wave1 = constrain(wave1, 50, 255);
    wave2 = constrain(wave2, 50, 255);

    // Создаём цвет (бело-голубой с оттенком)
    wolfLed[i] = CRGB(
      wave2 / 3,              // R (белый)
      wave2 / 3 + wave1 / 4,  // G (лёгкий зелёный оттенок)
      wave1 + wave2 / 2       // B (синий + белый)
    );

    threeLed[i] = CRGB(
      wave2 / 3,              // R (белый)
      wave2 / 3 + wave1 / 4,  // G (лёгкий зелёный оттенок)
      wave1 + wave2 / 2       // B (синий + белый)
    );

    // Добавляем динамический оттенок (опционально)
    wolfLed[i] += CHSV(hueShift + i * 2, 100, 20);
    threeLed[i] += CHSV(hueShift + i * 2, 100, 20);
  }
}

void handlePlayerQueries() {
  static bool flagTrack;
  static unsigned long trackTimer;
  if (millis() - trackTimer >= 2000) {
    flagTrack = 0;
  }
  if (myMP3.available()) {
    uint8_t type = myMP3.readType();
    switch (type) {
      case TimeOut:
        Serial.println("Таймаут");
        break;
      case WrongStack:
        Serial.println("Неправильный стек");
        break;
      case DFPlayerCardInserted:
        Serial.println("Карта вставлена");
        break;
      case DFPlayerCardRemoved:
        Serial.println("Карта извлечена");
        break;
      case DFPlayerError:
        Serial.println("Ошибка: " + String(myMP3.read()));
        break;
      case DFPlayerPlayFinished:
        Serial.println("Трек завершен: " + String(myMP3.read()));
        break;
      case DFPlayerFeedBack:
        Serial.println("Команда выполнена: " + String(myMP3.read()));
        break;
      default:
        Serial.println("Неизвестный тип: " + String(type));
        break;
    }
    Serial.println(type);
    if (type == 11) { // 11 - это код завершения трека от плеера
      int finishedTrack = myMP3.read();
      Serial.print("Завершился трек: ");
      Serial.println(finishedTrack);
      hintFlag = 1;

      // ДОБАВЛЕННЫЙ БЛОК ДЛЯ СНЯТИЯ "ЗАМКА"
      if (finishedTrack == TRACK_STORY_9_B_RU ||
          finishedTrack == TRACK_STORY_9_B_EN ||
          finishedTrack == TRACK_STORY_9_B_AR ||
          finishedTrack == TRACK_STORY_9_B_GE) { // Для GE, SP, CH номер трека один - 6
            cloudFiPlaying = false;
            Serial.println("Story B finished, unlocking game state.");
      }


      // Проверяем, что мы в одном из активных игровых состояний (1, 2, 3 или 4)
      if ((state >= 1 && state <= 4) && finishedTrack != TRACK_FON_WOLF) {
        if (!flagTrack) {
          myMP3.playMp3Folder(TRACK_FON_WOLF);
          trackTimer = millis();
          flagTrack = 1;
        }
      }
    }
  }
}

void handleFadeOut() {
  // Если процесс затухания не активен, ничего не делаем
  if (!isFadingOut) {
    return;
  }

  unsigned long elapsedTime = millis() - fadeStartTime;

  if (elapsedTime >= FADE_DURATION) {
    // Затухание завершено
    myMP3.volume(0);
    myMP3.stop();
    delay(50);
    myMP3.volume(value); // Возвращаем громкость для следующих треков
    myMP3.playMp3Folder(TRACK_FON_WOLF);
    isFadingOut = false; // Завершаем процесс
    Serial.println("Fade out complete. Playing background music.");
  } else {
    // Процесс затухания еще идет
    // Рассчитываем новую громкость на основе прошедшего времени
    float progress = (float)elapsedTime / FADE_DURATION;
    int newVolume = value * (1.0 - progress);
    myMP3.volume(newVolume);
  }
}

// Функции для фейерверка
void startFirework() {
  fireworkActive = true;
  fireworkStartTime = millis();
  Serial.println("firework:started");
}

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

  // Проверяем время окончания салюта

  // Создаем новые взрывы
  if (millis() - lastFireworkTime >= 600) {
    lastFireworkTime = millis();

    // Ищем свободный слот для нового взрыва
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions[i].phase == 0) {
        explosions[i].color = fireworkColors[random(6)];
        explosions[i].center = random(0, 10);  // Взрывы по всей длине ленты
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
        int radius = progress * 5;  // Меньший радиус для коротких лент

        for (int i = 0; i < 10; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= radius) {
            float intensity = 1.0 - (float)distance / radius;
            wolfLed[i] = explosions[e].color;
            threeLed[i] = explosions[e].color;
            wolfLed[i].fadeToBlackBy(255 - (intensity * 255));
            threeLed[i].fadeToBlackBy(255 - (intensity * 255));
          }
        }
      } else if (elapsed < 1000) {
        // Фаза затухания
        float fadeProgress = (float)(elapsed - 500) / 500.0;
        uint8_t fadeAmount = fadeProgress * 255;

        for (int i = 0; i < 10; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= 5) {
            wolfLed[i] = explosions[e].color;
            threeLed[i] = explosions[e].color;
            wolfLed[i].fadeToBlackBy(fadeAmount);
            threeLed[i].fadeToBlackBy(fadeAmount);
          }
        }
      } else {
        // Завершаем взрыв
        explosions[e].phase = 0;
        for (int i = 0; i < 10; i++) {
          int distance = abs(i - explosions[e].center);
          if (distance <= 5) {
            wolfLed[i] = CRGB::Black;
            threeLed[i] = CRGB::Black;
          }
        }
      }
    }
  }

  // Плавное затухание всех светодиодов
  EVERY_N_MILLISECONDS(20) {
    for (int i = 0; i < 10; i++) {
      if (wolfLed[i] != CRGB::Black) {
        wolfLed[i].fadeToBlackBy(8);
      }
      if (threeLed[i] != CRGB::Black) {
        threeLed[i].fadeToBlackBy(8);
      }
    }
  }

  FastLED.show();
}
