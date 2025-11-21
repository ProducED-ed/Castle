#include <Arduino.h>
#include "DFRobotDFPlayerMini.h"
#include "HardwareSerial.h"
#include <WebServer.h>
#include <HTTPClient.h>
#include <WiFi.h>

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
HardwareSerial dfplayerSerial(1);
DFRobotDFPlayerMini myDFPlayer;

// --- АУДИОФАЙЛЫ ---
const int TRACK_STORY_28_RU = 22;
const int TRACK_STORY_28_EN = 25;
const int TRACK_STORY_28_AR = 28;
const int TRACK_STORY_28_GE = 1;
const int TRACK_STORY_28_SP = 1;
const int TRACK_STORY_28_CH = 1;

const int TRACK_HINT_0_RU = 10;
const int TRACK_HINT_0_EN = 14;
const int TRACK_HINT_0_AR = 18;
const int TRACK_HINT_0_GE = 3;
const int TRACK_HINT_0_SP = 3;
const int TRACK_HINT_0_CH = 3;

const int TRACK_HINT_1_RU = 11;
const int TRACK_HINT_1_EN = 15;
const int TRACK_HINT_1_AR = 19;
const int TRACK_HINT_1_GE = 4;
const int TRACK_HINT_1_SP = 4;
const int TRACK_HINT_1_CH = 4;

const int TRACK_HINT_2_RU = 12;
const int TRACK_HINT_2_EN = 16;
const int TRACK_HINT_2_AR = 20;
const int TRACK_HINT_2_GE = 5;
const int TRACK_HINT_2_SP = 5;
const int TRACK_HINT_2_CH = 5;


const int TRACK_STORY_29A_RU = 23;
const int TRACK_STORY_29A_EN = 26;
const int TRACK_STORY_29A_AR = 29;
const int TRACK_STORY_29A_GE = 7;
const int TRACK_STORY_29A_SP = 7;
const int TRACK_STORY_29A_CH = 7;

const int TRACK_STORY_29B_RU = 24;
const int TRACK_STORY_29B_EN = 27;
const int TRACK_STORY_29B_AR = 30;
const int TRACK_STORY_29B_GE = 8;
const int TRACK_STORY_29B_SP = 8;
const int TRACK_STORY_29B_CH = 8;

const int TRACK_HINT_3_RU = 13;
const int TRACK_HINT_3_EN = 17;
const int TRACK_HINT_3_AR = 21;
const int TRACK_HINT_3_GE = 9;
const int TRACK_HINT_3_SP = 9;
const int TRACK_HINT_3_CH = 9;

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
int value = 30;

bool safeEndConfirmed = false;      // Флаг подтверждения от сервера
unsigned long safeEndSendTimer = 0; // Таймер для периодической отправки

// Настройки статического IP
IPAddress local_IP(192, 168, 0, 204);   

const char* externalApi = "http://192.168.0.100:3000/api";

WebServer server(80);
void sendLogToServer(String payload) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin("http://192.168.0.100:3000/api/log");
    http.addHeader("Content-Type", "application/json");
    int httpCode = http.POST(payload);
    http.end();
  }
}

// --- Прототипы функций ---
void handleSerialCommands();
void handlePlayerQueries();
void ledOn();
void ledOff();
void openLocker();
// Новая вспомогательная функция, чтобы не дублировать код
void startGameWonSequence(); 

void setup() {
  Serial.begin(115200);
  Serial.println("Инициализация сейфа...");

  dfplayerSerial.begin(9600, SERIAL_8N1, DFPLAYER_TX_PIN, DFPLAYER_RX_PIN);

  pinMode(LOCKER_PIN, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  
  // ВНИМАНИЕ: Для пина 36 используем INPUT.
  // Схема подключения геркона должна обеспечивать HIGH при размыкании.
  pinMode(REED_SWITCH_1_PIN, INPUT);
  
  pinMode(REED_SWITCH_2_PIN, INPUT_PULLUP);
  pinMode(BALL_SENSOR_PIN, INPUT);

  digitalWrite(LOCKER_PIN, LOW);
  ledOff();
  if (!myDFPlayer.begin(dfplayerSerial, true, true)) {
    Serial.println(F("!!! ВНИМАНИЕ: Не удалось найти DFPlayer Mini. Проверьте подключение. !!!"));
  } else {
    Serial.println(F("DFPlayer Mini инициализирован."));
    myDFPlayer.volume(value);
    myDFPlayer.stop();
  }

  currentState = IDLE;
  Serial.println("Состояние: IDLE");
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
  sendLogToServer("ESP32 Safe is ready. IP: " + WiFi.localIP().toString());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });
  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      sendLogToServer("Received command: " + body);
      if(body == "\"start\""){
        ledOff();
        Serial.println("Команда 'start': подсветка выключена.");
        currentState = AWAIT_GAME;
		safeEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
      }
      if(body == "\"restart\""){
        ledOn();
        openLocker();
        myDFPlayer.stop();
       
        currentState = IDLE;
		safeEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
      }
      if(body == "\"ready\""){
        ledOff();
        myDFPlayer.stop();
        currentState = IDLE;
		safeEndConfirmed = false; // ИЗМЕНЕНИЕ: Сбрасываем флаг
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
}

void loop() {
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
  handleSerialCommands();
  handlePlayerQueries();
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
            myDFPlayer.playMp3Folder(TRACK_HINT_0_GE);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (GE)\"}");
          }
          if(language == 5){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_SP);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (SP)\"}");
          }
          if(language == 6){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_CH);
            sendLogToServer("{\"log\":\"Safe: Playing Hint 0 (CH)\"}");
          }
          hintFlag = 0;
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
          myDFPlayer.playMp3Folder(TRACK_STORY_28_GE);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (GE)\"}");
        }
        if(language == 5){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_SP);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (SP)\"}");
        }
        if(language == 6){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_CH);
          sendLogToServer("{\"log\":\"Safe: Playing Story 28 (CH)\"}");
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
              myDFPlayer.playMp3Folder(TRACK_HINT_1_GE);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (GE)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_SP);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (SP)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_CH);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 1 (CH)\"}");
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
              myDFPlayer.playMp3Folder(TRACK_HINT_2_GE);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (GE)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_SP);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (SP)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_CH);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 2 (CH)\"}");
            }
          }
          hintFlag = 0;
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
      // Периодически отправляем, пока не получим подтверждение
      if (!safeEndConfirmed) {
        if (millis() - safeEndSendTimer > 1000) {
          SendData();
          safeEndSendTimer = millis();
        }
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
              myDFPlayer.playMp3Folder(TRACK_HINT_3_GE);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (GE)\"}");
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_SP);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (SP)\"}");
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_CH);
              sendLogToServer("{\"log\":\"Safe: Playing Hint 3 (CH)\"}");
            }
            hintFlag=0;
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
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    Serial.println(type);
    if (type == 11) {
      int finishedTrack = myDFPlayer.read();
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
        bool isTrack29A = (finishedTrack == TRACK_STORY_29A_RU || finishedTrack == TRACK_STORY_29A_AR || finishedTrack == TRACK_STORY_29A_GE
                        || finishedTrack == TRACK_STORY_29A_EN || finishedTrack == TRACK_STORY_29A_SP || finishedTrack == TRACK_STORY_29A_CH);
        
        if (isTrack29A && gameWonSequenceStep == 1) {
          gameWonSequenceStep = 2;
          myDFPlayer.playMp3Folder(TRACK_SAFE_END);
          sendLogToServer("{\"log\":\"Safe: Playing Safe End sound\"}");
        } 
        // Шаг 2 -> 3: Завершился TRACK_SAFE_END, открываем замок и запускаем TRACK_STORY_29B
        else if (finishedTrack == TRACK_SAFE_END && gameWonSequenceStep == 2) {
          gameWonSequenceStep = 3;
          openLocker();
          if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_RU); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (RU)\"}"); }
          if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_EN); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (EN)\"}"); }
          if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_AR); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (AR)\"}"); }
          if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_GE); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (GE)\"}"); }
          if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_SP); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (SP)\"}"); }
          if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_CH); sendLogToServer("{\"log\":\"Safe: Playing Story 29B (CH)\"}"); }
          hintFlag = 0; // Запрещаем подсказки после этого момента
        } 
        // Шаг 3 -> 4: Завершился TRACK_STORY_29B, сцена окончена
        else if (gameWonSequenceStep == 3) {
          bool isTrack29B = (finishedTrack == TRACK_STORY_29B_RU || finishedTrack == TRACK_STORY_29B_AR || finishedTrack == TRACK_STORY_29B_GE
                        || finishedTrack == TRACK_STORY_29B_EN || finishedTrack == TRACK_STORY_29B_SP || finishedTrack == TRACK_STORY_29B_CH);
          if (isTrack29B) {
            gameWonSequenceStep = 4;
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
  myDFPlayer.stop();
  // Начинаем процесс отправки данных
  safeEndSendTimer = millis(); // Устанавливаем таймер для немедленной первой отправки
  
  // В зависимости от языка, запускаем нужный трек 29A
  if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_RU); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (RU)\"}"); }
  if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_EN); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (EN)\"}"); }
  if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_AR); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (AR)\"}"); }
  if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_GE); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (GE)\"}"); }
  if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_SP); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (SP)\"}"); }
  if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_CH); sendLogToServer("{\"log\":\"Safe: Playing Story 29A (CH)\"}"); }
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
    sendLogToServer("Safe game finished, sending 'end' to server.");
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    // Пример POST-запроса
    String payload = "{\"safe\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}
