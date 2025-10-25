#include "GyverButton.h"
#include <Adafruit_NeoPixel.h>
#include "GyverTM1637.h"
#define IR_TIMEOUT 200
#define BASKET_IR_PIN A10 // Пин датчика пересечения мячей


Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, 9, NEO_GRB + NEO_KHZ800);

//TM1637
#define CLK A12
#define DIO A14

GyverTM1637 disp(CLK, DIO);

GButton butt5(38);
GButton butt6(36);
GButton butt7(34);
GButton butt8(32);

GButton butt1(40);
GButton butt2(42);
GButton butt3(44);
GButton butt4(46);

GButton helpButton(30);
GButton metallButton(31);

GButton boyButton(28);

GButton galetButton(26);
GButton flagButton(27);

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

bool doorFlags;
bool metallClick;
unsigned long doorTimer;
unsigned long doorDef;
unsigned long basketTimer;
bool _restartFlag;
bool _restartGalet;
bool isTrollFixed;
bool isLoose;

//Переменные
int SCORE_ROBOT = 0; // Переменная счета в баскетболл робота
int SCORE_MAN = 0; // Переменная счета в баскетболл человека

int state = 0;
void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10);

  Serial1.begin(9600);
  Serial1.setTimeout(10);

  pinMode(trollLed, OUTPUT);
  pinMode(SHERIF_EM1, OUTPUT);
  pinMode(SHERIF_EM2, OUTPUT);
  pinMode(basketLed, OUTPUT);
  pinMode(Solenoid, OUTPUT);
  pinMode(owlLed, OUTPUT);
  pinMode(BASKET_IR_PIN, INPUT_PULLUP);

  disp.clear();
  disp.brightness(7); 

  strip.begin();
  strip.setBrightness(255);
  strip.show();

  butt1.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt1.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt1.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt1.setType(HIGH_PULL);
  butt1.setDirection(NORM_OPEN);

  butt2.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt2.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt2.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt2.setType(HIGH_PULL);
  butt2.setDirection(NORM_OPEN);
  
  butt3.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt3.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt3.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt3.setType(HIGH_PULL);
  butt3.setDirection(NORM_OPEN);

  butt4.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt4.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt4.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt4.setType(HIGH_PULL);
  butt4.setDirection(NORM_OPEN);

  butt5.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt5.setTimeout(1000);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt5.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt5.setType(HIGH_PULL);
  butt5.setDirection(NORM_OPEN);

  butt6.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt6.setTimeout(1000);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt6.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt6.setType(HIGH_PULL);
  butt6.setDirection(NORM_OPEN);

  butt7.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt7.setTimeout(1000);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt7.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt7.setType(HIGH_PULL);
  butt7.setDirection(NORM_OPEN);

  butt8.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  butt8.setTimeout(1000);        // настройка таймаута на удержание (по умолчанию 500 мс)
  butt8.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  butt8.setType(HIGH_PULL);
  butt8.setDirection(NORM_OPEN);

  helpButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  helpButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  helpButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  helpButton.setType(HIGH_PULL);
  helpButton.setDirection(NORM_OPEN);

  metallButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  metallButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  metallButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  metallButton.setType(HIGH_PULL);
  metallButton.setDirection(NORM_OPEN);

  boyButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  boyButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  boyButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  boyButton.setType(HIGH_PULL);
  boyButton.setDirection(NORM_OPEN);

  galetButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  galetButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  galetButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  galetButton.setType(HIGH_PULL);
  galetButton.setDirection(NORM_OPEN);

  flagButton.setDebounce(50);        // настройка антидребезга (по умолчанию 80 мс)
  flagButton.setTimeout(300);        // настройка таймаута на удержание (по умолчанию 500 мс)
  flagButton.setClickTimeout(600);   // настройка таймаута между кликами (по умолчанию 300 мс)
  flagButton.setType(LOW_PULL);
  flagButton.setDirection(NORM_OPEN);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (state) {
    case 0:
    if (Serial1.available())
    {
      String buff = Serial1.readString();
      if (buff == "ready\r\n"){
        digitalWrite(trollLed, LOW);
        digitalWrite(owlLed, LOW);
        digitalWrite(basketLed, LOW);

        digitalWrite(SHERIF_EM1, LOW);
        digitalWrite(SHERIF_EM2, LOW);
        digitalWrite(Solenoid, LOW);
        
        SCORE_ROBOT = 0; // Переменная счета в баскетболл робота
        SCORE_MAN = 0;
        buttonSequence = 0;
        trollSequence = 0;
        score = 0;
        upHelp=0;
       _startBasket=0;
       light=0;
       lightFlashTimer=0;
       basket_ir_read_F = 0; 
       doorFlags=0;
       metallClick=0;
       doorTimer=0;
       doorDef=0;
       basketTimer=0;
       disp.clear();
       strip.clear();
       strip.show();
       disp.point(0);
      }
      else if (buff == "start\r\n"){
        state++;
      }
      else{
        HandleMessagges(buff);
      }
    }
     break;  
   case 1:
   galetButton.tick();
    if(galetButton.isPress()){
      Serial1.println("galet_on");
    }
    if(galetButton.isRelease()){
      Serial1.println("galet_off");
    }
    flagButton.tick();
    if(flagButton.isPress()){
      Serial1.println("flag2_on");
    }
    if(flagButton.isRelease()){
      Serial1.println("flag2_off");
    }
   StartTrollGame();
     break;
   case 2:
   OpenDoor();
     break;
   case 3:
   TrollGame();
     break;
   case 4:
    WorkShopGame();
     break;
    case 5:
    OpenBasket();
     break;
   case 6:
    BasketLesson();
     break;  
  case 7:
     Basket();
     break;
  case 8:
    if(!digitalRead(26) && !_restartGalet){
      Serial1.println("galet_on");
      _restartGalet = 1;
      delay(500);
    }
    if(digitalRead(26) && _restartGalet){
      Serial1.println("galet_off");
      _restartGalet = 0;
      delay(500);
    }
    if(digitalRead(27) && !_restartFlag){
      Serial1.println("flag2_on");
      _restartFlag = 1;
      delay(500);
    }
    if(_restartFlag && !digitalRead(27)){
      Serial1.println("flag2_off");
      delay(500);
      _restartFlag = 0;
    }
       SCORE_ROBOT = 0; // Переменная счета в баскетболл робота
       SCORE_MAN = 0;
       buttonSequence = 0;
       trollSequence = 0;
       score = 0;
       upHelp=0;
       _startBasket=0;
       light=0;
       lightFlashTimer=0;
       basket_ir_read_F = 0; 
       doorFlags=0;
       metallClick=0;
       doorTimer=0;
       doorDef=0;
       basketTimer=0;
      digitalWrite(trollLed, HIGH);
      digitalWrite(owlLed, HIGH);
      digitalWrite(basketLed, HIGH);

     if (Serial1.available())
    {
      String buff = Serial1.readString();
      if (buff == "ready\r\n"){
        digitalWrite(trollLed, LOW);
        digitalWrite(owlLed, LOW);
        digitalWrite(basketLed, LOW);

        digitalWrite(SHERIF_EM1, LOW);
        digitalWrite(SHERIF_EM2, LOW);
        digitalWrite(Solenoid, LOW);
        _restartFlag = 0;
        _restartGalet = 0;
        SCORE_ROBOT = 0; // Переменная счета в баскетболл робота
        SCORE_MAN = 0;
        buttonSequence = 0;
        trollSequence = 0;
        score = 0;
        upHelp=0;
       _startBasket=0;
       light=0;
       lightFlashTimer=0;
       basket_ir_read_F = 0; 
       doorFlags=0;
       metallClick=0;
       doorTimer=0;
       doorDef=0;
       basketTimer=0;
       disp.clear();
       strip.clear();
       strip.show();
       disp.point(0);
       state = 0;
      }
      else{
        HandleMessagges(buff);
      }
    }
 }
 HelpButton();
 DoorDefender();
}

void HandleMessagges(String message){
  if(message == "restart\r\n"){
      OpenLock(SHERIF_EM1);
      OpenLock(SHERIF_EM2);
      OpenLock(Solenoid);
      galetButton.resetStates();
      flagButton.resetStates();
      delay(100);
    state = 8;
  }
  if(message == "day_on\r\n"){
    digitalWrite(trollLed, HIGH);
  }
  if(message == "day_off\r\n"){
    digitalWrite(trollLed, LOW);
  }
  if(message == "open_door\r\n"){
    OpenLock(SHERIF_EM2);
  }
  if(message == "open_mine_door\r\n"){
    OpenLock(SHERIF_EM1);
  }
}


void WorkShopGame(){
  MetallButtonClick();
  if(metallClick){
    MetallBlink(2);
  }
  if (Serial1.available())
  {
    String buff = Serial1.readString();
    if (buff == "item_find\r\n"){
      metallClick = 0;
      strip.setPixelColor(2, strip.Color(0, 128, 128));
      strip.show();
    }
    else if (buff == "item_end\r\n"){
      metallClick = 0;
      strip.setPixelColor(2, strip.Color(0, 0, 0));
      strip.show();
      state++;
    }
     else if (buff == "light_on\r\n"){
        digitalWrite(owlLed,HIGH);
    }
    else if (buff == "light_off\r\n"){
        digitalWrite(owlLed,LOW);
    }
    else{
        HandleMessagges(buff);
      }
  }
}


void StartTrollGame(){
  if (Serial1.available())
  {
    String buff = Serial1.readString();
    if (buff == "start_troll\r\n"){
      state++;
    }
    else if (buff == "light_on\r\n"){
        digitalWrite(owlLed,HIGH);
    }
    else if (buff == "light_off\r\n"){
        digitalWrite(owlLed,LOW);
    }
    else{
        HandleMessagges(buff);
      }
  }
}

void TrollGame(){
  digitalWrite(trollLed, HIGH);
  
  switch (trollSequence) {
      case 0:
        _Troll_1();
        break;
      case 1:
        _Troll_2();
        break;
      case 2:
        _Troll_3();
        break;
      case 3:
        _Troll_4();
        break;
    }
    if (Serial1.available())
    {
      String buff = Serial1.readString();
      if (buff == "light_on\r\n"){
          digitalWrite(owlLed,HIGH);
      }
      else if (buff == "light_off\r\n"){
          digitalWrite(owlLed,LOW);
      }
      else if (buff == "cave_search1\r\n"){
         trollSequence=1;
         isTrollFixed = 0;
      }
      else if (buff == "cave_search2\r\n"){
          trollSequence=2;
          isTrollFixed = 0;
      }
      else if (buff == "cave_search3\r\n"){
          trollSequence=3;
          isTrollFixed = 0;
      }
      else if (buff == "troll\r\n"){
          strip.clear();
          strip.show();
          Serial1.println("cave_end");
          strip.setPixelColor(2, strip.Color(0, 0, 255));
          strip.show();
          state++;
      }
      else{
        HandleMessagges(buff);
      }
    }
}


void OpenDoor(){
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
      case 4:
        _Button_5();
        break;
    }
    butt1.tick();
    butt2.tick();
    butt3.tick();
    butt4.tick();
    if (Serial1.available())
    {
      String buff = Serial1.readString();
      if (buff == "light_on\r\n"){
          digitalWrite(owlLed,HIGH);
      }
      else if (buff == "light_off\r\n"){
          digitalWrite(owlLed,LOW);
      }
      else if (buff == "mine\r\n"){
          OpenLock(SHERIF_EM1);
          digitalWrite(trollLed, HIGH);
          Serial1.println("door_cave");
          state++;
      }
      else{
        HandleMessagges(buff);
      }
    }
}

void _Troll_1() {
  butt7.tick();
  if (butt7.isPress() && !isTrollFixed) {
    strip.clear();
    strip.setPixelColor(1, strip.Color(0, 0, 255));
    strip.show();
    Serial1.println("aluminium");
    isTrollFixed=1;
  }
}

void _Troll_2() {
  butt5.tick();
  butt7.tick();
  if (butt7.isRelease()){
    strip.clear();
    strip.show();
  }
  if (butt5.isPress() && !isTrollFixed) {
    strip.clear();
    strip.setPixelColor(3, strip.Color(0, 0, 255));
    strip.show();
    Serial1.println("bronze");
    isTrollFixed=1;
  }
}

void _Troll_3() {
  butt8.tick();
   butt5.tick();
  if (butt5.isRelease()){
    strip.clear();
    strip.show();
  }
  if (butt8.isPress() && !isTrollFixed) {
    strip.clear();
    isTrollFixed=1;
    strip.setPixelColor(0, strip.Color(0, 0, 255));
    strip.show();
    Serial1.println("copper");
  }
}

void _Troll_4() {
  butt6.tick();
  butt8.tick();
  if (butt8.isRelease()){
    strip.clear();
    strip.show();
  }
  if (butt6.isPress()) {
    trollSequence++;
    strip.clear();
    strip.show();
    Serial1.println("cave_end");
    strip.setPixelColor(2, strip.Color(0, 0, 255));
    strip.show();
    state++;
  }
}

void MetallBlink(int number){
  if (millis() - lightFlashTimer >= 18)
    {
      if (!upHelp)
      {
        light++;
        if (light == 75)
          upHelp = 1;
      }
      else
      {
        light--;
        if (light == 10)
          upHelp = 0;
      }
      float brightness = light / 75.0; // коэффициент яркости от 0 до 1
      int green = 128 * brightness;      // Красная составляющая
      int blue = 128 * brightness;     // Синяя составляющая
      strip.setPixelColor(number, strip.Color(0, green, blue));
      strip.show();
      lightFlashTimer = millis();
    }
}

//4-2-1-3-2
void _Button_1() {
  if (butt1.isPress()) {
    buttonSequence = 1;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt3.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt2.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt4.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
}
void _Button_2() {
  if (butt4.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt2.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt3.isPress()) {
    buttonSequence = 2;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt1.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
}
void _Button_3() {
  if (butt4.isPress()) {
    buttonSequence = 3;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt2.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt3.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt1.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
}
void _Button_4() {
  if (butt1.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt3.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt2.isPress()) {
    buttonSequence = 4;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt4.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
}
void _Button_5() {
  if (butt3.isPress()) {
    OpenLock(SHERIF_EM1);
    digitalWrite(trollLed, HIGH);
    Serial1.println("door_cave");
    state++;
  }
  if (butt2.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt4.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
  if (butt1.isPress()) {
    buttonSequence = 0;
    Serial1.println("cave_click");
    delay(100);
  }
}
void BasketLesson(){
  boyButton.tick();
  if(boyButton.isPress()){
    Serial1.println("boy_in");
  }
  if(boyButton.isRelease()){
    Serial1.println("boy_out");
  }
  static bool isStart;
  if(isStart){
    bool btnState = digitalRead(BASKET_IR_PIN);
    if(btnState && !basket_ir_read_F){
      state++;
      digitalWrite(basketLed, LOW);
      delay(1000);
      Serial1.println("lesson_basket_done");
      basket_ir_read_F = 1;
      _startBasket = 0;
    }
    if (!btnState && basket_ir_read_F) {  // обработчик отпускания
      delay(IR_TIMEOUT);
      basket_ir_read_F = 0;
    }
  }
  
  if (Serial1.available())
  {
    String buff = Serial1.readString();
    if (buff == "start_basket\r\n"){
      digitalWrite(basketLed, HIGH);
      isStart = 1;
    }
    else if (buff == "win\r\n"){
      SCORE_MAN=3;
      OUTPUT_TO_DISPLAY();
      delay(2000);
      PRINT_SCORE_ROBOT();
      delay(1000);
      PRINT_SCORE_MAN();
      Serial1.println("fr8nmr");
      state++;
    }
    else{
        HandleMessagges(buff);
      }
  }
}

void Basket(){
  boyButton.tick();
  if (SCORE_MAN == 3) {
      delay(1000);
      Serial1.println("fr8nmr");
      delay(1000);
      Serial1.println("fr8nmr");
      isLoose=0;
      state++;
    }
    if (SCORE_ROBOT == 3) { // Проиграли роботу
      delay(1000);
      Serial1.println("fr9nmr");
      delay(1000);
      OpenLock(Solenoid);
      SCORE_ROBOT = 0;
      isLoose = 1;
      //state++;
    }
  if(boyButton.isPress()){
    OUTPUT_TO_DISPLAY();
    if(isLoose){
        SCORE_ROBOT = 0;
        SCORE_MAN = 0;
        isLoose=0;
    }
    if(_startBasket){
      basketTimer = millis();
      digitalWrite(basketLed, HIGH);
    }
    else{
      Serial1.println("boy_in");
    }
  }

  if(boyButton.isRelease()){
    Serial1.println("boy_out");
  }


  if(boyButton.isHold()){
    OUTPUT_TO_DISPLAY();
    if (Serial1.available())
    {
      String buff = Serial1.readString();
      if (buff == "start_basket\r\n"){
        basketTimer = millis();
        digitalWrite(basketLed, HIGH);
        _startBasket = 1;
    }
    else if (buff == "start_basket_robot\r\n"){
      SCORE_ROBOT++;
      OUTPUT_TO_DISPLAY();
      delay(2000);
      PRINT_SCORE_ROBOT();
      delay(50);
    }
    else if (buff == "win\r\n"){
      SCORE_MAN=3;
      OUTPUT_TO_DISPLAY();
      delay(2000);
      PRINT_SCORE_ROBOT();
      delay(1000);
      PRINT_SCORE_MAN();
      delay(1000);
      Serial1.println("fr8nmr");
      isLoose = 0;
      state++;
    }
    else
    {
      HandleMessagges(buff);
    }
  }

  if(_startBasket){
      if(millis()-basketTimer <= 15000){
        bool btnState = digitalRead(BASKET_IR_PIN);
        if(btnState && !basket_ir_read_F){
          SCORE_MAN++;
          
          OUTPUT_TO_DISPLAY(); // Вывод значений на дисплей
          digitalWrite(basketLed, LOW);
          delay(1000);
          PRINT_SCORE_MAN(); // Вывод голов в Serial
          basket_ir_read_F = 1;
          _startBasket = 0;
        }
        if (!btnState && basket_ir_read_F) {  // обработчик отпускания
          delay(IR_TIMEOUT);
          basket_ir_read_F = 0;
        }
    }
    else{
      _startBasket = 0;
      digitalWrite(basketLed, LOW);
      delay(1000);
      Serial1.println("start_snitch");
      _startBasket = 0;
    }
  }

    if (SCORE_MAN == 3) {
      delay(1000);
      Serial1.println("fr8nmr");
      delay(1000);
      Serial1.println("fr8nmr");
      isLoose=0;
      state++;
    }
    if (SCORE_ROBOT == 3) { // Проиграли роботу
      delay(1000);
      Serial1.println("fr9nmr");
      delay(1000);
      OpenLock(Solenoid);
      SCORE_ROBOT = 0;
      isLoose = 1;
      //state++;
    }
  }
  else{
    for (int i = 0; i <= 3; i++) {
      disp.displayByte(i, 0x00);
    }
    disp.point(0);
    
  }

}

void PRINT_SCORE_MAN() { // Вывод голов в Serial
  switch (SCORE_MAN) {
    case 1:
      Serial1.println("fr61nmr");
      break;
    case 2:
      Serial1.println("fr62nmr");
      break;
  }
  delay(50);
}

void PRINT_SCORE_ROBOT() {
  switch (SCORE_ROBOT) {
    case 1:
      Serial1.println("fr71nmr");
      break;
    case 2:
      Serial1.println("fr72nmr");
      break;
  }
  delay(50);
}

void OUTPUT_TO_DISPLAY() { // Вывод на дисплей счета
  disp.point(1);
  if (SCORE_MAN < 10) {
    disp.display(2, 0);
    disp.display(3, SCORE_MAN);
  }
  else if (SCORE_MAN == 10) {
    disp.display(2, 1);
    disp.display(3, 0);
  }
  if (SCORE_ROBOT < 10) {
    disp.display(0, 0);
    disp.display(1, SCORE_ROBOT);
  }
  else if (SCORE_ROBOT == 10) {
    disp.display(0, 1);
    disp.display(1, 0);
  }
}

void OpenBasket(){
  if (Serial1.available())
  {
    String buff = Serial1.readString();
    if (buff == "opent_basket\r\n"){
      OpenLock(SHERIF_EM2);
    }
    else if (buff == "start_lesson\r\n"){
      state++;
    }
    else{
        HandleMessagges(buff);
      }
  }
}

void OpenLock(byte num) {
  digitalWrite(num, HIGH);
  delay(50);
  digitalWrite(num, LOW);
}

void DoorDefender()
{
  // if (level >= 4 && level <= 21 ) {
  if (millis() - doorDef >= 3000)
  {
    if (doorFlags)
    {
      if (state >= 3 && state <= 7)
      {
        digitalWrite(SHERIF_EM1, HIGH);
      }
      if (state >= 6 && state <= 7)
        {
          digitalWrite(SHERIF_EM2, HIGH);
        }
      doorTimer = millis();
      doorFlags = 0;
    }
    if (millis() - doorTimer >= 50)
    {
      if (!doorFlags)
      {
        if (state >= 3 && state <= 7)
        {
          digitalWrite(SHERIF_EM1, LOW);
        }
        if (state >= 6 && state <= 7)
        {
          digitalWrite(SHERIF_EM2, LOW);
        }
        doorDef = millis();
        doorFlags = 1;
      }
    }
  }
}

void HelpButton(){
  helpButton.tick();
  if (helpButton.isPress()) {
    Serial1.println("help");
  }
}

void MetallButtonClick(){
  metallButton.tick();
  if (metallButton.isPress()) {
    Serial1.println("metal");
    metallClick = 1;
  }
}



