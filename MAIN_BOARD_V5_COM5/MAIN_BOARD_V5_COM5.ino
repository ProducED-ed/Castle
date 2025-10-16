
//-------------------------- библиотеки
#define crimeDoorPin 38     // геркон на дверь в тюрьме первая комната
#define startDoorPin 41     // геркон открытия первой комнаты
#define voltagePin A1       // аналогвый пин напруга
#define goldMaterialPin 47  // проставить пин золота
//#define clockPin3 45
#define directorPin 43
#define goblinPin A15
#define dragonPin 42
#define studentPin 38
#define professorPin 44
#define witchPin 45
#define galetSwitchesPin 40


const int pinA = A14;    // Адресный вход A (LSB)
const int pinB = 53;     // Адресный вход B
const int pinC = 52;     // Адресный вход C (MSB)
const int board1 = 21;   // Выход Y
const int board2 = 20;   // Выход Y
const int board3 = A12;  // Выход Y
const int board4 = 33;   // Выход Y

#define GoldStripCount 1
#define GoldStripPin 50  // in the last version it was 48
#define CandleStripIndicCount 1
#define CandleStripIndicPin 51
#define CauldronStripIndicCount 9
#define CauldronStripIndicPin 48  // in the last version it was 49
#define CauldronRoomStripIndicCount 12
#define CauldronRoomStripIndicPin 12
#define memory_LedIndicCount 41
#define memory_LedIndicPin 11  // Поменял пины местами с final_LedIndicPin

#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include "GyverButton.h"
#include <Servo.h>
//----------------------
//---------------экзэмпляры классов
//-----------коммуникации
SoftwareSerial mySerial(10, 31);  // rx tx
//---------------------
//------------------------ленты адрески
Adafruit_NeoPixel GoldStrip = Adafruit_NeoPixel(GoldStripCount, GoldStripPin, NEO_GRB + NEO_KHZ800);                                    /// адреска в первой комнате
Adafruit_NeoPixel CandleStrip = Adafruit_NeoPixel(CandleStripIndicCount, CandleStripIndicPin, NEO_GRB + NEO_KHZ800);                    ////адреска на свече
Adafruit_NeoPixel CauldronStrip = Adafruit_NeoPixel(CauldronStripIndicCount, CauldronStripIndicPin, NEO_GRB + NEO_KHZ800);              ////адреска в котле
Adafruit_NeoPixel CauldronRoomStrip = Adafruit_NeoPixel(CauldronRoomStripIndicCount, CauldronRoomStripIndicPin, NEO_GRB + NEO_KHZ800);  /// потолочное освещение в комнате зельеварения
Adafruit_NeoPixel memory_Led = Adafruit_NeoPixel(memory_LedIndicCount, memory_LedIndicPin, NEO_GRB + NEO_KHZ800);                       /// финалочка адреска на башне на отдельной нане +20 пин

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(152, 8, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(101, 2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel* strips[] = {
  &GoldStrip,
  &CandleStrip,
  &CauldronStrip,
  &CauldronRoomStrip,
  &memory_Led,
  &strip1,
  &strip2
};
const int STRIPS = sizeof(strips) / sizeof(strips[0]);

// палитра цветов
uint32_t palette[] = {
  Adafruit_NeoPixel::Color(255, 180, 20),   // золотой
  Adafruit_NeoPixel::Color(180, 30, 255),   // фиолетовый
  Adafruit_NeoPixel::Color(20, 100, 255),   // синий
  Adafruit_NeoPixel::Color(255, 255, 255),  // белый (искра)
  Adafruit_NeoPixel::Color(255, 0, 0),      // красный (искра)
  Adafruit_NeoPixel::Color(0, 255, 0),      // синий (искра)
  Adafruit_NeoPixel::Color(0, 0, 255)       // зеленный (искра)

};
const int COLORS = sizeof(palette) / sizeof(palette[0]);


//--------------------------
Servo boyServo;

//------------------------------
//------------герконы кнопки и входы
GButton startDoor(startDoorPin);  // геркон на стартовой двери
GButton clock1Button(1);
GButton clock2Button(1);
GButton goldButton(goldMaterialPin);
//GButton clock3Button(clockPin3);
GButton directorButton(directorPin);
GButton goblinButton(goblinPin);
GButton galetSwitches(galetSwitchesPin);  // галетники в первой комнате
GButton dragonButton(dragonPin);
GButton studentButton(studentPin);
GButton professorButton(professorPin);
GButton witchButton(witchPin);

const byte KnockSens = A0;    // пьезодатчик аналоговый
const byte WindowSens = A13;  // датчик пересечения на окне

const byte firstCrystal = 27;  // геркон первого кристала
const byte secondCrystal = 24;
const byte thirdCrystal = 23;
const byte fourCrystal = 25;

//-------------------
//----------------RFID
OneWire myRFID(13);  // рфидка на котле в комнате зельеварения

byte addr[8];

// воспоминания
unsigned long _presentTimer;
byte _stages = 0;
bool _present = 0;
byte lightBr = 0;
byte iterator = 0;
bool symbolFade = 1;
bool flagSalut = 1;
byte _levels = 0;
byte _stones = 0;
int symbolBrightness = 0;
unsigned long symbolFadeTimer = 0;
unsigned long fadeWhiteTimer = 0;
unsigned long MemoryCheckInterval;
unsigned long MemoryItem1Interval;
unsigned long MemoryItem2Interval;
unsigned long MemoryItem3Interval;
unsigned long MemoryItem4Interval;
unsigned long MemoryItemsInterval;
unsigned long voltageTimer;
unsigned long lightFlashTimer;
unsigned long rainbowCyclesPreviousMillis = 0;
unsigned long pixelsInterval = 10;
unsigned long stepsTimer;
unsigned long libraryOpenTimer;
unsigned long helpsBankTimer;
unsigned long rfidCooldownEnd = 0;
int rainbowCycles = 0;
int rainbowCycleCycles = 0;
int repeatCount = 0;

int Timings[] = { 3000, 2500, 2000, 1500 };
int TimingsFade[] = { 5, 4, 3, 2 };
int CountsFade[] = { 1, 3, 5, 15 };
int CountsFallen[] = { 1, 2, 5, 10 };

//----------------------------------
// флажки состояний
bool tumbStateFlag;  // флажок для тумблера что бы знать состояние до уровня
bool FirstFlag = 0;
bool SecondFlag = 0;
bool ThirdFlag = 0;
bool FourFlag = 0;
bool KnockState = 0;
bool upHelp;
byte light;
bool FirstBottleTrue = 1;
bool FirstBottleFalse = 1;
bool SecondBottleTrue = 1;
bool SecondBottleFalse = 1;
bool ThirdBottleTrue = 1;
bool ThirdBottleFalse = 1;
bool FourBottleTrue = 1;
bool FourBottleFalse = 1;
bool helpFlag;
bool flagSound = 1;
bool _trollDoor = 0;

///////////////////таймеры туть
unsigned long FireInterval = 0;
unsigned long CauldronFireInterval = 0;
unsigned long CauldronFireMistakeInterval = 0;
unsigned long CauldronFireTrueInterval = 0;
unsigned long KnockInterval = 0;
unsigned long KnockIntervalLow = 0;
unsigned long Bottle1Timer = 0;
unsigned long Bottle2Timer = 0;
unsigned long Bottle3Timer = 0;
unsigned long Bottle4Timer = 0;
unsigned long sealSpaceTimer;
unsigned long doorDef;
unsigned long doorTimer;
unsigned long finalPresentation;
unsigned long safeTimer;
unsigned long crimeHelpTimer;
//////////////////
const int threshold = 300;  // пороговое значение для пьезодатчика изменить в зависимости от вибраций
int scrollNumber = 0;       // последовательность свитков
////////////
byte level = 0;  // уровни для квеста установить какой нужен в случае проверки
byte Bottle = 0;
String BotScore = "";
byte OwlScore = 0;
byte NumKnock = 0;
bool doorFlags = 1;
bool flagStory = 1;
//////двери - шерифы
const byte MansardDoor = A6;      // шериф мансарды
const byte PotionsRoomDoor = A3;  // шериф команты зельеварения
const byte LibraryDoor = A11;     // шериф библиотеки
const byte BankDoor = A4;         // шериф банка
const byte HightTowerDoor = A9;   // шериф последней башни
const byte HightTowerDoor2 = 30;  // шериф последней башни
const byte MemoryRoomDoor = A2;   // шериф комнаты воспоминаний
const byte CrimeDoor = 34;        // шериф тюрьмы
const byte BankStashDoor = A10;   // шериф тайника в банке

const byte doors[] = {
  MansardDoor,
  PotionsRoomDoor,
  LibraryDoor,
  BankDoor,
  HightTowerDoor,
  HightTowerDoor2,
  MemoryRoomDoor,
  CrimeDoor,
  BankStashDoor
};
const int DOORS = sizeof(doors) / sizeof(doors[0]);

// состояния
unsigned long lastOpen[DOORS];
bool active[DOORS];

// тайники - шерифы
const byte LibraryLight = A7;  // свет библиотеки


// соленоид
const byte KnockSol = A8;  ////соленоид тук тука
// свет обычные ленты
const byte HallLight = 37;          /// свет 1 комнаты
const byte UfHallLight = A5;        /// свет 1 комнаты уф
const byte MansardLight = 39;       /// свет внутри мансарды лента
const byte LastTowerTopLight = 36;  ////свет наверху led башни круглой(последней)
const byte Fireworks = 3;           // это огоньки фейрверка 3 пин
const byte BankRoomLight = 4;       ////бра в банке
const byte TorchLight = 7;          // факела в последней башне
// светодиодные шары
///////////////////////////keys RFID
unsigned int validHex[][8] = {
  { 0x1, 0xC9, 0x0, 0x0, 0x0, 0x0, 0x0, 0x34 },  // 1
  {
    0x1, 0xCA, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6D },  // 2
  {
    0x1, 0xCB, 0x0, 0x0, 0x0, 0x0, 0x0, 0x5A },
  { 0x1, 0xCC, 0x0, 0x0, 0x0, 0x0, 0x0, 0xDF }
};

// фильтр
int val[3];
int val_filter;
byte index;
String string = "";

unsigned long previousMillis = 0;
int interval = 10;                         // Интервал обновления (мс)
unsigned long directionChangeTime = 3000;  // Время до смены направления (мс)
unsigned long lastDirectionChange = 0;
unsigned long enemyTimer = 0;
unsigned long additionalTimer = 0;
unsigned long cometTimer1 = 0;
unsigned long dragonTimer;
int direction = 1;  // 1 - вперед, -1 - назад
int headPosition = 0;
// Цвет кометы (золотой)
uint32_t cometColor = strip1.Color(255, 215, 0);   // RGB для золотого цвета
uint32_t backgroundColor = strip1.Color(0, 0, 0);  // Черный фон
bool enemyFlag;

// Параметры кометы
int cometLength = 5;               // Длина кометы в диодах
int cometPosition = -cometLength;  // Начальная позиция кометы
int totalLEDs = 252;               // Общее количество светодиодов

bool clickFlag1;
bool clickFlag2;
bool clickFlag3;
bool clickFlag4;
bool clickFlag11;
bool clickFlag22;
bool clickFlag31;
bool clickFlag41;
bool snitchFlag = 1;
bool stepsFlag;
bool centralTowerGameFlag;
bool fireFlag;

unsigned long code1Timer;
unsigned long code2Timer;
unsigned long code3Timer;
unsigned long code4Timer;
unsigned long libraryDoorTimer;
bool Scroll1On;
bool Scroll2On;
bool Scroll3On;
bool Scroll4On;
bool Scroll11On;
bool Scroll21On;
bool Scroll31On;
bool Scroll41On;
bool dragonFlag;

bool isPotionEnd;
bool isDogEnd;
bool isOwlEnd;
bool isTrainEnd;
bool isTrollEnd;
bool isTrainBasket;
int ghostState = 0;

String dragonHints[] = { "dragon_crystal", "hint_2_b", "hint_2_c", "hint_2_z", "hint_5_b", "hint_5_c" };
String studentHints[] = { "hint_3_b", "hint_3_c", "hint_3_z", "hint_37_b", "hint_37_c", "hint_44_b", "hint_44_c", "hint_56_b" };
String professorHints[] = { "hint_6_c", "hint_6_b", "hint_10_b", "hint_10_c", "hint_11_c", "hint_11_b", "hint_11_z" };
String dwarfHints[] = { "hint_14_c", "hint_14_b", "hint_14_z" };
String witchHints[] = { "hint_17_c", "hint_17_b", "hint_17_z" };
String knightHints[] = { "hint_19_c", "hint_19_b", "hint_19_z" };
String goblinHints[] = { "hint_23_c", "hint_23_b", "hint_23_z" };
String trollHints[] = { "hint_26_c", "hint_26_b", "hint_26_z" };
String workshopHints[] = { "hint_32_b", "hint_32_c", "hint_32_d", "hint_32_e", "hint_32_z" };
String directorHints[] = { "hint_49_b", "hint_49_c", "hint_50_b", "hint_50_c", "hint_51_b", "hint_51_c" };

int dragonCounter = 0;
int studentCounter = 0;
int professorCounter = 0;
int dwarfCounter = 0;
int knightCounter = 0;
int trollCounter = 0;
int workshopCounter = 0;
int directorCounter = 0;
int goblinCounter = 0;
int witchCounter = 0;

unsigned long KayTimer = 0;

void MagicEffect() {
  digitalWrite(Fireworks, HIGH);
  static uint32_t lastUpdate = 0;
  static uint16_t timeCounter = 0;
  
  unsigned long now = millis();
  if (now - lastUpdate < 15) return;  // скорость анимации
  lastUpdate = now;
  
  timeCounter++;

  for (int s = 0; s < STRIPS; s++) {
    Adafruit_NeoPixel* strip = strips[s];
    int n = strip->numPixels();

    // Затухание предыдущих кадров для создания шлейфа
    for (int i = 0; i < n; i++) {
      uint32_t currentColor = strip->getPixelColor(i);
      uint8_t r = (currentColor >> 16) & 0xFF;
      uint8_t g = (currentColor >> 8) & 0xFF;
      uint8_t b = currentColor & 0xFF;
      
      // Плавное затухание
      r = r * 0.7;
      g = g * 0.7;
      b = b * 0.7;
      
      strip->setPixelColor(i, r, g, b);
    }

    // Создание новых "вспышек" фейерверка
    for (int i = 0; i < n; i++) {
      // Вероятность появления новой искры
      if (random(1000) < 8) {
        // Выбор случайного цвета из палитры
        int colorIndex = random(COLORS - 1);
        uint32_t baseColor = palette[colorIndex];
        
        uint8_t baseR = (baseColor >> 16) & 0xFF;
        uint8_t baseG = (baseColor >> 8) & 0xFF;
        uint8_t baseB = baseColor & 0xFF;
        
        // Случайная интенсивность для разнообразия
        float intensity = 0.7 + (random(50) / 100.0);
        
        uint8_t r = min(255, (int)(baseR * intensity));
        uint8_t g = min(255, (int)(baseG * intensity));
        uint8_t b = min(255, (int)(baseB * intensity));
        
        // Создание небольшого кластера искр
        int clusterSize = random(2, 5);
        for (int j = max(0, i - clusterSize/2); j <= min(n-1, i + clusterSize/2); j++) {
          float distance = abs(j - i) / (float)clusterSize;
          float fade = 1.0 - distance;
          
          uint8_t clusterR = r * fade;
          uint8_t clusterG = g * fade;
          uint8_t clusterB = b * fade;
          
          // Смешивание с существующим цветом
          uint32_t existingColor = strip->getPixelColor(j);
          uint8_t existingR = (existingColor >> 16) & 0xFF;
          uint8_t existingG = (existingColor >> 8) & 0xFF;
          uint8_t existingB = existingColor & 0xFF;
          
          strip->setPixelColor(j, 
            max(clusterR, existingR),
            max(clusterG, existingG),
            max(clusterB, existingB)
          );
        }
      }
    }

    // Добавление ярких белых искр-вспышек
    if (random(1000) < 15) {
      int sparkPos = random(n);
      byte intensity = random(200, 255);
      
      // Создание небольшой белой вспышки
      for (int i = max(0, sparkPos - 1); i <= min(n-1, sparkPos + 1); i++) {
        float distance = abs(i - sparkPos) / 2.0;
        byte sparkIntensity = intensity * (1.0 - distance);
        strip->setPixelColor(i, sparkIntensity, sparkIntensity, sparkIntensity);
      }
    }

    strip->show();
  }
}

///////////////////////////////48 свободный
void setup() {
  pinMode(firstCrystal, INPUT_PULLUP);
  pinMode(secondCrystal, INPUT_PULLUP);
  pinMode(thirdCrystal, INPUT_PULLUP);
  pinMode(fourCrystal, INPUT_PULLUP);

  pinMode(WindowSens, INPUT);

  pinMode(MansardDoor, OUTPUT);
  pinMode(PotionsRoomDoor, OUTPUT);
  pinMode(LibraryDoor, OUTPUT);
  pinMode(BankDoor, OUTPUT);
  pinMode(HightTowerDoor, OUTPUT);
  pinMode(MemoryRoomDoor, OUTPUT);

  pinMode(LibraryLight, OUTPUT);
  pinMode(BankStashDoor, OUTPUT);
  pinMode(KnockSol, OUTPUT);
  pinMode(HallLight, OUTPUT);
  pinMode(UfHallLight, OUTPUT);
  pinMode(CrimeDoor, OUTPUT);

  pinMode(MansardLight, OUTPUT);
  pinMode(LastTowerTopLight, OUTPUT);
  pinMode(Fireworks, OUTPUT);
  pinMode(BankRoomLight, OUTPUT);
  pinMode(TorchLight, OUTPUT);
  pinMode(HightTowerDoor2, OUTPUT);


  digitalWrite(MansardLight, LOW);
  digitalWrite(LastTowerTopLight, LOW);
  digitalWrite(Fireworks, LOW);
  digitalWrite(LibraryLight, LOW);
  digitalWrite(BankRoomLight, LOW);
  digitalWrite(HallLight, LOW);
  digitalWrite(UfHallLight, LOW);
  digitalWrite(HightTowerDoor2, LOW);
  digitalWrite(TorchLight, LOW);
  ///--------------------
  ///////порты uart
  Serial.begin(9600);  // raspberry
  Serial.setTimeout(10);
  Serial1.begin(9600);  // front left
  Serial1.setTimeout(10);
  Serial2.begin(9600);  // front right
  Serial2.setTimeout(10);
  Serial3.begin(9600);  // rear right
  Serial3.setTimeout(10);
  mySerial.begin(9600);  // rear left
  mySerial.setTimeout(10);

  strip1.begin();
  strip1.setBrightness(255);
  strip1.show();

  strip2.begin();
  strip2.setBrightness(255);
  strip2.show();

  //// инициализация лент
  GoldStrip.begin();
  GoldStrip.setBrightness(255);
  CandleStrip.begin();
  CandleStrip.setBrightness(255);
  CauldronStrip.begin();
  CauldronStrip.setBrightness(255);
  memory_Led.begin();
  memory_Led.setBrightness(255);
  CauldronRoomStrip.begin();
  CauldronRoomStrip.setBrightness(255);
  for (int i = 0; i <= 50; i++) {
    GoldStrip.setPixelColor(i, GoldStrip.Color(0, 0, 0));
    CandleStrip.setPixelColor(i, CandleStrip.Color(0, 0, 0));
    CauldronStrip.setPixelColor(i, CauldronStrip.Color(0, 0, 0));
    CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 0, 0));
    memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
  }
  GoldStrip.show();
  CandleStrip.show();
  CauldronStrip.show();
  CauldronRoomStrip.show();
  memory_Led.show();
  ////

  /// инициализация входов на библе гувера для цифровых входов
  startDoor.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  startDoor.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  startDoor.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  startDoor.setType(HIGH_PULL);
  startDoor.setDirection(NORM_OPEN);
  ///////////------
  goldButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  goldButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  goldButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  goldButton.setType(HIGH_PULL);
  goldButton.setDirection(NORM_OPEN);
  ///////////------

  goblinButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  goblinButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  goblinButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  goblinButton.setType(HIGH_PULL);
  goblinButton.setDirection(NORM_OPEN);

  clock1Button.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  clock1Button.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  clock1Button.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  clock1Button.setType(HIGH_PULL);
  clock1Button.setDirection(NORM_OPEN);

  clock2Button.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  clock2Button.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  clock2Button.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  clock2Button.setType(HIGH_PULL);
  clock2Button.setDirection(NORM_OPEN);

  //clock3Button.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  //clock3Button.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  //clock3Button.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  //clock3Button.setType(HIGH_PULL);
  //clock3Button.setDirection(NORM_OPEN);

  directorButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  directorButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  directorButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  directorButton.setType(HIGH_PULL);
  directorButton.setDirection(NORM_OPEN);

  dragonButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  dragonButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  dragonButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  dragonButton.setType(HIGH_PULL);
  dragonButton.setDirection(NORM_OPEN);

  studentButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  studentButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  studentButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  studentButton.setType(HIGH_PULL);
  studentButton.setDirection(NORM_OPEN);

  professorButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  professorButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  professorButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  professorButton.setType(HIGH_PULL);
  professorButton.setDirection(NORM_OPEN);

  witchButton.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  witchButton.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  witchButton.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  witchButton.setType(HIGH_PULL);
  witchButton.setDirection(NORM_OPEN);

  galetSwitches.setDebounce(50);       // настройка антидребезга (по умолчанию 80 мс)
  galetSwitches.setTimeout(300);       // настройка таймаута на удержание (по умолчанию 500 мс)
  galetSwitches.setClickTimeout(600);  // настройка таймаута между кликами (по умолчанию 300 мс)
  galetSwitches.setType(HIGH_PULL);
  galetSwitches.setDirection(NORM_OPEN);


  ////----------
  int Voltage = analogRead(voltagePin);
  // Serial.println(Voltage);
  float volt = 0.0048 * Voltage * 3;

  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  pinMode(pinC, OUTPUT);
  pinMode(board1, INPUT);
  pinMode(board2, INPUT);
  pinMode(board3, INPUT);
  pinMode(board4, INPUT);

  boyServo.attach(49);
  boyServo.write(0);
  
}
void loop() {

  handleLocks();
  //digitalWrite(pinA, 0);
  //digitalWrite(pinB, 0);
  //digitalWrite(pinC, 1);
  //bool reading = !digitalRead(board2);
  //Serial.println(!digitalReadExpander(2, board2));
  // int Voltage = analogRead(voltagePin);
  // float volt =0.0048*Voltage*3;
  // основной цикл программы все уровни на всех башня должны здесь присутствовать
  switch (level) {
    case 0:
      PowerOn();
      break;
    case 1:
      StartDoor();
      break;
    case 2:
      ClockGame();
      break;
    case 3:
      Clock2Game();
      break;
    case 4:
      GaletGame();
      break;
    case 5:
      ThreeGame();
      break;
    case 6:
      Flags();
      break;
    case 7:
      MapGame();
      break;
    case 8:
      OpenBank();
      break;
    case 9:
      Scrolls();
      break;
    case 10:
      Oven();
      break;
    case 11:
      Library();
      break;
    case 12:
      LibraryGame();
      break;
    case 13:
      CentralTowerGame();
      break;
    case 14:
      CentralTowerGameDown();
      break;
    case 15:
      SealSpace();
      break;
    case 16:
      MemoryRoom();
      break;
    case 17:
      CrimeHelp();
      break;
    case 18:
      BasketLesson();
      break;
    case 19:
      Basket();
      break;
    case 20:
      FinalPresentation();
      break;
    case 21:
      if (Serial.available()) {
        String buff = Serial.readString();
        if (buff == "restart") {
          OpenAll();
          RestOn();
        }
      }
      break;
    case 22:
      break;
    case 23:
      break;
    case 25:
      RestOn();
      break;
    case 30:
      Restart();
      break;
  }
  //DoorDefender();
  if (fireFlag) {
    FireCup();
  }

  if (level > 0 && level < 18) {
    HelpHandler("");
  }

  //VoltageDisplay();
}
// метод открывания тайников и дверей каждая после своего уровня и до конца игры

void PowerOn() {
  // опрашиваем дверь пещеры троллей без нее не запустим
  static bool requestFlag = 0;
  if (!requestFlag) {
    delay(2000);
    Serial2.println("door_request");
    requestFlag = 1;
  }
  if (Serial.available()) {  // есть что на вход?
    String buff = Serial.readString();
    // Serial.println(buff);
    if (buff == "start") {
      ////////рассылка всем башня
      //if (_trollDoor && !digitalRead(startDoorPin))
      //{
      boyServo.attach(49);
      boyServo.write(0);
      Serial.println("modalend");
      delay(100);
      mySerial.println("start");
      Serial1.println("start");
      delay(1000);
      Serial1.println("start");
      Serial2.println("start");
      delay(1000);
      Serial2.println("start");
      Serial3.println("start");
      delay(1000);
      Serial3.println("start");
      mySerial.println("start");
      delay(1000);
      mySerial.println("start");
      Serial.println("startgo");
      requestFlag = 0;
      dragonCounter = 0;
      studentCounter = 0;
      professorCounter = 0;
      dwarfCounter = 0;
      knightCounter = 0;
      trollCounter = 0;
      workshopCounter = 0;
      directorCounter = 0;
      goblinCounter = 0;
      witchCounter = 0;
      dragonTimer = millis();
      level++;
      //}
      //else
      //Serial.println("modal");
    }

    // Unlocks(buff);
    if (buff == "restart") {
      OpenAll();
      requestFlag = 0;
      RestOn();
    }

    if (buff == "demoOn") {
      Serial1.println("demo_on");
      Serial2.println("demo_on");
      Serial3.println("demo_on");
      mySerial.println("demo_on");
      delay(1000);
      mySerial.println("demo_on");
      requestFlag = 0;
      level = 30;
    }
    if (buff == "ready") {
      Serial1.println("ready");
      Serial2.println("ready");
      Serial3.println("ready");
      mySerial.println("ready");
    }

  }
  if (Serial2.available()) {  // есть что на вход?
    String buff = Serial2.readString();
    if (buff == "close_mine\r\n") {
      _trollDoor = 1;
    }
    if (buff == "open_mine\r\n") {
      _trollDoor = 0;
    }
  }
}

int levenshteinDistance(String s1, String s2) {
  int len1 = s1.length();
  int len2 = s2.length();
  int matrix[len1 + 1][len2 + 1];
  
  for (int i = 0; i <= len1; i++) matrix[i][0] = i;
  for (int j = 0; j <= len2; j++) matrix[0][j] = j;
  
  for (int i = 1; i <= len1; i++) {
    for (int j = 1; j <= len2; j++) {
      int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
      matrix[i][j] = min(
        matrix[i-1][j] + 1,    // удаление
        min(
          matrix[i][j-1] + 1,  // вставка
          matrix[i-1][j-1] + cost // замена
        )
      );
    }
  }
  return matrix[len1][len2];
}

int calculateSimilarity(String s1, String s2) {
  int distance = levenshteinDistance(s1, s2);
  int maxLen = max(s1.length(), s2.length());
  //if (maxLen == 0) return 100;
  //Serial.println(100 - (distance * 100 / maxLen));
  return 100 - (distance * 100 / maxLen);
}

void HelpTowersHandler() {
  if (Serial1.available()) {
    String buff = Serial1.readString();
    if (buff == "help\r\n") {
      HelpHandler("workshop");
    }
  }
  if (Serial2.available()) {
    String buff = Serial2.readString();
    if (buff == "help\r\n") {
      HelpHandler("troll");
    }
  }
  if (Serial3.available()) {
    String buff = Serial3.readString();
    if (buff == "help\r\n") {
      HelpHandler("knight");
    }
  }
  if (mySerial.available()) {
    String buff = mySerial.readString();
    if (buff == "help\r\n") {
      HelpHandler("dwaf");
    }
  }
}

void HelpHandler(String from) {
  if (flagSound) {
    directorButton.tick();
    goblinButton.tick();
    dragonButton.tick();
    studentButton.tick();
    professorButton.tick();
    witchButton.tick();
    //дракон
    //if (!digitalReadExpander(4, board3)) {
    if (dragonButton.isPress()) {
      if (level == 1 && !dragonFlag) {
        Serial.println(dragonHints[0]);
        dragonFlag = 1;
      }
      if (level == 1 && dragonCounter > 0) {
        Serial.println(dragonHints[dragonCounter]);
      }
      if (level < 4 && level > 1) {
        Serial.println(dragonHints[3]);
      }
      if (level == 4) {
        dragonCounter = (dragonCounter == 4) ? 5 : 4;
        Serial.println(dragonHints[dragonCounter]);
      }
      if (level > 4) {
        Serial.println(dragonHints[3]);
      }
      if (level != 4) {
        dragonCounter++;
        if (dragonCounter > 2)
          dragonCounter = 1;
      }


      flagSound = 0;
    }

    //студент
    //if (!digitalReadExpander(1, board1)) {
    if (studentButton.isPress()) {
      if (level > 1 && level < 4) {
	    studentCounter = (studentCounter == 0) ? 1 : 0; // Правильно: сначала _b (индекс 0), потом _c (индекс 1)
	    Serial.println(studentHints[studentCounter]);
	  }
      if (level > 3 && level < 11) {
        Serial.println(studentHints[2]);
      }

      if (level == 11) {
        studentCounter = (studentCounter == 3) ? 4 : 3;
        Serial.println(studentHints[studentCounter]);
      }

      if (level > 11 && level < 17) {
        studentCounter = (studentCounter == 5) ? 6 : 5; // Правильно: сначала _b (индекс 5), потом _c (индекс 6)
        Serial.println(studentHints[studentCounter]);
      }

      if (level == 17) {
        Serial.println(studentHints[7]);
      }
      flagSound = 0;
    }

    //профессор
    //if (!digitalReadExpander(7, board3)) {
    if (professorButton.isPress()) {
      if (level > 4 && level < 6) {
        professorCounter = (professorCounter == 1) ? 0 : 1;
        Serial.println(professorHints[professorCounter]);
      }
      if (level == 6) {
        professorCounter = (professorCounter == 2) ? 3 : 2; // Правильно: сначала _b (индекс 2), потом _c (индекс 3)
        Serial.println(professorHints[professorCounter]);
      }
      if (level == 7) {
        if (!isPotionEnd || !isDogEnd || !isOwlEnd || !isTrainEnd) {
          professorCounter = (professorCounter == 5) ? 4 : 5;
          Serial.println(professorHints[professorCounter]);
        } else {
          Serial.println(professorHints[6]);
        }
      }
      if (level > 7) {
        // professorCounter = (professorCounter == 5) ? 4 : 5;
        Serial.println(professorHints[6]);
      }
      flagSound = 0;
    }

    //гоблин
    if (goblinButton.isPress()) {
      if (level == 9) {
        goblinCounter = (goblinCounter == 1) ? 0 : 1;
        Serial.println(goblinHints[goblinCounter]);
      }
      if (level > 9 && level < 18) {
        Serial.println(goblinHints[2]);
      }
      flagSound = 0;
    }

    //директор
    if (directorButton.isPress()) {
      if (level == 13) {
        directorCounter = (directorCounter == 1) ? 0 : 1;
        Serial.println(directorHints[directorCounter]);
      }
      if (level == 14) {
        directorCounter = (directorCounter == 2) ? 3 : 2;
        Serial.println(directorHints[directorCounter]);
      }
      if (level > 14 && level < 18) {
        directorCounter = (directorCounter == 4) ? 5 : 4; // Правильно: сначала _b (индекс 4), потом _c (индекс 5)
        Serial.println(directorHints[directorCounter]);
      }
      flagSound = 0;
    }

    //ведьма
    if (witchButton.isPress()) {
      if (level == 7 && !isPotionEnd) {
        witchCounter = (witchCounter == 1) ? 0 : 1;
        Serial.println(witchHints[witchCounter]);
      }
      if (isPotionEnd) {
        Serial.println(witchHints[2]);
      }
      flagSound = 0;
    }

    //гном
    if (from == "dwaf") {
      if (level == 7 && !isOwlEnd) {
        dwarfCounter = (dwarfCounter == 1) ? 0 : 1;
        Serial.println(dwarfHints[dwarfCounter]);
      }
      if (isOwlEnd) {
        Serial.println(dwarfHints[2]);
      }
      flagSound = 0;
    }

    //рыцарь
    if (from == "knight") {
      if (level == 7 && !isDogEnd) {
        knightCounter = (knightCounter == 1) ? 0 : 1;
        Serial.println(knightHints[knightCounter]);
      }
      if (isDogEnd) {
        Serial.println(knightHints[2]);
      }
      flagSound = 0;
    }

    //троль
    if (from == "troll") {
      if (level == 7 && !isTrollEnd) {
        trollCounter = (trollCounter == 1) ? 0 : 1;
        Serial.println(trollHints[trollCounter]);
      }
      if (isTrollEnd) {
        Serial.println(trollHints[2]);
      }
      flagSound = 0;
    }

    //плотник
    if (from == "workshop") {
      if (level == 10) {
        Serial.println(workshopHints[workshopCounter]);
      }
      if (level > 10 && level < 18) {
        Serial.println(workshopHints[4]);
      }

      workshopCounter++;
      if (workshopCounter > 3)
        workshopCounter = 0;

      flagSound = 0;
    }
  }
}

// стартовая дверь на замке с ключом
void StartDoor() {
  startDoor.tick();
  if (startDoor.isRelease()) {
    Serial.println("open_door");
    digitalWrite(HallLight, HIGH);
    delay(5000);
    boyServo.write(130);

    dragonFlag = 0;
    KayTimer = millis();
    level++;
  }

  if (millis() - dragonTimer >= 120000) {
    if (!dragonFlag) {
      Serial.println("dragon_crystal_repeat");
    }
    dragonTimer = millis();
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    //int similarity = stringSimilarity(buff, "owl_end");
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
  }
}
// тумблер в первой комнате реагирует на изменение состояния
void ClockGame() {
  static bool repeat;
  if (millis() - KayTimer >= 30000) {
    if (repeat) {
      Serial.println("kay_repeat");
      repeat = 0;
    }
  }

  digitalWrite(pinA, 0);
  digitalWrite(pinB, 0);
  digitalWrite(pinC, 0);
  bool reading = !digitalRead(board2);
  if (reading) {
    Serial.println("clock1");
    digitalWrite(UfHallLight, HIGH);
    digitalWrite(HallLight, LOW);
    repeat = 0;
    level++;
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "first_clock") {
      Serial.println("clock1");
      digitalWrite(UfHallLight, HIGH);
      digitalWrite(HallLight, LOW);
      level++;
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
    if (buff == "kay_repeat") {
      repeat = 1;
      Serial.println("kay");
      KayTimer = millis();
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
  }
}

/// галетники в первой комнате
void Clock2Game() {
  //clock3Button.tick();
  //if (clock3Button.isPress()) {
  if (!digitalReadExpander(2, board2)) {
    Serial.println("clock2");
    //OpenLock(MansardDoor);
    for (int i = 0; i < 3; i++) {
      digitalWrite(UfHallLight, HIGH);
      delay(500);
      digitalWrite(UfHallLight, LOW);
      delay(500);
    }
    digitalWrite(UfHallLight, LOW);
    stepsTimer = millis();
    level++;
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "second_clock") {
      Serial.println("clock2");
      //OpenLock(MansardDoor);
	  
      // Добавлена логика мигания и выключения УФ-светодиода,
      // аналогичная блоку физического взаимодействия.
      for (int i = 0; i < 3; i++) {
        digitalWrite(UfHallLight, HIGH);
        delay(500);
        digitalWrite(UfHallLight, LOW);
        delay(500);
      }
      digitalWrite(UfHallLight, LOW);
      stepsTimer = millis();
      level++;
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
  }
}

void GaletGame() {
  static bool galet1;
  static bool galet2;
  static bool galet3;
  static bool galet4;
  static bool galet5;
  static bool startSteps;
  static unsigned long stepsTimer;

  if (!startSteps) {
    stepsTimer = millis();
  }
  if (millis() - stepsTimer >= 6000) {
    if (startSteps) {
      Serial.println("steps");
      startSteps = 0;
    }
  }

  galetSwitches.tick();
  if (galetSwitches.isPress()) {
    galet5 = 1;
    Serial.println("galet1");
  }
  if (galetSwitches.isRelease()) {
    galet5 = 0;
    Serial.println("galet1_off");
  }

  if (Serial1.available()) {
    String buf1 = Serial1.readString();
    if (buf1 == "galet_on\r\n") {
      galet1 = 1;
      Serial.println("galet2");
    }
    if (buf1 == "galet_off\r\n") {
      galet1 = 0;
      Serial.println("galet2_off");
    }
  }

  if (Serial2.available()) {
    String buf2 = Serial2.readString();
    if (buf2 == "galet_on\r\n") {
      galet2 = 1;
      Serial.println("galet3");
    }
    if (buf2 == "galet_off\r\n") {
      Serial.println("galet3_off");
      galet2 = 0;
    }
  }

  if (Serial3.available()) {
    String buf4 = Serial3.readString();
    if (buf4 == "galet_on\r\n") {
      galet3 = 1;
      Serial.println("galet4");
    }
    if (buf4 == "galet_off\r\n") {
      Serial.println("galet4_off");
      galet3 = 0;
    }
  }

  if (mySerial.available()) {
    String buf4 = mySerial.readString();
    if (buf4 == "galet_on\r\n") {
      galet4 = 1;
      Serial.println("galet5");
    }
    if (buf4 == "galet_off\r\n") {
      Serial.println("galet5_off");
      galet4 = 0;
    }
  }

  if (galet1 && galet2 && galet3 && galet4 && galet5) {
    delay(200);
    OpenLock(MansardDoor);
    Serial.println("galet_on");
    galet1 = 0;
    galet2 = 0;
    galet3 = 0;
    galet4 = 0;
    galet5 = 0;
    startSteps = 0;
    level++;
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "after_story_clock2") {
      startSteps = 1;
    }
    if (buff == "student_hide") {
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      delay(1000);
      boyServo.detach();
    }
    if (buff == "open_mansard_door") {
      delay(200);
      OpenLock(MansardDoor);
      Serial.println("galet_on");
      galet1 = 0;
      galet2 = 0;
      galet3 = 0;
      galet4 = 0;
      galet5 = 0;
      startSteps = 0;
      level++;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
}

void ThreeGame() {
  static bool wolfFlag;
  static bool safeFlag;
  static bool suitcaseFlag;
  if (suitcaseFlag && safeFlag && wolfFlag) {
    suitcaseFlag = 0;
    safeFlag = 0;
    wolfFlag = 0;
    level++;
    Serial.println("three_game_end");
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "suitcase_end") {
      suitcaseFlag = 1;
    }
    if (buff == "safe_end") {
      safeFlag = 1;
    }
    if (buff == "wolf_end") {
      wolfFlag = 1;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
}

/// обработка флагов чисто на портах
void Flags() {
  // digitalWrite(MansardLight, LOW);
  if (Serial1.available()) {
    String buf1 = Serial1.readString();
    if (buf1 == "flag1_on\r\n") {
      FirstFlag = 1;
      Serial.println("flag1_on");
    }
    if (buf1 == "flag1_off\r\n") {
      FirstFlag = 0;
      Serial.println("flag1_off");
    }
  }

  if (Serial2.available()) {
    String buf2 = Serial2.readString();
    if (buf2 == "flag2_on\r\n") {
      SecondFlag = 1;
      Serial.println("flag2_on");
    }
    if (buf2 == "flag2_off\r\n") {
      SecondFlag = 0;
      Serial.println("flag2_off");
    }
  }

  if (Serial3.available()) {
    String buf4 = Serial3.readString();
    if (buf4 == "flag3_on\r\n") {
      ThirdFlag = 1;
      Serial.println("flag3_on");
    }
    if (buf4 == "flag3_off\r\n") {
      ThirdFlag = 0;
      Serial.println("flag3_off");
    }
  }

  if (mySerial.available()) {
    String buf4 = mySerial.readString();
    if (buf4 == "flag4_on\r\n") {
      FourFlag = 1;
      Serial.println("flag4_on");
    }
    if (buf4 == "flag4_off\r\n") {
      FourFlag = 0;
      Serial.println("flag4_off");
    }
  }

  if (FirstFlag && SecondFlag && ThirdFlag && FourFlag) {
    Serial.println("flagsendmr");
    level++;
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "m2lck") {
      Serial.println("flagsendmr");
      level++;
      //OpenLock(MansardStashDoor);
      // иногда приходится отправлять несколько раз иногда с первого раза не читает обязательно задержка в 1 с не знаю с чем связано
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
  }
}


void MapGame() {
  static String game = "";
  static bool activePotionRoom;
  static bool potionPulsation;


  if (game == "fish") {
    digitalWrite(pinA, 0);
    digitalWrite(pinB, 0);
    digitalWrite(pinC, 0);
    bool reading1 = !digitalRead(21);
    if (reading1) {
      Serial.println("door_witch");
      OpenLock(PotionsRoomDoor);
      activePotionRoom = 1;
      ////дописать свет
      for (int j = 0; j <= 255; j++) {
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 197, 143));
        }
        CauldronRoomStrip.setBrightness(j);
        CauldronRoomStrip.show();
        delay(2);
      }
      game = "";
    }
  }

  if (activePotionRoom && !isPotionEnd) {
    Bottles();
  }
  if (isPotionEnd) {
    digitalWrite(pinA, 0);
    digitalWrite(pinB, 0);
    digitalWrite(pinC, 1);
    bool reading = !digitalRead(21);
    if (reading) {
      if (!potionPulsation) {
        potionPulsation = 1;
        Serial.println("item_find");
        Serial2.println("item_find");
        Serial3.println("item_find");
      }
    }
  }

  if (potionPulsation) {
    PotionPulsation();
  }

  if (isPotionEnd && isDogEnd && isOwlEnd && isTrainEnd && isTrollEnd) {
    activePotionRoom = 0;
    isPotionEnd = 0;
    isDogEnd = 0;
    isOwlEnd = 0;
    isTrainEnd = 0;
    isTrollEnd = 0;
    game = "";
    delay(1000);
    helpsBankTimer = millis();
    Serial.println("material_end");
    delay(1000);
    level++;
  }

  if (mySerial.available()) {
    String buff = mySerial.readStringUntil('\n');
    buff.trim();
    if (calculateSimilarity(buff, "owl_end") >= 80) {
      Serial.println("owl_end");
      Serial1.println("light_off");
      Serial2.println("light_off");
      Serial3.println("light_off");
      isOwlEnd = 1;
    }
    if (calculateSimilarity(buff, "door_owl") >= 80) {
      Serial.println("door_owl");
    }
    if (calculateSimilarity(buff, "owl_flew") >= 80) {
      Serial.println("owl_flew");
    }
    if (calculateSimilarity(buff, "light") >= 80) {
      Serial.println("light_on");
      Serial1.println("light_on");
      Serial2.println("light_on");
      Serial3.println("light_on");
    }
    if (calculateSimilarity(buff, "dark") >= 80) {
      Serial.println("light_off");
      Serial1.println("light_off");
      Serial2.println("light_off");
      Serial3.println("light_off");
    }
    if (calculateSimilarity(buff, "help") >= 80) {
      HelpHandler("dwaf");
    }
  }

  if (Serial3.available()) {
    String buff = Serial3.readStringUntil('\n');
    buff.trim();
    if (calculateSimilarity(buff, "dog_lock") >= 80) {
      Serial.println("dog_lock");
      isDogEnd = 1;
    }
    if (calculateSimilarity(buff, "door_dog") >= 80) {
      Serial.println("door_dog");
    }
    if (calculateSimilarity(buff, "dog_sleep") >= 80) {
      Serial.println("dog_sleep");
    }
    if (calculateSimilarity(buff, "dog_growl") >= 80) {
      Serial.println("dog_growl");
    }
    if (calculateSimilarity(buff, "story_20_a") >= 98) {
      Serial.println("story_20_a");
    }
    if (calculateSimilarity(buff, "story_20_b") >= 98) {
      Serial.println("story_20_b");
    }
    if (calculateSimilarity(buff, "story_22_c") >= 98) {
      Serial.println("story_20_c");
    }
    if (calculateSimilarity(buff, "help") >= 78) {
      HelpHandler("knight");
    }
    if (calculateSimilarity(buff, "crystal") >= 78) {
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      potionPulsation = 0;
      //Serial2.println("item_find");
      Serial.println("item_find");
      Serial2.println("item_find");
    }
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "key") {
      game = "key";
      mySerial.println("out");
      Serial3.println("key");
    }
    if (buff == "train_end") {
      isTrainEnd = 1;
      Serial2.println("start_troll");
      delay(1000);
      Serial2.println("start_troll");
    }
    if (buff == "fish") {
      game = "fish";
      mySerial.println("out");
      Serial3.println("out");
    }
    if (buff == "owl") {
      game = "owl";
      mySerial.println("owl");
      Serial3.println("out");
    }

    if (buff == "cave_search1") {
      Serial2.println("cave_search1");
      delay(1000);
      Serial2.println("cave_search1");
    }
    if (buff == "cave_search2") {
      Serial2.println("cave_search2");
      delay(1000);
      Serial2.println("cave_search2");
    }
    if (buff == "cave_search3") {
      Serial2.println("cave_search3");
      delay(1000);
      Serial2.println("cave_search3");
    }

    if (buff == "pedlock") {
      Serial3.println("skip_padlock");
      Serial.println("door_dog");
    }

    if (buff == "dog") {
      Serial3.println("skip_dog");
      Serial.println("dog_lock");
      isDogEnd = 1;
    }

    if (buff == "owl_door") {
      mySerial.println("owl_door");
      Serial.println("door_owl");
    }

    if (buff == "owl_skip") {
      mySerial.println("skip");
      Serial.println("owl_end");
      isOwlEnd = 1;
    }

    if (buff == "open_potions_stash") {
      Serial.println("four_bottle");
      rainbow();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      for (int i = 255; i >= 50; i--) {
        CauldronRoomStrip.setBrightness(i);
        CauldronRoomStrip.show();
        delay(2);
      }
      isPotionEnd = true;
    }

    if (buff == "cat") {
      Serial.println("door_witch");
      OpenLock(PotionsRoomDoor);
      activePotionRoom = 1;
      ////дописать свет
      for (int j = 0; j <= 255; j++) {
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 197, 143));
        }
        CauldronRoomStrip.setBrightness(j);
        CauldronRoomStrip.show();
        delay(2);
      }
      game = "";
    }

    if (buff == "mine") {
      Serial2.println("mine");
      Serial.println("door_cave");
    }
    if (buff == "troll") {
      Serial2.println("troll");
      Serial.println("cave_end");
      isTrollEnd = 1;
    }

    if (buff == "train") {
      game = "";
      mySerial.println("out");
      Serial3.println("out");
    }

    if (buff == "out") {
      game = "";
      mySerial.println("out");
      Serial3.println("out");
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }

    if (buff == "skin") {
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      potionPulsation = 0;
      Serial2.println("item_find");
      Serial3.println("item_find");
    }
  }

  if (Serial2.available()) {
    String buff = Serial2.readString();
    if (calculateSimilarity(buff, "aluminium\r\n") >= 78) {
      Serial.println("cave_search1");
    }
    if (calculateSimilarity(buff, "bronze\r\n") >= 78) {
      Serial.println("cave_search2");
    }
    if (calculateSimilarity(buff, "copper\r\n") >= 78) {
      Serial.println("cave_search3");
    }
    if (calculateSimilarity(buff, "cave_end\r\n") >= 78) {
      Serial.println("cave_end");
      isTrollEnd = 1;
    }
    if (calculateSimilarity(buff, "cave_click\r\n") >= 78) {
      Serial.println("cave_click");
    }
    if (calculateSimilarity(buff, "door_cave\r\n") >= 78) {
      Serial.println("door_cave");
    }
    if (calculateSimilarity(buff, "help\r\n") >= 78) {
      HelpHandler("troll");
    }
    if (calculateSimilarity(buff, "metal\r\n") >= 78) {
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      potionPulsation = 0;
      //Serial2.println("item_find");
      Serial.println("item_find");
      Serial3.println("item_find");
    }
  }
}

void GoldPulsation() {
  if (millis() - lightFlashTimer >= 18) {
    if (!upHelp) {
      light++;
      if (light == 75)
        upHelp = 1;
    } else {
      light--;
      if (light == 10)
        upHelp = 0;
    }
    float brightness = light / 75.0;  // коэффициент яркости от 0.13 до 1
    int red = 255 * brightness;       // Красная составляющая
    int green = 255 * brightness;     // Зеленая составляющая
    int blue = 0;                     // Синяя составляющая (0 для золотого)
    GoldStrip.setPixelColor(0, GoldStrip.Color(red, green, blue));
    GoldStrip.show();
    lightFlashTimer = millis();
  }
}

void PotionPulsation() {
  if (millis() - lightFlashTimer >= 18) {
    if (!upHelp) {
      light++;
      if (light == 75)
        upHelp = 1;
    } else {
      light--;
      if (light == 10)
        upHelp = 0;
    }
    float brightness = light / 75.0;  // коэффициент яркости от 0 до 1
    int red = 128 * brightness;       // Красная составляющая
    int blue = 128 * brightness;      // Синяя составляющая
    CauldronStrip.setPixelColor(0, CauldronStrip.Color(red, 0, blue));
    CauldronStrip.show();
    lightFlashTimer = millis();
  }
}

////функция с печкой
void Oven() {
  static bool goldPulsation;
  static bool potionPulsation;

  goldButton.tick();
  digitalWrite(pinA, 0);
  digitalWrite(pinB, 0);
  digitalWrite(pinC, 1);
  bool reading = !digitalRead(21);
  if (reading) {
    if (!potionPulsation) {
      potionPulsation = 1;
      Serial1.println("potion");
      Serial.println("item_find");
      Serial2.println("item_find");
      Serial3.println("item_find");
      goldPulsation = 0;
    }
  }

  if (goldButton.isPress()) {
    if (!goldPulsation) {
      goldPulsation = 1;
      Serial1.println("gold");
      Serial.println("item_find");
      Serial2.println("item_find");
      Serial3.println("item_find");
      potionPulsation = 0;
    }
  }

  if (goldPulsation) {
    CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
    CauldronStrip.show();
    GoldPulsation();
  }

  if (potionPulsation) {
    GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
    GoldStrip.show();
    PotionPulsation();
  }

  if (Serial1.available()) {
    String buff = Serial1.readString();
    //Serial.println(buff);
    if (buff == "help\r\n") {
      HelpHandler("workshop");
    }
    if (buff == "fire1\r\n") {
      Serial.println("fire1");
    }
    if (buff == "fire2\r\n") {
      Serial.println("fire2");
    }
    if (buff == "fire3\r\n") {
      Serial.println("fire3");
    }
    if (buff == "fire0\r\n") {
      Serial.println("fire0");
    }
    if (buff == "item_add\r\n") {
      Serial.println("item_add");
    }
    if (buff == "broom\r\n") {
      Serial.println("broom");
    }
    if (buff == "helmet\r\n") {
      Serial.println("helmet");
    }
    if (buff == "story_35\r\n") {
      Serial.println("story_35");
      Serial1.println("item_end");
      Serial2.println("item_end");
      Serial3.println("item_end");
      delay(1000);
      Serial1.println("day_off");
      Serial2.println("day_off");
      Serial3.println("day_off");
      mySerial.println("day_off");
      GoldStrip.setPixelColor(0, GoldStrip.Color(0, 0, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(0, 0, 0));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      GoldStrip.clear();
      CandleStrip.clear();
      CauldronStrip.clear();
      CauldronRoomStrip.clear();
      memory_Led.clear();
      strip1.clear();
      strip2.clear();
      GoldStrip.show();
      CandleStrip.show();
      CauldronStrip.show();
      CauldronRoomStrip.show();
      memory_Led.show();
      strip1.show();
      strip2.show();
      digitalWrite(MansardLight, LOW);
      digitalWrite(LastTowerTopLight, LOW);
      digitalWrite(Fireworks, LOW);
      digitalWrite(LibraryLight, LOW);
      digitalWrite(BankRoomLight, LOW);
      digitalWrite(HallLight, LOW);
      digitalWrite(UfHallLight, LOW);
      digitalWrite(HightTowerDoor2, LOW);
      digitalWrite(TorchLight, LOW);
      digitalWrite(HallLight, HIGH);
      level++;
    }
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "open_workshop") {
      Serial1.println("open_workshop");
    }
    if (buff == "workshop") {
      Serial1.println("skip");
      Serial.println("story_35");
      Serial1.println("item_end");
      Serial2.println("item_end");
      Serial3.println("item_end");
      delay(1000);
      Serial1.println("day_off");
      Serial2.println("day_off");
      Serial3.println("day_off");
      mySerial.println("day_off");
      GoldStrip.setPixelColor(0, GoldStrip.Color(0, 0, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(0, 0, 0));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      GoldStrip.clear();
      CandleStrip.clear();
      CauldronStrip.clear();
      CauldronRoomStrip.clear();
      memory_Led.clear();
      strip1.clear();
      strip2.clear();
      GoldStrip.show();
      CandleStrip.show();
      CauldronStrip.show();
      CauldronRoomStrip.show();
      memory_Led.show();
      strip1.show();
      strip2.show();
      digitalWrite(MansardLight, LOW);
      digitalWrite(LastTowerTopLight, LOW);
      digitalWrite(Fireworks, LOW);
      digitalWrite(LibraryLight, LOW);
      digitalWrite(BankRoomLight, LOW);
      digitalWrite(HallLight, LOW);
      digitalWrite(UfHallLight, LOW);
      digitalWrite(HightTowerDoor2, LOW);
      digitalWrite(TorchLight, LOW);
      digitalWrite(HallLight, HIGH);
      level++;
    }
    if (buff == "skin") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("skin");
      Serial2.println("item_find");
      Serial3.println("item_find");
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }

  if (Serial2.available()) {
    String buff = Serial2.readString();
    if (buff == "metal\r\n") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("metal");
      Serial3.println("item_find");
      Serial.println("item_find");
    }
    if (buff == "help\r\n") {
      HelpHandler("troll");
    }
  }

  if (Serial3.available()) {
    String buff = Serial3.readString();
    if (buff == "crystal\r\n") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("crystal");
      Serial2.println("item_find");
      Serial.println("item_find");
    }
    if (buff == "help\r\n") {
      HelpHandler("knight");
    }
  }

  if (mySerial.available()) {
    String buff = mySerial.readString();
    if (buff == "help\r\n") {
      HelpHandler("dwaf");
    }
  }
}

/// подставка под котел

////игра с бутылками
void Bottles() {
  // Если активен "период охлаждения" после ошибки, выходим из функции
  if (millis() < rfidCooldownEnd) {
    return;
  }

  switch (Bottle) {
    case 0:
      SecondBottle(); // Логика в вашем коде перепутана, оставляю как есть
      break;
    case 1:
      FirstBottle(); // Логика в вашем коде перепутана, оставляю как есть
      break;
    case 2:
      ThirdBottle();
      break;
    case 3:
      FourBottle();
      break;
  }
}

///////первая бутылка
void FirstBottle() {
  if (myRFID.search(addr)) {
    Bottle1Timer = millis();
    byte result = 1;
    for (int i = 0; i < 8; i++) {
      result &= addr[i] == validHex[0][i];
    }
    if (result) {
      if (FirstBottleTrue) {
        Serial.println("second_bottle");
        CauldronTrueFire();
        FirstBottleTrue = 0;
      }
      if (FirstBottleTrue == 0) {
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 250, 30));
        }
        CauldronRoomStrip.show();
        CauldronFire();
      }
    } else {
      if (FirstBottleFalse) {
        Serial.println("mistake_bottle");
        FirstBottleFalse = 0;
        Bottle = 0;
        FirstBottleTrue = 1;
        SecondBottleTrue = 1; SecondBottleFalse = 1;
        ThirdBottleTrue = 1;  ThirdBottleFalse = 1;
        FourBottleTrue = 1;   FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 250; // Запускаем "охлаждение" на 250 мс
      }
      CauldronMistakeFire();
    }
    myRFID.reset_search();
  } else {
    if (millis() - Bottle1Timer >= 100) {
      FirstBottleFalse = 1;
      if (FirstBottleTrue == 0)
        Bottle++;
      for (int i = 0; i <= 12; i++) {
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(30, 0, 250));
      }
      CauldronRoomStrip.show();
      CauldronFire();
    }
  }
}
///////вторая бутылка
void SecondBottle() {
  if (myRFID.search(addr)) {
    Bottle2Timer = millis();
    byte result = 1;
    for (int i = 0; i < 8; i++) {
      result &= addr[i] == validHex[1][i];
    }
    if (result) {
      if (SecondBottleTrue) {
        Serial.println("first_bottle");
        CauldronTrueFire();
        SecondBottleTrue = 0;
      }
      if (SecondBottleTrue == 0) {
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(150, 250, 0));
        }
        CauldronRoomStrip.show();
        CauldronFire();
      }
    } else {
      if (SecondBottleFalse) {
        Serial.println("mistake_bottle");
        SecondBottleFalse = 0;
        Bottle = 0;
        FirstBottleTrue = 1;  FirstBottleFalse = 1;
        SecondBottleTrue = 1;
        ThirdBottleTrue = 1;  ThirdBottleFalse = 1;
        FourBottleTrue = 1;   FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 250; // Запускаем "охлаждение" на 250 мс
      }
      CauldronMistakeFire();
    }
    myRFID.reset_search();
  } else {
    if (millis() - Bottle2Timer >= 100) {
      SecondBottleFalse = 1;
      if (SecondBottleTrue == 0)
        Bottle++;
      for (int i = 0; i <= 12; i++) {
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 250, 30));
      }
      CauldronRoomStrip.show();
      CauldronFire();
    }
  }
}
//////третья бутылка
void ThirdBottle() {
  if (myRFID.search(addr)) {
    Bottle3Timer = millis();
    byte result = 1;
    for (int i = 0; i < 8; i++) {
      result &= addr[i] == validHex[2][i];
    }
    if (result) {
      if (ThirdBottleTrue) {
        Serial.println("third_bottle");
        CauldronTrueFire();
        ThirdBottleTrue = 0;
      }
      if (ThirdBottleTrue == 0) {
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(250, 250, 250));
        }
        CauldronRoomStrip.show();
        CauldronFire();
      }
    } else {
      if (ThirdBottleFalse) {
        Serial.println("mistake_bottle");
        ThirdBottleFalse = 0;
        Bottle = 0;
        FirstBottleTrue = 1;  FirstBottleFalse = 1;
        SecondBottleTrue = 1; SecondBottleFalse = 1;
        ThirdBottleTrue = 1;
        FourBottleTrue = 1;   FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 250; // Запускаем "охлаждение" на 250 мс
      }
      CauldronMistakeFire();
    }
    myRFID.reset_search();
  } else {
    if (millis() - Bottle3Timer >= 100) {
      ThirdBottleFalse = 1;
      if (ThirdBottleTrue == 0)
        Bottle++;
      for (int i = 0; i <= 12; i++) {
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(150, 250, 0));
      }
      CauldronRoomStrip.show();
      CauldronFire();
    }
  }
}
/////четвертая бутылка
void FourBottle() {
  if (myRFID.search(addr)) {
    Bottle4Timer = millis();
    byte result = 1;
    for (int i = 0; i < 8; i++) {
      result &= addr[i] == validHex[3][i];
    }
    if (result) {
      if (FourBottleTrue) {
        Serial.println("four_bottle");
        rainbow();
        CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
        CauldronStrip.show();
        for (int i = 255; i >= 50; i--) {
          CauldronRoomStrip.setBrightness(i);
          CauldronRoomStrip.show();
          delay(2);
        }
        isPotionEnd = true;
      }
    } else {
      if (FourBottleFalse) {
        Serial.println("mistake_bottle");
        FourBottleFalse = 0;
        Bottle = 0;
        FirstBottleTrue = 1;  FirstBottleFalse = 1;
        SecondBottleTrue = 1; SecondBottleFalse = 1;
        ThirdBottleTrue = 1;  ThirdBottleFalse = 1;
        FourBottleTrue = 1;
        rfidCooldownEnd = millis() + 250; // Запускаем "охлаждение" на 250 мс
      }
      CauldronMistakeFire();
    }
    myRFID.reset_search();
  } else {
    if (millis() - Bottle4Timer >= 100) {
      FourBottleFalse = 1;
      for (int i = 0; i <= 12; i++) {
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(250, 250, 250));
      }
      CauldronRoomStrip.show();
      CauldronFire();
    }
  }
}

void BasketLesson() {
  static bool isSend;
  BasketEffectLesson();
  if (isTrainBasket) {
    if (!isSend) {
      Serial.println("story_59");
      isSend = 1;
    }
  }
  if (Serial2.available()) {
    String buf = Serial2.readString();

    if (buf == "boy_in\r\n") {
      Serial.println("boy_in");
    }
    if (buf == "boy_out\r\n") {
      Serial.println("boy_out");
      snitchFlag = 1;
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
    }
    if (buf == "lesson_basket_done\r\n") {
      Serial.println("flying_ball");
      snitchFlag = 0;
      enemyTimer = millis();
      additionalTimer = millis();
      isSend = 0;
      level++;
    }
  }
  if (Serial.available()) {
    String buf = Serial.readString();
    if (buf == "start_game_basket") {
      snitchFlag = 0;
      enemyTimer = millis();
      additionalTimer = millis();
    }
    if (buf == "basket") {
      Serial.println("win");
      Serial2.println("win");
      level += 2;
    }
    if (buf == "restart") {
      OpenAll();
      RestOn();
    }
  }
}
////играем в баскет и передаем счет на башню сов
void Basket() {
  BasketEffect();
  if (Serial2.available()) {
    String buf = Serial2.readString();
    if (buf == "fr71nmr\r\n") {
      BotScore = "1";
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_1_bot");
    }
    if (buf == "boy_in\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      additionalTimer = millis();
    }
    if (buf == "boy_out\r\n") {
      snitchFlag = 1;
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
    }
    if (buf == "fr72nmr\r\n") {
      BotScore = "2";
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_2_bot");
    }
    if (buf == "fr73nmr\r\n") {
      BotScore = "3";
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_3_bot");
    }
    if (buf == "fr74nmr\r\n") {
      BotScore = "4";
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_4_bot");
    }
    if (buf == "fr8nmr\r\n") {
      Serial.println("win");
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
      level=20;
    }
    if (buf == "fr61nmr\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_1_player");
    }
    if (buf == "fr62nmr\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_2_player");
    }
    if (buf == "fr63nmr\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_3_player");
    }
    if (buf == "fr64nmr\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_4_player");
    }

    if (buf == "start_snitch\r\n") {
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("start_snitch");
    }

    if (buf == "fr9nmr\r\n") {
      Serial.println("win_robot");
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
      //level=21;
    }
  }

  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "basket") {
      Serial.println("win");
      Serial2.println("win");
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
      level=20;
    } else if (buff == "restart") {
      OpenAll();
      RestOn();
    } else if (buff == "soundoff") {
      flagSound = 1;
    } else if (buff == "help")
      Serial.println("help_12");
    else
      Unlocks(buff);
  }
}
//////функция со стуком в библиотеке
void Library() {
  //digitalWrite(UfHallLight, HIGH);
  switch (ghostState) {
    case 0:
      if (!digitalReadExpander(0, board2)) {
        Serial.println("h_clock");
        digitalWrite(UfHallLight, HIGH);
        digitalWrite(HallLight, LOW);
        ghostState++;
      }
      break;
    case 1:
      if (!digitalReadExpander(6, board1)) {
        Serial.println("uf_clock");
        for (int i = 0; i < 3; i++) {
          digitalWrite(UfHallLight, HIGH);
          delay(500);
          digitalWrite(UfHallLight, LOW);
          delay(500);
        }
        digitalWrite(UfHallLight, LOW);
        digitalWrite(HallLight, LOW);
        ghostState++;
        
      }
      break;
    case 6:
      if (millis() - KnockInterval >= 3000) {
        if (millis() - KnockIntervalLow >= 100) {
          KnockIntervalLow = millis();
          KnockState = !KnockState;
          digitalWrite(KnockSol, KnockState);
          NumKnock++;
        }
        if (NumKnock == 4) {
          KnockInterval = millis();
          NumKnock = 0;
        }
      }
      if (++index > 2)
        index = 0;
      val[index] = analogRead(KnockSens);
      val_filter = middle_of_3(val[0], val[1], val[2]);
      // Serial.println(analogRead(KnockSens));
      if (KnockState != 1) {
        if (analogRead(KnockSens) <= threshold) {
          Serial.println("punch");
          digitalWrite(KnockSol, LOW);
          //OpenLock(LibraryDoor);
          digitalWrite(LibraryLight, HIGH);
          libraryDoorTimer = millis();
          level++;
        }
      }
      break;
  }


  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "rrt3lck") {
      //OpenLock(LibraryDoor);
      digitalWrite(KnockSol, LOW);
      digitalWrite(LibraryLight, HIGH);
      level++;
    } 
    if (buff == "restart") {
      OpenAll();
      RestOn();
    } 
    if (buff == "ghost_skip") {
      Serial.println("punch");
      digitalWrite(KnockSol, LOW);
      digitalWrite(LibraryLight, HIGH);
      libraryDoorTimer = millis();
      level++;
    } 
    if (buff == "student_open") {
      Serial1.println("servo");
      delay(1000);
      boyServo.attach(49);
      boyServo.write(130);
      delay(1000);
      boyServo.detach();
    } 
    if (buff == "ghost") {
      //поезд
      if (ghostState == 2) {
        Serial.println("story_40");
        //ghostState++;
      }
      // волк
      if (ghostState == 3) {
        Serial.println("story_41");
        //ghostState++;
      }
      //поезд
      if (ghostState == 4) {
        Serial.println("story_42");
        ghostState = 6;
      }
      if(ghostState<5)
        ghostState++;
    } 
    if (buff == "soundon") {
      flagSound = 0;
    } 
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();

}

void LibraryGame() {
  if (digitalReadExpander(5, board3)) {
    Serial.println("lib_door");
    
    level++;
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "open_library") {
      OpenLock(LibraryDoor);
      digitalWrite(LibraryLight, HIGH);
    }
    if (buff == "ghost") {
      Serial.println("star_hint");
    }
    if (buff == "student_hide") {
      boyServo.attach(49);
      boyServo.write(0);
      delay(1000);
      boyServo.detach();
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
}

void CentralTowerGame() {
  static int state = 0;
  static bool flag =0;
  //FireCup();
switch (state) {
    case 0:
      if (!digitalReadExpander(3, board4)) {
        //state++;
//Serial.println("fire");
        //delay(1500);
        flag = 1;
      }
      if (digitalReadExpander(3, board4) && flag) {
        state++;
Serial.println("fire");
      }
      break;
    case 1:
      if (!digitalReadExpander(2, board4)) {
        Serial2.println("opent_basket");
Serial.println("door_basket");
        fireFlag = 1;
        state = 0;
        flag = 0;
        level++;
      }
      break;
}

  // Добавлен обработчик команд от сервера, чтобы принять команду "student_hide"
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "student_hide") {
      boyServo.attach(49);
      boyServo.write(0); // Поворачиваем сервопривод в скрытое положение
      delay(1000);
      boyServo.detach();
    }

    if (buff == "door_top") {
      Serial1.println("day_on");
      Serial2.println("day_on");
      Serial3.println("day_on");
      mySerial.println("day_on");
for (int i = 0; i <= 50; i++) {
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 255, 255));
}
      CauldronRoomStrip.show();
      digitalWrite(MansardLight, HIGH);
      digitalWrite(LibraryLight, HIGH);
      digitalWrite(BankRoomLight, HIGH);
      digitalWrite(HallLight, HIGH);
      digitalWrite(UfHallLight, LOW);
      digitalWrite(TorchLight, HIGH);
      OpenLock(HightTowerDoor2);
digitalWrite(LastTowerTopLight, HIGH);
      flag = 0;
    }
    if (buff == "cup") {
      Serial2.println("opent_basket");
Serial.println("door_basket");
      fireFlag = 1;
      state = 0;
      level++;
    }
    if (buff == "restart") {
      flag = 0;
OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
}
    if (buff == "soundoff") {
      flagSound = 1;
}
  }
  HelpTowersHandler();
}

void CentralTowerGameDown() {
  static int state = 0;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50; // Задержка антидребезга
  static bool lastSensor0State = HIGH;
  static bool lastSensor1State = HIGH;
  
  // Чтение текущих состояний с антидребезгом
  bool currentSensor0State = digitalReadExpander(0, board4);
  bool currentSensor1State = digitalReadExpander(1, board4);
  
  unsigned long currentTime = millis();
  
  // Антидребезг для сенсора 0
  if (currentSensor0State != lastSensor0State) {
    lastDebounceTime = currentTime;
  }
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    // Используем стабильное состояние после антидребезга
    currentSensor0State = lastSensor0State;
  }
  lastSensor0State = currentSensor0State;
  
  // Антидребезг для сенсора 1
  if (currentSensor1State != lastSensor1State) {
    lastDebounceTime = currentTime;
  }
  if ((currentTime - lastDebounceTime) > debounceDelay) {
    currentSensor1State = lastSensor1State;
  }
  lastSensor1State = currentSensor1State;

  // Обработка состояний
  switch (state) {
    case 0: // Ожидание нажатия первого геркона
      if (currentSensor0State == LOW) {
        state++;
        Serial.println("door_spell - Step 1");
        lastDebounceTime = currentTime; // Сброс времени для следующего состояния
      }
      break;
      
    case 1: // Ожидание отпускания первого геркона
      if (currentSensor0State == HIGH) {
        state++;
        Serial.println("door_spell - Ready for sensor 1");
      }
      break;
      
    case 2: // Ожидание нажатия второго геркона
      if (currentSensor1State == LOW) {
        state++;
        Serial.println("door_spell - Step 2");
        lastDebounceTime = currentTime;
      }
      break;
      
    case 3: // Ожидание отпускания второго геркона
      if (currentSensor1State == HIGH) {
        state++;
        Serial.println("door_spell - Ready for sensor 0 again");
      }
      break;
      
    case 4: // Второе нажатие первого геркона
      if (currentSensor0State == LOW) {
        state++;
        Serial.println("door_spell - Step 3");
        lastDebounceTime = currentTime;
      }
      break;
      
    case 5: // Ожидание отпускания первого геркона
      if (currentSensor0State == HIGH) {
        state++;
        Serial.println("door_spell - Ready for sensor 1 again");
      }
      break;
      
    case 6: // Второе нажатие второго геркона
      if (currentSensor1State == LOW) {
        state++;
        Serial.println("door_spell - Step 4");
        lastDebounceTime = currentTime;
      }
      break;
      
    case 7: // Ожидание отпускания второго геркона
      if (currentSensor1State == HIGH) {
        state++;
        Serial.println("door_spell - Ready for final sequence");
      }
      break;
      
    case 8: // Третье нажатие первого геркона
      if (currentSensor0State == LOW) {
        state++;
        Serial.println("door_spell - Step 5");
        lastDebounceTime = currentTime;
      }
      break;
      
    case 9: // Ожидание отпускания первого геркона
      if (currentSensor0State == HIGH) {
        state++;
        Serial.println("door_spell - Final step ready");
      }
      break;
      
    case 10: // Финальное нажатие второго геркона
      if (currentSensor1State == LOW) {
        OpenLock(HightTowerDoor);
        digitalWrite(TorchLight, HIGH);
        Serial.println("door_spell");
        level++;
        state = 0; // Сброс состояния для возможного повторного использования
      }
      break;
  }

  // Обработка Serial команд (оставлено без изменений)
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "spell") {
      OpenLock(HightTowerDoor);
      digitalWrite(TorchLight, HIGH);
      Serial.println("door_spell");
      level++;
      state = 0; // Сброс состояния
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
      state = 0; // Сброс состояния
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  
  HelpTowersHandler();
}
// доделать что бы в банке работала хуйня с зельем
/////нужно подуть в окно
void OpenBank() {
  if (millis() - helpsBankTimer >= 30000) {
    int helpCounter = 0;
    helpCounter = random(0, 4);
    switch (helpCounter) {
      case 0:
        Serial.println("story_22_a");
        break;
      case 1:
        Serial.println("story_22_b");
        break;
      case 2:
        Serial.println("story_22_c");
        break;
    }
    helpsBankTimer = millis();
  }

  Fire();
  if (digitalRead(WindowSens)) {
    CandleStrip.setPixelColor(0, CandleStrip.Color(0, 0, 0));
    CandleStrip.show();
    Serial.println("miror");
    scrollNumber = -1;
    //digitalWrite(BankRoomLight, HIGH);
    //OpenLock(BankDoor);
    level++;
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "open_bank_door") {
      CandleStrip.setPixelColor(0, CandleStrip.Color(0, 0, 0));
      CandleStrip.show();
      Serial.println("miror");
      scrollNumber = -1;
      //digitalWrite(BankRoomLight, HIGH);
      //OpenLock(BankDoor);
      level++;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
}

void Scrolls() {
  switch (scrollNumber) {
    case 0:
      ScrollOne();
      break;
    case 1:
      ScrollTwo();
      break;
    case 2:
      ScrollThree();
      break;
    case 3:
      ScrollFour();
      break;
    case 4:
      ScrollFive();
      //if(millis()-safeTimer>=4000){
      //  OpenLock(BankStashDoor);
      //  level++;
      //}
      break;
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    Serial.println(buff);
    if (buff == "open_safe") {
      OpenLock(BankStashDoor);
      level++;
    }
    if (buff == "safe") {
      scrollNumber++;
      Serial.println("safe_end");
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      safeTimer = millis();
    }
    if (buff == "open_bank") {
      digitalWrite(BankRoomLight, HIGH);
      OpenLock(BankDoor);
      scrollNumber = 0;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
  //HelpButton("help_19");
}

void ScrollOne() {
  ///--------
  if (!digitalReadExpander(2, board3)) {
    //if (!digitalReadExpander(0, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll1On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll11On) {
        scrollNumber++;
        Serial.println("safe_fix");
        Scroll11On = 1;
      }
    }
  } else {
    Scroll11On = 0;
    Scroll1On = 0;
    code1Timer = millis();
  }

  ///--------
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll2On = 1;
    }
    if (millis() - code2Timer >= 2000) {
      //scrollNumber++;
      if (!Scroll21On) {
        Serial.println("safe_fix");
        Scroll21On = 1;
      }
    }
  } else {
    Scroll2On = 0;
    Scroll21On = 0;
    code2Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(2, board3)) {
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll3On = 1;
    }
    if (millis() - code3Timer >= 2000) {
      if (!Scroll31On) {
        Serial.println("safe_fix");
        Scroll31On = 1;
      }
    }
  } else {
    Scroll3On = 0;
    Scroll31On = 0;
    code3Timer = millis();
  }
  ///--------
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll4On = 1;
    }
    if (millis() - code4Timer >= 2000) {
      if (!Scroll41On) {
        Serial.println("safe_fix");
        Scroll41On = 1;
      }
    }
  } else {
    Scroll4On = 0;
    Scroll41On = 0;
    code4Timer = millis();
  }
}
void ScrollTwo() {
  ///--------
  //if (!digitalReadExpander(0, board3)) {
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll1On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll11On) {
        Scroll11On = 1;
        scrollNumber = 0;
        Scroll11On = 0;
        Scroll1On = 0;
        code1Timer = millis();
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll11On = 0;
    Scroll1On = 0;
    code1Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(1, board3)) {
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll2On = 1;
    }
    if (millis() - code2Timer >= 2000) {
      if (!Scroll21On) {
        scrollNumber++;
        Serial.println("safe_fix");
        Scroll21On = 1;
      }
    }
  } else {
    Scroll2On = 0;
    Scroll21On = 0;
    code2Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(2, board3)) {
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll3On = 1;
    }
    if (millis() - code3Timer >= 2000) {
      if (!Scroll31On) {
        scrollNumber = 0;
        Scroll31On = 1;
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll3On = 0;
    Scroll31On = 0;
    code3Timer = millis();
  }
  ///--------
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll4On = 1;
    }
    if (millis() - code4Timer >= 2000) {
      if (!Scroll41On) {
        scrollNumber = 0;
        Scroll41On = 1;
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll4On = 0;
    Scroll41On = 0;
    code4Timer = millis();
  }
}
void ScrollThree() {
  ///--------
  if (!digitalReadExpander(1, board3)) {
    //if (!digitalReadExpander(0, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll1On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll11On) {
        Scroll11On = 1;
        scrollNumber = 0;
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll11On = 0;
    Scroll1On = 0;
    code1Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(1, board3)) {
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll2On = 1;
    }
    if (millis() - code2Timer >= 2000) {
      if (!Scroll21On) {
        Serial.println("safe_fix");
        scrollNumber = 0;
        Scroll21On = 1;
      }
    }
  } else {
    Scroll2On = 0;
    Scroll21On = 0;
    code2Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(2, board3)) {
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll3On = 1;
    }
    if (millis() - code3Timer >= 2000) {

      if (!Scroll31On) {
        scrollNumber++;
        Serial.println("safe_fix");
        Scroll31On = 1;
      }
    }
  } else {
    Scroll3On = 0;
    Scroll31On = 0;
    code3Timer = millis();
  }
  ///--------
  //if (!digitalReadExpander(3, board3)) {
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll4On = 1;
    }
    if (millis() - code4Timer >= 2000) {
      if (!Scroll41On) {
        scrollNumber = 0;
        Serial.println("safe_fix");
        Scroll41On = 1;
      }
    }
  } else {
    Scroll4On = 0;
    Scroll41On = 0;
    code4Timer = millis();
  }
}

void ScrollFour() {
  ///--------
  // if (!digitalReadExpander(0, board3)) {
  //if (!digitalReadExpander(3, board3)) {
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll1On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll11On) {
        Scroll11On = 1;
        scrollNumber = 0;
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll11On = 0;
    Scroll1On = 0;
    code1Timer = millis();
  }

  ///--------
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll2On = 1;
    }
    if (millis() - code2Timer >= 2000) {
      scrollNumber = 0;
      if (!Scroll21On) {
        scrollNumber = 0;
        Serial.println("safe_fix");
        Scroll21On = 1;
      }
    }
  } else {
    Scroll2On = 0;
    Scroll21On = 0;
    code2Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(2, board3)) {
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll3On = 1;
    }
    if (millis() - code3Timer >= 2000) {
      if (!Scroll31On) {
        scrollNumber = 0;
        Serial.println("safe_fix");
        Scroll31On = 1;
      }
    }
  } else {
    Scroll3On = 0;
    Scroll31On = 0;
    code3Timer = millis();
  }
  ///--------
  //if (!digitalReadExpander(3, board3)) {
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll4On = 1;
    }
    if (millis() - code4Timer >= 2000) {
      if (!Scroll41On) {
        scrollNumber++;
        Serial.println("safe_fix");
        Scroll41On = 1;
      }
    }
  } else {
    Scroll4On = 0;
    Scroll41On = 0;
    code4Timer = millis();
  }
}

void ScrollFive() {
  ///--------
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll4On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll41On) {
        Scroll41On = 1;
        scrollNumber = 0;
        Serial.println("safe_fix");
      }
    }
  } else {
    Scroll41On = 0;
    Scroll4On = 0;
    code4Timer = millis();
  }

  ///--------
  //if (!digitalReadExpander(1, board3)) {
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll2On = 1;
    }
    if (millis() - code2Timer >= 2000) {
      scrollNumber = 0;
      if (!Scroll21On) {
        scrollNumber = 0;
        Serial.println("safe_fix");
        Scroll21On = 1;
      }
    }
  } else {
    Scroll2On = 0;
    Scroll21On = 0;
    code2Timer = millis();
  }

  ///--------
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll3On = 1;
    }
    if (millis() - code3Timer >= 2000) {
      if (!Scroll31On) {
        scrollNumber = 0;
        Serial.println("safe_fix");
        Scroll31On = 1;
      }
    }
  } else {
    Scroll3On = 0;
    Scroll31On = 0;
    code3Timer = millis();
  }
  ///--------
  //if (!digitalReadExpander(3, board3)) {
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) {
      Serial.println("safe_turn");
      Scroll1On = 1;
    }
    if (millis() - code1Timer >= 2000) {
      if (!Scroll11On) {
        scrollNumber++;
        Serial.println("safe_end");
        GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
        GoldStrip.show();
        safeTimer = millis();
        Scroll11On = 1;
      }
    }
  } else {
    Scroll1On = 0;
    Scroll11On = 0;
    code1Timer = millis();
  }
}


void SealSpace() {
  static unsigned long lastPress = 0;
  // Обработка геркона
  if (digitalReadExpander(4, board4) && (millis() - lastPress > 500)) {
    Serial.println("cristal_up");
    OpenLock(MemoryRoomDoor);
    level++;
    lastPress = millis();
  }

  // Обработка команд
  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    
    if (cmd == "crystals") {
      Serial.println("cristal_up");
      OpenLock(MemoryRoomDoor);
      level++;
    }
    else if (cmd == "restart") {
      OpenAll();
      RestOn();
    }
    else if (cmd == "soundon") {
      flagSound = 0;
      Serial.println("Sound ON");
    }
    else if (cmd == "soundoff") {
      flagSound = 1;
      Serial.println("Sound OFF");
    }
  }

  HelpTowersHandler();
}

void MemoryRoom() {
  boyServo.detach();
  switch (_stages) {
    case 0:
      giftGame();
      break;
    case 1:
      startGame();
      break;
    case 2:
      _presentation();
      break;
    case 3:
      Game();
      break;
  }
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "memory_room_end") {
      Serial.println("memory_room_end");
      for (long firstPixelHue = 0; firstPixelHue < 2 * 65536; firstPixelHue += 556) {
        for (int i = 0; i < memory_Led.numPixels(); i++) {  // For each pixel in strip...
          int pixelHue = firstPixelHue + (i * 65536L / memory_Led.numPixels());
          memory_Led.setPixelColor(i, memory_Led.gamma32(memory_Led.ColorHSV(pixelHue)));
        }
        memory_Led.show();
        delay(20);  // Pause for a moment
      }

      digitalWrite(LastTowerTopLight, HIGH);
      digitalWrite(TorchLight, HIGH);
      OpenLock(CrimeDoor);
      for (int i = 255; i >= 50; i--) {
        memory_Led.setBrightness(i);
        memory_Led.show();
        delay(2);
      }
      level++;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
}

void CrimeHelp() {
  
  static bool flag = 0;
  if (millis() - crimeHelpTimer >= 20000) {
    Serial.println("story_55");
    crimeHelpTimer = millis();
  }
  if (!digitalReadExpander(7, board1)) {
    flag = 1;
  }

  if (digitalReadExpander(7, board1) && flag) {
    Serial.println("crime_end");
    Serial2.println("start_lesson");
    delay(1000);
    Serial2.println("start_lesson");
    flag = 0;
    level++;
  }


  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "basket") {
      Serial.println("win");
      Serial2.println("win");
      level++;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
}

void OpenDoor(int pin) {
  digitalWrite(pin, 1);
  delay(100);
  digitalWrite(pin, 0);
}


void FinalPresentation() {
  digitalWrite(Fireworks, HIGH);
  MagicEffect();
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "restart") {
      OpenAll();
      RestOn();
    }
  }
}


void rainbow() {
  for (long firstPixelHue = 0; firstPixelHue < 2 * 65536; firstPixelHue += 556) {
    for (int i = 0; i < CauldronStrip.numPixels(); i++) {  // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / CauldronStrip.numPixels());
      CauldronStrip.setPixelColor(i, CauldronStrip.gamma32(CauldronStrip.ColorHSV(pixelHue)));
      CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.gamma32(CauldronStrip.ColorHSV(pixelHue)));
    }
    CauldronStrip.show();  // Update strip with new contents
    CauldronRoomStrip.show();
    delay(20);  // Pause for a moment
  }
  for (int i = 0; i <= 12; i++) {
    CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 0, 250));
  }
  CauldronRoomStrip.show();
}

void OpenLock(int pin) {
  if (pin == -1) {
    // отключить все двери
    for (int i = 0; i < DOORS; i++) {
      active[i] = false;
      digitalWrite(doors[i], LOW);
    }
    return;
  }

  // ищем пин в списке дверей
  for (int i = 0; i < DOORS; i++) {
    if (doors[i] == pin) {
      active[i] = true;  // активируем этот пин
      return;
    }
  }
}

// обработка дверей (вызов в loop)
void handleLocks() {
  static unsigned long lastCheck = 0;
  unsigned long now = millis();

  // Проверяем состояние каждые 100мс вместо постоянной проверки
  if (now - lastCheck < 100) {
    return;
  }
  lastCheck = now;

  for (int i = 0; i < DOORS; i++) {
    if (active[i]) {
      // Если замк уже включен, проверяем сколько времени прошло
      if (digitalRead(doors[i]) == HIGH) {
        if (now - lastOpen[i] >= 500) {
          digitalWrite(doors[i], LOW);  // выключаем после 500мс
        }
      }
      // Если замк выключен и прошло 5 секунд
      else if (now - lastOpen[i] >= 5000) {
        digitalWrite(doors[i], HIGH);
        lastOpen[i] = now;  // обновляем время при включении
      }
    }
  }
}

void OpenAll() {
  for (int i = 0; i < DOORS; i++) {
    digitalWrite(doors[i], HIGH);
  }
  delay(500);  // полсекунды открыты
  for (int i = 0; i < DOORS; i++) {
    digitalWrite(doors[i], LOW);
  }
}


void Fire() {
  if (millis() - FireInterval >= random(150, 180)) {
    CandleStrip.setPixelColor(0, CandleStrip.Color(random(150, 250), 30, 0));
    CandleStrip.show();
    FireInterval = millis();
  }
}

void FireCup() {
  if (millis() - FireInterval >= random(150, 180)) {
    strip2.setPixelColor(65, strip2.Color(random(150, 250), 30, 0));
    strip2.show();
    FireInterval = millis();
  }
}

void CauldronFire() {
  if (millis() - CauldronFireInterval >= random(100, 130)) {
    for (int i = 0; i <= 9; i++) {
      CauldronStrip.setPixelColor(i, CauldronStrip.Color(0, random(100, 250), random(0, 10)));
    }
    CauldronStrip.show();
    CauldronFireInterval = millis();
  }
}

void CauldronMistakeFire() {
  if (millis() - CauldronFireMistakeInterval >= random(50, 70)) {
    for (int j = 0; j <= 9; j++) {
      CauldronStrip.setPixelColor(j, CauldronStrip.Color(random(100, 200), random(15, 25), random(0, 10)));
      CauldronRoomStrip.setPixelColor(j, CauldronRoomStrip.Color(random(100, 250), 0, 0));
    }
    CauldronStrip.show();

    CauldronRoomStrip.show();
    CauldronFireMistakeInterval = millis();
  }
}

void CauldronTrueFire() {
  for (int i = 0; i <= 50;) {
    if (millis() - CauldronFireTrueInterval >= random(30, 70)) {
      for (int j = 0; j <= 9; j++) {
        CauldronStrip.setPixelColor(j, CauldronStrip.Color(100 - i, i * 5, 100 - i));
        CauldronRoomStrip.setPixelColor(j, CauldronRoomStrip.Color(i * 4, 0, i * 5));
      }
      CauldronRoomStrip.show();
      CauldronStrip.show();
      CauldronFireTrueInterval = millis();
      i++;
    }
  }
}

float middle_of_3(float a, float b, float c) {
  int middle;
  if ((a <= b) && (a <= c)) {
    middle = (b <= c) ? b : c;
  } else {
    if ((b <= a) && (b <= c)) {
      middle = (a <= c) ? a : c;
    } else {
      middle = (a <= b) ? a : b;
    }
  }
  return middle;
}
// окрашиваем кристаллы в красный если они не убраны приглашаем в игру
void giftGame() {
  if (!digitalRead(firstCrystal)) {
    for (int i = 0; i <= 3; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
    }
    memory_Led.show();
  } else {
    for (int i = 0; i <= 3; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
    }
    memory_Led.show();
  }
  if (!digitalRead(secondCrystal)) {
    for (int i = 4; i <= 7; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
    }
    memory_Led.show();
  } else {
    for (int i = 4; i <= 7; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
    }
    memory_Led.show();
  }
  if (!digitalRead(thirdCrystal)) {
    for (int i = 8; i <= 11; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
    }
    memory_Led.show();
  } else {
    for (int i = 8; i <= 11; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
    }
    memory_Led.show();
  }
  if (!digitalRead(fourCrystal)) {
    for (int i = 12; i <= 15; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
    }
    memory_Led.show();
  } else {
    for (int i = 12; i <= 15; i++) {
      memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
    }
    memory_Led.show();
  }
  if (digitalRead(firstCrystal) && digitalRead(secondCrystal) && digitalRead(thirdCrystal) && digitalRead(fourCrystal)) {
    _stages++;
  }
}
// что бы начать игру нужно убрать все кристалы
void startGame() {

  for (int i = 0; i <= 15; i++) {
    memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 250));
  }
  memory_Led.show();
  if (digitalRead(firstCrystal) && digitalRead(secondCrystal) && digitalRead(thirdCrystal) && digitalRead(fourCrystal)) {
    _stages++;
    lightBr = 250;
  }
}

void _presentation() {
  static bool up = 0;
  static byte flashCount = 0;
  static bool flagHistory = 0;
  static bool flagMus = 0;
  if (!digitalRead(firstCrystal) && !digitalRead(secondCrystal) && !digitalRead(thirdCrystal) && !digitalRead(fourCrystal)) {
    if (!flagMus) {
      Serial.println("start_crystal");
      flagMus = 1;
    }
    if (millis() - MemoryCheckInterval >= 700) {
      if (millis() - MemoryItemsInterval >= 5) {
        if (!up) {
          lightBr--;
          if (lightBr == 0)
            up = 1;
        } else {
          flagHistory = 1;
          lightBr++;
          if (lightBr == 250) {
            up = 0;
            flashCount++;
          }
        }
        if (!flagHistory) {
          switch (_levels) {
            case 1:
              for (int i = 16; i <= 19; i++) {
                memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, lightBr));
              }
              memory_Led.show();
              break;
            case 2:
              for (int i = 20; i <= 23; i++) {
                memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, lightBr));
              }
              memory_Led.show();
              break;
            case 3:
              for (int i = 24; i <= 27; i++) {
                memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, lightBr));
              }
              memory_Led.show();
              break;
          }
        }
        for (int i = 0; i <= 15; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, lightBr));
        }
        memory_Led.show();
        MemoryItemsInterval = millis();
      }
      if (flashCount == 2) {
        up = 0;
        lightBr = 250;
        flashCount = 0;
        _presentTimer = millis();
        _present = 0;
        _stones = 0;
        switch (_levels) {
          case 0:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 250, 0));
            }
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
            }
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 250));
            }
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 0));
            }
            memory_Led.show();
            break;
          case 1:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 0));
            }
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 250));
            }
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 250, 0));
            }
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
            }
            memory_Led.show();
            break;
          case 2:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 250));
            }
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 250, 0));
            }
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
            }
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 0));
            }
            memory_Led.show();

            break;
          case 3:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 0, 0));
            }
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 0));
            }
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 250, 0));
            }
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 250));
            }
            memory_Led.show();

            break;
        }
        repeatCount = 0;
        iterator = 0;
        symbolFade = 1;
        symbolBrightness = 0;
        symbolFadeTimer = 0;
        flagHistory = 0;
        flagMus = 0;
        _stages++;
      }
    }
  } else {
    if (lightBr > 0) {
      for (int i = 0; i <= 15; i++) {
        memory_Led.setPixelColor(i, memory_Led.Color(250, 250, 250));
      }
      memory_Led.show();
    }
    lightBr = 0;
    flagMus = 0;
    up = 0;
    flashCount = 0;
    MemoryCheckInterval = millis();
  }
}
void Game() {
  if (!_present) {
    if (millis() - _presentTimer >= Timings[_levels]) {
      if (millis() - fadeWhiteTimer >= 5) {
        lightBr -= CountsFallen[_levels];
// здесь задаем первоначальный цвет кристалов
        switch (_levels) {
          case 0:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, lightBr, 0));
}
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, 0, 0));
}
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, lightBr));
}
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, 0));
}
            memory_Led.show();

            break;

          /* --- ИЗМЕНЕНО: Закомментированы case 1 и 2 для сокращения уровней ---
          case 1:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, 0));
}
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, lightBr));
}
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, lightBr, 0));
}
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, 0, 0));
}
            memory_Led.show();

            break;
          case 2:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, lightBr));
}
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, lightBr, 0));
}
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, 0, 0));
}
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, 0));
}
            memory_Led.show();

            break;
          */ // --- КОНЕЦ ИЗМЕНЕНИЯ ---
          
          case 3:
            for (int i = 0; i <= 3; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, 0, 0));
}
            for (int i = 4; i <= 7; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(lightBr, lightBr, 0));
}
            for (int i = 8; i <= 11; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, lightBr, 0));
}
            for (int i = 12; i <= 15; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(0, 0, lightBr));
}
            memory_Led.show();

            break;
}
        fadeWhiteTimer = millis();
if (!lightBr)
          _present = 1;
}
    }
  }
  // здесь основная логика игры
  else {
    switch (_levels) {
      case 0:
        if (_stones < 1)
          firstCaseLeds();
else {
          for (int i = 16; i <= 31; i++) {
            memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
memory_Led.show();
          }
        }
        firstCaseLogic();
        break;

      /* --- ИЗМЕНЕНО: Закомментированы case 1 и 2 для сокращения уровней ---
      case 1:
        if (_stones < 1)
          secondCaseLeds();
        else {
          for (int i = 16; i <= 31; i++) {
            memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
            memory_Led.show();
          }
        }
        secondCaseLogic();
        break;
      case 2:
        if (_stones < 1)
          thirdCaseLeds();
        else {
          for (int i = 16; i <= 31; i++) {
            memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
            memory_Led.show();
          }
        }
        thirdCaseLogic();
        break;
      */ // --- КОНЕЦ ИЗМЕНЕНИЯ ---

      case 3:
        if (_stones < 1)
          fourCaseLeds();
else {
          for (int i = 16; i <= 31; i++) {
            memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
memory_Led.show();
          }
        }
        fourCaseLogic();
        break;
}
  }
}
// метод для светодиодов 1 уровень
void firstCaseLeds() {
  static unsigned long _repeatTimer = 0;
  // if (repeatCount <= 3) {
  if (millis() - _repeatTimer >= 1000) {
    if (millis() - symbolFadeTimer >= TimingsFade[_levels]) {
      if (iterator == 0) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 16; i <= 19; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 1) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 20; i <= 23; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, 0, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 2) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 24; i <= 27; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, symbolBrightness, 0));
        }
        memory_Led.show();

        symbolFadeTimer = millis();
      }
      if (iterator == 3) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 28; i <= 31; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, 0, symbolBrightness));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 4) {
        _repeatTimer = millis();
        iterator = 0;
        // repeatCount++;
        // }
      }
    }
  }
}
// метод для кристалов 1 уровень
void firstCaseLogic() {
  switch (_stones) {
    case 0:
      if (digitalRead(fourCrystal)) {
        if (millis() - MemoryItem4Interval >= 100) {
          Serial.println("true_crystal");
_stones++;
        }
      } else
        MemoryItem4Interval = millis();
mistakeStones(1, 1, 1, 0);
      break;
    case 1:
      if (digitalRead(secondCrystal)) {
        if (millis() - MemoryItem2Interval >= 100) {
          Serial.println("true_crystal");
_stones++;
        }
      } else
        MemoryItem2Interval = millis();
mistakeStones(1, 0, 1, 0);
      mistakeStonesDown(0, 0, 0, 1);
      break;
    case 2:
      if (digitalRead(firstCrystal)) {
        if (millis() - MemoryItem1Interval >= 100) {
          Serial.println("true_crystal");
_stones++;
        }
      } else
        MemoryItem1Interval = millis();
mistakeStones(0, 0, 1, 0);
      mistakeStonesDown(0, 1, 0, 1);
      break;
    case 3:
      if (digitalRead(thirdCrystal)) {
        if (millis() - MemoryItem3Interval >= 100) {
          symbolBrightness = 0;
for (int i = 0; i <= 255; i++) {

            // --- НАЧАЛО ИЗМЕНЕНИЯ: Анимация зажигает первые ДВЕ ячейки (бывшие 1 и 2) ---
            for (int j = 16; j <= 23; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(i, i, i));
            }
            // --- КОНЕЦ ИЗМЕНЕНИЯ ---

            /* --- СТАРЫЙ КОД (одна ячейка):
            for (int j = 16; j <= 19; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(i, i, i));
            }
            */

            for (int j = 24; j <= 31; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(0, 0, 0));
}
            memory_Led.show();
            delay(2);
}
          Serial.println("first_level");

          // --- НАЧАЛО ИЗМЕНЕНИЯ: Прямой переход на последний уровень (бывший 4-й) ---
          //_levels++; // Старый переход на следующий уровень
          _levels = 3; // Новый переход сразу на последний уровень
          // --- КОНЕЦ ИЗМЕНЕНИЯ ---

          _stages = 0;
          return;
}
      } else
        MemoryItem3Interval = millis();
// mistakeStones(0, 0, 0, 1);
      mistakeStonesDown(1, 1, 0, 1);
      break;
}
}
// метод для кристалов 2 уровень
void secondCaseLogic() {
  switch (_stones) {
    case 0:
      if (digitalRead(thirdCrystal)) {
        if (millis() - MemoryItem3Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem3Interval = millis();
      mistakeStones(1, 1, 0, 1);
      break;
    case 1:
      if (digitalRead(fourCrystal)) {
        if (millis() - MemoryItem4Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem4Interval = millis();
      mistakeStones(1, 1, 0, 0);
      mistakeStonesDown(0, 0, 1, 0);
      break;
    case 2:
      if (digitalRead(secondCrystal)) {
        if (millis() - MemoryItem2Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem2Interval = millis();
      mistakeStones(1, 0, 0, 0);
      mistakeStonesDown(0, 0, 1, 1);
      break;
    case 3:
      if (digitalRead(firstCrystal)) {
        if (millis() - MemoryItem1Interval >= 100) {
          symbolBrightness = 0;
          for (int i = 0; i <= 255; i++) {
            for (int j = 16; j <= 23; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(i, i, i));
            }
            for (int j = 23; j <= 31; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(0, 0, 0));
            }
            memory_Led.show();
            delay(2);
          }
          Serial.println("second_level");
          _levels++;
          _stages = 0;
          return;
        }
      } else
        MemoryItem1Interval = millis();
      // mistakeStones(0, 0, 0, 1);
      mistakeStonesDown(0, 1, 1, 1);
      break;
  }
}
// метод для светодиодов 2 уровень
void secondCaseLeds() {
  static unsigned long _repeatTimer = 0;
  // if (repeatCount <= 3) {
  if (millis() - _repeatTimer >= 1000) {
    // if (repeatCount <= 3) {
    if (millis() - symbolFadeTimer >= TimingsFade[_levels]) {

      if (iterator == 0) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 16; i <= 19; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 1) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 20; i <= 23; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, 0, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 2) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 24; i <= 27; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, 0, symbolBrightness));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 3) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 28; i <= 31; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 4) {
        _repeatTimer = millis();
        iterator = 0;
        // repeatCount++;
        // }
      }
    }
  }
}
// метод для кристалов 3 уровень
void thirdCaseLogic() {
  switch (_stones) {
    case 0:
      if (digitalRead(thirdCrystal)) {
        if (millis() - MemoryItem3Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem3Interval = millis();
      mistakeStones(1, 1, 0, 1);
      break;
    case 1:
      if (digitalRead(fourCrystal)) {
        if (millis() - MemoryItem4Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem4Interval = millis();
      mistakeStones(1, 1, 0, 0);
      mistakeStonesDown(0, 0, 1, 0);
      break;
    case 2:
      if (digitalRead(firstCrystal)) {
        if (millis() - MemoryItem1Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem1Interval = millis();
      mistakeStones(0, 1, 0, 0);
      mistakeStonesDown(0, 0, 1, 1);
      break;
    case 3:
      if (digitalRead(secondCrystal)) {
        if (millis() - MemoryItem2Interval >= 100) {
          symbolBrightness = 0;
          for (int i = 0; i <= 255; i++) {
            for (int j = 16; j <= 27; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(i, i, i));
            }
            for (int j = 28; j <= 31; j++) {
              memory_Led.setPixelColor(j, memory_Led.Color(0, 0, 0));
            }
            memory_Led.show();
            delay(2);
          }
          Serial.println("third_level");
          _levels++;
          _stages = 0;
          return;
        }
      } else
        MemoryItem2Interval = millis();
      // mistakeStones(0, 0, 0, 1);
      mistakeStonesDown(1, 0, 1, 1);
      break;
  }
}
// метод для светодиодов 3 уровня игра кристаллы
void thirdCaseLeds() {
  // if (repeatCount <= 5) {
  static unsigned long _repeatTimer = 0;
  // if (repeatCount <= 3) {
  if (millis() - _repeatTimer >= 1000) {
    if (millis() - symbolFadeTimer >= TimingsFade[_levels]) {

      if (iterator == 0) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 16; i <= 19; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, 0, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 1) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 20; i <= 23; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 2) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 24; i <= 27; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, 0, symbolBrightness));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 3) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 28; i <= 31; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 4) {
        _repeatTimer = millis();
        iterator = 0;
        // repeatCount++;
        //}
      }
    }
  }
}
// метод для кристалов 4 уровень последняя игра
void fourCaseLogic() {
  switch (_stones) {
    case 0:
      if (digitalRead(firstCrystal)) {
        if (millis() - MemoryItem1Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem1Interval = millis();
      mistakeStones(0, 1, 1, 1);
      break;
    case 1:
      if (digitalRead(fourCrystal)) {
        if (millis() - MemoryItem4Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem4Interval = millis();
      mistakeStones(0, 1, 1, 0);
      mistakeStonesDown(1, 0, 0, 0);
      break;
    case 2:
      if (digitalRead(secondCrystal)) {
        if (millis() - MemoryItem2Interval >= 100) {
          Serial.println("true_crystal");
          _stones++;
        }
      } else
        MemoryItem2Interval = millis();
      mistakeStones(0, 0, 1, 0);
      mistakeStonesDown(1, 0, 0, 1);
      break;
    case 3:
      if (digitalRead(thirdCrystal)) {
        if (millis() - MemoryItem3Interval >= 100) {
          symbolBrightness = 0;
          Serial.println("memory_room_end");
          for (long firstPixelHue = 0; firstPixelHue < 2 * 65536; firstPixelHue += 556) {
            for (int i = 0; i < memory_Led.numPixels(); i++) {  // For each pixel in strip...
              int pixelHue = firstPixelHue + (i * 65536L / memory_Led.numPixels());
              memory_Led.setPixelColor(i, memory_Led.gamma32(memory_Led.ColorHSV(pixelHue)));
            }
            memory_Led.show();
            delay(20);  // Pause for a moment
          }

          _levels++;
          _stages = 0;
          for (int i = 255; i >= 50; i--) {
            memory_Led.setBrightness(i);
            memory_Led.show();
            delay(2);
          }
          OpenLock(CrimeDoor);
          level++;
          crimeHelpTimer++;
          return;
        }
      } else
        MemoryItem3Interval = millis();
      // mistakeStones(0, 0, 0, 1);
      mistakeStonesDown(1, 1, 0, 1);
      break;
  }
}
// метод для светодиодов 4 уровень последней игры
void fourCaseLeds() {
  static unsigned long _repeatTimer = 0;
  // if (repeatCount <= 3) {
  if (millis() - _repeatTimer >= 1000) {
    // if (repeatCount <= 8) {
    if (millis() - symbolFadeTimer >= TimingsFade[_levels]) {

      if (iterator == 0) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 16; i <= 19; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, 0, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 1) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 20; i <= 23; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, 0, symbolBrightness));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 2) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 24; i <= 27; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(symbolBrightness, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 3) {
        if (symbolFade) {
          symbolBrightness += CountsFade[_levels];
          if (symbolBrightness == 255)
            symbolFade = 0;
        } else {
          symbolBrightness -= CountsFade[_levels];
          if (symbolBrightness == 0) {
            symbolFade = 1;
            iterator++;
          }
        }
        for (int i = 28; i <= 31; i++) {
          memory_Led.setPixelColor(i, memory_Led.Color(0, symbolBrightness, 0));
        }
        memory_Led.show();
        symbolFadeTimer = millis();
      }
      if (iterator == 4) {
        _repeatTimer = millis();
        iterator = 0;
        // repeatCount++;
        // }
      }
    }
  }
}

// неверно поставили кристал
void mistakeStones(bool _stone1, bool _stone2, bool _stone3, bool _stone4) {
  static int mistakeCount = 0;
  if (_stone1) {
    if (!digitalRead(firstCrystal)) {
      MemoryItem1Interval = millis();
    } else {
      if (millis() - MemoryItem1Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            // Serial.println("mistake_crystal");
            //_levels = 0;
            _stages = 0;
            return;
          }
        }
      }
    }
  }
  if (_stone2) {
    if (!digitalRead(secondCrystal)) {
      MemoryItem2Interval = millis();
    } else {
      if (millis() - MemoryItem2Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
  if (_stone3) {
    if (!digitalRead(thirdCrystal)) {
      MemoryItem3Interval = millis();
    } else {
      if (millis() - MemoryItem3Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
  if (_stone4) {
    if (!digitalRead(fourCrystal)) {
      MemoryItem4Interval = millis();
    } else {
      if (millis() - MemoryItem4Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
}
// если не верно вытащили кристал
void mistakeStonesDown(bool _stone1, bool _stone2, bool _stone3, bool _stone4) {
  static int mistakeCount = 0;
  if (_stone1) {
    if (digitalRead(firstCrystal)) {
      MemoryItem1Interval = millis();
    } else {
      if (millis() - MemoryItem1Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
  if (_stone2) {
    if (digitalRead(secondCrystal)) {
      MemoryItem2Interval = millis();
    } else {
      if (millis() - MemoryItem2Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
  if (_stone3) {
    if (digitalRead(thirdCrystal)) {
      MemoryItem3Interval = millis();
    } else {
      if (millis() - MemoryItem3Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
  if (_stone4) {
    if (digitalRead(fourCrystal)) {
      MemoryItem4Interval = millis();
    } else {
      if (millis() - MemoryItem4Interval >= 100) {
        Serial.println("mistake_crystal");
        for (int l = 0; l <= 1; l++) {
          for (int j = 0; j <= 250; j++) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          for (int j = 250; j >= 0; j--) {
            for (int i = 0; i <= 31; i++) {
              memory_Led.setPixelColor(i, memory_Led.Color(j, 0, 0));
            }
            memory_Led.show();
            delay(3);
          }
          if (l == 1) {
            //_levels = 0;
            _stages = 0;
            // Serial.println("mistake_crystal");
            return;
          }
        }
      }
    }
  }
}

// мерим напряжение шлем на сервак
void VoltageDisplay() {
  if (millis() - voltageTimer >= 5000) {
    int Voltage = analogRead(voltagePin);
    float volt = 0.0048 * Voltage * 4.7;
    // Serial.println(Voltage);
    if (volt >= 13.30 && volt <= 15.50) {
      string = "HIGH";
    }
    if (volt >= 12.40 && volt < 13.30) {
      string = "NORMAL";
    }
    if (volt < 12.40) {
      string = "LOW";
    }
    Serial.println(string);
    Serial.println(volt);
    voltageTimer = millis();
  }
}

void HelpButton(String help) {
}

void RestOn() {
  static bool _dataQueue = 0;
  static unsigned long _towerTimer = 0;
  static byte _towerCounter = 0;
  static bool doorEvent = 0;
  static bool mansardEvent = 0;
  static bool cauldronEvent = 0;
  static bool scroll1Event = 0;
  static bool scroll2Event = 0;
  static bool scroll3Event = 0;
  static bool scroll4Event = 0;
  static bool scroll5Event = 0;
  static bool sealEvent = 0;
  static bool sealSpaceEvent = 0;
  static bool finalEvent = 0;
  static unsigned long bugTimerScroll = 0;
  boyServo.detach();
  delay(500);
  // опрашиваем сам замок на предметы которые могли оставить
  if (digitalRead(startDoorPin)) {
    if (!doorEvent) {
      //Serial.println("open_door");
      // delay(50);
      doorEvent = 1;
    }
  }
  if (!digitalRead(startDoorPin)) {
    if (doorEvent) {
      //Serial.println("close_door");
      // delay(50);
      doorEvent = 0;
    }
  }


  // по опчереди отправляем рестарт всем башням
  Serial1.println("restart");
  delay(100);
  Serial2.println("restart");
  delay(100);
  Serial3.println("restart");
  delay(100);
  mySerial.println("restart");
  delay(100);

  Serial.println("restart");
  OpenAll();
  level = 0;

  // опрашиваем все башни если есть что полезное шлем на сервак
}

void Restart() {
  if (Serial.available()) {
    String buff = Serial.readString();
    if (buff == "open_mansard_door") {
      OpenDoor(MansardDoor);
    }
    if (buff == "open_bank_door") {
      OpenDoor(BankDoor);
    }
    if (buff == "open_potion_door") {
      OpenDoor(PotionsRoomDoor);
    }
    if (buff == "open_owl_door") {
      mySerial.println("open_door");
    }
    if (buff == "open_dog_door") {
      Serial3.println("open_door");
    }
    if (buff == "open_low_tower_door") {
      OpenDoor(HightTowerDoor);
    }
    if (buff == "open_high_tower_door") {
      OpenDoor(HightTowerDoor2);
    }
    if (buff == "open_library_door") {
      OpenDoor(LibraryDoor);
    }
    if (buff == "open_workshop_door") {
      Serial1.println("open_door");
    }
    if (buff == "open_safe_door") {
      OpenDoor(BankStashDoor);
    }
    if (buff == "open_memory_door") {
      OpenDoor(MemoryRoomDoor);
    }
    if (buff == "open_basket_door") {
      Serial2.println("open_door");
    }
    if (buff == "open_mine_door") {
      Serial2.println("open_mine_door");
    }
    if (buff == "ready") {
      Serial1.println("ready");
      delay(500);
      Serial1.println("ready");
      Serial2.println("ready");
      delay(500);
      Serial2.println("ready");
      Serial3.println("ready");
      delay(500);
      Serial3.println("ready");
      mySerial.println("ready");
      delay(500);
      mySerial.println("ready");
      digitalWrite(MansardLight, LOW);
      digitalWrite(MansardLight, LOW);
      digitalWrite(LastTowerTopLight, LOW);
      digitalWrite(Fireworks, LOW);
      digitalWrite(BankRoomLight, LOW);
      digitalWrite(TorchLight, LOW);
      digitalWrite(HallLight, LOW);
      digitalWrite(UfHallLight, LOW);
      digitalWrite(LibraryLight, LOW);
      for (int i = 0; i <= 200; i++) {
        CandleStrip.setPixelColor(i, CandleStrip.Color(0, 0, 0));
        CauldronStrip.setPixelColor(i, CauldronStrip.Color(0, 0, 0));
        CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 0, 0));
        memory_Led.setPixelColor(i, memory_Led.Color(0, 0, 0));
        strip1.setPixelColor(i, strip1.Color(0, 0, 0));
        strip2.setPixelColor(i, strip2.Color(0, 0, 0));
        GoldStrip.setPixelColor(i, GoldStrip.Color(0, 0, 0));
        CandleStrip.setPixelColor(i, CandleStrip.Color(0, 0, 0));
      }
      CandleStrip.show();
      CauldronStrip.show();
      CauldronRoomStrip.show();
      memory_Led.show();
      //strip1.show();
      //strip2.show();
      GoldStrip.show();
      delay(2000);
      level = 0;
      return;
    }
  }
  digitalWrite(MansardLight, HIGH);
  digitalWrite(LastTowerTopLight, HIGH);
  digitalWrite(Fireworks, LOW);
  digitalWrite(BankRoomLight, HIGH);
  digitalWrite(TorchLight, HIGH);
  digitalWrite(HallLight, HIGH);
  digitalWrite(UfHallLight, HIGH);
  digitalWrite(LibraryLight, HIGH);
  for (int i = 0; i <= 200; i++) {
    CandleStrip.setPixelColor(i, CandleStrip.Color(255, 255, 255));
    CauldronStrip.setPixelColor(i, CauldronStrip.Color(255, 255, 255));
    CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 255, 255));
    memory_Led.setPixelColor(i, memory_Led.Color(255, 255, 255));
    strip1.setPixelColor(i, strip1.Color(255, 255, 255));
    strip2.setPixelColor(i, strip2.Color(255, 255, 255));
    GoldStrip.setPixelColor(i, GoldStrip.Color(255, 255, 255));
  }
  CandleStrip.show();
  CauldronStrip.show();
  CauldronRoomStrip.show();
  memory_Led.show();
  GoldStrip.show();
  //strip1.show();
  //strip2.show();
  _presentTimer;
  _stages = 0;
  _present = 0;
  lightBr = 0;
  //_trollDoor = 0;
  iterator = 0;
  symbolFade = 1;
  flagSalut = 1;
  _levels = 0;
  _stones = 0;
  symbolBrightness = 0;
  symbolFadeTimer = 0;
  fadeWhiteTimer = 0;
  MemoryCheckInterval;
  MemoryItem1Interval;
  MemoryItem2Interval;
  MemoryItem3Interval;
  MemoryItem4Interval;
  MemoryItemsInterval;
  voltageTimer;
  lightFlashTimer;
  rainbowCyclesPreviousMillis = 0;
  pixelsInterval = 10;
  rainbowCycles = 0;
  rainbowCycleCycles = 0;
  repeatCount = 0;

  //----------------------------------
  // флажки состояний
  tumbStateFlag;  // флажок для тумблера что бы знать состояние до уровня
  FirstFlag = 0;
  SecondFlag = 0;
  ThirdFlag = 0;
  FourFlag = 0;
  KnockState = 0;
  upHelp;
  light;
  FirstBottleTrue = 1;
  FirstBottleFalse = 1;
  SecondBottleTrue = 1;
  SecondBottleFalse = 1;
  ThirdBottleTrue = 1;
  ThirdBottleFalse = 1;
  FourBottleTrue = 1;
  FourBottleFalse = 1;
  helpFlag;
  flagSound = 1;
  FireInterval = 0;
  CauldronFireInterval = 0;
  CauldronFireMistakeInterval = 0;
  CauldronFireTrueInterval = 0;
  KnockInterval = 0;
  KnockIntervalLow = 0;
  Bottle1Timer = 0;
  Bottle2Timer = 0;
  Bottle3Timer = 0;
  Bottle4Timer = 0;
  sealSpaceTimer;
  doorDef;
  doorTimer;
  finalPresentation;
  scrollNumber = 0;  // последовательность свитков
  ////////////
  Bottle = 0;
  BotScore = "";
  OwlScore = 0;
  NumKnock = 0;
  doorFlags = 1;
  flagStory = 1;

  previousMillis = 0;
  interval = 10;               // Интервал обновления (мс)
  directionChangeTime = 3000;  // Время до смены направления (мс)
  lastDirectionChange = 0;
  enemyTimer = 0;
  additionalTimer = 0;
  cometTimer1 = 0;
  dragonTimer;
  direction = 1;  // 1 - вперед, -1 - назад
  headPosition = 0;
  // Цвет кометы (золотой)
  cometColor = strip1.Color(255, 215, 0);   // RGB для золотого цвета
  backgroundColor = strip1.Color(0, 0, 0);  // Черный фон
  enemyFlag = 0;

  // Параметры кометы
  cometLength = 5;               // Длина кометы в диодах
  cometPosition = -cometLength;  // Начальная позиция кометы
  totalLEDs = 252;               // Общее количество светодиодов

  clickFlag1 = 0;
  clickFlag2 = 0;
  clickFlag3 = 0;
  clickFlag4 = 0;
  clickFlag11 = 0;
  clickFlag22 = 0;
  clickFlag31 = 0;
  clickFlag41 = 0;
  snitchFlag = 1;
  stepsFlag = 0;
  centralTowerGameFlag = 0;
  fireFlag = 0;

  code1Timer = 0;
  code2Timer = 0;
  code3Timer = 0;
  code4Timer = 0;
  libraryDoorTimer = 0;
  Scroll1On = 0;
  Scroll2On = 0;
  Scroll3On = 0;
  Scroll4On = 0;
  Scroll11On = 0;
  Scroll21On = 0;
  Scroll31On = 0;
  Scroll41On = 0;

  isPotionEnd = 0;
  isDogEnd = 0;
  isOwlEnd = 0;
  isTrainEnd = 0;
  isTrollEnd = 0;
  isTrainBasket = 0;
  ghostState = 0;
}
// метод для открытия дверей
void Unlocks(String buff) {
  Serial1.println(buff);
  Serial2.println(buff);
  Serial3.println(buff);
  mySerial.println(buff);
  if (buff == "m1lck") {  // ок
    OpenLock(MansardDoor);
  }
  if (buff == "m2lck") {  // ок
    //OpenLock(MansardStashDoor);
  }
  if (buff == "rlb2lck") {  // ок
    OpenLock(PotionsRoomDoor);
  }
  if (buff == "rrt3lck") {  // ok
    OpenLock(LibraryDoor);
  }
  if (buff == "rlt2lck") {  // ok
    OpenLock(BankDoor);
  }
  if (buff == "rlt3lck") {  // ok
    OpenLock(BankStashDoor);
  }
  if (buff == "ht2lck") {  // ok
    OpenLock(HightTowerDoor);
  }
  if (buff == "rrb7lck") {  // ok
  }
  if (buff == "ht4lck") {  // ok
    OpenLock(MemoryRoomDoor);
  }
  if (buff == "open_stash") {
    // OpenLock(MansardStashDoor);
    OpenLock(BankStashDoor);
  }
  if (buff == "demoOn") {
    level = 30;
  }
}

void BasketEffectLesson() {
  if (!snitchFlag) {
    if (millis() - cometTimer1 >= 1000) {
      interval = interval + random(-4, 4);
      if (interval < 2) {
        interval = 1;
      }
      cometTimer1 = millis();
    }

    // Проверяем, нужно ли обновлять позицию
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      updateComet();
    }

    // Проверяем, нужно ли изменить направление
    if (millis() - lastDirectionChange >= random(2000, 4000)) {
      lastDirectionChange = millis();
      direction *= -1;  // Меняем направление
    }
  }
  //cometTimer = millis();
}

void BasketEffect() {
  if (!snitchFlag) {
    if (millis() - enemyTimer >= random(10000, 20000)) {
      if (!enemyFlag) {
        enemyFlag = 1;
        Serial.println("red_ball");
        interval = 1;
        additionalTimer = millis();
      }
    }
    if (millis() - additionalTimer >= 5000 && enemyFlag) {
      snitchFlag = 1;
      LooseSnitch();
      Serial2.println("start_basket_robot");
      return;
    }

    if (millis() - cometTimer1 >= 1000) {
      interval = interval + random(-4, 4);
      if (interval < 2) {
        interval = 1;
      }
      if(enemyFlag){
        interval = 1;
      }
      cometTimer1 = millis();
    }

    // Проверяем, нужно ли обновлять позицию
    if (millis() - previousMillis >= interval) {
      previousMillis = millis();
      updateComet();
    }

    // Проверяем, нужно ли изменить направление
    if (millis() - lastDirectionChange >= random(2000, 4000)) {
      lastDirectionChange = millis();
      direction *= -1;  // Меняем направление
    }
  }
  //cometTimer = millis();
}

void updateComet() {
  // Очищаем все светодиоды
  static unsigned long buttonTimer1;
  static unsigned long buttonTimer2;
  static unsigned long buttonTimer3;
  static unsigned long buttonTimer4;
  strip1.clear();
  strip2.clear();

  for (int i = 0; i < cometLength; i++) {
    int pos = cometPosition + i;
    float fade;
    if (direction < 0) {
      fade = 1.0 - (float)(i) / (cometLength * 0.7);
    } else {
      fade = 1.0 - (float)(cometLength - i) / (cometLength * 0.7);
    }

    fade = constrain(fade, 0.05, 1.0);

    uint32_t fadedColor;
    // Рассчитываем цвет с учетом затухания
    if (!enemyFlag) {
      uint8_t r = (uint8_t)(255 * fade);
      uint8_t g = (uint8_t)(215 * fade);
      uint8_t b = 0;
      fadedColor = strip1.Color(r, g, b);
    } else {
      uint8_t r = (uint8_t)(255 * fade);
      uint8_t g = 0;
      uint8_t b = 0;
      fadedColor = strip1.Color(r, g, b);
    }

    if (pos < 0 && pos >= -32) {
      strip2.setPixelColor(pos + 65, fadedColor);
    }
    if (pos < -32 && pos >= -107) {
      strip1.setPixelColor(pos + 184, fadedColor);
    }
    if (pos < -107 && pos >= -140) {
      strip2.setPixelColor(pos + 139, fadedColor);
    }
    if (pos < -140 && pos >= -217) {
      strip1.setPixelColor(pos + 218, fadedColor);
    }



    if (pos >= 0 && pos < 78) {
      // Комет на первой ленте
      strip1.setPixelColor(pos, fadedColor);
    } else if (pos >= 78 && pos < 111) {
      // Комет перешел на вторую ленту
      strip2.setPixelColor(pos - 78, fadedColor);
    } else if (pos >= 111 && pos < 185) {
      // Комет вышел за пределы второй ленты - начинаем сначала
      strip1.setPixelColor(pos - 33, fadedColor);
    } else if (pos >= 185 && pos < 217) {
      // Комет вышел за пределы второй ленты - начинаем сначала
      strip2.setPixelColor(pos - 152, fadedColor);
    }
    if (pos == 217) {
      cometPosition = 0;
    }
    if (pos < -217) {
      cometPosition = 0;
    }
  }

  // Обновляем позицию головы
  //headPosition += direction;

  if (digitalReadExpander(6, board2)) {
    if (!clickFlag1) {
      clickFlag1 = 1;
      buttonTimer1 = millis();
      if ((cometPosition >= 23 && cometPosition <= 29) || (cometPosition >= -193 && cometPosition <= -186)) {
        CatchSnitch();
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch1");
        } else {
          Serial.println("catch1");
        }

        Serial2.println("start_basket");
      }
    }
  } else {
    clickFlag1 = 0;
  }

  if (digitalReadExpander(5, board2)) {
    if (!clickFlag2) {
      clickFlag2 = 1;
      buttonTimer2 = millis();
      if ((cometPosition >= 123 && cometPosition <= 129) || (cometPosition >= -92 && cometPosition <= -86)) {
        CatchSnitch();
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch2");
        } else {
          Serial.println("catch2");
        }
        Serial2.println("start_basket");
      }
    }
  } else {
    clickFlag2 = 0;
  }

  if (digitalReadExpander(5, board4)) {
    if (!clickFlag4) {
      Serial.println("Press");
      clickFlag4 = 1;
      buttonTimer4 = millis();
      if ((cometPosition >= 92 && cometPosition <= 98) || (cometPosition >= -123 && cometPosition <= -116)) {
        CatchSnitch();
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch3");
        } else {
          Serial.println("catch3");
        }
        Serial2.println("start_basket");
      }
    }
  } else {
    clickFlag4 = 0;
  }

  if (digitalReadExpander(6, board4)) {
    if (!clickFlag3) {
      clickFlag3 = 1;
      buttonTimer3 = millis();
      if ((cometPosition >= 186 && cometPosition <= 193) || (cometPosition >= -29 && cometPosition <= -22)) {
        CatchSnitch();
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch4");
        } else {
          Serial.println("catch4");
        }
        Serial2.println("start_basket");
      }
    }
  } else {
    clickFlag3 = 0;
  }


  if (millis() - buttonTimer1 <= 200 && clickFlag1) {
    strip1.setPixelColor(29, strip1.Color(0, 0, 255));
    strip1.show();
  }
  if (millis() - buttonTimer2 <= 200 && clickFlag2) {
    strip1.setPixelColor(96, strip1.Color(0, 0, 255));
    strip1.show();
  }
  if (millis() - buttonTimer3 <= 200 && clickFlag3) {
    strip2.setPixelColor(40, strip2.Color(0, 0, 255));
    strip2.show();
  }
  if (millis() - buttonTimer4 <= 200 && clickFlag4) {
    strip2.setPixelColor(21, strip2.Color(0, 0, 255));
    strip2.show();
  }

  strip1.show();
  strip2.show();
  cometPosition += direction;
  //Serial.println(cometPosition);
}

bool digitalReadExpander(int pin, int boardNumber) {
  digitalWrite(pinA, pin & 0b001);
  digitalWrite(pinB, (pin >> 1) & 0b001);
  digitalWrite(pinC, (pin >> 2) & 0b001);
  delay(1);
  //delayMicroseconds(10);
  return digitalRead(boardNumber);
}

void CatchSnitch() {
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(0, 255, 0));
    strip1.setPixelColor(i, strip1.Color(0, 255, 0));
  }
  strip1.show();
  strip2.show();
  delay(200);
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(0, 0, 0));
    strip1.setPixelColor(i, strip1.Color(0, 0, 0));
  }
  strip1.show();
  strip2.show();
  delay(500);
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(0, 255, 0));
    strip1.setPixelColor(i, strip1.Color(0, 255, 0));
  }
  strip1.show();
  strip2.show();
}

void LooseSnitch() {
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(255, 0, 0));
    strip1.setPixelColor(i, strip1.Color(255, 0, 0));
  }
  strip1.show();
  strip2.show();
  delay(200);
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(0, 0, 0));
    strip1.setPixelColor(i, strip1.Color(0, 0, 0));
  }
  strip1.show();
  strip2.show();
  delay(500);
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(255, 0, 0));
    strip1.setPixelColor(i, strip1.Color(255, 0, 0));
  }
  strip1.show();
  strip2.show();
}
