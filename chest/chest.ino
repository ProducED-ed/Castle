#include <WiFi.h>
#include "GyverButton.h"
#include "HardwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <WebServer.h>
#include <HTTPClient.h>
HardwareSerial mySerial(1); 

byte light;
int symbolBrightness;
int iterator;
int buttonSequence;
int score;
int mistakeLed = 22;
int insideLed = 23;
int ledsSym[] = {25,5,18,19};
int state = 0;
bool isMistake = false;

int SHERIF_EM2 = 21;

//геркон двигалок
const byte eyeSymbolGerkon = 33;

bool up;
bool fadeUp = 1;
bool symbolFade = 1;

unsigned long prevTwinkleTime = 0;
unsigned long prevBreathTime = 0;
int breathPhase = 0;         // Фаза эффекта дыхания
bool twinkleMode = true; 

////Timers
unsigned long eyeSymbolTimer;
unsigned long symbolFadeTimer;
int pause_duration = 2000;
unsigned long pauseTimer = 0;
unsigned long mistakeTimer = 0;
unsigned long effectTimer = 0;
int arrayLenght = 0;

const byte numLeds = 4;

unsigned long previousMillis = 0; // Таймер
const int interval = 1;         // Интервал мерцания (мс)

byte currentLed = 0;      // Текущий светодиод
int brightness = 0;       // Текущая яркость
bool fadingUp = true;     // Флаг увеличения яркости
unsigned long timerEndLed;
unsigned long timerEndLock;
unsigned long timerDoor;
bool storyFlag;
int language=1;
bool afterEffect;
bool hintFlag = 1;

// --- Системные треки (1-2) ---
const int TRACK_FON_SUITCASE = 1;
const int TRACK_SUITCASE_END = 2;

// --- Истории (3-29) ---
// Блок Story 8
const int TRACK_STORY_8_RU = 10;
const int TRACK_STORY_8_EN = 17;
const int TRACK_STORY_8_AR = 24;
const int TRACK_STORY_8_GE = 4;
const int TRACK_STORY_8_SP = 4;
const int TRACK_STORY_8_CH = 4;

// Блок Story 7
const int TRACK_STORY_7_RU = 9;
const int TRACK_STORY_7_EN = 16;
const int TRACK_STORY_7_AR = 23;
const int TRACK_STORY_7_GE = 3;
const int TRACK_STORY_7_SP = 3;
const int TRACK_STORY_7_CH = 3;

// --- Подсказки (5-29) ---
// Блок Hint 1
const int TRACK_HINT_1_RU = 12;
const int TRACK_HINT_1_EN = 19;
const int TRACK_HINT_1_AR = 26;
const int TRACK_HINT_1_GE = 6;
const int TRACK_HINT_1_SP = 6;
const int TRACK_HINT_1_CH = 6;

// Блок Hint 2
const int TRACK_HINT_2_RU = 13;
const int TRACK_HINT_2_EN = 20;
const int TRACK_HINT_2_AR = 27;
const int TRACK_HINT_2_GE = 7;
const int TRACK_HINT_2_SP = 7;
const int TRACK_HINT_2_CH = 7;

// Блок Hint 3
const int TRACK_HINT_3_RU = 14;
const int TRACK_HINT_3_EN = 21;
const int TRACK_HINT_3_AR = 28;
const int TRACK_HINT_3_GE = 8;
const int TRACK_HINT_3_SP = 8;
const int TRACK_HINT_3_CH = 8;

// Блок Hint 4
const int TRACK_HINT_4_RU = 15;
const int TRACK_HINT_4_EN = 22;
const int TRACK_HINT_4_AR = 29;
const int TRACK_HINT_4_GE = 8;
const int TRACK_HINT_4_SP = 8;
const int TRACK_HINT_4_CH = 8;

// Блок Hint 0
const int TRACK_HINT_0_RU = 11;
const int TRACK_HINT_0_EN = 18;
const int TRACK_HINT_0_AR = 25;
const int TRACK_HINT_0_GE = 5;
const int TRACK_HINT_0_SP = 5;
const int TRACK_HINT_0_CH = 5;

int hint_counter = 0;

const char* ssid = "Castle";
const char* password = "questquest";
//const char* ssid = "ProducED";
//const char* password = "32744965";

// Настройки статического IP
IPAddress local_IP(192, 168, 0, 203);   

const char* externalApi = "http://192.168.0.100:3000/api";

WebServer server(80);

GButton butt1(32);
GButton butt2(35);
GButton butt3(34);
GButton butt4(39);

GButton helpButton(36);

DFRobotDFPlayerMini myMP3;


void setup() {
  delay(2000);
  Serial.begin(115200);
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  arrayLenght = sizeof(ledsSym) / sizeof(int);
  for (int i = 0; i < arrayLenght; i++)
  {
    pinMode(ledsSym[i], OUTPUT);
    digitalWrite(ledsSym[i], LOW);
  }

  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt1.setType(HIGH_PULL);
  butt1.setDirection(NORM_OPEN);
  //------
  butt2.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt2.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt2.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt2.setType(HIGH_PULL);
  butt2.setDirection(NORM_OPEN);
  ////-------
  butt3.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt3.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt3.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt3.setType(HIGH_PULL);
  butt3.setDirection(NORM_OPEN);
  ///////////------
  butt4.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt4.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt4.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt4.setType(HIGH_PULL);
  butt4.setDirection(NORM_OPEN);

  helpButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  helpButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  helpButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  helpButton.setType(HIGH_PULL);
  helpButton.setDirection(NORM_OPEN);

  pinMode(mistakeLed, OUTPUT); 
  digitalWrite(mistakeLed, LOW);

  pinMode(insideLed, OUTPUT);
  digitalWrite(insideLed, LOW);

  pinMode(SHERIF_EM2, OUTPUT);
  digitalWrite(SHERIF_EM2, LOW);

  pinMode(eyeSymbolGerkon, INPUT);
  
  // Подключение к Wi-Fi
  //WiFi.begin(ssid, password);
  if (!myMP3.begin(mySerial)) {
    Serial.println("DFPlayer Mini not detected!");
  }    
  myMP3.volume(30);
  myMP3.stop();

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
      if(body == "\"game\""){
        state = 1;
        myMP3.playMp3Folder(TRACK_FON_SUITCASE);
        //myMP3.enableLoop();
        hint_counter = 0;
        isMistake = false;
        score = 0;
        storyFlag=0;
        buttonSequence = 0;
        for (int i = 0; i < arrayLenght; i++)
        {
          analogWrite(ledsSym[i], 0);
        }
        butt1.resetStates();
        butt2.resetStates();
        butt3.resetStates();
        butt4.resetStates();
      }
      if(body == "\"restart\""){
        state = 6;
        for (int i = 0; i < arrayLenght; i++)
        {
          analogWrite(ledsSym[i], 255);
        }
        myMP3.stop();
        digitalWrite(insideLed, HIGH);
        OpenLock(SHERIF_EM2);
      }
      if(body == "\"ready\""){
        state = 0;
        for (int i = 0; i < arrayLenght; i++)
        {
          analogWrite(ledsSym[i], 0);
        }
        myMP3.stop();
        digitalWrite(insideLed, LOW);
      }
      if(body == "\"day_on\""){
        state = 6;
        for (int i = 0; i < arrayLenght; i++)
        {
          analogWrite(ledsSym[i], 255);
        }
        digitalWrite(insideLed, HIGH);
      }
      if(body == "\"day_off\""){
        state = 6;
        for (int i = 0; i < arrayLenght; i++)
        {
          analogWrite(ledsSym[i], 0);
        }
        digitalWrite(insideLed, LOW);
      }
      if(body == "\"start\""){
        state = 0;
        isMistake = false;
        score = 0;
        storyFlag=0;
        buttonSequence = 0;
        butt1.resetStates();
        butt2.resetStates();
        butt3.resetStates();
        butt4.resetStates();
        myMP3.stop();
        hintFlag = 1;
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
      if(body == "\"open_door\""){
        OpenLock(SHERIF_EM2);
        state = 6;
      }
      if(body == "\"skip\""){
        //OpenLock(SHERIF_EM2);
        timerEndLed = millis();
      timerEndLock = millis();
        state = 3;
        myMP3.stop();
        myMP3.playMp3Folder(TRACK_SUITCASE_END);
      }
      if(body == "\"firework\""){
        state = 5; // Устанавливаем состояние 5, которое вы создали для эффекта
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
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim(); // Убираем лишние пробелы и символы переноса строки

    if (command == "firework") {
      state = 5; // Переключаемся в состояние фейерверка
    } else {
      // Если пришла любая другая команда, выключаем светодиоды и переходим в базовое состояние
      if (state == 5) {
        for (int i = 0; i < arrayLenght; i++) {
          analogWrite(ledsSym[i], 0);
        }
        state = 0; // Возвращаемся в состояние ожидания
      }
    }
  }
  server.handleClient();
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
    delay(2000);
  }
  handlePlayerQueries();
  helpButton.tick();
  if(helpButton.isPress()){
    Serial.println("1");
    if(state == 0 && hintFlag){
      myMP3.pause(); // Ставим фоновую музыку на паузу
      delay(50);
      if(language == 1){
        myMP3.playMp3Folder(TRACK_HINT_0_RU);
      }
      if(language == 2){
        myMP3.playMp3Folder(TRACK_HINT_0_EN);
      }
      if(language == 3){
        myMP3.playMp3Folder(TRACK_HINT_0_AR);
      }
      if(language == 4){
        myMP3.playMp3Folder(TRACK_HINT_0_GE);
      }
      if(language == 5){
        myMP3.playMp3Folder(TRACK_HINT_0_SP);
      }
      if(language == 6){
        myMP3.playMp3Folder(TRACK_HINT_0_CH);
      }
      hintFlag = 0;
    }
    if(state > 0 && state < 3 && hintFlag){
      myMP3.pause(); // Ставим фоновую музыку на паузу
      delay(50);
      Serial.println(hint_counter);
      if(hint_counter == 0) {
          if(language == 1){
            myMP3.playMp3Folder(TRACK_HINT_1_RU);
          }
          if(language == 2){
            myMP3.playMp3Folder(TRACK_HINT_1_EN);
          }
          if(language == 3){
            myMP3.playMp3Folder(TRACK_HINT_1_AR);
          }
          if(language == 4){
            myMP3.playMp3Folder(TRACK_HINT_1_GE);
          }
          if(language == 5){
            myMP3.playMp3Folder(TRACK_HINT_1_SP);
          }
          if(language == 6){
            myMP3.playMp3Folder(TRACK_HINT_1_CH);
          }
      }
      if(hint_counter == 1) {
        if(language == 1){
          myMP3.playMp3Folder(TRACK_HINT_2_RU);
        }
        if(language == 2){
          myMP3.playMp3Folder(TRACK_HINT_2_EN);
        }
        if(language == 3){
          myMP3.playMp3Folder(TRACK_HINT_2_AR);
        }
        if(language == 4){
          myMP3.playMp3Folder(TRACK_HINT_2_GE);
        }
        if(language == 5){
          myMP3.playMp3Folder(TRACK_HINT_2_SP);
        }
        if(language == 6){
          myMP3.playMp3Folder(TRACK_HINT_2_CH);
        }
      }
      if(hint_counter == 2) {
        if(language == 1){
          myMP3.playMp3Folder(TRACK_HINT_3_RU);
        }
        if(language == 2){
          myMP3.playMp3Folder(TRACK_HINT_3_EN);
        }
        if(language == 3){
          myMP3.playMp3Folder(TRACK_HINT_3_AR);
        }
        if(language == 4){
          myMP3.playMp3Folder(TRACK_HINT_3_GE);
        }
        if(language == 5){
          myMP3.playMp3Folder(TRACK_HINT_3_SP);
        }
        if(language == 6){
          myMP3.playMp3Folder(TRACK_HINT_3_CH);
        }
      }
      hintFlag = 0;
      hint_counter++;
      if(hint_counter>2){
        hint_counter=0;
      }
    }
    if(state > 3 && hintFlag){
      if(language == 1){
        myMP3.playMp3Folder(TRACK_HINT_4_RU);
      }
      if(language == 2){
        myMP3.playMp3Folder(TRACK_HINT_4_EN);
      }
      if(language == 3){
        myMP3.playMp3Folder(TRACK_HINT_4_AR);
      }
      if(language == 4){
        myMP3.playMp3Folder(TRACK_HINT_4_GE);
      }
      if(language == 5){
        myMP3.playMp3Folder(TRACK_HINT_4_SP);
      }
      if(language == 6){
        myMP3.playMp3Folder(TRACK_HINT_4_CH);
      }
      hintFlag = 0;
    }
  }

  //Serial.println(!digitalRead(eyeSymbolGerkon));
 switch (state) {
   case 0:
     for (int i = 0; i < arrayLenght; i++)
     {
       analogWrite(ledsSym[i], 0);
     }
     digitalWrite(insideLed, LOW);
     digitalWrite(mistakeLed, LOW);
     break;
   case 1:
     digitalWrite(insideLed, HIGH);
     ButtonSymbols();
     break;
   case 2:
     SymbolEye();
     break;
   case 3:
      randomTwinkleEffect();
     break;
    case 4:
    //if(myMP3.readState() == 512){
    //  OpenLock(SHERIF_EM2);
    //  state++;
    //}
     for (int i = 0; i < arrayLenght; i++)
     {
       analogWrite(ledsSym[i], 255);
     }
     //digitalWrite(insideLed, LOW);
     break;
   case 5: // Состояние для эффекта "firework"
     randomTwinkleEffect();
     break;
 }

 if(state > 3 && state < 6){
  if(millis()-timerDoor >= 3000){
    OpenLock(SHERIF_EM2);
    timerDoor = millis();
  }
 }
}

void SendData(){
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(externalApi);
    http.addHeader("Content-Type", "application/json");
    
    // Пример POST-запроса
    String payload = "{\"suitcase\":\"end\"}";
    int httpCode = http.POST(payload);
    http.end();
  }
}


//двигалки
void SymbolEye() {
  static bool flag;
    butt1.tick();
    butt2.tick();
    butt3.tick();
    butt4.tick();
    //if(myMP3.readState() != 513){
      
    //if (myMP3.readState() == 512) {
    //  if(!flag){
    //    myMP3.playMp3Folder(1);
    //    delay(1000);
    //    myMP3.enableLoop();
    //    flag = 1;
    //  } 
    //}
  if (!digitalRead(eyeSymbolGerkon) && butt1.isHold() && butt2.isHold() && butt3.isHold() && butt4.isHold()) {
    if (millis() - eyeSymbolTimer >= 500) {
      Serial.println("wineye");
      SendData();
      myMP3.pause(); // Ставим фоновую музыку на паузу
      delay(50);
      myMP3.playMp3Folder(TRACK_SUITCASE_END);
      timerEndLed = millis();
      timerEndLock = millis();
      state++;
      flag=0;
    }
  }
  else eyeSymbolTimer = millis();

  if(!butt1.isHold() || !butt2.isHold() || !butt3.isHold() || !butt4.isHold()){
    state--;
    for (int i = 0; i < arrayLenght; i++)
    {
      analogWrite(ledsSym[i], 0);
    }
  }
  
}



//символы следим за кнопками светим адреской
void ButtonSymbols() {
  if(!isMistake){
    switch (buttonSequence) {
      case 0:
        _Button_1();
        break;
      case 1:
        _Button_2();
        break;
      case 2:
        _Button_3();
        break;
      case 3:
        _Button_4();
        break;
    }

    butt1.tick();
    butt2.tick();
    butt3.tick();
    butt4.tick();
  }
  else{
    Mistake();
  }

}

void Mistake(){
  if(millis()-mistakeTimer <= pause_duration){
      digitalWrite(mistakeLed, HIGH);
      iterator = 0;
      symbolBrightness = 0;
      symbolFadeTimer = millis();
      for (int i = 0; i < arrayLenght; i++)
      {
        analogWrite(ledsSym[i], 0);
      }
      butt1.resetStates();
      butt2.resetStates();
      butt3.resetStates();
      butt4.resetStates();
  }
  else{
    isMistake = false;
    digitalWrite(mistakeLed, LOW);
  }
}



//логика для кнопок
void _Button_1() {
  
  if (butt2.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt3.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt4.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt1.isHold()) {
    score++;
    //symbolBrightness = 0;
    Serial.println("trubutmr");
    for(int i =0; i<=255;i++){
      analogWrite(ledsSym[0], i);
      delay(3);
    }
  }

  if (score == 1) {
    Serial.println("1stage");
    buttonSequence++;
    symbolBrightness = 0;
  }
}
void _Button_2() {
  if (!butt1.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt2.isHold()) {
    score++;
    Serial.println("trubutmr");
    for(int i =0; i<=255;i++){
      analogWrite(ledsSym[1], i);
      delay(3);
    }
  }
  if (butt3.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt4.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (score == 2) {
    Serial.println("2stage");
    buttonSequence++;
  }
}

void _Button_3() {
  if (!butt1.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (!butt2.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt3.isHold()) {
    score++;
    Serial.println("trubutmr");
    for(int i =0; i<=255;i++){
      analogWrite(ledsSym[2], i);
      delay(3);
    }
  }
  if (butt4.isPress()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (score == 3) {
    Serial.println("3stage");
    symbolBrightness = 0;
    buttonSequence++;
  }
}

void _Button_4() {
  if (!butt1.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (!butt2.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (!butt3.isHold()) {
    score = 0;
    Serial.println("fsbutmr");
    buttonSequence = 0;
    mistakeTimer = millis();
    isMistake = true;
  }
  if (butt4.isHold()) {
    score++;
    Serial.println("trubutmr");
    for(int i =0; i<=255;i++){
      analogWrite(ledsSym[3], i);
      delay(3);
    }
  }
  if (score == 4) {
    Serial.println("4stage");
    symbolBrightness = 0;
    for (int i = 0; i < arrayLenght; i++)
    {
      analogWrite(ledsSym[i], 255);
    }
    if(!storyFlag){
      myMP3.pause(); // Ставим фоновую музыку на паузу
      delay(50);
      if(language == 1)
        myMP3.playMp3Folder(TRACK_STORY_7_RU);
      if(language == 2)
        myMP3.playMp3Folder(TRACK_STORY_7_EN);
      if(language == 3)
        myMP3.playMp3Folder(TRACK_STORY_7_AR);
      if(language == 4)
        myMP3.playMp3Folder(TRACK_STORY_7_GE);
      if(language == 5)
        myMP3.playMp3Folder(TRACK_STORY_7_SP); 
      if(language == 6)
        myMP3.playMp3Folder(TRACK_STORY_7_CH);       
      storyFlag = 1;
    }
    
    delay(500);
    state++;
  }
}

void OpenLock(byte num) {
  digitalWrite(num, HIGH);
  delay(250);
  digitalWrite(num, LOW);
}


void randomTwinkleEffect() {
  unsigned long currentMillis = millis();
  
  // Если прошло достаточно времени
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Плавное изменение яркости
    if (fadingUp) {
      brightness += 5;
      if (brightness >= 255) {
        brightness = 255;
        fadingUp = false;
      }
    } else {
      brightness -= 5;
      if (brightness <= 0) {
        brightness = 0;
        fadingUp = true;
        currentLed = random(numLeds); // Выбираем случайный светодиод
      }
    }

    // Включаем текущий светодиод с заданной яркостью
    //Serial.println(currentLed);
    analogWrite(ledsSym[currentLed], brightness);

    // Гасим остальные светодиоды
    for (byte i = 0; i < numLeds; i++) {
      if (i != currentLed) {
        analogWrite(ledsSym[i], 0);
      }
    }
  }
}
void handlePlayerQueries() {
  static bool flagTrack;
  static unsigned long trackTimer;
  if(millis()- trackTimer >= 2000){
    flagTrack = 0;
  }
  if (myMP3.available()) {
    uint8_t type = myMP3.readType();
    if (type == 5) {
      int finishedTrack = myMP3.read();
      Serial.print("Завершился трек: ");
      Serial.println(finishedTrack);
      hintFlag = 1;

      // ИЗМЕНЕНИЕ 3: Эта логика возобновляет фоновую музыку после окончания любого другого трека.
      // Она уже была в коде и должна работать правильно.
      if (state >= 1 && state < 3 && finishedTrack != TRACK_FON_SUITCASE) {
        if(!flagTrack){
          myMP3.playMp3Folder(TRACK_FON_SUITCASE);
          trackTimer = millis();
          flagTrack = 1;
        }
        // Возобновляем фоновую музыку с места паузы
      }

      if (finishedTrack == TRACK_FON_SUITCASE) {
        if(!flagTrack){
          myMP3.playMp3Folder(TRACK_FON_SUITCASE);
          trackTimer = millis();
          flagTrack = 1;
        }
        // Возобновляем фоновую музыку с места паузы
      }

      if(finishedTrack == TRACK_HINT_0_RU || finishedTrack == TRACK_HINT_1_RU || finishedTrack == TRACK_HINT_2_RU || finishedTrack == TRACK_HINT_3_RU || finishedTrack == TRACK_HINT_4_RU
      || finishedTrack == TRACK_HINT_0_EN || finishedTrack == TRACK_HINT_1_EN || finishedTrack == TRACK_HINT_2_EN || finishedTrack == TRACK_HINT_3_EN || finishedTrack == TRACK_HINT_4_EN
      || finishedTrack == TRACK_HINT_0_AR || finishedTrack == TRACK_HINT_1_AR || finishedTrack == TRACK_HINT_2_AR || finishedTrack == TRACK_HINT_3_AR || finishedTrack == TRACK_HINT_4_AR)
      {
        hintFlag = 1;
        Serial.println("again");
      }

        if (finishedTrack == TRACK_SUITCASE_END && !flagTrack) {
          state++;
          OpenLock(SHERIF_EM2);
          if(language == 1)
            myMP3.playMp3Folder(TRACK_STORY_8_RU);
          if(language == 2)
            myMP3.playMp3Folder(TRACK_STORY_8_EN);
          if(language == 3)
            myMP3.playMp3Folder(TRACK_STORY_8_AR);
          if(language == 4)
            myMP3.playMp3Folder(TRACK_STORY_8_GE);
          if(language == 5)
            myMP3.playMp3Folder(TRACK_STORY_8_SP); 
          if(language == 6)
            myMP3.playMp3Folder(TRACK_STORY_8_CH);   
          trackTimer = millis();
          flagTrack = 1;  
        }
    }
  }
}

