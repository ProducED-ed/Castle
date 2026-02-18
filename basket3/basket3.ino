#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>
#include "GyverTM1637.h"
#define IR_TIMEOUT 200
#define BASKET_IR_PIN A10 

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
bool isLoose;
bool hasSentReadyLog = false;
bool lessonIsStarted = false;
unsigned long robotScoreTimer = 0; // Таймер для паузы после гола робота
bool waitingForRobotScore = false; // Флаг: ждем ли мы эти 2 секунды?

int SCORE_ROBOT = 0;
int SCORE_MAN = 0; 
int state = 0;

void sendLog(String message) {
  Serial1.println("log:basket:" + message);
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
}

void loop() {
  static int previousState = -1;
  if (state != previousState) {
    String logMsg = "State changed to " + String(state);
    sendLog(logMsg);
    previousState = state;
  }

  switch (state) {
    case 0:
      while (Serial1.available()) { 
        String buff = Serial1.readStringUntil('\n');
        buff.trim();
        if (buff == "ready") {
          if (!hasSentReadyLog) { sendLog("Ready received."); hasSentReadyLog = true; }
          HandleMessagges("ready");
        }
        else if (buff == "start") { state = 1; hasSentReadyLog = false; }
        else if (buff == "restart") { HandleMessagges("restart"); }
        else { HandleMessagges(buff); }
      }
      break;
    case 1:
      galetButton.tick();
      if (galetButton.isPress()) Serial1.println("galet_on");
      if (galetButton.isRelease()) Serial1.println("galet_off");
      flagButton.tick();
      if (flagButton.isPress()) Serial1.println("flag2_on");
      if (flagButton.isRelease()) Serial1.println("flag2_off");
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
      
      state = 0;
      CheckState();
      
      // Если это ready, ставим флаг, чтобы не спамить логами
      if (message == "ready") hasSentReadyLog = true;
      return;
  }
  // --- ВАШИ ФИКСЫ (Оставляем) ---
  else if (message == "troll") {
    strip.clear(); strip.show();
    // ИЗМЕНЕНО: Добавлено логирование перед командой
    sendLog("Troll game SKIPPED. Sending cave_end command. troll_complete");
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
    sendLog("Opening Mine Door");
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
  // if(message == "open_mine_door") OpenLock(SHERIF_EM1); // Дубликат удален
}

void CheckState() {
  if (!digitalRead(26)) { // Проверка галетника
    if (!_restartGalet) { 
      delay(30); // <--- ДОБАВЛЕНО: Задержка перед отправкой
      Serial1.println("galet_on"); 
      _restartGalet = 1; 
    }
  } else { 
    if (_restartGalet) { 
      Serial1.println("galet_off"); 
      _restartGalet = 0; 
    }
  }
  
  if (digitalRead(27)) { // Проверка флага
    if (!_restartFlag) { 
      delay(30); // <--- ДОБАВЛЕНО: Задержка перед отправкой
      Serial1.println("flag2_on"); 
      _restartFlag = 1; 
    }
  } else { 
    if (_restartFlag) { 
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
    String buff = Serial1.readStringUntil('\n'); buff.trim();
    if (buff == "start_troll") state++;
    else if (buff == "light_on") digitalWrite(owlLed,HIGH);
    else if (buff == "light_off") digitalWrite(owlLed,LOW);
    else HandleMessagges(buff);
  }
}

void TrollGame(){
  digitalWrite(trollLed, HIGH);
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
      else if (buff == "cave_search1") { trollSequence=1; isTrollFixed = 0; }
      else if (buff == "cave_search2") { trollSequence=2; isTrollFixed = 0; }
      else if (buff == "cave_search3") { trollSequence=3; isTrollFixed = 0; }
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
          state++;
      }
      else HandleMessagges(buff);
  }
}

void _Troll_1() {
  butt7.tick();
  if (butt7.isPress() && !isTrollFixed) {
    strip.clear(); strip.setPixelColor(1, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("aluminium"); isTrollFixed=1;
  }
}
void _Troll_2() {
  butt5.tick(); butt7.tick();
  if (butt7.isRelease()){ strip.clear(); strip.show(); }
  if (butt5.isPress() && !isTrollFixed) {
    strip.clear(); strip.setPixelColor(3, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("bronze"); isTrollFixed=1;
  }
}
void _Troll_3() {
  butt8.tick(); butt5.tick();
  if (butt5.isRelease()){ strip.clear(); strip.show(); }
  if (butt8.isPress() && !isTrollFixed) {
    strip.clear(); isTrollFixed=1; strip.setPixelColor(0, strip.Color(0, 0, 255)); strip.show();
    Serial1.println("copper");
  }
}
void _Troll_4() {
  butt6.tick(); butt8.tick();
  if (butt8.isRelease()){ strip.clear(); strip.show(); }
  if (butt6.isPress()) {
    trollSequence++; strip.clear(); strip.show();
    // ИЗМЕНЕНО: Добавлено логирование перед командой
    sendLog("Troll game finished. Sending cave_end command. troll_complete");
    delay(10);
    // КОНЕЦ
    Serial1.println("cave_end");
    strip.setPixelColor(2, strip.Color(0, 0, 255)); strip.show();
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
  if (butt4.isPress() || butt2.isPress() || butt1.isPress()) { buttonSequence = 0; Serial1.println("cave_reset"); delay(100); }
  if (butt3.isPress()) { buttonSequence = 2; Serial1.println("cave_click"); delay(100); }
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
void BasketLesson(){
  static bool lastBoyState = HIGH; 
  static unsigned long lastDebounceTime = 0;
  bool reading = digitalRead(28); 

  if (reading != lastBoyState) { lastDebounceTime = millis(); }

  if ((millis() - lastDebounceTime) > 50) {
     static bool currentBoyState = HIGH;
     if (reading != currentBoyState) {
       currentBoyState = reading;
       if (currentBoyState == LOW) { // PRESSED
          sendLog("BoyButton PRESSED");
          
          
             delay(50); // Пауза перед отправкой команды
             Serial1.println("boy_in_lesson");
             
             delay(50); // Пауза перед отправкой лога
             sendLog("Sent boy_in_lesson");
             
             delay(100);
          
       }
       if (currentBoyState == HIGH) { // RELEASED
          if (!lessonIsStarted) {
             Serial1.println("boy_out_lesson");
             // ИЗМЕНЕНО: Добавлен текст "Boy removed" для срабатывания страховки
             sendLog("Lesson: Boy removed.");
             delay(100);
          }
       }
     }
  }
  lastBoyState = reading;

  if(lessonIsStarted){
    bool btnState = digitalRead(BASKET_IR_PIN);
    if(btnState && !basket_ir_read_F){
      state++; digitalWrite(basketLed, LOW);
      Serial1.println("lesson_goal"); sendLog("Lesson Goal");
      delay(5000);
      Serial1.println("lesson_basket_done"); sendLog("Lesson Done");
      basket_ir_read_F = 1; _startBasket = 0;
    }
    if (!btnState && basket_ir_read_F) { delay(IR_TIMEOUT); basket_ir_read_F = 0; }
  }
  
  while (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n'); buff.trim();
    if (buff == "start_basket"){ digitalWrite(basketLed, HIGH); lessonIsStarted = 1; sendLog("Start Basket"); }
    else if (buff == "start_lesson") { lessonIsStarted = 1; sendLog("Start Lesson (Arm)");
    if (digitalRead(28) == LOW) { // Если мальчик уже стоит
           Serial1.println("boy_in_lesson");
           sendLog("Force sent: boy_in_lesson");
           delay(50);
       }
       }
    else if (buff == "restart") { HandleMessagges("restart"); }
    else if (buff == "win"){
      SCORE_MAN=3; OUTPUT_TO_DISPLAY(); delay(2000); PRINT_SCORE_ROBOT(); delay(1000); PRINT_SCORE_MAN();
      Serial1.println("fr8nmr"); state+=2; lessonIsStarted = 0;
    }
    else { HandleMessagges(buff); }
  }
}

void Basket(){
  // --- 1. Читаем команды от сервера ---
  while (Serial1.available()) {
      String buff = Serial1.readStringUntil('\n');
      buff.trim();
      
      if (buff == "start_basket"){
        basketTimer = millis();
        digitalWrite(basketLed, HIGH);
        _startBasket = 1;
        
        // Рукопожатие: Подтверждаем получение старта
        Serial1.println("confirm_start"); 
        
        sendLog("Basket: Round started.");
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
        // Принудительная победа от сервера
        SCORE_MAN=3;
        OUTPUT_TO_DISPLAY(); delay(2000); PRINT_SCORE_ROBOT(); delay(1000);
        PRINT_SCORE_MAN(); delay(1000); Serial1.println("fr8nmr"); isLoose = 0; state++;
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

  // --- 2. ЛОГИКА ПОБЕДЫ (ПРИОРИТЕТ №1) ---
  // Проверяем ДО проверки датчика. Если выиграли - вибрации нам не страшны.
  if (SCORE_MAN >= 3) {
      OUTPUT_TO_DISPLAY(); 
      // Шлем команду победы несколько раз для надежности
      Serial1.println("fr8nmr"); delay(50);
      Serial1.println("fr8nmr"); delay(50);
      Serial1.println("fr8nmr");
      
      sendLog("Basket: Player WON (3 goals). Ignoring sensor vibration.");
      isLoose=0; 
      state++; // Переходим на следующий уровень
      return;  // Выходим, не проверяя датчик
  }

  // --- 3. УМНЫЙ ДАТЧИК МАЛЬЧИКА (Anti-Vibration) ---
  bool physicalReading = (digitalRead(28) == LOW); // LOW = Мальчик стоит
  static bool gameIsActive = true;                 // Текущий статус игры
  static unsigned long debounceTimer = 0;          // Таймер вибрации
  static bool debounceActive = false;              // Флаг, что мы "подозреваем" снятие

  // А) Мальчик физически СТОИТ
  if (physicalReading) {
      debounceActive = false; // Сбрасываем подозрения, контакт есть
      
      if (!gameIsActive) {
          // Если игра была на паузе - ВОЗОБНОВЛЯЕМ
          gameIsActive = true;
          Serial1.println("boy_in_game");
          sendLog("Basket: Boy returned (Game Resumed).");
          
          // Восстанавливаем состояние (но не сбрасываем счет!)
          if(isLoose){ 
             SCORE_ROBOT = 0; SCORE_MAN = 0; isLoose=0; _startBasket=0; basket_ir_read_F=0;
          }
          if (_startBasket) digitalWrite(basketLed, HIGH);
      }
  }
  // Б) Мальчик физически ОТСУТСТВУЕТ (или геркон дребезжит)
  else {
      // Если мы еще не запустили таймер проверки - запускаем
      if (!debounceActive && gameIsActive) {
          debounceTimer = millis();
          debounceActive = true;
      }
      
      // Если таймер тикает уже больше 500 мс - значит это НЕ вибрация
      if (debounceActive && (millis() - debounceTimer > 500)) {
          gameIsActive = false; // Ставим игру на паузу
          debounceActive = false; // Таймер сработал
          
          Serial1.println("boy_out_game");
          sendLog("Basket: Boy removed confirmed > 500ms (Game Paused).");
          digitalWrite(basketLed, LOW);
      }
  }

  // --- 4. ИГРОВОЙ ПРОЦЕСС ---
  // Игра идет, только если gameIsActive == true (Мальчик стоит стабильно)
  if (gameIsActive && !isLoose) {
      OUTPUT_TO_DISPLAY();
      
      if(_startBasket){
          if(millis() - basketTimer <= 15000){ // 15 секунд на бросок
              bool btnState = digitalRead(BASKET_IR_PIN);
              
              // Гол игрока
              if(btnState && !basket_ir_read_F){
                  SCORE_MAN++;
                  OUTPUT_TO_DISPLAY();
                  
                  // Эффект гола
                  digitalWrite(basketLed, LOW); 
                  Serial1.println(SCORE_MAN == 1 ? "fr61nmr" : "fr62nmr"); // Сразу шлем код
                  
                  basket_ir_read_F = 1;
                  _startBasket = 0; // Гол забит, ждем следующего запуска
                  sendLog("Basket: Goal scored!");
                  
                  delay(300); 
                  if (gameIsActive) digitalWrite(basketLed, HIGH);
              }
              if (!btnState && basket_ir_read_F) { delay(IR_TIMEOUT); basket_ir_read_F = 0; }
          }
          else{ 
              // Время вышло (Тайм-аут)
              _startBasket = 0;
              digitalWrite(basketLed, LOW);
              delay(1000);
              Serial1.println("start_snitch"); 
              sendLog("Basket: Time out. Snitch started.");
          }
      }

      // Проигрыш (Робот забил 3)
      if (SCORE_ROBOT == 3) {
          Serial1.println("fr9nmr"); delay(50);
          Serial1.println("fr9nmr");
          sendLog("Basket: Robot won.");
          isLoose = 1; 
          _startBasket = 0; 
          delay(3000); 
          OpenLock(Solenoid);
      }
  }
}

void PRINT_SCORE_MAN() {
  switch (SCORE_MAN) { case 1: Serial1.println("fr61nmr"); break; case 2: Serial1.println("fr62nmr"); break; } delay(50);
}
void PRINT_SCORE_ROBOT() {
  switch (SCORE_ROBOT) { case 1: Serial1.println("fr71nmr"); break; case 2: Serial1.println("fr72nmr"); break; } delay(50);
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
