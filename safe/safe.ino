#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "HardwareSerial.h"
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>
// --- ДОБАВЛЕНО ДЛЯ OTA ---
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
// -------------------------

// --- ПИНИ ---
const int LOCKER_PIN = 18;
const int LED_PIN_1 = 21;
const int LED_PIN_2 = 22;
const int REED_SWITCH_1_PIN = 36;
const int REED_SWITCH_2_PIN = 32;
const int BALL_SENSOR_PIN = 39;
const int DFPLAYER_TX_PIN = 16;
const int DFPLAYER_RX_PIN = 17;

// --- НАСТРОЙКИ DFPLAYER ---
// Serial2 (не Serial1) — Serial1 на ESP32 имеет нестабильный RX из-за
// конфликта default-пинов с QSPI flash (pins 9/10). Serial2 default RX=16/TX=17
// что точно соответствует нашей разводке к DFPlayer.
HardwareSerial dfplayerSerial(2);
DFRobotDFPlayerMini myDFPlayer;

// --- АУДИОФАЙЛЫ ---
const int TRACK_STORY_28_RU = 22;
const int TRACK_STORY_28_EN = 25;
const int TRACK_STORY_28_AR = 28;
const int TRACK_STORY_28_FR = 31;
const int TRACK_STORY_28_UK = 38;
const int TRACK_STORY_28_PL = 44;

const int TRACK_HINT_0_RU = 10;
const int TRACK_HINT_0_EN = 14;
const int TRACK_HINT_0_AR = 18;
const int TRACK_HINT_0_FR = 34;
const int TRACK_HINT_0_UK = 41;
const int TRACK_HINT_0_PL = 47;

const int TRACK_HINT_1_RU = 11;
const int TRACK_HINT_1_EN = 15;
const int TRACK_HINT_1_AR = 19;
const int TRACK_HINT_1_FR = 35;
const int TRACK_HINT_1_UK = 42;
const int TRACK_HINT_1_PL = 48;

const int TRACK_HINT_2_RU = 12;
const int TRACK_HINT_2_EN = 16;
const int TRACK_HINT_2_AR = 20;
const int TRACK_HINT_2_FR = 36;
const int TRACK_HINT_2_UK = 43;
const int TRACK_HINT_2_PL = 49;


const int TRACK_STORY_29A_RU = 23;
const int TRACK_STORY_29A_EN = 26;
const int TRACK_STORY_29A_AR = 29;
const int TRACK_STORY_29A_FR = 32;
const int TRACK_STORY_29A_UK = 39;
const int TRACK_STORY_29A_PL = 45;

const int TRACK_STORY_29B_RU = 24;
const int TRACK_STORY_29B_EN = 27;
const int TRACK_STORY_29B_AR = 30;
const int TRACK_STORY_29B_FR = 33;
const int TRACK_STORY_29B_UK = 40;
const int TRACK_STORY_29B_PL = 46;

const int TRACK_HINT_3_RU = 13;
const int TRACK_HINT_3_EN = 17;
const int TRACK_HINT_3_AR = 21;
const int TRACK_HINT_3_FR = 37;
const int TRACK_HINT_3_UK = 50;
const int TRACK_HINT_3_PL = 51;

const int TRACK_FON_SAFE = 2;
const int TRACK_SAFE_END = 6;


// --- СОСТОЯНИЯ ИГРЫ ---
enum GameState { IDLE, AWAIT_GAME, GAME_ACTIVE, GAME_WON };
GameState currentState = IDLE;

// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ---
bool story28_played = false;
int hint_counter = 0;
int gameWonSequenceStep = 0;
unsigned long lastDebounceTime_1 = 0;
unsigned long lastDebounceTime_2 = 0;
unsigned long debounceDelay = 200;
const char* ssid = "Castle";
const char* password = "questquest";
//const char* ssid = "ProducED";
//const char* password = "32744965";
int language = 1;
unsigned long doorTimer;
bool hintFlag=1;
// 2026-05-28: time-based re-arm для hintFlag. Раньше hintFlag возвращался в 1
// ТОЛЬКО по событию DFPlayer "трек закончился" (type==11). DFRobotDFPlayerMini
// ненадёжно парсит events на ESP32 (см. clc-safe-dfplayer-fix) → если событие
// потеряно, hintFlag навсегда 0 → подсказки больше не играют. Фолбэк: через
// HINT_REARM_MS после проигрыша подсказки принудительно re-arm.
unsigned long hintPlayedAt = 0;
const unsigned long HINT_REARM_MS = 20000;  // 20 сек — re-arm подсказки независимо от DFPlayer event
int value = 30;

bool safeEndConfirmed = false;      // Флаг подтверждения от сервера
unsigned long stepEnteredAt = 0;    // Момент входа в текущий gameWonSequenceStep (для time-based fallback если DFPlayer не шлёт events)
unsigned long safeEndSendTimer = 0; // Таймер для периодической отправки
int safeEndSendAttempts = 0;        // Счётчик попыток (макс 3, не циклить бесконечно)

// === DIAG MODE (см. [[clc-tech-pult-diag-panel]]) ===
bool diagModeActive = false;
unsigned long diagLastSnapshot = 0;
const unsigned long DIAG_SNAPSHOT_INTERVAL = 200;

// Настройки статического IP
IPAddress local_IP(192, 168, 4, 204);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

const char* externalApi = "http://192.168.4.1:3000/api";

WebServer server(80);
void sendLogToServer(String payload) {
  // 2026-05-25 fix v3: HTTP logging ПОЛНОСТЬЮ отключён.
  //
  // Контекст: Safe ESP32 уходила offline после команды 'skip' / 'safe:end'.
  // Фиксы v1 (убрать SendData spam) и v2 (sendLog только на cmd=0x3D + timeout
  // 1.5с) не помогли — Safe умирала уже на ПЕРВОМ DFPlayer event.
  // 58 sendLogToServer call sites в коде — слишком многие в hot paths
  // (DFPlayer events, gameWonSequence, handle /data). Каждый блокирует loop
  // на HTTP POST → WDT crash. Полное отключение HTTP-логирования —
  // единственный надёжный способ исключить эту причину.
  //
  // Debug-логи теперь идут только через Serial (USB кабель к ноутбуку).
  // Если на проде логи нужны — подключи USB-TTL к Safe ESP32.
  Serial.print("[SAFE-LOG] ");
  Serial.println(payload);
}

// --- Прототипы функций ---
void handleSerialCommands();
void handlePlayerQueries();
void ledOn();
void ledOff();
void openLocker();
// Новая вспомогательная функция, чтобы не дублировать код
void startGameWonSequence();
// DIAG
void handleDiagSet(String body);
void sendDiagSnapshot();

void setup() {
  // Отключаем встроенный Brown-Out Detector — он может валить ESP32
  // в reset-loop при power-on с просаженным БП до того как успеет
  // запуститься основной код. Без этого cold-boot часто фейлится.
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  Serial.begin(115200);
  Serial.println("Инициализация сейфа...");

  // GPIO setup
  pinMode(LOCKER_PIN, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(REED_SWITCH_1_PIN, INPUT);
  pinMode(REED_SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(BALL_SENSOR_PIN, INPUT);
  digitalWrite(LOCKER_PIN, LOW);
  ledOff();

  // DFPlayer init ДО WiFi — иначе DFPlayer не шлёт track-finished events
  // в handlePlayerQueries (видимо WiFi stack захватывает что-то нужное
  // DFPlayer Serial-у при init). BOD disable в первой строке защищает
  // от brown-out при peak ток audio peripheral на cold boot.
  dfplayerSerial.begin(9600, SERIAL_8N1, DFPLAYER_TX_PIN, DFPLAYER_RX_PIN);
  if (!myDFPlayer.begin(dfplayerSerial, true, true)) {
    Serial.println(F("!!! ВНИМАНИЕ: Не удалось найти DFPlayer Mini. Проверьте подключение. !!!"));
  } else {
    Serial.println(F("DFPlayer Mini инициализирован."));
    myDFPlayer.volume(value);
    myDFPlayer.stop();
  }

  currentState = IDLE;
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.persistent(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    // Cold-boot rescue: если за 15 сек не подключились — сами себя ребутаем.
    // На cold boot от слабого/шумного БП ESP32 может зависнуть на WiFi init
    // (BOD или race) — warm reset через ESP.restart() решает.
    if (millis() - wifiStart > 15000UL) {
      Serial.println("WiFi connect timeout — restarting ESP32");
      delay(500);
      ESP.restart();
    }
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());
  sendLogToServer("{\"log\":\"ESP32 Safe is ready. IP: " + WiFi.localIP().toString() + "\"}");
  Serial.println("Состояние: IDLE");

  // --- НАСТРОЙКА OTA  ---
  ArduinoOTA.setHostname("Safe-ESP32"); // Имя устройства в сети
  
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "sketch" : "filesystem";
    // Отключаем все моторы/светодиоды для безопасности
    Serial.println("Start updating " + type);
    // Можно добавить сюда выключение strip.clear()
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    // Можно мигать светодиодом, но необязательно
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
  Serial.println("OTA Ready");
  // ---------------------------------------------

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });
  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      sendLogToServer("{\"log\":\"Safe received command: " + body + "\"}");

      // === DIAG MODE ===
      if (body == "\"diag_on\"") {
        diagModeActive = true;
        myDFPlayer.stop();
        digitalWrite(LOCKER_PIN, LOW);
        ledOff();
        Serial.println("DIAG ON");
        server.send(200, "application/json", "{\"status\":\"diag_on\"}");
        return;
      }
      if (body == "\"diag_off\"") {
        diagModeActive = false;
        digitalWrite(LOCKER_PIN, LOW);
        ledOff();
        myDFPlayer.stop();
        currentState = IDLE;
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

      if(body == "\"start\""){
        ledOff();
        Serial.println("Команда 'start': подсветка выключена.");
        currentState = AWAIT_GAME;
		safeEndConfirmed = false; safeEndSendAttempts = 0; // Сбрасываем флаг + счётчик попыток
      }
      if(body == "\"restart\""){
        ledOn();
        openLocker();
        myDFPlayer.stop();
       
        currentState = IDLE;
		safeEndConfirmed = false; safeEndSendAttempts = 0; // Сбрасываем флаг + счётчик попыток
      }
      if(body == "\"ready\""){
        ledOff();
        myDFPlayer.stop();
        currentState = IDLE;
		safeEndConfirmed = false; safeEndSendAttempts = 0; // Сбрасываем флаг + счётчик попыток
      }
      if(body == "\"game\""){
        ledOn();
        story28_played = false;
        hint_counter = 0;
        currentState = GAME_ACTIVE;
     
        myDFPlayer.playMp3Folder(TRACK_FON_SAFE);
        sendLogToServer("{\"log\":\"Safe: Playing Fon Safe sound\"}");
        Serial.println("Состояние: GAME_ACTIVE");
      }
      if(body == "\"skip\""){
        Serial.println("Принудительный переход в GAME_WON.");
        startGameWonSequence();
      }

	  // Добавляем обработку подтверждения от сервера
      if(body == "\"confirm_safe_end\"") {
        safeEndConfirmed = true;
      }
      if(body == "\"open_door\""){
        openLocker();
        // ИЗМЕНЕНИЕ 1: Добавлено открытие замка
        currentState = IDLE;
      }

      if(body == "\"day_on\""){
        ledOn();
        if (currentState == IDLE) {
          currentState = AWAIT_GAME;
          Serial.println("Состояние: AWAIT_GAME");
        }
      }
      if (body == "\"volume_up\"") {
        value = value + 1;
        if (value >= 30) value = 30;
        myDFPlayer.volume(value);
        Serial.println(value);
      }

      if (body == "\"volume_down\"") {
        value = value - 1;
        if (value <= 0) value = 0;
        myDFPlayer.volume(value);
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
            myDFPlayer.volume(value);
            Serial.print("Установлена громкость: ");
            Serial.println(value);
        } else {
            Serial.println("Некорректный уровень громкости");
        }
    }
}
      if(body == "\"day_off\""){
        ledOff();
      }
      if(body == "\"language_1\""){
        language = 1;
      }
      if(body == "\"language_2\""){
        language = 2;
      }
      if(body == "\"language_3\""){
        language = 3;
      }
      if(body == "\"language_4\""){
        language = 4;
      }
      if(body == "\"language_5\""){
        language = 5;
      }
      if(body == "\"language_6\""){
        language = 6;
      }
      
      Serial.println("Received POST: " + body);
      server.send(200, "application/json", "{\"status\":\"received\"}");
    } else {
      server.send(400, "text/plain", "Bad Request");
    }
  });
  
  server.begin();
  Serial.println("HTTP server started");
  // --- ВИЗУАЛЬНЫЙ ТЕСТ ПРОШИВКИ ---
  for(int i=0; i<5; i++) {
    digitalWrite(LED_PIN_1, HIGH); 
    digitalWrite(LED_PIN_2, HIGH);
    delay(200);
    digitalWrite(LED_PIN_1, LOW); 
    digitalWrite(LED_PIN_2, LOW);
    delay(200);
  }
  // -------------------------------
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  // === DIAG MODE: ранний выход, игнорируем игровую логику ===
  if (diagModeActive) {
    if (millis() - diagLastSnapshot >= DIAG_SNAPSHOT_INTERVAL) {
      diagLastSnapshot = millis();
      sendDiagSnapshot();
    }
    return;
  }
  // === END DIAG ===

  // WiFi watchdog: non-blocking, restart только после 60 сек непрерывной потери
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
    wifiDownSince = 0;  // соединение в порядке
  }
  handleSerialCommands();
  handlePlayerQueries();

  // 2026-05-28: time-based re-arm hintFlag. Если DFPlayer не прислал событие
  // "трек закончился" (type==11, ненадёжно на ESP32) — принудительно
  // восстанавливаем hintFlag через HINT_REARM_MS, чтобы подсказки не умирали
  // после первого нажатия геркона.
  if (!hintFlag && hintPlayedAt != 0 && (millis() - hintPlayedAt > HINT_REARM_MS)) {
    hintFlag = 1;
    hintPlayedAt = 0;
    sendLogToServer("{\"log\":\"Safe: hintFlag re-armed by timeout\"}");
  }

  switch (currentState) {
    case AWAIT_GAME:
      if (digitalRead(REED_SWITCH_2_PIN) == LOW && (millis() - lastDebounceTime_2) > debounceDelay) {
        if (hintFlag)
        {
          if(language == 1){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_RU);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (RU)\"}");
          }
          if(language == 2){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_EN);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (EN)\"}");
          }
          if(language == 3){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_AR);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (AR)\"}");
          }
          if(language == 4){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_FR);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (FR)\"}");
          }
          if(language == 5){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_UK);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (UK)\"}");
          }
          if(language == 6){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_PL);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (PL)\"}");
          }
          hintFlag = 0;
          hintPlayedAt = millis();  // 2026-05-28: для time-based re-arm
        }
        lastDebounceTime_2 = millis();
      }
      break;
    case GAME_ACTIVE:
      // ИЗМЕНЕНИЕ 2: Теперь геркон срабатывает на HIGH (размыкание)
      if (digitalRead(REED_SWITCH_1_PIN) == HIGH && !story28_played && (millis() - lastDebounceTime_1) > debounceDelay) {
        story28_played = true;
        myDFPlayer.pause(); // Ставим фоновую музыку на паузу
        delay(50);
        hintFlag = 0;
        hintPlayedAt = millis();  // 2026-05-28: для time-based re-arm
        if(language == 1){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_RU);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (RU)\"}");
        }
        if(language == 2){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_EN);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (EN)\"}");
        }
        if(language == 3){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_AR);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (AR)\"}");
        }
        if(language == 4){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_FR);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (FR)\"}");
        }
        if(language == 5){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_UK);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (UK)\"}");
        }
        if(language == 6){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_PL);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (PL)\"}");
        }
        
        lastDebounceTime_1 = millis();
      }

      if (digitalRead(REED_SWITCH_2_PIN) == LOW && (millis() - lastDebounceTime_2) > debounceDelay && hintFlag) {
        if (hintFlag) {
          myDFPlayer.pause();
          delay(50);
          if(hint_counter == 0) {
            if(language == 1){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_RU);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (RU)\"}");
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_EN);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (EN)\"}");
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_AR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (AR)\"}");
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_FR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (FR)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_UK);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (UK)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_PL);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (PL)\"}");
            }
          }
          if(hint_counter == 1){
            if(language == 1){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_RU);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (RU)\"}");
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_EN);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (EN)\"}");
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_AR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (AR)\"}");
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_FR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (FR)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_UK);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (UK)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_PL);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (PL)\"}");
            }
          }
          hintFlag = 0;
          hintPlayedAt = millis();  // 2026-05-28: для time-based re-arm
          hint_counter++;
          if(hint_counter > 1){
            hint_counter = 0;
          }
        }
        lastDebounceTime_2 = millis();
      }

      // --- ИЗМЕНЕНИЕ ---
      if (digitalRead(BALL_SENSOR_PIN) == HIGH) {
        Serial.println("Датчик пересечения сработал! Переход в GAME_WON.");
        startGameWonSequence();
      }
      break;
    case GAME_WON:
      // 2026-05-25: было — POST раз в секунду пока не пришёл confirm_safe_end.
      // Это вешало ESP32: каждый POST синхронно блокировал loop на 5+ сек (timeout),
      // heap фрагментировался, ESP32 уходила в reboot и не возвращалась.
      // Теперь — макс 3 попытки с интервалом 5 сек. После SendData() в response
      // body сервер возвращает {"confirm":true} → safeEndConfirmed=true локально
      // (см. SendData() ниже).
      if (!safeEndConfirmed && safeEndSendAttempts < 3) {
        if (safeEndSendAttempts == 0 || millis() - safeEndSendTimer > 5000) {
          SendData();
          safeEndSendTimer = millis();
          safeEndSendAttempts++;
        }
      }

      // Time-based fallback на случай если DFPlayer не шлёт track-finished
      // events (library DFRobotDFPlayerMini не парсит CMD=0x3D на Safe).
      // Длины треков (максимум на всех языках, 25 мая 2026):
      //   29A      ≈ 10 сек → fallback 12 сек (буфер 2 сек)
      //   SAFE_END ≈  5 сек → fallback  7 сек
      //   29B      ≈  5 сек → fallback  7 сек
      // Если треки удлинятся (новые языки / переозвучка) — увеличить здесь.
      if (gameWonSequenceStep == 1 && (millis() - stepEnteredAt > 12000UL)) {
        Serial.println("FALLBACK: 29A timeout — переход к SAFE_END");
        sendLogToServer("{\"log\":\"Safe FALLBACK step1→2 by timer (DFPlayer event missing)\"}");
        gameWonSequenceStep = 2;
        stepEnteredAt = millis();
        myDFPlayer.playMp3Folder(TRACK_SAFE_END);
      }
      else if (gameWonSequenceStep == 2 && (millis() - stepEnteredAt > 7000UL)) {
        Serial.println("FALLBACK: SAFE_END timeout — открываем дверь + 29B");
        sendLogToServer("{\"log\":\"Safe FALLBACK step2→3 by timer (open door + 29B)\"}");
        gameWonSequenceStep = 3;
        stepEnteredAt = millis();
        openLocker();
        if(language == 1) myDFPlayer.playMp3Folder(TRACK_STORY_29B_RU);
        else if(language == 2) myDFPlayer.playMp3Folder(TRACK_STORY_29B_EN);
        else if(language == 3) myDFPlayer.playMp3Folder(TRACK_STORY_29B_AR);
        else if(language == 4) myDFPlayer.playMp3Folder(TRACK_STORY_29B_FR);
        else if(language == 5) myDFPlayer.playMp3Folder(TRACK_STORY_29B_UK);
        else if(language == 6) myDFPlayer.playMp3Folder(TRACK_STORY_29B_PL);
        hintFlag = 0;
      }
      else if (gameWonSequenceStep == 3 && (millis() - stepEnteredAt > 7000UL)) {
        Serial.println("FALLBACK: 29B timeout — сцена завершена");
        sendLogToServer("{\"log\":\"Safe FALLBACK step3→4 by timer (sequence done)\"}");
        gameWonSequenceStep = 4;
        hintFlag = 1;  // 2026-05-30: разрешаем HINT_3 после окончания 29B
        hintPlayedAt = 0;
      }
      if (digitalRead(REED_SWITCH_2_PIN) == LOW && gameWonSequenceStep >= 4 && (millis() - lastDebounceTime_2) > debounceDelay) {
        if (hintFlag){
          if(language == 1){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_RU);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (RU)\"}");
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_EN);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (EN)\"}");
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_AR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (AR)\"}");
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_FR);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (FR)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_UK);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (UK)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_PL);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (PL)\"}");
            }
            hintFlag=0;
            hintPlayedAt = millis();  // 2026-05-30: для time-based re-arm (повтор HINT_3)
        }
        lastDebounceTime_2 = millis();
      }
      if(millis()-doorTimer>=3000 && gameWonSequenceStep > 2){
        openLocker();
        doorTimer = millis();
      }
      break;
    case IDLE:
      // Ничего не делаем
      break;
  }
}

void handleSerialCommands() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    Serial.print("Получена команда: ");
    Serial.println(command);

    if (command == "day_on") {
      ledOn();
      if (currentState == IDLE) {
        currentState = AWAIT_GAME;
        Serial.println("Состояние: AWAIT_GAME");
      }
    } else if (command == "day_off") {
      ledOff();
    } else if (command == "start") {
      ledOff();
      Serial.println("Команда 'start': подсветка выключена.");
    } else if (command == "game") {
      ledOn();
      story28_played = false;
      hint_counter = 0;
      currentState = GAME_ACTIVE;
      myDFPlayer.loop(TRACK_FON_SAFE);
      Serial.println("Состояние: GAME_ACTIVE");
    } else if (command == "restart") {
      ledOn();
      openLocker();
    } else if (command == "open_door") {
      // --- ИЗМЕНЕНИЕ ---
      if (currentState != GAME_WON) {
        Serial.println("Принудительный переход в GAME_WON.");
        startGameWonSequence();
      }
    }
  }
}

// --- ИЗМЕНЕНИЕ ---
// Вся логика для GAME_WON была переписана
void handlePlayerQueries() {
  static bool flagTrack;
  static unsigned long trackTimer;
  if(millis()- trackTimer >= 2000){
    flagTrack = 0;
  }

  // 2026-05-25 fix v5: RAW DFPlayer parser УДАЛЁН (был выше).
  //
  // Через USB Serial debug найдено что в Safe ESP32 БЫЛИ ДВА конкурирующих
  // parser'а: RAW (читает dfplayerSerial напрямую) и library (через
  // myDFPlayer.available() ниже). Оба читали с одного UART, на CMD=0x3D
  // оба триггерили playMp3Folder(SAFE_END) → DFPlayer library state corrupt
  // → ESP32 reset → offline.
  //
  // v4 пытался убрать library handler (оставить RAW) — стало ХУЖЕ: ESP32
  // моментально крашится на startGameWonSequence (видимо library требует
  // что-то от available() для корректной работы playMp3Folder).
  //
  // v5: наоборот — убираем RAW parser, оставляем только library handler.
  // Логика transition (step 1→2→3→4) полностью реализована в library
  // handler ниже. Time-based fallback в case GAME_WON (стр ~575+) подхватит
  // если library не парсит CMD=0x3D — через 30 сек step продвинется по
  // таймеру (это и работало в логе до v4).
  //
  // Тот же паттерн что в chest.ino (commit 3357d03) — там тоже убрали RAW.

  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    Serial.print("DFPlayer event type=");
    Serial.print(type);
    Serial.print(" value=");
    Serial.println(value);
    sendLogToServer("{\"log\":\"Safe DFPlayer LIB event: type=" + String(type) + " value=" + String(value) + " state=" + String(currentState) + " step=" + String(gameWonSequenceStep) + "\"}");
    if (type == 11) {
      int finishedTrack = value;
      Serial.print("Завершился трек: ");
      Serial.println(finishedTrack);
      hintFlag = 1;

      if (currentState == GAME_ACTIVE && finishedTrack != TRACK_FON_SAFE) {
        if(!flagTrack){
          myDFPlayer.playMp3Folder(TRACK_FON_SAFE);
          sendLogToServer("{\"log\":\"Safe: Playing Fon Safe sound\"}");
          trackTimer = millis();
          flagTrack = 1;
        }
      }
      
      if (currentState == GAME_WON) {
        // Шаг 1 -> 2: Завершился TRACK_STORY_29A, теперь запускаем TRACK_SAFE_END
        bool isTrack29A = (finishedTrack == TRACK_STORY_29A_RU || finishedTrack == TRACK_STORY_29A_AR || finishedTrack == TRACK_STORY_29A_FR
                        || finishedTrack == TRACK_STORY_29A_EN || finishedTrack == TRACK_STORY_29A_UK || finishedTrack == TRACK_STORY_29A_PL);
        
        if (isTrack29A && gameWonSequenceStep == 1) {
          gameWonSequenceStep = 2;
          stepEnteredAt = millis();
          myDFPlayer.playMp3Folder(TRACK_SAFE_END);
          sendLogToServer("{\"log\":\"Safe: Playing Safe End sound\"}");
        } 
        // Шаг 2 -> 3: Завершился TRACK_SAFE_END, открываем замок и запускаем TRACK_STORY_29B
        else if (finishedTrack == TRACK_SAFE_END && gameWonSequenceStep == 2) {
          gameWonSequenceStep = 3;
          stepEnteredAt = millis();
          openLocker();
          if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_RU); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (RU)\"}"); }
          if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_EN); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (EN)\"}"); }
          if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_AR); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (AR)\"}"); }
          if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_FR); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (FR)\"}"); }
          if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_UK); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (UK)\"}"); }
          if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_PL); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (PL)\"}"); }
          hintFlag = 0; // Запрещаем подсказки после этого момента
        } 
        // Шаг 3 -> 4: Завершился TRACK_STORY_29B, сцена окончена
        else if (gameWonSequenceStep == 3) {
          bool isTrack29B = (finishedTrack == TRACK_STORY_29B_RU || finishedTrack == TRACK_STORY_29B_AR || finishedTrack == TRACK_STORY_29B_FR
                        || finishedTrack == TRACK_STORY_29B_EN || finishedTrack == TRACK_STORY_29B_UK || finishedTrack == TRACK_STORY_29B_PL);
          if (isTrack29B) {
            gameWonSequenceStep = 4;
            stepEnteredAt = millis();
            hintFlag = 1;  // 2026-05-30: разрешаем HINT_3 после окончания 29B
            hintPlayedAt = 0;
            Serial.println("Сцена победы полностью завершена.");
          }
        }
      }
    }
  }
}

// --- НОВАЯ ФУНКЦИЯ ---
void startGameWonSequence() {
  if (currentState == GAME_WON) return; // Защита от повторного запуска

  currentState = GAME_WON;
  gameWonSequenceStep = 1;
  stepEnteredAt = millis();
  myDFPlayer.stop();
  // Начинаем процесс отправки данных
  safeEndSendTimer = millis(); // Устанавливаем таймер для немедленной первой отправки
  
  // В зависимости от языка, запускаем нужный трек 29A
  if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_RU); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (RU)\"}"); }
  if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_EN); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (EN)\"}"); }
  if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_AR); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (AR)\"}"); }
  if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_FR); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (FR)\"}"); }
  if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_UK); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (UK)\"}"); }
  if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_PL); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (PL)\"}"); }
}

void ledOn() {
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, HIGH);
  Serial.println("Подсветка включена");
}

void ledOff() {
  digitalWrite(LED_PIN_1, LOW);
  digitalWrite(LED_PIN_2, LOW);
  Serial.println("Подсветка выключена");
}

void openLocker() {
  Serial.println("Открытие замка...");
  digitalWrite(LOCKER_PIN, HIGH);
  delay(500);
  digitalWrite(LOCKER_PIN, LOW);
  Serial.println("Замок закрыт");
}

void SendData(){
  if (WiFi.status() == WL_CONNECTED) {
    sendLogToServer("{\"log\":\"Safe game finished, sending 'end' to server.\"}");
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(3000);  // не блокировать loop надолго — 3 сек хватит
    String payload = "{\"safe\":\"end\"}";
    int httpCode = http.POST(payload);
    // 2026-05-25: если сервер вернул {"confirm":true} прямо в body —
    // ставим safeEndConfirmed=true локально (не ждём встречный
    // confirm_safe_end HTTP request, который тоже может теряться).
    if (httpCode == 200) {
      String resp = http.getString();
      if (resp.indexOf("\"confirm\"") >= 0 && resp.indexOf("true") >= 0) {
        safeEndConfirmed = true;
        Serial.println("Safe: end confirmed via response body");
      }
    }
    http.end();
  }
}

// === DIAG: парсер команд diag_set:<output>:<value> ===
// Safe-специфика. Выходы:
//   LOCKER_PIN (GPIO 18) — электрозамок, ТОЛЬКО импульс 500мс (соленоид сгорит)
//   LED_PIN_1 (GPIO 21), LED_PIN_2 (GPIO 22) — подсветка сейфа
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
  int onv = val.toInt() ? HIGH : LOW;

  if      (key == "led_1")    digitalWrite(LED_PIN_1, onv);
  else if (key == "led_2")    digitalWrite(LED_PIN_2, onv);
  else if (key == "leds_all") {
    digitalWrite(LED_PIN_1, onv);
    digitalWrite(LED_PIN_2, onv);
  }
  else if (key == "lock_pulse") {
    // 500мс импульс на электрозамок (как openLocker())
    digitalWrite(LOCKER_PIN, HIGH);
    delay(500);
    digitalWrite(LOCKER_PIN, LOW);
  }
  else if (key == "volume") {
    value = constrain(val.toInt(), 0, 30);
    myDFPlayer.volume(value);
  }
  else if (key == "play")       myDFPlayer.play(val.toInt());
  else if (key == "stop_audio") myDFPlayer.stop();
}

// === DIAG: периодический snapshot всех I/O ===
// Формат: {"diag":{"in":[3],"out":[3],"vol":N,"rssi":N,"heap":N,"uptime":N,"temp":N}}
// in[0..2]: reed_1(36), reed_2(32), ball(39)
// Нормализация: 1 = активен.
//   reed_1 (INPUT, без pullup): active=HIGH (внешний pulldown)
//   reed_2 (INPUT_PULLUP): active=LOW (геркон к GND)
//   ball   (INPUT): active=HIGH
void sendDiagSnapshot() {
  if (WiFi.status() != WL_CONNECTED) return;

  String payload = "{\"diag\":{\"in\":[";
  payload += (digitalRead(REED_SWITCH_1_PIN) == HIGH ? "1" : "0"); payload += ",";
  payload += (digitalRead(REED_SWITCH_2_PIN) == LOW  ? "1" : "0"); payload += ",";
  payload += (digitalRead(BALL_SENSOR_PIN)   == HIGH ? "1" : "0");
  payload += "],\"out\":[";
  payload += String(digitalRead(LED_PIN_1));   payload += ",";
  payload += String(digitalRead(LED_PIN_2));   payload += ",";
  payload += String(digitalRead(LOCKER_PIN));
  payload += "],\"vol\":";
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
  http.begin("http://192.168.4.1:3000/api?device=safe");
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(150);
  http.POST(payload);
  http.end();
}
