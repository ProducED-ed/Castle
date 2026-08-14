// Сколько ждать после подачи питания, прежде чем включать периферию.
static const unsigned long STARTUP_SETTLE_MS = 1200;

#include <FastLED.h>
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WebServer.h>
#include <HTTPClient.h>
#include "GyverButton.h"
#include <Adafruit_PCF8574.h>
#include <WiFi.h>
#include <Arduino.h>
// --- ДОБАВЛЕНО ДЛЯ OTA ---
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
// -------------------------
HardwareSerial mySerial(1);

Adafruit_PCF8574 OUTPUTS;

// === DIAG MODE (тех-пульт /tech → вкладка "Диагностика") ===
// Когда diagModeActive == true:
//  - вся игровая логика в loop() пропускается (early return)
//  - входящие /data команды игнорируются, кроме "diag_*"
//  - каждые 200ms ESP32 шлёт snapshot всех I/O на /api?device=wolf
bool diagModeActive = false;
unsigned long diagLastSnapshot = 0;
const unsigned long DIAG_SNAPSHOT_INTERVAL = 200;  // ms
volatile unsigned long vibCount = 0;  // счётчик прерываний с пина 25 (для отображения в diag)

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


unsigned long prevTime = 0;
unsigned long wolfTimer = 0;
unsigned long doorTimer = 0;
unsigned long repeatDoorTimer = 0;
bool doorRepeatActive = false;

// 2026-08-07: отложенное гашение по day_off — см. обработчик day_off.
bool dayOffPending = false;
unsigned long dayOffTimer = 0;
const unsigned long DAY_OFF_DELAY_MS = 20000;
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
// 2026-08-14. Подсказки жили ТОЛЬКО на событии «трек доиграл» от DFPlayer, а
// эти события на ESP32 теряются (см. историю с Сейфом). Одно потерянное
// событие — и hintFlag навсегда остаётся нулём: геркон нажимают, а подсказки
// нет до конца партии. Ровно это видели на CLC4: сперва замолчал Волк, потом
// Поезд. Страховка по времени: через HINT_REARM_MS подсказка разрешается сама.
// Правило: рядом с каждым hintFlag = 0 обязателен hintPlayedAt —
//   millis() — если подсказку надо разрешить обратно по таймеру,
//   0        — если разрешать будет другой код (старт, ready, конец истории).
unsigned long hintPlayedAt = 0;
const unsigned long HINT_REARM_MS = 20000;

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
const int TRACK_STORY_9_A_FR = 42;
const int TRACK_STORY_9_A_UK = 52;
const int TRACK_STORY_9_A_PL = 62;

// Блок Story B
const int TRACK_STORY_9_B_RU = 13;
const int TRACK_STORY_9_B_EN = 23;
const int TRACK_STORY_9_B_AR = 33;
const int TRACK_STORY_9_B_FR = 43;
const int TRACK_STORY_9_B_UK = 53;
const int TRACK_STORY_9_B_PL = 63;

// Блок Story C
const int TRACK_STORY_9_C_RU = 14;
const int TRACK_STORY_9_C_EN = 24;
const int TRACK_STORY_9_C_AR = 34;
const int TRACK_STORY_9_C_FR = 44;
const int TRACK_STORY_9_C_UK = 54;
const int TRACK_STORY_9_C_PL = 64;

// --- Подсказки (8-41) ---
// Блок Hint 1
const int TRACK_HINT_1_RU = 16;
const int TRACK_HINT_1_EN = 26;
const int TRACK_HINT_1_AR = 36;
const int TRACK_HINT_1_FR = 46;
const int TRACK_HINT_1_UK = 56;
const int TRACK_HINT_1_PL = 66;

// Блок Hint 2
const int TRACK_HINT_2_RU = 17;
const int TRACK_HINT_2_EN = 27;
const int TRACK_HINT_2_AR = 37;
const int TRACK_HINT_2_FR = 47;
const int TRACK_HINT_2_UK = 57;
const int TRACK_HINT_2_PL = 67;

// Блок Hint 3
const int TRACK_HINT_3_RU = 18;
const int TRACK_HINT_3_EN = 28;
const int TRACK_HINT_3_AR = 38;
const int TRACK_HINT_3_FR = 48;
const int TRACK_HINT_3_UK = 58;
const int TRACK_HINT_3_PL = 68;

// Блок Hint 4
const int TRACK_HINT_4_RU = 19;
const int TRACK_HINT_4_EN = 29;
const int TRACK_HINT_4_AR = 39;
const int TRACK_HINT_4_FR = 49;
const int TRACK_HINT_4_UK = 59;
const int TRACK_HINT_4_PL = 69;

// Блок Hint 5
const int TRACK_HINT_5_RU = 20;
const int TRACK_HINT_5_EN = 30;
const int TRACK_HINT_5_AR = 40;
const int TRACK_HINT_5_FR = 50;
const int TRACK_HINT_5_UK = 60;
const int TRACK_HINT_5_PL = 70;

// Блок Hint 6
const int TRACK_HINT_6_RU = 21;
const int TRACK_HINT_6_EN = 31;
const int TRACK_HINT_6_AR = 41;
const int TRACK_HINT_6_FR = 51;
const int TRACK_HINT_6_UK = 61;
const int TRACK_HINT_6_PL = 71;

// Блок Hint 0
const int TRACK_HINT_0_RU = 15;
const int TRACK_HINT_0_EN = 25;
const int TRACK_HINT_0_AR = 35;
const int TRACK_HINT_0_FR = 45;
const int TRACK_HINT_0_UK = 55;
const int TRACK_HINT_0_PL = 65;

bool storyFlag1;
bool storyFlag2;
bool storyFlag3;
bool ghostFlag = 0;

// ЗАЩИТА ОТ ГОНКИ КОМАНД (2026-07-29). Сервер шлёт команды на ESP32
// АСИНХРОННО — каждую в отдельном потоке с retry (см. send_esp32_command()
// в CastleServer.py), поэтому порядок доставки НЕ гарантирован. При быстром
// скипе уровней с пульта "game" может прийти ПОСЛЕ "skip" и откатить
// состояние назад — фоновая музыка запустится заново и будет играть до
// конца квеста. Флаг снимается только при старте новой игры.
// Аналогичная защита стоит в chest.ino (там баг и проявился у клиента CLC2).
bool stageFinished = false;

// === РЕЖИМ ЭТАПА ВОЛКА ===
// Переключатель в Settings на пульте. Сервер присылает wolf_mode_normal /
// wolf_mode_easy, состояние переживает рестарт (сервер шлёт его при старте).
//
// 14.08.2026 смысл тумблера перевернули: во всём квесте включённый тумблер
// теперь означает ЛЁГКИЙ режим — раньше у Волка он означал сложный, а у
// Чемоданов лёгкий, и это путало.
//
// НОРМАЛЬНЫЙ (по умолчанию): отпустили луну — через MOON_RELEASE_TIMEOUT_MS
//   всё гаснет и этап откатывается в state=1. Вернули луну раньше — таймер
//   сбрасывается.
// ЛЁГКИЙ: отпустили луну — НИЧЕГО не происходит, прогресс сохраняется.
bool wolfEasyMode = false;
const unsigned long MOON_RELEASE_TIMEOUT_MS = 5000;   // только в нормальном режиме

// GRACE ДЛЯ ОБЛАКОВ (оба режима). Раньше разрыв цепи облаков в WolfGame()
// откатывал этап МГНОВЕННО — хватало микро-дрожания руки в момент касания
// геркона волка, и всё собиралось заново. Теперь разрыв должен продержаться
// дольше CLOUD_GRACE_MS, иначе он игнорируется.
const unsigned long CLOUD_GRACE_MS = 400;
unsigned long cloudBrokenSince = 0;   // 0 = цепь целая
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
IPAddress local_IP(192, 168, 4, 201);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

const char* externalApi = "http://192.168.4.1:3000/api";

WebServer server(80);

DFRobotDFPlayerMini myMP3;


void WolfSendData() {
  if (WiFi.status() == WL_CONNECTED) {
    sendLogToServer("{\"log\":\"Wolf game finished, sending 'end' to server.\"}");
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
    sendLogToServer("{\"log\":\"Ghost game finished, sending 'end' to server.\"}");
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");

    // Пример POST-запроса
    String payload = "{\"ghost\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}

void sendLogToServer(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.4.1:3000/api");
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);

    if (httpCode < 0) {
      Serial.println("Error sending log, code: " + String(httpCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected, cannot send log.");
  }
}

// --- Переменные для датчика вибрации ---
// volatile обязательно для переменных, используемых в прерываниях
volatile bool vibrationDetected = false; 

// Функция, которая вызывается мгновенно при ударе
void IRAM_ATTR vibrationISR() {
  vibrationDetected = true;
  vibCount++;
}

// === DIAG: парсер команд diag_set:<output>:<value> ===
// Wolf-специфика. По схеме PCF8574 OUTPUTS (0x20):
//   pin 0 = SH1 → электрозамок   pin 4 = three1Led
//   pin 1 = rightCloudLed         pin 5 = three2Led
//   pin 2 = wolfEyeLed            pin 6 = three3Led
//   pin 3 = leftCloudLed          pin 7 = moonLed
// WS2812: threeLed[10] (GPIO 18) — подсветка тройки; wolfLed[10] (GPIO 19) — глаза/туман волка
void handleDiagSet(String body) {
  int firstQuote = body.indexOf('"');
  int lastQuote  = body.lastIndexOf('"');
  if (firstQuote < 0 || lastQuote <= firstQuote) return;
  String s = body.substring(firstQuote + 1, lastQuote);
  s = s.substring(strlen("diag_set:"));

  int colon = s.indexOf(':');
  if (colon < 0) return;
  String key = s.substring(0, colon);
  String val = s.substring(colon + 1);

  auto hexToCRGB = [](const String& h) -> CRGB {
    long n = strtol(h.c_str(), NULL, 16);
    return CRGB((n >> 16) & 0xFF, (n >> 8) & 0xFF, n & 0xFF);
  };

  int onv = val.toInt() ? HIGH : LOW;
  if      (key == "moon_led")     OUTPUTS.digitalWrite(7, onv);
  else if (key == "left_cloud")   OUTPUTS.digitalWrite(3, onv);
  else if (key == "right_cloud")  OUTPUTS.digitalWrite(1, onv);
  else if (key == "wolf_eye")     OUTPUTS.digitalWrite(2, onv);
  else if (key == "three1")       OUTPUTS.digitalWrite(4, onv);
  else if (key == "three2")       OUTPUTS.digitalWrite(5, onv);
  else if (key == "three3")       OUTPUTS.digitalWrite(6, onv);
  else if (key == "wolf_strip") {
    CRGB c = hexToCRGB(val);
    for (int i = 0; i < 10; i++) wolfLed[i] = c;
    FastLED.show();
  } else if (key == "three_strip") {
    CRGB c = hexToCRGB(val);
    for (int i = 0; i < 10; i++) threeLed[i] = c;
    FastLED.show();
  } else if (key == "volume") {
    value = constrain(val.toInt(), 0, 30);
    myMP3.volume(value);
  } else if (key == "play") {
    myMP3.play(val.toInt());
  } else if (key == "stop_audio") {
    myMP3.stop();
  } else if (key == "reset_vib") {
    vibCount = 0;
  } else if (key == "lock_pulse") {
    // 500мс импульс на электрозамок (PCF pin 0). Намеренно НЕ делаем
    // continuous-toggle: solenoid сгорит если оставить его под напряжением.
    // Логика идентична игровой OpenLock(SH1).
    OUTPUTS.digitalWrite(0, HIGH);
    delay(500);
    OUTPUTS.digitalWrite(0, LOW);
  }
}

// === DIAG: периодический snapshot всех I/O ===
// Формат: {"diag":{"in":[8],"vib":N,"out":[8],"wolf":"<6hex>","three":"<6hex>",
//                  "vol":N,"rssi":N,"heap":N,"uptime":N,"temp":N}}
// in[0..7]: moon(32), wolf(33), help(23), ghost(25), leftCloud(35),
//           rightCloud1(36), rightCloud2(39), rightCloud3(34)
// Нормализация: 1 = ЗАМКНУТ/НАЖАТ.
//   HIGH_PULL сенсоры (всё кроме ghost) активны при digitalRead==LOW.
//   ghost (LOW_PULL) активен при digitalRead==HIGH.
void sendDiagSnapshot() {
  if (WiFi.status() != WL_CONNECTED) return;

  auto inv = [](int pin) { return digitalRead(pin) == LOW ? "1" : "0"; };
  String payload = "{\"diag\":{\"in\":[";
  payload += inv(32); payload += ",";  // moon
  payload += inv(33); payload += ",";  // wolf
  payload += inv(23); payload += ",";  // help
  payload += (digitalRead(25) == HIGH ? "1" : "0"); payload += ",";  // ghost (LOW_PULL)
  payload += inv(35); payload += ",";  // leftCloud
  payload += inv(36); payload += ",";  // rightCloud1
  payload += inv(39); payload += ",";  // rightCloud2
  payload += inv(34);                  // rightCloud3
  payload += "],\"vib\":";
  payload += String(vibCount);
  payload += ",\"out\":[";
  // Порядок для UI: moon, leftCloud, wolfEye, rightCloud, three1, three2, three3
  payload += String(OUTPUTS.digitalRead(7)); payload += ",";  // moon
  payload += String(OUTPUTS.digitalRead(3)); payload += ",";  // leftCloud
  payload += String(OUTPUTS.digitalRead(2)); payload += ",";  // wolfEye
  payload += String(OUTPUTS.digitalRead(1)); payload += ",";  // rightCloud
  payload += String(OUTPUTS.digitalRead(4)); payload += ",";  // three1
  payload += String(OUTPUTS.digitalRead(5)); payload += ",";  // three2
  payload += String(OUTPUTS.digitalRead(6));                  // three3
  payload += "],\"wolf\":\"";
  char buf[7];
  snprintf(buf, sizeof(buf), "%02x%02x%02x", wolfLed[0].r, wolfLed[0].g, wolfLed[0].b);
  payload += buf;
  payload += "\",\"three\":\"";
  snprintf(buf, sizeof(buf), "%02x%02x%02x", threeLed[0].r, threeLed[0].g, threeLed[0].b);
  payload += buf;
  payload += "\",\"vol\":";
  payload += String(value);
  payload += ",\"rssi\":";
  payload += String(WiFi.RSSI());
  payload += ",\"heap\":";
  payload += String(ESP.getFreeHeap());
  payload += ",\"uptime\":";
  payload += String(millis() / 1000UL);
  payload += ",\"temp\":";
  payload += String(temperatureRead(), 1);
  payload += "}}";

  HTTPClient http;
  http.begin("http://192.168.4.1:3000/api?device=wolf");
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(150);
  http.POST(payload);
  http.end();
}

void setup() {
  // Отключаем встроенный Brown-Out Detector — фикс cold-boot WiFi fails
  // на слабом/шумном БП. См. [[clc-safe-esp32-power-issue]] в памяти.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  delay(2000);
  Serial.begin(115200);
  // Пауза перед включением периферии. При холодном пуске конденсаторы лент,
  // звукового модуля и самой платы ещё пустые: если поднимать всё это сразу,
  // бросок тока роняет питание. А встроенный детектор просадки у нас выключен
  // строкой выше — значит вместо честной перезагрузки чип просто зависает, и
  // устройство «не стартует, пока не нажмёшь EN». Проверено на поезде CLC4
  // 13.08.2026: без паузы не поднимался ни разу, с паузой — три холодных
  // пуска подряд из трёх.
  delay(STARTUP_SETTLE_MS);

  delay(100);

  // --- ИСПРАВЛЕНИЕ I2C (снижение скорости) ---
  // Явно инициализируем шину I2C перед использованием
  Wire.begin();
  // Устанавливаем скорость 10 кГц для стабильной работы на длинных линиях
  Wire.setClock(10000); 
  // ------------------------------------------

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
  pinMode(25, INPUT_PULLDOWN);
  // Подключаем прерывание: срабатывает, когда сигнал меняется с LOW на HIGH (RISING)
  attachInterrupt(digitalPinToInterrupt(25), vibrationISR, RISING);

  Serial.println("1");
  if (!myMP3.begin(mySerial, true, true)) {
    Serial.println("DFPlayer Mini not detected!");
  }
  myMP3.volume(value);
  myMP3.stop();

  Serial.println("2");
   if (!WiFi.config(local_IP, gateway, subnet)) {
     Serial.println("STA Failed to configure");
   }
  Serial.println("3");
  WiFi.persistent(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  Serial.println("4");
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    if (millis() - wifiStart > 15000UL) {
      Serial.println("WiFi connect timeout — restarting ESP32");
      delay(500);
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // --- НАСТРОЙКА OTA ---
  ArduinoOTA.setHostname("Wolf-ESP32"); // 
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  sendLogToServer("{\"log\":\"ESP32 Wolf is ready. IP: " + WiFi.localIP().toString() + "\"}");

  server.on("/", HTTP_POST, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });

  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      sendLogToServer("{\"log\":\"Wolf received command: " + body + "\"}");

      // === DIAG MODE ===
      if (body == "\"diag_on\"") {
        diagModeActive = true;
        myMP3.stop();
        for (uint8_t p = 0; p < 8; p++) OUTPUTS.digitalWrite(p, LOW);
        for (int i = 0; i < 10; i++) { wolfLed[i] = CRGB::Black; threeLed[i] = CRGB::Black; }
        FastLED.show();
        vibCount = 0;
        Serial.println("DIAG ON");
        server.send(200, "application/json", "{\"status\":\"diag_on\"}");
        return;
      }
      if (body == "\"diag_off\"") {
        diagModeActive = false;
        for (uint8_t p = 0; p < 8; p++) OUTPUTS.digitalWrite(p, LOW);
        for (int i = 0; i < 10; i++) { wolfLed[i] = CRGB::Black; threeLed[i] = CRGB::Black; }
        FastLED.show();
        myMP3.stop();
        state = 0;
        Serial.println("DIAG OFF");
        server.send(200, "application/json", "{\"status\":\"diag_off\"}");
        return;
      }
      if (body.startsWith("\"diag_set:")) {
        handleDiagSet(body);
        server.send(200, "application/json", "{\"status\":\"diag_set\"}");
        return;
      }
      if (diagModeActive) {
        Serial.println("DIAG ignored: " + body);
        server.send(200, "application/json", "{\"status\":\"ignored_diag\"}");
        return;
      }
      // === END DIAG ===

      if (body == "\"game\"") {
        // Этап уже пройден — опоздавший "game" из-за асинхронной доставки.
        if (stageFinished) {
          sendLogToServer("{\"log\":\"Wolf: 'game' IGNORED - stage already finished (race guard)\"}");
          server.send(200, "application/json", "{\"status\":\"ignored_finished\"}");
          return;
        }
        state = 1;
        doorRepeatActive = false;
        myMP3.playMp3Folder(TRACK_FON_WOLF);
        sendLogToServer("{\"log\":\"Wolf: Playing Fon Wolf sound\"}");
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
        hintPlayedAt = 0;        // разрешит start/ready, а не таймер
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
		dayOffPending = false;    // 2026-08-07: снимаем отложенное гашение от прошлой игры
        stageFinished = false;     // новая игра — снимаем защиту от гонки
      }
      if (body == "\"start\"") {
        state = 0;
        doorRepeatActive = false;
        hintFlag = 1;
        hintPlayedAt = 0;
        myMP3.stop();
		wolfEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
		dayOffPending = false;    // 2026-08-07: снимаем отложенное гашение от прошлой игры
        stageFinished = false;     // новая игра — снимаем защиту от гонки
      }
      if (body == "\"ready\"") {
        state = 0;
        doorRepeatActive = false;
        hintFlag = 1;            // готовность = подсказки снова разрешены
        hintPlayedAt = 0;
        myMP3.stop();
		wolfEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
		dayOffPending = false;    // 2026-08-07: снимаем отложенное гашение от прошлой игры
        stageFinished = false;     // новая игра — снимаем защиту от гонки
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
      // Режим сложности этапа (2026-07-31). Сервер шлёт при переключении
      // тумблера и при старте квеста, чтобы состояние пережило рестарт ESP32.
      if (body == "\"wolf_mode_easy\"") {
        wolfEasyMode = true;
        delayActive = false;               // снимаем возможный отсчёт отката
        sendLogToServer("{\"log\":\"Wolf: mode = EASY (no moon rollback)\"}");
        server.send(200, "application/json", "{\"status\":\"wolf_mode_easy\"}");
        return;
      }
      if (body == "\"wolf_mode_normal\"") {
        wolfEasyMode = false;
        sendLogToServer("{\"log\":\"Wolf: mode = NORMAL (moon rollback 5s)\"}");
        server.send(200, "application/json", "{\"status\":\"wolf_mode_normal\"}");
        return;
      }

      if (body == "\"skip\"") {
        stageFinished = true;   // этап закрыт — не давать "game" откатить состояние
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
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_STORY_9_C_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (PL)\"}");
        }
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
        // 2026-08-07. Тот же баг, что нашли в chest.ino на CLC2 07.08.
        // Сервер шлёт day_off в момент, когда пройдена ТРЕТЬЯ из трёх игр.
        // Если третьим оказался Волк, day_off прилетает в ту же секунду,
        // что и его победа, а доставка команд асинхронная — day_off может
        // обогнать подтверждение от сервера. Тогда state 5 → 0, ветка
        // "подтверждение получено → OpenLock(SH1)" не выполнялась никогда,
        // и дверь оставалась запертой.
        if (state == 5) {
          OpenLock(SH1);          // этап пройден — замок обязан открыться
        }
        if (state >= 5) {
          // Финал ещё идёт: гасим не сейчас, а когда замок отобьёт импульсы.
          dayOffPending = true;
          dayOffTimer = millis();
        } else {
          state = 0;
          doorRepeatActive = false;
        }
      }

      if (body == "\"ghost_game\"") {
        myMP3.stop();
        delay(50);
        myMP3.disableLoop();               // Сначала выключаем повтор
        myMP3.playMp3Folder(TRACK_ghost);  // Запускаем трек
        sendLogToServer("{\"log\":\"Wolf: Playing Ghost sound\"}");
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
  ArduinoOTA.handle();
  server.handleClient();
  // 2026-08-03: watchdog потери WiFi (перенесён из safe.ino, где работает в
  // проде). Раньше здесь был голый reconnect + delay(2000): если переподключение
  // не удавалось, устройство висело в этом цикле бесконечно и само не
  // восстанавливалось — требовалось снятие питания башни вручную (кейс Волка
  // 29.07 и Поезда 03.08). Плюс delay(2000) блокировал loop целиком.
  // Теперь: попытка reconnect не чаще раза в 10 сек, а если связи нет дольше
  // 60 сек — перезагружаем ESP32, она поднимется и подключится заново.
  {
    static unsigned long lastReconnectAttempt = 0;
    static unsigned long wifiDownSince = 0;
    if (WiFi.status() != WL_CONNECTED) {
      if (wifiDownSince == 0) wifiDownSince = millis();
      if (millis() - lastReconnectAttempt > 10000UL) {
        lastReconnectAttempt = millis();
        Serial.println("WiFi lost — reconnecting");
        WiFi.reconnect();
      }
      if (millis() - wifiDownSince > 60000UL) {
        Serial.println("WiFi down >60s — restarting ESP32");
        delay(500);
        ESP.restart();
      }
    } else {
      wifiDownSince = 0;   // соединение в порядке
    }
  }

  // === DIAG MODE: ранний выход, игнорируем игровую логику ===
  if (diagModeActive) {
    if (millis() - diagLastSnapshot >= DIAG_SNAPSHOT_INTERVAL) {
      diagLastSnapshot = millis();
      sendDiagSnapshot();
    }
    return;
  }
  // === END DIAG ===

  handlePlayerQueries();
  handleFadeOut();

  // Обработка салюта (добавь эту строку)
  if (fireworkActive) {
    handleFirework();
    return;  // Пропускаем остальную логику во время салюта
  }

  // Страховка: событие «трек доиграл» от DFPlayer могло потеряться.
  if (!hintFlag && hintPlayedAt != 0 && millis() - hintPlayedAt > HINT_REARM_MS) {
    hintFlag = 1;
    hintPlayedAt = 0;
  }
  helpButton.tick();
  if (helpButton.isPress() && ghostFlag == 0) {
    if (state == 0 && hintFlag) {
      myMP3.pause();  // Ставим фоновую музыку на паузу
      delay(50);
      if (language == 1) {
        myMP3.playMp3Folder(TRACK_HINT_0_RU);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (RU)\"}");
      }
      if (language == 2) {
        myMP3.playMp3Folder(TRACK_HINT_0_EN);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (EN)\"}");
      }
      if (language == 3) {
        myMP3.playMp3Folder(TRACK_HINT_0_AR);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (AR)\"}");
      }
      if (language == 4) {
        myMP3.playMp3Folder(TRACK_HINT_0_FR);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (FR)\"}");
      }
      if (language == 5) {
        myMP3.playMp3Folder(TRACK_HINT_0_UK);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (UK)\"}");
      }
      if (language == 6) {
        myMP3.playMp3Folder(TRACK_HINT_0_PL);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 0 (PL)\"}");
      }
      hintFlag = 0;
      hintPlayedAt = millis();
    }
    // 2026-08-14: было state < 3, то есть на этапах 3 (левое облако) и 4 (сам
    // волк) кнопка подсказки не делала НИЧЕГО. Игрок доходит до волка — и
    // остаётся без помощи. Пять подсказок рассчитаны на всю игру, поэтому
    // отдаём их до победы (state 5).
    if (state > 0 && state < 5 && hintFlag) {
      Serial.println("1");
      myMP3.pause();  // Ставим фоновую музыку на паузу
      delay(50);
      if (hint_counter == 0) {
        Serial.println("2");
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_1_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_1_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_1_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_1_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_1_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_1_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 1 (PL)\"}");
        }
      } 
      if(hint_counter == 1)
       {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_2_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_2_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_2_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_2_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_2_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_2_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 2 (PL)\"}");
        }
      }
      if(hint_counter == 2)
       {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_3_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_3_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_3_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_3_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_3_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_3_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 3 (PL)\"}");
        }
      }
      if(hint_counter == 3)
       {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_4_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_4_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_4_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_4_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_4_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_4_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 4 (PL)\"}");
        }
      }
      if(hint_counter == 4)
       {
        if (language == 1) {
          myMP3.playMp3Folder(TRACK_HINT_5_RU);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (RU)\"}");
        }
        if (language == 2) {
          myMP3.playMp3Folder(TRACK_HINT_5_EN);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (EN)\"}");
        }
        if (language == 3) {
          myMP3.playMp3Folder(TRACK_HINT_5_AR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (AR)\"}");
        }
        if (language == 4) {
          myMP3.playMp3Folder(TRACK_HINT_5_FR);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (FR)\"}");
        }
        if (language == 5) {
          myMP3.playMp3Folder(TRACK_HINT_5_UK);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (UK)\"}");
        }
        if (language == 6) {
          myMP3.playMp3Folder(TRACK_HINT_5_PL);
          sendLogToServer("{\"log\":\"Wolf: Playing Hint 5 (PL)\"}");
        }
      }
      hint_counter++;
      if(hint_counter>4){
        hint_counter=0;
      }
      hintFlag=0;
      hintPlayedAt = millis();
    }
    if (state > 4 && hintFlag) {
      if (language == 1) {
        myMP3.playMp3Folder(TRACK_HINT_6_RU);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (RU)\"}");
      }
      if (language == 2) {
        myMP3.playMp3Folder(TRACK_HINT_6_EN);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (EN)\"}");
      }
      if (language == 3) {
        myMP3.playMp3Folder(TRACK_HINT_6_AR);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (AR)\"}");
      }
      if (language == 4) {
        myMP3.playMp3Folder(TRACK_HINT_6_FR);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (FR)\"}");
      }
      if (language == 5) {
        myMP3.playMp3Folder(TRACK_HINT_6_UK);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (UK)\"}");
      }
      if (language == 6) {
        myMP3.playMp3Folder(TRACK_HINT_6_PL);
        sendLogToServer("{\"log\":\"Wolf: Playing Hint 6 (PL)\"}");
      }
      hintFlag = 0;
      hintPlayedAt = millis();
    }
  }


  if (ghostFlag == 1 && vibrationDetected) {
    ghostFlag = 0;           // Выключаем игру, чтобы не сработало повторно
    vibrationDetected = false; // Сбрасываем флаг вибрации
    
    myMP3.stop();
    Serial.println("ghost_game_WIN");
    GhostSendData();
    
    // Небольшая задержка, чтобы успокоить датчик
    delay(100); 
  }
  
  // Если игра НЕ активна, но вибрация идет - просто сбрасываем флаг, чтобы он не копился
  if (ghostFlag == 0 && vibrationDetected) {
     vibrationDetected = false;
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
      ghostEffect(); // Продолжаем анимацию
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
        ghostEffect();
        FastLED.show();
      }
      break;
    case 7:
      break;
  }

  // Отложенное гашение по day_off (см. обработчик day_off).
  // Ждём, пока финал отработает и замок отобьёт свои импульсы,
  // и только потом уводим Волка в "выключенное" состояние.
  if (dayOffPending && millis() - dayOffTimer >= DAY_OFF_DELAY_MS) {
    dayOffPending = false;
    state = 0;
    doorRepeatActive = false;
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
      if (language == 1) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_RU);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (RU)\"}");
      }
      if (language == 2) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_EN);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (EN)\"}");
      }
      if (language == 3) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_AR);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (AR)\"}");
      }
      if (language == 4) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_FR);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (FR)\"}");
      }
      if (language == 5) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_UK);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (UK)\"}");
      }
      if (language == 6) {
        myMP3.playMp3Folder(TRACK_STORY_9_A_PL);
        sendLogToServer("{\"log\":\"Wolf: Playing Story 9 A (PL)\"}");
      }
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
    // Луна отпущена. В НОРМАЛЬНОМ режиме откат не запускается вообще —
    // гасим только индикатор, прогресс этапа сохраняется.
    if (!delayActive) {
      OUTPUTS.digitalWrite(moonLed, LOW);
      delayStartTime = millis();
      delayActive = true;
      Serial.println(wolfEasyMode ? "Moon released - easy mode, no rollback"
                                  : "Moon released - normal mode timer started");
    }

    if (!wolfEasyMode && delayActive &&
        millis() - delayStartTime >= MOON_RELEASE_TIMEOUT_MS) {
      OUTPUTS.digitalWrite(moonLed, HIGH);
      OUTPUTS.digitalWrite(rightCloudLed, HIGH);
      state = 1;
      delayActive = false;
      Serial.println("Normal mode: moon timeout - state = 1");
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
    // Луна отпущена. В НОРМАЛЬНОМ режиме откат не запускается вообще —
    // гасим только индикатор, прогресс этапа сохраняется.
    if (!delayActive) {
      OUTPUTS.digitalWrite(moonLed, LOW);
      delayStartTime = millis();
      delayActive = true;
      Serial.println(wolfEasyMode ? "Moon released - easy mode, no rollback"
                                  : "Moon released - normal mode timer started");
    }

    if (!wolfEasyMode && delayActive &&
        millis() - delayStartTime >= MOON_RELEASE_TIMEOUT_MS) {
      OUTPUTS.digitalWrite(moonLed, HIGH);
      OUTPUTS.digitalWrite(rightCloudLed, HIGH);
      state = 1;
      delayActive = false;
      Serial.println("Normal mode: moon timeout - state = 1");
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
      if (!TRACK_Flag) {
        myMP3.playMp3Folder(TRACK_CLOUD);
        sendLogToServer("{\"log\":\"Wolf: Playing Cloud sound\"}");
      }
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
    if (language == 1) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_RU);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (RU)\"}");
    }
    if (language == 2) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_EN);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (EN)\"}");
    }
    if (language == 3) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_AR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (AR)\"}");
    }
    if (language == 4) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_FR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (FR)\"}");
    }
    if (language == 5) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_UK);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (UK)\"}");
    }
    if (language == 6) {
      myMP3.playMp3Folder(TRACK_STORY_9_B_PL);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 B (PL)\"}");
    }
    
    TRACK_Flag = 0;
    cloudFiPlaying = true;
  }

  OUTPUTS.digitalWrite(wolfEyeLed, HIGH);

  // --- Проверка сброса (разрыв цепи) ПЕРЕД ВСЕМ ОСТАЛЬНЫМ ---
  lightCircut1 = !rightCloud3Gerk.isHold();
  lightCircut2 = !leftCloudGerk.isHold();
  
  if ((lightCircut1 || lightCircut2)) {
    // 2026-07-31: не откатываем мгновенно. Микро-разрыв (дрожание руки в
    // момент касания геркона волка) не должен обнулять собранную цепь —
    // ждём CLOUD_GRACE_MS непрерывного разрыва.
    if (cloudBrokenSince == 0) {
      cloudBrokenSince = millis();
      Serial.println("Cloud circuit broken - grace period started");
    }
    if (millis() - cloudBrokenSince >= CLOUD_GRACE_MS) {
      Serial.println("Circuit broken! Resetting to State 1");

      state = 1; // Сбрасываем этап
      cloudBrokenSince = 0;

      // Важно: Сбрасываем флаги, чтобы аудио не блокировало логику
      cloudFiPlaying = false;
      TRACK_Flag = 1; // Чтобы при возврате история могла начаться заново (по желанию)
      return; // Выходим из функции немедленно
    }
    // Разрыв ещё в пределах grace — продолжаем, победу засчитываем как обычно
  } else {
    cloudBrokenSince = 0;   // цепь восстановлена
  }
  // -----------------------------------------------------------------------

  // Проверяем геркон ВОЛКА немедленно ---
  // Это позволяет выиграть "в первой фазе", прервав story_B.
  if (wolfGerk.isHold()) {
    ghostEffect(); // Включить анимацию
    FastLED.show();
    myMP3.stop();   // Прервать story_B (или любой другой трек)
    delay(50);
    
    // Воспроизвести историю победы story_C
    if (language == 1) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (RU)\"}");
    }
    if (language == 2) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (EN)\"}");
    }
    if (language == 3) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (AR)\"}");
    }
    if (language == 4) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_FR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (FR)\"}");
    }
    if (language == 5) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_UK);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (UK)\"}");
    }
    if (language == 6) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_PL);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (PL)\"}");
    }
    
    state = 5; // Переходим в состояние отправки данных о победе 
    stageFinished = true;   // этап закрыт — не давать "game" откатить состояние
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
    ghostEffect();
    FastLED.show();
    myMP3.stop();
    delay(50);
    if (language == 1) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_RU);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (RU)\"}");
    }
    if (language == 2) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_EN);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (EN)\"}");
    }
    if (language == 3) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_AR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (AR)\"}");
    }
    if (language == 4) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_FR);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (FR)\"}");
    }
    if (language == 5) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_UK);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (UK)\"}");
    }
    if (language == 6) {
      myMP3.playMp3Folder(TRACK_STORY_9_C_PL);
      sendLogToServer("{\"log\":\"Wolf: Playing Story 9 C (PL)\"}");
    }
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

// Голубой призрак — анимация победы над волком. Светящееся пятно медленно
// плавает по ленте туда-сюда, оставляя за собой мягкий шлейф, и всё это
// «дышит» яркостью. Волк и дерево ходят вразнобой, поэтому духа видно как
// одно существо, обходящее поляну, а не как две одинаковые мигалки.
//
// 2026-08-14: заменила прежний auroraEffect(). Тот на каждом кадре подмешивал
// в каждый пиксель random8() — вблизи это читалось как рябь, а не как свечение.
// Здесь случайности нет вовсе: всё движение — синусы, поэтому картинка плавная
// при любой частоте вызова.
static void ghostStrip(CRGB *leds, uint8_t n, uint16_t pos, uint8_t hue, uint8_t breath) {
  // Шлейф: прошлый кадр не стираем, а притушаем.
  fadeToBlackBy(leds, n, 36);

  // Холодная дымка, чтобы лента не проваливалась в черноту между проходами.
  CRGB haze = CHSV(hue, 210, breath / 5 + 6);
  for (uint8_t i = 0; i < n; i++) leds[i] |= haze;

  // Пятно живёт в дробной позиции: две соседние точки делят яркость, поэтому
  // оно едет плавно, а не перескакивает по светодиодам.
  uint8_t idx = pos >> 8;
  uint8_t frac = pos & 0xFF;
  CRGB core = CHSV(hue, 190, breath);          // ядро голубое, а не выбеленное
  CRGB halo = CHSV(hue + 6, 220, breath / 3);

  CRGB a = core; a.nscale8_video(255 - frac);
  CRGB b = core; b.nscale8_video(frac);
  if (idx < n) leds[idx] += a;
  if (idx + 1 < n) leds[idx + 1] += b;

  // Ореол по бокам — размывает края, без него пятно выглядит точкой.
  if (idx > 0) leds[idx - 1] += halo;
  if (idx + 2 < n) leds[idx + 2] += halo;
}

void ghostEffect() {
  const uint8_t N = 10;
  // Кадр считаем не чаще 20 мс: функцию зовут и из быстрого цикла, и по
  // таймеру, а скорость шлейфа не должна зависеть от того, кто её позвал.
  static uint32_t lastFrame = 0;
  if (millis() - lastFrame < 20) return;
  lastFrame = millis();

  uint8_t hue = 138 + (sin8(millis() / 40) >> 5);   // лёд ↔ бирюза, очень медленно
  uint8_t breath = beatsin8(9, 80, 210);            // общее «дыхание»
  uint16_t posWolf = beatsin16(7, 0, (N - 1) * 255);
  uint16_t posTree = beatsin16(5, 0, (N - 1) * 255, 0, 21845);   // дерево вразнобой

  ghostStrip(wolfLed, N, posWolf, hue, breath);
  ghostStrip(threeLed, N, posTree, hue + 4, breath);
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
      hintPlayedAt = 0;

      // ДОБАВЛЕННЫЙ БЛОК ДЛЯ СНЯТИЯ "ЗАМКА"
      if (finishedTrack == TRACK_STORY_9_B_RU ||
          finishedTrack == TRACK_STORY_9_B_EN ||
          finishedTrack == TRACK_STORY_9_B_AR ||
        finishedTrack == TRACK_STORY_9_B_FR ||
        finishedTrack == TRACK_STORY_9_B_UK ||
        finishedTrack == TRACK_STORY_9_B_PL) { 
            cloudFiPlaying = false;
            Serial.println("Story B finished, unlocking game state.");
      }


      // Проверяем, что мы в одном из активных игровых состояний (1, 2, 3 или 4)
      if ((state >= 1 && state <= 4) && finishedTrack != TRACK_FON_WOLF) {
        if (!flagTrack) {
          myMP3.playMp3Folder(TRACK_FON_WOLF);
          sendLogToServer("{\"log\":\"Wolf: Playing Fon Wolf sound\"}");
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
