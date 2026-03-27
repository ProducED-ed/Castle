#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>
#include "GyverTM1637.h"
#define IR_TIMEOUT 200
#define BASKET_IR_PIN A10
void CheckState(bool force = false);

Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, 9, NEO_GRB + NEO_KHZ800);
#define CLK A12
#define DIO A14

GyverTM1637 disp(CLK, DIO);

// ... (Кнопки как раньше) ...
GButton butt1(40); GButton butt2(42); GButton butt3(44); GButton butt4(46);
GButton butt5(38); GButton butt6(36); GButton butt7(34); GButton butt8(32);
GButton helpButton(30); GButton metallButton(31); GButton galetButton(26); GButton flagButton(27);
// GButton boyButton(28); // Убрали

int SHERIF_EM1 = 12;
int SHERIF_EM2 = 23;
int Solenoid = 21;
int trollLed = 47;
int basketLed = 52;
int owlLed = 48;

int buttonSequence = 0;
int trollSequence = 0;
int score = 0;
bool upHelp;
bool _startBasket;
byte light;
unsigned long lightFlashTimer;
boolean basket_ir_read_F = 0; 

bool doorFlags = 1;
bool metallClick;
unsigned long doorTimer;
unsigned long doorDef;
unsigned long basketTimer;
bool _restartFlag;
bool _restartGalet;
bool isTrollFixed;
int activeTrollButton = -1;       // Какой геркон сейчас зажег диод
unsigned long activeTrollLedTimer = 0; // Таймер для 5 секунд
bool isLoose;
bool hasSentReadyLog = false;
bool lessonIsStarted = false;
unsigned long robotScoreTimer = 0; // Таймер для паузы после гола робота
bool waitingForRobotScore = false; // Флаг: ждем ли мы эти 2 секунды?

int SCORE_ROBOT = 0;
int SCORE_MAN = 0;
// --- ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ДЛЯ МАЛЬЧИКА ---
bool lesson_lastBoyState = HIGH;
unsigned long lesson_lastDebounceTime = 0;
bool lesson_currentBoyState = HIGH;

bool game_gameIsActive = true;
unsigned long game_debounceTimer = 0;          
bool game_debounceActive = false;
// ------------------------------------------
int state = 0;

void sendLog(String message) {
  Serial1.print("log:basket:");
  Serial1.println(message);
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(trollLed, OUTPUT);
  pinMode(SHERIF_EM1, OUTPUT);
  pinMode(SHERIF_EM2, OUTPUT);
  pinMode(basketLed, OUTPUT);
  pinMode(Solenoid, OUTPUT);
  pinMode(owlLed, OUTPUT);
  pinMode(BASKET_IR_PIN, INPUT_PULLUP);
  pinMode(28, INPUT_PULLUP); // boyButton

  digitalWrite(SHERIF_EM1, LOW);
  digitalWrite(SHERIF_EM2, LOW);
  digitalWrite(Solenoid, LOW);

  disp.clear(); disp.brightness(7); 
  strip.begin(); strip.setBrightness(255); strip.show();

  // Устанавливаем ВСЕ пины кнопок в режим входа
  for (int i = 30; i <= 46; i++) {
    pinMode(i, INPUT_PULLUP); // Это создаст базовую стабильность
  }
  
  butt1.setDebounce(50); butt1.setType(HIGH_PULL); butt1.setDirection(NORM_OPEN);
  butt2.setDebounce(50); butt2.setType(HIGH_PULL); butt2.setDirection(NORM_OPEN);
  butt3.setDebounce(50); butt3.setType(HIGH_PULL); butt3.setDirection(NORM_OPEN);
  butt4.setDebounce(50); butt4.setType(HIGH_PULL); butt4.setDirection(NORM_OPEN);
  butt5.setDebounce(50); butt5.setType(HIGH_PULL); butt5.setDirection(NORM_OPEN);
  butt6.setDebounce(50); butt6.setType(HIGH_PULL); butt6.setDirection(NORM_OPEN);
  butt7.setDebounce(50); butt7.setType(HIGH_PULL); butt7.setDirection(NORM_OPEN);
  butt8.setDebounce(50); butt8.setType(HIGH_PULL); butt8.setDirection(NORM_OPEN);
  helpButton.setDebounce(50); helpButton.setType(HIGH_PULL); helpButton.setDirection(NORM_OPEN);
  metallButton.setDebounce(50); metallButton.setType(HIGH_PULL); metallButton.setDirection(NORM_OPEN);
  galetButton.setDebounce(50); galetButton.setType(HIGH_PULL); galetButton.setDirection(NORM_OPEN);
  flagButton.setDebounce(50); flagButton.setType(LOW_PULL); flagButton.setDirection(NORM_OPEN);
  sendLog("Bask: SYS START");
}

void loop() {
  // ОПРОС ВСЕХ КНОПОК В КАЖДОМ ЦИКЛЕ (КРИТИЧНО!)
  butt1.tick(); butt2.tick(); butt3.tick(); butt4.tick();
  butt5.tick(); butt6.tick(); butt7.tick(); butt8.tick();
  galetButton.tick(); flagButton.tick();
  static int previousState = -1;
  if (state != previousState) {
    String logMsg = "State changed to " + String(state);
    sendLog(logMsg);
    previousState = state;
  }

  galetButton.tick();
  // Если нажали галетник и он еще не был обработан
  if (galetButton.isPress() && _restartGalet == 0) {
      Serial1.println("galet_on");
      _restartGalet = 1;
  }
  // Если отпустили галетник
  if (galetButton.isRelease() && _restartGalet == 1) {
      Serial1.println("galet_off");
      _restartGalet = 0;
  }

  flagButton.tick();
  // Если нажали флаг
  if (flagButton.isPress() && _restartFlag == 0) {
      Serial1.println("flag2_on");
      _restartFlag = 1;
  }
  // Если отпустили флаг
  if (flagButton.isRelease() && _restartFlag == 1) {
      Serial1.println("flag2_off");
      _restartFlag = 0;
  }

  switch (state) {
    case 0:
      while (Serial1.available()) { 
        String buff = Serial1.readStringUntil('\n');
        buff.trim();
        if (buff == "ready") {
          HandleMessagges("ready");
        }
        else if (buff == "start") { state = 1; hasSentReadyLog = false; }
        else if (buff == "restart") { HandleMessagges("restart"); }
        else { HandleMessagges(buff); }
      }
      break;
    case 1:
      galetButton.tick();
      // Синхронизация галетника
      if (galetButton.isPress() && _restartGalet == 0) {
          Serial1.println("galet_on");
          _restartGalet = 1;
      }
      if (galetButton.isRelease() && _restartGalet == 1) {
          Serial1.println("galet_off");
          _restartGalet = 0;
      }

      flagButton.tick();
      if (flagButton.isPress() && _restartFlag == 0) {
          Serial1.println("flag2_on");
          _restartFlag = 1;
      }
      if (flagButton.isRelease() && _restartFlag == 1) {
          Serial1.println("flag2_off");
          _restartFlag = 0;
      }

      StartTrollGame();
      break;
    case 2: OpenDoor(); break;
    case 3: TrollGame(); break;
    case 4: WorkShopGame(); break;
    case 5: OpenBasket(); break;
    case 6: BasketLesson(); break;
    case 7: Basket(); break;
    case 8: 
       while (Serial1.available()){
        String buff = Serial1.readStringUntil('\n'); buff.trim();
        if (buff == "ready") {
             HandleMessagges("ready");
        }
        else if (buff == "restart"){ 
             HandleMessagges("restart");
        }
        else { HandleMessagges(buff); }
      }
      break;
  }
  HelpButton();
  DoorDefender(); // Вернули оригинальный DoorDefender
}

void HandleMessagges(String message) {
  if (message.length() > 0 && message != "restart" && message != "ready") sendLog("CMD: " + message);

  // 1. ЛОГИКА ЗАМКОВ (ТОЛЬКО ПРИ RESTART)
  if(message == "restart"){
      hasSentReadyLog = false;
      // Открываем замки только если это РЕСТАРТ
      OpenLock(SHERIF_EM1); 
      delay(150);
      OpenLock(SHERIF_EM2);
  }

  // 2. ОБЩАЯ ЛОГИКА СБРОСА (И ДЛЯ RESTART, И ДЛЯ READY)
  if (message == "restart" || message == "ready") {
      // Принудительное отключение всех силовых выходов
      digitalWrite(SHERIF_EM1, LOW);
      digitalWrite(SHERIF_EM2, LOW);
      digitalWrite(Solenoid, LOW);
      
      digitalWrite(trollLed, LOW);
      digitalWrite(owlLed, LOW);
      digitalWrite(basketLed, LOW);
      
      SCORE_ROBOT = 0;
      SCORE_MAN = 0;
      buttonSequence = 0;
      trollSequence = 0;
      score = 0;
      _startBasket = 0;
      
      isTrollFixed = 0; 
      isLoose = 0;
      light = 0;
      upHelp = 0;
      
      disp.clear(); strip.clear(); strip.show(); disp.point(0);
      
      _restartGalet = 0;
      _restartFlag = 0;
      lessonIsStarted = false;
      
      doorDef = millis(); 
      doorTimer = millis();
      doorFlags = 1;

      bool boyPhysical = digitalRead(28);
      lesson_lastBoyState = boyPhysical;
      lesson_currentBoyState = boyPhysical;
      game_gameIsActive = (boyPhysical == LOW); // LOW означает, что он стоит
      game_debounceActive = false;
      game_debounceTimer = 0;
      lesson_lastDebounceTime = millis();
      
      state = 0;
      // --- Всегда отвечаем на ready ---
      if (message == "ready") {
          Serial1.println("log:basket:System Ready");
          hasSentReadyLog = true;
      }
      return;
  }

  else if (message == "check_state") {
      CheckState(true);
  }
  
  else if (message == "troll") {
    strip.clear(); strip.show();
    // ИЗМЕНЕНО: Добавлено логирование перед командой
    sendLog("Troll: SKIPPED");
    delay(10);
    // КОНЕЦ
    Serial1.println("cave_end"); 
    sendLog("Skipping Troll -> State 4");
    strip.setPixelColor(2, strip.Color(0, 0, 255)); strip.show();
    state = 4; 
  }
  // --- ВАЖНЫЙ ФИКС ДЛЯ ДВЕРИ ---
  else if (message == "mine" || message == "open_mine_door") {
    OpenLock(SHERIF_EM1);
    digitalWrite(trollLed, HIGH);
    Serial1.println("door_cave");
    if (state < 3) state = 3;
  }
  else if (message == "start_troll") {
     if (state < 1) state = 1;
  }

  if(message == "troll_light_on") {
    digitalWrite(trollLed, HIGH);
    sendLog("trollLed ON");
  }
  if(message == "troll_light_off") {
    digitalWrite(trollLed, LOW);
    sendLog("trollLed OFF");
  }
  if(message == "open_door") OpenLock(SHERIF_EM2);
  if(message == "opent_basket") OpenLock(SHERIF_EM2);
}

void CheckState(bool force) {
  // 1. Читаем физику (Галетник=26, Флаг=27)
  // Галетник (Pin 26): Active LOW.
  bool g = !digitalRead(26);
  // Флаг (Pin 27): Active HIGH.
  bool f = digitalRead(27);

  // 2. Короткий лог + задержка
  if (force) {
    Serial1.print("log:basket:G");
    Serial1.print(g); 
    Serial1.print("_F");
    Serial1.println(f);
    Serial1.flush();
    delay(150); // Важная задержка для синхронизации с Main Board
  }

  // 3. Отправка команд Галетника
  if (g) {
    if (!_restartGalet || force) { 
      Serial1.println("galet_on"); 
      _restartGalet = 1; 
    }
  } else {
    if (_restartGalet || force) { 
      Serial1.println("galet_off"); 
      _restartGalet = 0; 
    }
  }
  
  delay(20);

  // 4. Отправка команд Флага
  if (f) {
    if (!_restartFlag || force) { 
      Serial1.println("flag2_on"); 
      if(force) { delay(20); Serial1.println("flag2_on"); } // Дублируем
      _restartFlag = 1;
    }
  } else {
    if (_restartFlag || force) { 
      Serial1.println("flag2_off"); 
      _restartFlag = 0; 
    }
  }
}

void WorkShopGame(){
  MetallButtonClick();
  if(metallClick) MetallBlink(2);
  while (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n'); buff.trim();
    if (buff == "item_find"){ metallClick = 0; strip.setPixelColor(2, strip.Color(0, 128, 128)); strip.show(); }
    else if (buff == "item_end"){ metallClick = 0; strip.setPixelColor(2, strip.Color(0, 0, 0)); strip.show(); state++; }
    else if (buff == "light_on") digitalWrite(owlLed,HIGH);
    else if (buff == "light_off") digitalWrite(owlLed,LOW);
    else HandleMessagges(buff);
  }
}

void StartTrollGame(){
  while (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n'); 
    buff.trim();
    
    if (buff == "start_troll") {
        state = 2; // ЯВНО фиксируем состояние 2
        butt5.tick(); butt6.tick(); butt7.tick(); butt8.tick();
        sendLog("Troll: State 2");
        return;    // Мгновенный выход, чтобы не прочитать вторую команду за раз
    }
    else if (buff == "light_on") {
        digitalWrite(owlLed, HIGH);
    }
    else if (buff == "light_off") {
        digitalWrite(owlLed, LOW);
    }
    else {
        HandleMessagges(buff);
    }
  }
}

void TrollGame(){
  digitalWrite(trollLed, HIGH);

  // 1. Обязательно опрашиваем ВСЕ герконы каждый цикл
  butt5.tick(); butt6.tick(); butt7.tick(); butt8.tick();

  // 2. Очистка от ложных помех
  if (trollSequence != 0) butt8.isPress();
  if (trollSequence != 1) butt5.isPress();
  if (trollSequence != 2) butt7.isPress();
  if (trollSequence != 3) butt6.isPress();

  // 3. УМНОЕ ГАШЕНИЕ СВЕТОДИОДА
  if (activeTrollButton != -1) {
      bool isMagnetPresent = false;
      if (activeTrollButton == 7) isMagnetPresent = butt7.state();
      else if (activeTrollButton == 5) isMagnetPresent = butt5.state();
      else if (activeTrollButton == 8) isMagnetPresent = butt8.state();
      
      // Если магнита нет И прошло больше 5 секунд с момента включения
      if (!isMagnetPresent && (millis() - activeTrollLedTimer > 5000)) {
          strip.clear();
          strip.show();
          activeTrollButton = -1; // Сбрасываем слежение
      }
  }

  // 4. Запуск нужного этапа
  switch (trollSequence) {
      case 0: _Troll_1(); break;
      case 1: _Troll_2(); break;
      case 2: _Troll_3(); break;
      case 3: _Troll_4(); break;
  }
  while (Serial1.available()) {
      String buff = Serial1.readStringUntil('\n'); buff.trim();
      if (buff == "light_on") digitalWrite(owlLed,HIGH);
      else if (buff == "light_off") digitalWrite(owlLed,LOW);
      
      // Проверяем, что подтверждение пришло именно для ТЕКУЩЕГО шага.
      // Это предотвращает "фидбек-петлю", когда шум на пинах заставляет плату 
      // бесконечно спамить командами aluminium/bronze/copper.
      
      else if (buff == "cave_search1") { 
          if (trollSequence == 0) { // Переходим к предмету 2, только если мы еще на 1-м
              trollSequence = 1; 
              isTrollFixed = 0;      // Разрешаем поиск следующего предмета
          }
      }
      else if (buff == "cave_search2") { 
          if (trollSequence == 1) { // Переходим к предмету 3, только если мы на 2-м
              trollSequence = 2; 
              isTrollFixed = 0;
          }
      }
      else if (buff == "cave_search3") { 
          if (trollSequence == 2) { // Переходим к финальному предмету
              trollSequence = 3; 
              isTrollFixed = 0; 
          }
      }
      else if (buff == "troll"){
          strip.clear(); strip.show();
          Serial1.println("cave_end");
          strip.setPixelColor(2, strip.Color(0, 0, 255)); strip.show();
          state++;
      }
      else HandleMessagges(buff);
    }
}

void OpenDoor(){
  switch (buttonSequence) {
      case 0: _Button_1(); break;
      case 1: _Button_2(); break;
      case 2: _Button_3(); break;
      case 3: _Button_4(); break;
      case 4: _Button_5(); break;
  }
  butt1.tick(); butt2.tick(); butt3.tick(); butt4.tick();
  while (Serial1.available()) {
      String buff = Serial1.readStringUntil('\n'); buff.trim();
      if (buff == "light_on") digitalWrite(owlLed,HIGH);
      else if (buff == "light_off") digitalWrite(owlLed,LOW);
      else if (buff == "mine"){
          OpenLock(SHERIF_EM1);
          digitalWrite(trollLed, HIGH);
          Serial1.println("door_cave");
          // ОЧИСТКА ПАМЯТИ КНОПОК
          butt7.isPress(); butt5.isPress(); butt8.isPress(); butt6.isPress();
          state++;
      }
      else HandleMessagges(buff);
  }
}

void _Troll_1() {
  if (butt7.isPress() && !isTrollFixed) {
    strip.clear(); strip.setPixelColor(1, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("aluminium"); 
    isTrollFixed = 1;
    activeTrollButton = 8;
    activeTrollLedTimer = millis();
  }
}

void _Troll_2() {
  if (butt5.isPress() && !isTrollFixed) {
    strip.clear(); strip.setPixelColor(3, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("bronze"); 
    isTrollFixed = 1;
    activeTrollButton = 5;
    activeTrollLedTimer = millis();
  }
}

void _Troll_3() {
  if (butt8.isPress() && !isTrollFixed) {
    strip.clear(); strip.setPixelColor(0, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("copper");
    isTrollFixed = 1;
    activeTrollButton = 7;
    activeTrollLedTimer = millis();
  }
}

void _Troll_4() {
  if (butt6.isPress()) {
    trollSequence++; 
    strip.clear(); 
    strip.setPixelColor(2, strip.Color(0, 0, 255)); strip.show();
    delay(10);
    Serial1.println("cave_end");
    activeTrollButton = -1; // Финальный диод не гасим!
    state++;
  }
}

void MetallBlink(int number){
  if (millis() - lightFlashTimer >= 18) {
      if (!upHelp) { light++; if (light == 75) upHelp = 1; }
      else { light--; if (light == 10) upHelp = 0; }
      float brightness = light / 75.0;
      int green = 128 * brightness; int blue = 128 * brightness;
      strip.setPixelColor(number, strip.Color(0, green, blue));
      strip.show();
      lightFlashTimer = millis();
  }
}

void _Button_1() {
  if (butt1.isPress()) { buttonSequence = 1; Serial1.println("cave_click"); delay(100); }
  if (butt3.isPress() || butt2.isPress() || butt4.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
}
void _Button_2() {
  if (butt4.isPress() || butt2.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
  if (butt3.isPress()) { buttonSequence = 2; Serial1.println("cave_click"); delay(100); }
  // Ничего не сбрасываем, просто издаем звук "клик", без шкалы.
  if (butt1.isPress()) { Serial1.println("cave_repeat"); delay(100); }
}
void _Button_3() {
  if (butt4.isPress()) { buttonSequence = 3; Serial1.println("cave_click"); delay(100); }
  if (butt2.isPress() || butt3.isPress() || butt1.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
}
void _Button_4() {
  if (butt1.isPress() || butt3.isPress() || butt4.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
  if (butt2.isPress()) { buttonSequence = 4; Serial1.println("cave_click"); delay(100); }
}
void _Button_5() {
  if (butt3.isPress()) {
    OpenLock(SHERIF_EM1); digitalWrite(trollLed, HIGH); Serial1.println("door_cave"); state++;
  }
  if (butt2.isPress() || butt4.isPress() || butt1.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
}

// --- BasketLesson: Прямое чтение кнопки + Логи ---
void BasketLesson() {
  bool reading = digitalRead(28); 

  if (reading != lesson_lastBoyState) { lesson_lastDebounceTime = millis(); }

  if ((millis() - lesson_lastDebounceTime) > 50) {
     if (reading != lesson_currentBoyState) {
       lesson_currentBoyState = reading;
       
       if (lesson_currentBoyState == LOW) { 
             game_gameIsActive = true; // <--- СИНХРОНИЗАЦИЯ С 19 УРОВНЕМ!
             delay(50);
             Serial1.println("I"); // ШИФР: boy_in_lesson
             delay(50);
       }
       if (lesson_currentBoyState == HIGH) { 
             game_gameIsActive = false; // <--- СИНХРОНИЗАЦИЯ С 19 УРОВНЕМ!
          if (!lessonIsStarted) {
             Serial1.println("O"); // ШИФР: boy_out_lesson
             delay(50);
          }
       }
     }
  }
  lesson_lastBoyState = reading;

  if (lessonIsStarted) {
    bool btnState = digitalRead(BASKET_IR_PIN);
    if (btnState && !basket_ir_read_F) {
      digitalWrite(basketLed, LOW);
      Serial1.println("G"); // ШИФР: lesson_goal
      
      delay(5000); 
      Serial1.println("D"); // ШИФР: lesson_basket_done
      
      delay(50);
      state++; // Смена статуса
      
      basket_ir_read_F = 1; _startBasket = 0;
    }
    if (!btnState && basket_ir_read_F) { 
      delay(IR_TIMEOUT);
      basket_ir_read_F = 0; 
    }
  }
  
  while (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n'); buff.trim();
    if (buff == "start_basket"){ 
        digitalWrite(basketLed, HIGH); lessonIsStarted = 1; 
        Serial1.println("C"); // ШИФР: confirm_start
    }
    else if (buff == "start_lesson") { 
        if (!lessonIsStarted) { // <--- ЗАЩИТА ОТ ДВОЙНОЙ ОТПРАВКИ С СЕРВЕРА
            lessonIsStarted = 1; 
            if (digitalRead(28) == LOW) {
               Serial1.println("I"); // ШИФР: boy_in_lesson
               delay(50);
            }
        }
    }
    else if (buff == "restart") { HandleMessagges("restart"); }
    else if (buff == "win"){
      SCORE_MAN=3; OUTPUT_TO_DISPLAY(); delay(2000); PRINT_SCORE_ROBOT(); delay(1000); PRINT_SCORE_MAN();
      Serial1.println("3"); // ШИФР: fr8nmr (win)
      state+=2; lessonIsStarted = 0;
    }
    else { HandleMessagges(buff); }
  }
}

void Basket() {
  while (Serial1.available()) {
      String buff = Serial1.readStringUntil('\n'); buff.trim();
      
      if (buff == "start_basket"){
        basketTimer = millis();
        digitalWrite(basketLed, HIGH);
        _startBasket = 1;
        Serial1.println("C"); // ШИФР: confirm_start
      }
      else if (buff == "start_basket_robot"){
        if (!isLoose && SCORE_ROBOT < 3) {
            SCORE_ROBOT++;
            OUTPUT_TO_DISPLAY(); 
            robotScoreTimer = millis();
            waitingForRobotScore = true; 
        }
      }
      else if (buff == "win"){
        SCORE_MAN=3; OUTPUT_TO_DISPLAY(); delay(2000); PRINT_SCORE_ROBOT(); delay(1000);
        PRINT_SCORE_MAN(); delay(1000); Serial1.println("3"); // ШИФР: fr8nmr (win)
        isLoose = 0; state++;
      }
      else if (buff == "restart") { HandleMessagges("restart"); }
      else { HandleMessagges(buff); }
  }

  if (waitingForRobotScore) {
      if (millis() - robotScoreTimer >= 2000) {
          PRINT_SCORE_ROBOT();
          waitingForRobotScore = false;
      }
  }

  if (SCORE_MAN >= 3) {
      OUTPUT_TO_DISPLAY();
      Serial1.println("3"); // ШИФР: fr8nmr (win)
      delay(50);
      isLoose=0; 
      state++; 
      return;
  }

  bool physicalReading = (digitalRead(28) == LOW);

  if (physicalReading) {
      game_debounceActive = false;
      if (!game_gameIsActive) {
          game_gameIsActive = true;
          Serial1.println("I"); // ШИФР: boy_in_game
          if(isLoose){ SCORE_ROBOT = 0; SCORE_MAN = 0; isLoose=0; _startBasket=0; basket_ir_read_F=0; }
          if (_startBasket) digitalWrite(basketLed, HIGH);
      }
  } else {
      if (!game_debounceActive && game_gameIsActive) {
          game_debounceTimer = millis();
          game_debounceActive = true;
      }
      // Оставили 500 мс антидребезг (этого более чем достаточно, если память очищена!)
      if (game_debounceActive && (millis() - game_debounceTimer > 500)) {
          game_gameIsActive = false;
          game_debounceActive = false;
          Serial1.println("O"); // ШИФР: boy_out_game
          digitalWrite(basketLed, LOW);
      }
  }

  if (game_gameIsActive && !isLoose) {
      OUTPUT_TO_DISPLAY();
      if(_startBasket){
          if(millis() - basketTimer <= 15000){ // 15 секунд на бросок
              bool btnState = digitalRead(BASKET_IR_PIN);
              if(btnState && !basket_ir_read_F){
                  SCORE_MAN++;
                  OUTPUT_TO_DISPLAY();
                  digitalWrite(basketLed, LOW);
                  if (SCORE_MAN < 3) {
                      Serial1.println(SCORE_MAN == 1 ? "1" : "2"); // ШИФР: fr61nmr / fr62nmr
                  }
                  basket_ir_read_F = 1; _startBasket = 0;
                  delay(300);
                  if (game_gameIsActive) digitalWrite(basketLed, HIGH);
              }
              if (!btnState && basket_ir_read_F) { delay(IR_TIMEOUT); basket_ir_read_F = 0; }
          } else { 
              _startBasket = 0;
              digitalWrite(basketLed, LOW);
              delay(1000);
              Serial1.println("S"); // ШИФР: start_snitch
          }
      }
      if (SCORE_ROBOT == 3) {
          Serial1.println("6"); // ШИФР: fr9nmr (lose)
          isLoose = 1; 
          _startBasket = 0; 
          delay(3000); 
          OpenLock(Solenoid);
      }
  }
}

void PRINT_SCORE_MAN() {
  switch (SCORE_MAN) { case 1: Serial1.println("1"); break; case 2: Serial1.println("2"); break; } delay(50);
}
void PRINT_SCORE_ROBOT() {
  
}

void OUTPUT_TO_DISPLAY() {
  disp.point(1);
  if (SCORE_MAN < 10) { disp.display(2, 0); disp.display(3, SCORE_MAN); } else if (SCORE_MAN == 10) { disp.display(2, 1); disp.display(3, 0); }
  if (SCORE_ROBOT < 10) { disp.display(0, 0); disp.display(1, SCORE_ROBOT); } else if (SCORE_ROBOT == 10) { disp.display(0, 1); disp.display(1, 0); }
}

void OpenBasket(){
  state++; // Сразу переходим к уроку
}

void OpenLock(byte num) {
  digitalWrite(num, HIGH);
  delay(300); // Увеличено
  digitalWrite(num, LOW);
}

// Вернули старую логику DoorDefender
void DoorDefender() {
  if (millis() - doorDef >= 3000) {
    if (doorFlags) {
      if (state >= 3 && state <= 7) digitalWrite(SHERIF_EM1, HIGH);
      if (state >= 6 && state <= 7) digitalWrite(SHERIF_EM2, HIGH);
      doorTimer = millis(); doorFlags = 0;
    }
  }
  if (!doorFlags && (millis() - doorTimer >= 50)) {
      if (state >= 3 && state <= 7) digitalWrite(SHERIF_EM1, LOW);
      if (state >= 6 && state <= 7) digitalWrite(SHERIF_EM2, LOW);
      doorDef = millis(); doorFlags = 1;
  }
}

void HelpButton(){
  helpButton.tick();
  if (helpButton.isPress()) Serial1.println("help");
}
void MetallButtonClick(){
  metallButton.tick();
  if (metallButton.isPress()) { Serial1.println("metal"); metallClick = 1; }
}
