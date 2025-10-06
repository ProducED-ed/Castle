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
//const char* ssid = "Castle";
//const char* password = "questquest";
const char* ssid = "ProducED";
const char* password = "32744965";
int language = 1;
unsigned long doorTimer;
bool hintFlag=1;

// Настройки статического IP
IPAddress local_IP(192, 168, 0, 204);   

const char* externalApi = "http://192.168.0.120:3000/api";

WebServer server(80);
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
    myDFPlayer.volume(24);
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

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/plain", "ESP32 Server is running");
  });
  server.on("/data", HTTP_POST, []() {
    if (server.hasArg("plain")) {
      String body = server.arg("plain");
      if(body == "\"start\""){
        ledOff();
        Serial.println("Команда 'start': подсветка выключена.");
        currentState = AWAIT_GAME;
      }
      if(body == "\"restart\""){
        ledOn();
        openLocker();
        myDFPlayer.stop();
       
        currentState = IDLE;
      }
      if(body == "\"ready\""){
        ledOff();
        myDFPlayer.stop();
        currentState = IDLE;
      }
      if(body == "\"game\""){
        ledOn();
        story28_played = false;
        hint_counter = 0;
        currentState = GAME_ACTIVE;
     
        myDFPlayer.playMp3Folder(TRACK_FON_SAFE);
        Serial.println("Состояние: GAME_ACTIVE");
      }
      if(body == "\"skip\""){
        // --- ИЗМЕНЕНИЕ ---
        Serial.println("Принудительный переход в GAME_WON.");
        startGameWonSequence();
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
          }
          if(language == 2){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_EN);
          }
          if(language == 3){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_AR);
          }
          if(language == 4){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_GE);
          }
          if(language == 5){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_SP);
          }
          if(language == 6){
            myDFPlayer.playMp3Folder(TRACK_HINT_0_CH);
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
        }
        if(language == 2){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_EN);
        }
        if(language == 3){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_AR);
        }
        if(language == 4){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_GE);
        }
        if(language == 5){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_SP);
        }
        if(language == 6){
          myDFPlayer.playMp3Folder(TRACK_STORY_28_CH);
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
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_EN);
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_AR);
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_GE);
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_SP);
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_1_CH);
            }
          }
          if(hint_counter == 1){
            if(language == 1){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_RU);
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_EN);
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_AR);
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_GE);
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_SP);
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_2_CH);
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
        SendData();
        startGameWonSequence();
      }
      break;
    case GAME_WON:
      if (digitalRead(REED_SWITCH_2_PIN) == LOW && gameWonSequenceStep >= 4 && (millis() - lastDebounceTime_2) > debounceDelay) {
        if (hintFlag){
          if(language == 1){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_RU);
            }
            if(language == 2){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_EN);
            }
            if(language == 3){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_AR);
            }
            if(language == 4){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_GE);
            }
            if(language == 5){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_SP);
            }
            if(language == 6){
              myDFPlayer.playMp3Folder(TRACK_HINT_3_CH);
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
        } 
        // Шаг 2 -> 3: Завершился TRACK_SAFE_END, открываем замок и запускаем TRACK_STORY_29B
        else if (finishedTrack == TRACK_SAFE_END && gameWonSequenceStep == 2) {
          gameWonSequenceStep = 3;
          openLocker();
          if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_RU); }
          if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_EN); }
          if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_AR); }
          if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_GE); }
          if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_SP); }
          if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29B_CH); }
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
  
  // В зависимости от языка, запускаем нужный трек 29A
  if(language == 1){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_RU); }
  if(language == 2){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_EN); }
  if(language == 3){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_AR); }
  if(language == 4){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_GE); }
  if(language == 5){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_SP); }
  if(language == 6){ myDFPlayer.playMp3Folder(TRACK_STORY_29A_CH); }
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
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    
    // Пример POST-запроса
    String payload = "{\"safe\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}