
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

String serial1Buffer = "";
String serial2Buffer = "";
String serial3Buffer = "";
String mySerialBuffer = "";

bool isUfBlinking = false;
int ufBlinkCount = 0;
unsigned long ufBlinkTimer = 0;
const int UF_BLINK_TOTAL = 6; // 3 включения + 3 выключения
bool isRainbowActive = false;
long rainbowHue = 0;
unsigned long rainbowTimer = 0;

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
// Флаги активации дверей/этапов для подсказки hint_11_z ---

bool isPotionDoorOpened = false;  // Дверь ведьмы (рыба) открыта

bool isDogDoorOpened = false;  // Дверь рыцаря (собака/ключ) открыта

bool isOwlDoorOpened = false;  // Дверь гнома (сова) открыта

bool isStartDoorOpenedGlobal = false; // Флаг физического открытия стартовой двери для подсказок

bool isTrainStarted = false;  // Игра с поездом (после проектора) активна

bool lessonSaluteActive = false; // Флаг для анимации салюта в уроке
bool discoBallsActive = false;   // Флаг для диско-шаров при голе
unsigned long discoBallsTimer = 0; // Таймер для диско-шаров
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

// Глобальные флаги состояния для PowerOn() и RestOn()
// --- ДОБАВЛЕНО: Таймер для состояния прослушивания ---
unsigned long readyListenTimer = 0;
const unsigned long READY_LISTEN_DURATION = 3000; // 3 секунды для прослушивания ответов от башен
// ---
bool _dataQueue = 0;
unsigned long _towerTimer = 0;
byte _towerCounter = 0;
bool doorEvent = 0;
bool mansardEvent = 0;
bool libraryEvent = 0;
bool galet1Event = 0;
bool galet2Event = 0;
bool galet3Event = 0;
bool galet4Event = 0;
bool galet5Event = 0;
bool sealEvent = 0;
bool sealSpaceEvent = 0;
bool finalEvent = 0;
bool crimeEvent = 0;
bool safeEvent = 0;
unsigned long bugTimerScroll = 0;
bool firstRun = true;
bool helpsBankTimerWaiting = false; // Флаг: ждем, пока сервер доиграет подсказку
bool isBankerFirstHint = true;
bool isRestInitialized = false; // Флаг для однократной инициализации режима Rest

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
byte previousLevel = 0; // Переменная для отслеживания смены уровня ---
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

// Переменные для мерцания света в библиотеке
bool isLibraryFlickering = false;
unsigned long libraryFlickerTimer = 0;
unsigned long libraryFlickerIntervalTimer = 0;

String dragonHints[] = { "dragon_crystal", "hint_2_b", "hint_2_c", "hint_2_z", "hint_5_b", "hint_5_c" };
String studentHints[] = { "hint_3_b", "hint_3_c", "hint_3_z", "hint_37_b", "hint_37_c", "hint_38_b", "hint_44_b", "hint_44_c" };
String professorHints[] = { "hint_6_b", "hint_6_c", "hint_10_b", "hint_10_c", "hint_11_b", "hint_11_c", "hint_11_z" };
String dwarfHints[] = { "hint_14_b", "hint_14_c", "hint_14_z" };
String witchHints[] = { "hint_17_b", "hint_17_c", "hint_17_z" };
String knightHints[] = { "hint_19_b", "hint_19_c", "hint_19_z" };
String goblinHints[] = { "hint_23_b", "hint_23_c", "hint_23_z" };
String trollHints[] = { "hint_26_b", "hint_26_c", "hint_26_z" };
String workshopHints[] = { "hint_32_b", "hint_32_c", "hint_32_d", "hint_32_e", "hint_32_z" };
String directorHints[] = { "hint_49_b", "hint_49_c", "hint_50_b", "hint_50_c", "hint_51_b", "hint_51_c", "hint_56_b" };

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
// Глобальные переменные для отслеживания состояния "мальчика" в режиме ожидания ---
bool boyStateInitialized = false;
bool lastBoyState = true; // По умолчанию 'out' (true)
unsigned long boyCheckTimer = 0;

void MagicEffect() {
  
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
        for (int j = max(0, i - clusterSize / 2); j <= min(n - 1, i + clusterSize / 2); j++) {
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
                               max(clusterB, existingB));
        }
      }
    }

    // Добавление ярких белых искр-вспышек
    if (random(1000) < 15) {
      int sparkPos = random(n);
      byte intensity = random(200, 255);

      // Создание небольшой белой вспышки
      for (int i = max(0, sparkPos - 1); i <= min(n - 1, sparkPos + 1); i++) {
        float distance = abs(i - sparkPos) / 2.0;
        byte sparkIntensity = intensity * (1.0 - distance);
        strip->setPixelColor(i, sparkIntensity, sparkIntensity, sparkIntensity);
      }
    }

    strip->show();
  }
}

void handleUfBlinking() {
  // Если мигание не активно, ничего не делаем
  if (!isUfBlinking) {
    return;
  }

  // Проверяем, прошло ли 500 мс
  if (millis() - ufBlinkTimer >= 500) {
    ufBlinkTimer = millis(); // Сбрасываем таймер

    if (ufBlinkCount % 2 == 0) {
      // Четный шаг: включаем свет
      digitalWrite(UfHallLight, HIGH);
    } else {
      // Нечетный шаг: выключаем свет
      digitalWrite(UfHallLight, LOW);
    }

    ufBlinkCount++; // Увеличиваем счетчик шагов

    // Если мы выполнили все 6 шагов (3 вкл + 3 выкл)
    if (ufBlinkCount >= UF_BLINK_TOTAL) {
      isUfBlinking = false; // Останавливаем мигание
      digitalWrite(UfHallLight, LOW); // Гарантированно выключаем свет 
      ufBlinkCount = 0;
    }
  }
}

///////////////////////////////48 свободный
void setup() {
  // ДОБАВЛЕНО: Лог о начале загрузки
  Serial.begin(115200); // Начинаем серийный порт как можно раньше
  Serial.println("log:main:Booting up...");
  delay(20); // Небольшая задержка, чтобы лог успел уйти
  // КОНЕЦ
  pinMode(firstCrystal, INPUT_PULLUP);
  pinMode(secondCrystal, INPUT_PULLUP);
  pinMode(thirdCrystal, INPUT_PULLUP);
  pinMode(fourCrystal, INPUT_PULLUP);

  pinMode(WindowSens, INPUT_PULLUP);

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
  for (int i = 0; i < DOORS; i++) {
    digitalWrite(doors[i], LOW);
  }
  ///--------------------
  ///////порты uart
  Serial.begin(115200);  // raspberry
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
  pinMode(board1, INPUT_PULLUP);
  pinMode(board2, INPUT_PULLUP);
  pinMode(board3, INPUT_PULLUP);
  pinMode(board4, INPUT_PULLUP);

  boyServo.attach(49);
  boyServo.write(0);
  boyServo.detach();
}
void loop() {
  MonitorSerialBuffer(); // монитор буферизации
  handleLocks();
  handleUfBlinking();
  handleLibraryFlicker();
  handleRainbow();
  // Таймер эффекта ПОСЛЕ ГОЛА (Зеленая волна)
  if (discoBallsActive) {
    // Проверяем, не прошло ли 10 секунд
    if (millis() - discoBallsTimer >= 10000) { 
      // ВРЕМЯ ВЫШЛО: Выключаем всё
      discoBallsActive = false;
      
      digitalWrite(Fireworks, LOW); // ВЫКЛЮЧАЕМ РЕЛЕ ДИСКО-ШАРОВ
      
      snitchFlag = 0; 
      enemyTimer = millis();
      additionalTimer = millis();
      enemyFlag = 0;
      
      // Очищаем ленту от зеленой волны
      strip1.clear(); strip2.clear(); strip1.show(); strip2.show();
    } else {
      // ВРЕМЯ ИДЕТ: Держим всё включенным
      
      digitalWrite(Fireworks, HIGH); // ДЕРЖИМ РЕЛЕ ВКЛЮЧЕННЫМ
      
      // И запускаем анимацию на ленте (она неблокирующая)
      GreenWaveEffect(); 
    }
  }
  if (level != previousLevel) {
        // ДОБАВЛЕНО: Логирование смены состояния
        Serial.print("log:main:State changed to ");
        Serial.println(level);
        delay(10); // ДОБАВЛЕНО: Задержка между логом и командой
        // КОНЕЦ
    Serial.print("level_"); // Отправляем префикс
    Serial.println(level);  // Отправляем номер нового уровня
    previousLevel = level;  // Обновляем "предыдущий" уровень
  }
  // Если мы НЕ в режиме ожидания (уровень 25), сбрасываем флаг инициализации
  if (level != 25) {
    isRestInitialized = false;
  }
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
      while (Serial.available()) {
        ClearSatelliteBuffers();
        String buff = Serial.readStringUntil('\n');
        buff.trim();
        if (buff.indexOf("restart") != -1) {
          // [FIX] Добавляем задержки при рассылке, как в RestOn
          Serial1.println("restart"); delay(20);
          Serial2.println("restart"); delay(20);
          Serial3.println("restart"); delay(20);
          mySerial.println("restart"); delay(50);
          mySerial.println("restart");
          
          OpenAll();
          RestOn();
          
          // [ВАЖНО] Сбрасываем флаг инициализации RestOn
          isRestInitialized = false; 
          
          level = 25;
          break; 
        }
      }
      // Если после обработки команд уровень изменился (на 25), 
      // НЕ ЗАПУСКАЕМ FinalPresentation!
      if (level == 20) {
         FinalPresentation();
      }
      break;
    case 21:
      while (Serial.available()) {
        String buff = Serial.readStringUntil('\n');
        buff.trim();
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
    // --- ДОБАВЛЕНО: Новое состояние для прослушивания башен после команды ready ---
    case 26:
      ListenForReady();
      break;
    // ---
    case 30:
      Restart();
      break;
  }
  //DoorDefender();
  if (fireFlag) {
    FireCup();
  }

  if (level > 0 && level < 20) {
    HelpHandler("");
  }

  //VoltageDisplay();
}

// Функция призрачного мерцания ---
void handleLibraryFlicker() {
  // Если мерцание не активно, выходим
  if (!isLibraryFlickering) {
    return;
  }

  unsigned long currentMillis = millis();

  // 1. Проверяем, не прошло ли 5 секунд
  if (currentMillis - libraryFlickerTimer >= 5000) {
    isLibraryFlickering = false;
    // После мерцания оставляем свет включенным
    digitalWrite(LibraryLight, HIGH); 
    return;
  }

  // 2. Логика "Свеча на ветру"
  if (currentMillis - libraryFlickerIntervalTimer >= (150 + random(0, 200))) {
    libraryFlickerIntervalTimer = currentMillis;

    // С шансом 60% (6 из 10) "задуваем" свечу
    if (random(10) < 6) { 
      digitalWrite(LibraryLight, LOW);
      // Сразу же устанавливаем таймер, чтобы включить ее обратно
      // через время (1000-2000 мс)
      libraryFlickerIntervalTimer = currentMillis + random(1000, 2000);
    } else {
      // В остальных 40% случаев свеча горит ровно
      digitalWrite(LibraryLight, HIGH);
    }
  }
}
// метод открывания тайников и дверей каждая после своего уровня и до конца игры

// ---------------------------------------------------------------------------------
// ПРИЧИНА: Ранее эта функция постоянно опрашивала все датчики и башни,
// отправляя множество сообщений на сервер и вызывая "флатер" состояний.
// Теперь она только ожидает и обрабатывает входящие команды, не занимаясь
// активным поллингом, что делает систему более стабильной в режиме ожидания.
// ---------------------------------------------------------------------------------
void PowerOn() {
  handleIdleBoySensor();
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();

    // ДОБАВЛЕНО: Логирование входящей команды от сервера
    if (buff.length() > 0) {
      Serial.print("log:main:PowerOn: Received command: ");
      Serial.println(buff);
      delay(10); // Задержка для разделения лога и возможной пересылки
    }
    // КОНЕЦ

    // Сначала проверяем приоритетные команды через indexOf для надежности
    if (buff.indexOf("restart") != -1) {
      Serial1.println("restart");
      Serial2.println("restart");
      Serial3.println("restart");
      mySerial.println("restart");
      OpenAll(); 
      RestOn();
      level = 25;
      return;
    }
    
    if (buff.indexOf("ready") != -1) {
       // ... (код ready как был, но с защитой от повтора) ...
       _dataQueue = 0; _towerTimer = 0; _towerCounter = 0; doorEvent = 0;
       mansardEvent = 0; libraryEvent = 0; galet1Event = 0; galet2Event = 0;
       galet3Event = 0; galet4Event = 0; galet5Event = 0; sealEvent = 0;
       sealSpaceEvent = 0; finalEvent = 0; bugTimerScroll = 0;
       
       Serial1.println("ready"); Serial2.println("ready");
       Serial3.println("ready"); mySerial.println("ready");

       if (digitalRead(startDoorPin)) Serial.println("open_door");
       if (!digitalRead(galetSwitchesPin)) Serial.println("galet_on");
       if (digitalReadExpander(4, board4)) Serial.println("cristal_up");
       if (digitalReadExpander(7, board1)) Serial.println("boy_out");
       if (digitalReadExpander(5, board3)) Serial.println("lib_door");
       if (digitalReadExpander(1, board2)) Serial.println("crime_open");
       if (digitalReadExpander(3, board3) && digitalReadExpander(0, board3) && digitalReadExpander(1, board3) && digitalReadExpander(2, board3)) Serial.println("safe_open");
       
       digitalWrite(MansardLight, LOW); digitalWrite(LastTowerTopLight, LOW);
       digitalWrite(Fireworks, LOW); digitalWrite(BankRoomLight, LOW);
       digitalWrite(TorchLight, LOW); digitalWrite(HallLight, LOW);
       digitalWrite(UfHallLight, LOW); digitalWrite(LibraryLight, LOW);
       for (int s = 0; s < STRIPS; s++) { strips[s]->clear(); strips[s]->show(); }
       
       boyStateInitialized = false;
       readyListenTimer = millis();
       level = 26; 
       return;
    }

    if (buff == "start") {
      boyServo.attach(49); boyServo.write(0);
      delay(500);boyServo.detach(); Serial.println("modalend"); delay(100);
      mySerial.println("start"); Serial1.println("start");
      Serial2.println("start"); Serial3.println("start");
      delay(1000); Serial.println("startgo");
      // ... (сброс переменных) ...
      _dataQueue = 0; crimeEvent = 0; safeEvent = 0; _towerTimer = 0;
      _towerCounter = 0; doorEvent = 0; mansardEvent = 0; libraryEvent = 0;
      galet1Event = 0; galet2Event = 0; galet3Event = 0; galet4Event = 0;
      galet5Event = 0; sealEvent = 0; sealSpaceEvent = 0; finalEvent = 0;
      bugTimerScroll = 0; dragonCounter = 0; studentCounter = 0; professorCounter = 0;
      dwarfCounter = 0; knightCounter = 0; trollCounter = 0; workshopCounter = 0;
      directorCounter = 0; goblinCounter = 0; witchCounter = 0; firstRun = true;
      isPotionDoorOpened = false; isDogDoorOpened = false; isOwlDoorOpened = false;
      isTrainStarted = false; isBankerFirstHint = true; dragonTimer = millis();
      boyStateInitialized = false;
      level++;
    }
    else if (buff == "open_mansard_door") OpenDoor(MansardDoor);
    else if (buff == "open_crime_door") OpenDoor(CrimeDoor);
    else if (buff == "open_bank_door") OpenDoor(BankDoor);
    else if (buff == "open_potion_door") OpenDoor(PotionsRoomDoor);
    else if (buff == "open_owl_door") mySerial.println("open_door");
    else if (buff == "open_dog_door") Serial3.println("open_door");
    else if (buff == "open_low_tower_door") OpenDoor(HightTowerDoor);
    else if (buff == "open_high_tower_door") OpenDoor(HightTowerDoor2);
    else if (buff == "open_library_door") OpenDoor(LibraryDoor);
    else if (buff == "open_workshop_door") Serial1.println("open_door");
    else if (buff == "open_safe_door") OpenDoor(BankStashDoor);
    else if (buff == "open_memory_door") OpenDoor(MemoryRoomDoor);
    else if (buff == "open_basket_door") Serial2.println("open_door");
    else if (buff == "open_mine_door") Serial2.println("open_mine_door");
    else Unlocks(buff);
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
      int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
      matrix[i][j] = min(
        matrix[i - 1][j] + 1,  // удаление
        min(
          matrix[i][j - 1] + 1,        // вставка
          matrix[i - 1][j - 1] + cost  // замена
          ));
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

  // --- Process Serial1 (workshop) ---
  while (Serial1.available()) {
    char c = Serial1.read();
    if (c == '\n') { // Конец строки
      serial1Buffer.trim();
      if (serial1Buffer.length() > 0) {
        
        Serial.println(serial1Buffer); // Пересылаем ВСЕ сообщения на сервер

        // ДОБАВЛЕНО: Страховка на случай потери команды story_35
        if (serial1Buffer.startsWith("log:") && serial1Buffer.indexOf("workshop_complete") != -1) {
          Serial.println("log:main:Insurance triggered for workshop_complete from workshop");
          delay(10);
          Serial.println("story_35");
        }
        // КОНЕЦ

        // Обрабатываем специфичные команды, как и раньше
        if (serial1Buffer.indexOf("fire1") != -1) {
          Serial.println("fire1");
        }
        if (serial1Buffer.indexOf("fire2") != -1) {
          Serial.println("fire2");
        }
        if (serial1Buffer.indexOf("fire3") != -1) {
          Serial.println("fire3");
        }
        if (serial1Buffer.indexOf("fire0") != -1) {
          Serial.println("fire0");
        }
        if (serial1Buffer.indexOf("item_add") != -1) {
          Serial.println("item_add");
        }
        if (serial1Buffer.indexOf("broom") != -1) {
          Serial.println("broom");
        }
        if (serial1Buffer.indexOf("helmet") != -1) {
          Serial.println("helmet");
        }
        if (serial1Buffer.indexOf("story_35") != -1) {
          Serial.println("story_35");
          Serial1.println("item_end"); Serial2.println("item_end"); Serial3.println("item_end"); mySerial.println("item_end");
          delay(1000); 
          Serial1.println("day_off"); Serial2.println("day_off"); Serial3.println("day_off"); mySerial.println("day_off");
          GoldStrip.clear();
          CandleStrip.clear(); CauldronStrip.clear(); CauldronRoomStrip.clear(); memory_Led.clear(); strip1.clear(); strip2.clear();
          GoldStrip.show(); CandleStrip.show(); CauldronStrip.show(); CauldronRoomStrip.show(); memory_Led.show(); strip1.show(); strip2.show();
          digitalWrite(MansardLight, LOW); digitalWrite(LastTowerTopLight, LOW); digitalWrite(Fireworks, LOW);
          digitalWrite(LibraryLight, LOW);
          digitalWrite(BankRoomLight, LOW); digitalWrite(HallLight, LOW); digitalWrite(UfHallLight, LOW); digitalWrite(HightTowerDoor2, LOW);
          digitalWrite(TorchLight, LOW); digitalWrite(HallLight, HIGH);
          level++;
        }
        if (serial1Buffer == "help") {
          HelpHandler("workshop");
        }
      }
      serial1Buffer = ""; // Очищаем буфер
    } else if (c >= 32 && serial1Buffer.length() < 256) {
       serial1Buffer += c;
    }
  }

  // --- Process Serial2 (basket3) ---
  while (Serial2.available()) {
    char c = Serial2.read();
    if (c == '\n') {
      serial2Buffer.trim();
      if (serial2Buffer.length() > 0) {
        Serial.println(serial2Buffer); // Пересылаем ВСЕ

        // ДОБАВЛЕНО: Страховка на случай потери команды boy_out
        if (serial2Buffer.startsWith("log:") && serial2Buffer.indexOf("Boy removed") != -1) {
          Serial.println("log:main:Insurance triggered for boy_out from basket3");
          delay(10);
          // Определяем, какую именно команду дублировать в зависимости от уровня
          if (level == 18) Serial.println("boy_out_lesson");
          else if (level == 19) Serial.println("boy_out_game");
          else if (level == 17) Serial.println("crime_end"); // Страховка для тюрьмы
          else Serial.println("boy_out"); // Общая страховка
        }
        // КОНЕЦ

        if (serial2Buffer == "help") {
          HelpHandler("troll");
        }
      }
      serial2Buffer = "";
    } else if (c >= 32 && serial2Buffer.length() < 256) {
       serial2Buffer += c;
    }
  }

  // --- Process Serial3 (dog) ---
  while (Serial3.available()) {
    char c = Serial3.read();
    if (c == '\n') {
      serial3Buffer.trim();
      if (serial3Buffer.length() > 0) {
        Serial.println(serial3Buffer); // Пересылаем ВСЕ
        if (serial3Buffer == "help") {
          HelpHandler("knight");
        }
      }
      serial3Buffer = "";
    } else if (c >= 32 && serial3Buffer.length() < 256) {
      serial3Buffer += c;
    }
  }

  // --- Process mySerial (owls) ---
  while (mySerial.available()) {
    char c = mySerial.read();
    if (c == '\n') {
      mySerialBuffer.trim();
      if (mySerialBuffer.length() > 0) {
        
        Serial.println(mySerialBuffer); // Пересылаем ВСЕ сообщения на сервер

        // ДОБАВЛЕНО: Страховка на случай потери команды owl_end
        if (mySerialBuffer.startsWith("log:") && mySerialBuffer.indexOf("owls_complete") != -1) {
          Serial.println("log:main:Insurance triggered for owls_complete from owls");
          delay(10);
          Serial.println("owl_end");
        }
        // КОНЕЦ
        
        // Дублируем логику пересылки из MapGame(),
        // чтобы 'light' и 'dark' всегда доставлялись башням,
        // независимо от того, какая функция "выиграла гонку".
        
        // Свет вкл
        if (mySerialBuffer.indexOf("light") != -1 && mySerialBuffer.indexOf("light_off") == -1) {
          Serial1.println("light_on");
          Serial2.println("light_on");
          Serial3.println("light_on");
        } 
        // Свет выкл
        else if (mySerialBuffer.indexOf("dark") != -1) {
          Serial1.println("light_off");
          Serial2.println("light_off");
          Serial3.println("light_off");
        }

        // Если чистая команда "help" потерялась, сработаем по логу
        if (mySerialBuffer.indexOf("help") != -1 || mySerialBuffer.indexOf("Help button pressed") != -1) {
          HelpHandler("dwaf");
        }
      }
      mySerialBuffer = "";
    } else if (c >= 32 && mySerialBuffer.length() < 256) {
       mySerialBuffer += c;
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

    // [FIX] ВЕЗДЕ: flagSound = 0 перенесен внутрь успешной отправки

    // Дракон
    if (dragonButton.isPress()) {
      if ( (level < 4 && level > 1) || (level == 1 && isStartDoorOpenedGlobal) ) {
        Serial.println(dragonHints[3]); flagSound = 0;
      }
      else if (level == 1 && !dragonFlag) {
        Serial.println(dragonHints[0]); dragonFlag = 1; flagSound = 0;
      }
      else if (level == 1 && dragonCounter > 0) {
        Serial.println(dragonHints[dragonCounter]); flagSound = 0;
      }
      else if (level == 4) {
        Serial.println(dragonHints[dragonCounter]);
        dragonCounter = (dragonCounter == 4) ? 5 : 4;
        flagSound = 0;
      }
      else if (level > 4) {
        Serial.println(dragonHints[3]); flagSound = 0;
      }
      
      if (level != 4) {
        dragonCounter++;
        if (dragonCounter > 2) dragonCounter = 1;
      }
    }

    // Студент
    if (studentButton.isPress()) {
      bool sent = false;
      if (level > 1 && level < 4) {
        Serial.println(studentHints[studentCounter]);
        studentCounter = (studentCounter == 0) ? 1 : 0; 
        sent = true;
      }
      if (level > 3 && level < 11) {
        Serial.println(studentHints[2]); sent = true;
      }
      if (level == 11) {
        if (ghostState >= 2) {
          Serial.println(studentHints[5]); sent = true;
        } else {
          if (studentCounter < 3 || studentCounter > 4) studentCounter = 3;
          Serial.println(studentHints[studentCounter]);
          studentCounter = (studentCounter == 3) ? 4 : 3;
          sent = true;
        }
      }
      if (level == 12) {
        if (studentCounter < 6 || studentCounter > 7) studentCounter = 6;
        Serial.println(studentHints[studentCounter]);
        studentCounter = (studentCounter == 6) ? 7 : 6;
        sent = true;
      }
      if (level > 12 && level < 17) { 
         Serial.println(studentHints[2]); sent = true;
      }
      
      if (sent) flagSound = 0;
    }

    // Профессор
    if (professorButton.isPress()) {
      bool sent = false;
      if (level > 4 && level < 6) {
        Serial.println(professorHints[professorCounter]);
        professorCounter = (professorCounter == 1) ? 0 : 1;
        sent = true;
      }
      if (level == 6) {
        Serial.println(professorHints[professorCounter]);
        professorCounter = (professorCounter == 2) ? 3 : 2;
        sent = true;
      }
      if (level == 7) {
        if (!isPotionDoorOpened || !isDogDoorOpened || !isOwlDoorOpened || !isTrainStarted) {
          Serial.println(professorHints[professorCounter]);
          professorCounter = (professorCounter == 5) ? 4 : 5;
        } else {
          Serial.println(professorHints[6]);
        }
        sent = true;
      } else if (level > 7) { // [FIX] level > 7 тоже говорит финальную фразу
        Serial.println(professorHints[6]);
        sent = true;
      }
      
      if (sent) flagSound = 0;
    }

    // Гоблин
    if (goblinButton.isPress()) {
      bool sent = false;
      if (level == 9) {
        Serial.println(goblinHints[goblinCounter]);
        goblinCounter = (goblinCounter == 1) ? 0 : 1;
        sent = true;
      }
      if (level > 9 && level < 18) {
        Serial.println(goblinHints[2]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Директор
    if (directorButton.isPress()) {
      bool sent = false;
      if (level == 13) {
        if (directorCounter > 1) directorCounter = 0;
        Serial.println(directorHints[directorCounter]);
        directorCounter = (directorCounter == 0) ? 1 : 0;
        sent = true;
      }
      if (level == 14) {
        if (directorCounter < 2 || directorCounter > 3) directorCounter = 2;
        Serial.println(directorHints[directorCounter]);
        directorCounter = (directorCounter == 2) ? 3 : 2;
        sent = true;
      }
      if (level > 14 && level < 17) {
        if (directorCounter < 4 || directorCounter > 5) directorCounter = 4;
        Serial.println(directorHints[directorCounter]);
        directorCounter = (directorCounter == 4) ? 5 : 4;
        sent = true;
      }
      if (level == 17 || level == 18) {
        Serial.println(directorHints[6]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Ведьма (Кнопка)
    if (witchButton.isPress()) {
      bool sent = false;
      if (level == 7 && !isPotionEnd) {
        Serial.println(witchHints[witchCounter]);
        witchCounter = (witchCounter == 1) ? 0 : 1;
        sent = true;
      }
      // [FIX] Добавлено условие level > 7 для подсказки "Z"
      if (isPotionEnd || level > 7) {
        Serial.println(witchHints[2]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Гном (Башня)
    if (from == "dwaf") {
      bool sent = false;
      if (level == 7 && !isOwlEnd) {
        Serial.println(dwarfHints[dwarfCounter]);
        dwarfCounter = (dwarfCounter == 1) ? 0 : 1;
        sent = true;
      }
      // [FIX] Добавлено условие level > 7
      if (isOwlEnd || level > 7) {
        Serial.println(dwarfHints[2]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Рыцарь (Башня)
    if (from == "knight") {
      bool sent = false;
      if (level == 7 && !isDogEnd) {
        Serial.println(knightHints[knightCounter]);
        knightCounter = (knightCounter == 1) ? 0 : 1;
        sent = true;
      }
      // [FIX] Добавлено условие level > 7
      if (isDogEnd || level > 7) {
        Serial.println(knightHints[2]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Тролль (Башня)
    if (from == "troll") {
      bool sent = false;
      if (level == 7 && !isTrollEnd) {
        Serial.println(trollHints[trollCounter]);
        trollCounter = (trollCounter == 1) ? 0 : 1;
        sent = true;
      }
      // [FIX] Добавлено условие level > 7
      if (isTrollEnd || level > 7) {
        Serial.println(trollHints[2]);
        sent = true;
      }
      if (sent) flagSound = 0;
    }

    // Плотник (Башня)
    if (from == "workshop") {
      bool sent = false;
      if (level == 10) {
        Serial.println(workshopHints[workshopCounter]);
        sent = true;
      }
      if (level > 10 && level < 18) {
        Serial.println(workshopHints[4]);
        sent = true;
      }

      workshopCounter++;
      if (workshopCounter > 3) workshopCounter = 0;
      
      if (sent) flagSound = 0;
    }
  }
}

// стартовая дверь на замке с ключом
// стартовая дверь на замке с ключом
void StartDoor() {
  static bool doorOpened = false;
  
  startDoor.tick();
  
  // [FIX] Объединили логику в один блок проверки
  if (startDoor.isRelease() && !doorOpened) {
    Serial.println("open_door");
    digitalWrite(HallLight, HIGH);
    
    isStartDoorOpenedGlobal = true; // Флаг для Дракона
    doorOpened = true;              // Блокируем повторное срабатывание

    // --- ЛОГИКА СЕРВОПРИВОДА ---
    if (!boyServo.attached()) {
         boyServo.attach(49);
    }
    delay(50);          
    boyServo.write(130); // Поворот
     
    // Цикл ожидания 3 секунды, чтобы серво доехал
    unsigned long startWait = millis();
    while(millis() - startWait < 3000) {
        // Если пришел рестарт во время движения сервы
        if (Serial.available()) {
           String b = Serial.readStringUntil('\n');
           if(b.indexOf("restart") != -1) { 
               boyServo.detach(); 
               OpenAll(); RestOn(); level=25; return; 
           }
        }
    }
    // Отключаем сервопривод после движения
    if (boyServo.attached()) {
        boyServo.detach();
    }
    // ---------------------------
    
    dragonFlag = 0;
    KayTimer = millis();
    level++; // Переход на следующий уровень
  }

  // Таймер для повтора подсказки дракона
  if (millis() - dragonTimer >= 120000) {
    if (!dragonFlag) {
      Serial.println("dragon_crystal_repeat");
    }
    dragonTimer = millis();
  }

  // Чтение команд (рестарт, звук)
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
  }
}
// тумблер в первой комнате реагирует на изменение состояния
void ClockGame() {
  static bool repeat;
  // Флаг защиты. True = мы только зашли в уровень и ждем, пока датчик "отпустит".
  static bool switchMustBeOffFirst = true; 

  if (millis() - KayTimer >= 30000) {
    if (repeat) {
      Serial.println("kay_repeat");
      repeat = 0;
    }
  }

  // Считываем датчик
  bool reading = !digitalReadExpander(0, board2); 
  
  // --- ЛОГИКА ЗАЩИТЫ ---
  
  // 1. Если датчик НЕ АКТИВЕН (магнита нет), мы "взводим" курок.
  // Теперь система готова принять сигнал победы.
  if (!reading) {
    switchMustBeOffFirst = false; 
  }

  // 2. Если датчик АКТИВЕН (магнит есть) И (&&) курок взведен (!switchMustBeOffFirst)
  if (reading && !switchMustBeOffFirst) {
    Serial.println("clock1");
    digitalWrite(UfHallLight, HIGH);
    digitalWrite(HallLight, LOW);
    repeat = 0;
    level++;
    
    // Сразу же снова включаем защиту для следующего раза (рестарта)
    switchMustBeOffFirst = true;
  }
  // ---------------------

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "first_clock") {
      Serial.println("clock1");
      digitalWrite(UfHallLight, HIGH);
      digitalWrite(HallLight, LOW);
      level++;
      switchMustBeOffFirst = true;
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
    if (buff.indexOf("restart") != -1) {
      repeat = 0;
      switchMustBeOffFirst = true; // Обязательно сбрасываем защиту при рестарте
      Serial1.println("restart");
      Serial2.println("restart");
      Serial3.println("restart");
      mySerial.println("restart");
      // --------------------------
      
      lessonSaluteActive = false;
      discoBallsActive = false;
      digitalWrite(Fireworks, LOW);
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
  }
}

/// Часы в первой комнате
void Clock2Game() {
  //clock3Button.tick();
  //if (clock3Button.isPress()) {
  if (!digitalReadExpander(2, board2)) {
    Serial.println("clock2");
    if (!isUfBlinking) { // Запускаем, только если еще не мигает
            isUfBlinking = true;
            ufBlinkCount = 0;
            ufBlinkTimer = millis();
        }
        // --- (Конец удаления) --- 
        
        stepsTimer = millis();
        level++; 
  }

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "second_clock") {
      Serial.println("clock2");
      //OpenLock(MansardDoor);

      // Добавлена логика мигания и выключения УФ-светодиода,
      // аналогичная блоку физического взаимодействия.
    if (!isUfBlinking) { // Запускаем, только если еще не мигает
          isUfBlinking = true;
          ufBlinkCount = 0;
          ufBlinkTimer = millis();
      }
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
      level = 25;
      return;
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

  while (Serial1.available()) {
    String buf1 = Serial1.readStringUntil('\n');
    buf1.trim();
    if (buf1.startsWith("log:")) {
      Serial.println(buf1);
    } else if (buf1 == "galet_on") {
      galet1 = 1;
      Serial.println("galet2");
    } else if (buf1 == "galet_off") {
      galet1 = 0;
      Serial.println("galet2_off");
    }
  }

  while (Serial2.available()) {
    String buf2 = Serial2.readStringUntil('\n');
    buf2.trim();
    if (buf2.startsWith("log:")) {
      Serial.println(buf2);
    } else if (buf2 == "galet_on") {
      galet2 = 1;
      Serial.println("galet3");
    } else if (buf2 == "galet_off") {
      Serial.println("galet3_off");
      galet2 = 0;
    }
  }

  while (Serial3.available()) {
    String buf3 = Serial3.readStringUntil('\n');
    buf3.trim();
    if (buf3.startsWith("log:")) {
      Serial.println(buf3);
    } else if (buf3 == "galet_on") {
      galet3 = 1;
      Serial.println("galet4");
    } else if (buf3 == "galet_off") {
      Serial.println("galet4_off");
      galet3 = 0;
    }
  }

  while (mySerial.available()) {
    String buf4 = mySerial.readStringUntil('\n');
    buf4.trim();
    if (buf4.startsWith("log:")) {
      Serial.println(buf4);
    } else if (buf4 == "galet_on") {
      galet4 = 1;
      Serial.println("galet5");
    } else if (buf4 == "galet_off") {
      Serial.println("galet5_off");
      galet4 = 0;
    }
  }

  if (galet1 && galet2 && galet3 && galet4 && galet5) {
    delay(200);
    Serial.println("galet_on");
    galet1 = 0;
    galet2 = 0;
    galet3 = 0;
    galet4 = 0;
    galet5 = 0;
    startSteps = 0;
  }

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "after_story_clock2") {
      startSteps = 1;
    }
    if (buff == "student_hide") {
      boyServo.attach(49);
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
    if (buff == "open_mansard_door") {
      delay(200);
      OpenLock(MansardDoor);
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
      galet1 = 0;
      galet2 = 0;
      galet3 = 0;
      galet4 = 0;
      galet5 = 0;
      startSteps = 0;
      RestOn();
      level = 25;
      return;
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
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "student_hide") {
      boyServo.attach(49);
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
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
      suitcaseFlag = 0;
      safeFlag = 0;
      wolfFlag = 0;
      RestOn();
      level = 25;
      return;
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
  while (Serial1.available()) {
    String buf1 = Serial1.readStringUntil('\n');
    buf1.trim();
    if (buf1.startsWith("log:")) {
      Serial.println(buf1);
    } else if (buf1 == "flag1_on") {
      FirstFlag = 1;
      Serial.println("flag1_on");
    } else if (buf1 == "flag1_off") {
      FirstFlag = 0;
      Serial.println("flag1_off");
    }
  }

  while (Serial2.available()) {
    String buf2 = Serial2.readStringUntil('\n');
    buf2.trim();
    if (buf2.startsWith("log:")) {
      Serial.println(buf2);
    } else if (buf2 == "flag2_on") {
      SecondFlag = 1;
      Serial.println("flag2_on");
    } else if (buf2 == "flag2_off") {
      SecondFlag = 0;
      Serial.println("flag2_off");
    }
  }

  while (Serial3.available()) {
    String buf3 = Serial3.readStringUntil('\n');
    buf3.trim();
    if (buf3.startsWith("log:")) {
      Serial.println(buf3);
    } else if (buf3 == "flag3_on") {
      ThirdFlag = 1;
      Serial.println("flag3_on");
    } else if (buf3 == "flag3_off") {
      ThirdFlag = 0;
      Serial.println("flag3_off");
    }
  }

  while (mySerial.available()) {
    String buf4 = mySerial.readStringUntil('\n');
    buf4.trim();
    if (buf4.startsWith("log:")) {
      Serial.println(buf4);
    } else if (buf4 == "flag4_on") {
      FourFlag = 1;
      Serial.println("flag4_on");
    } else if (buf4 == "flag4_off") {
      FourFlag = 0;
      Serial.println("flag4_off");
    }
  }

  if (FirstFlag && SecondFlag && ThirdFlag && FourFlag) {
    Serial.println("flagsendmr");
    level++;
  }
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "student_hide") {
      boyServo.attach(49);
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
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
      level = 25;
      return;
    }
  }
}


void MapGame() {
  static bool potionFadingOut = false;
  boyServo.detach();
  static String game = "";
  static bool activePotionRoom;
  static bool potionPulsation;

  // Переменные для неблокирующей анимации ---
  static unsigned long potionFadeTimer = 0;
  static int potionBrightness = 0;
  static bool potionFading = false;

  // Переменные для неблокирующей отправки "start_troll"
  static bool isWaitingForTroll = false;
  static unsigned long trollWaitTimer = 0;

  // Переменные для неблокирующего перехода на следующий уровень
  static bool isFinishingMap = false;
  static unsigned long finishMapTimer = 0;

  // Обработчик неблокирующей анимации ---
  // Этот блок должен выполняться в каждой итерации MapGame()
  if (potionFading) {
    // Проверяем, прошло ли 2 мс
    if (millis() - potionFadeTimer >= 2) { 
      potionFadeTimer = millis();
      CauldronRoomStrip.setBrightness(potionBrightness);
      CauldronRoomStrip.show();
      
      potionBrightness++;
      if (potionBrightness > 255) {
        potionFading = false; // Анимация завершена
      }
    }
  }
  if (potionFadingOut) {
    if (millis() - potionFadeTimer >= 2) {
       potionFadeTimer = millis();
       CauldronRoomStrip.setBrightness(potionBrightness);
       CauldronRoomStrip.show();
       potionBrightness--;
       if (potionBrightness <= 50) potionFadingOut = false;
    }
  }

  // 1. Неблокирующий таймер для повторной отправки start_troll
  if (isWaitingForTroll && (millis() - trollWaitTimer >= 1000)) {
    isWaitingForTroll = false; // Сбрасываем флаг
    Serial2.println("start_troll"); // Отправляем вторую команду
  }

  // 2. Неблокирующий таймер для завершения уровня (вместо delay)
  if (isFinishingMap) {
    // Ждем 1 секунду после отправки 'material_end'
    if (millis() - finishMapTimer >= 1000) {
      isFinishingMap = false; // Сбрасываем
      level++; // <-- ПЕРЕХОДИМ НА УРОВЕНЬ 8
    }
    // ВАЖНО: Выходим из MapGame, пока ждем таймер.
    // Это позволяет loop() продолжать работу, но предотвращает
    // выполнение остальной логики MapGame (чтение RFID и т.д.)
    return; 
  }

  if (game == "fish") {
    // digitalWrite(pinA, 0);
    // digitalWrite(pinB, 0);
    // digitalWrite(pinC, 0);
    // bool reading1 = !digitalRead(21);
    bool reading1 = !digitalReadExpander(0, board1);
    if (reading1) {
      Serial.println("door_witch");
      isPotionDoorOpened = true;
      OpenLock(PotionsRoomDoor);
      activePotionRoom = 1;
// --- Запуск неблокирующей анимации ---
      // Мы больше не блокируем код циклом for с delay()
      if (!potionFading) { // Убедимся, что не запускаем повторно
        potionFading = true;
        potionBrightness = 0;
        potionFadeTimer = millis();
        // Устанавливаем цвет один раз
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 197, 143));
        }
      }
      game = "";
    }
  }

  if (activePotionRoom && !isPotionEnd) {
    Bottles();
  }
  if (isPotionEnd) {
    //digitalWrite(pinA, 0);
    //digitalWrite(pinB, 0);
    //digitalWrite(pinC, 1);
    //bool reading = !digitalRead(21);
    bool reading = !digitalReadExpander(4, board1);
    if (reading) {
      if (!potionPulsation) {
        potionPulsation = 1;
        Serial1.println("potion");
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
    helpsBankTimer = millis();
    Serial.println("material_end");

    // --- Новая логика неблокирующего перехода ---
    isFinishingMap = true;
    finishMapTimer = millis();
    // level++; // <-- Перенесено в таймер
  }

  while (mySerial.available()) {
    String buff = mySerial.readStringUntil('\n');
    buff.trim();
    
    // Печатаем всё, что приходит (для отладки)
    if (buff.length() > 0) Serial.println(buff);

    // -----------------------------------------------------------
    // [FIX] НАДЕЖНАЯ ОБРАБОТКА КОМАНД (через indexOf)
    // Работает и для чистых команд, и если они склеились с логом
    // -----------------------------------------------------------

    // 1. Дверь (Команда + Лог нажатия)
    if (buff.indexOf("door_owl") != -1 || buff.indexOf("Owl button pressed") != -1) {
        Serial.println("door_owl");
        isOwlDoorOpened = true;
    }

    // 2. Финал игры Сов
    if (buff.indexOf("owl_end") != -1) {
        Serial.println("owl_end");
        Serial1.println("light_off"); 
        Serial2.println("light_off"); 
        Serial3.println("light_off");
        isOwlEnd = 1;
    }

    // 3. Промежуточный этап (сова улетела)
    if (buff.indexOf("owl_flew") != -1) {
        Serial.println("owl_flew");
    }

    // 4. Управление светом (ВКЛ)
    // Ищем "light", но исключаем "light_off" (чтобы не сработало ложно)
    if (buff.indexOf("light") != -1 && buff.indexOf("light_off") == -1) { 
        Serial.println("light_on");
        Serial1.println("light_on");
        Serial2.println("light_on");
        Serial3.println("light_on");
    } 
    // 5. Управление светом (ВЫКЛ) - ловим "dark" или "light_off"
    else if (buff.indexOf("dark") != -1 || buff.indexOf("light_off") != -1) { 
        Serial.println("light_off");
        Serial1.println("light_off");
        Serial2.println("light_off");
        Serial3.println("light_off");
    }

    // 6. Помощь
    if (buff.indexOf("help") != -1 || buff.indexOf("Help button pressed") != -1) {
        HelpHandler("dwaf");
    }
  }

  while (Serial3.available()) {
    String buff = Serial3.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
      // Проверяем логи
      if (buff.indexOf("dog_lock") != -1) {
        Serial.println("dog_lock");
        isDogEnd = 1;
      }
      if (buff.indexOf("door_dog") != -1) {
        Serial.println("door_dog");
        isDogDoorOpened = true;
      }
      if (buff.indexOf("dog_sleep") != -1) {
        Serial.println("dog_sleep");
      }
      if (buff.indexOf("dog_growl") != -1) {
        Serial.println("dog_growl");
      }
      continue;
    }
    
    // Быстрые проверки
    if (buff == "dog_lock") {
      Serial.println("dog_lock");
      isDogEnd = 1;
    } else if (buff == "door_dog") {
      Serial.println("door_dog");
      isDogDoorOpened = true;
    } else if (buff == "dog_sleep") {
      Serial.println("dog_sleep");
    } else if (buff == "dog_growl") {
      Serial.println("dog_growl");
    } else if (buff == "story_20_a") {
      Serial.println("story_20_a");
    } else if (buff == "story_20_b") {
      Serial.println("story_20_b");
    } else if (buff == "story_22_c") { // Вероятно, опечатка, должно быть story_20_c
      Serial.println("story_20_c");
    } else if (buff == "help") {
      HelpHandler("knight");
    } else if (buff == "crystal") {
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      potionPulsation = 0;
      Serial.println("item_find");
      Serial2.println("item_find");
    }
  }

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "student_hide") {
      boyServo.attach(49);
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
    if (buff == "key") {
      game = "key";
      mySerial.println("out");
      Serial3.println("key");
    }
    if (buff == "train_active") {
      isTrainStarted = true;
    }
    if (buff == "train_end") {
      isTrainEnd = 1;
      Serial2.println("start_troll"); // Отправляем первую команду
      // --- Новая логика неблокирующей отправки ---
      isWaitingForTroll = true;     // Взводим флаг таймера
      trollWaitTimer = millis();    // Запускаем таймер
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
    }

    if (buff == "dog") {
      Serial3.println("skip_dog");
      isDogEnd = 1;
    }

    if (buff == "owl_door") {
      mySerial.println("owl_door");
      isOwlDoorOpened = true;
    }

    if (buff == "owl_skip") {
      mySerial.println("skip");
      isOwlEnd = 1;
      Serial1.println("light_off");
      Serial2.println("light_off");
      Serial3.println("light_off");
    }

    if (buff == "open_potions_stash") {
      Serial.println("four_bottle");
      rainbow();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      if (!potionFading) {
         potionFading = true;
         potionBrightness = 255; // Начинаем с максимума
      }
      isPotionEnd = true;
    }

    if (buff == "cat") {
      OpenLock(PotionsRoomDoor);
      activePotionRoom = 1;
// --- Запуск неблокирующей анимации (для 'cat') ---
      if (!potionFading) { // Убедимся, что не запускаем повторно
        potionFading = true;
        potionBrightness = 0;
        potionFadeTimer = millis();
        // Устанавливаем цвет один раз
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(255, 197, 143));
        }
      }
      game = "";
    }

    if (buff == "mine") {
      Serial2.println("start_troll"); // Сначала будим
      delay(100);
      Serial2.println("mine");        // Потом открываем
    }
    // Корректная обработка СКИПА игры с троллем ---
    if (buff == "troll") { 
      Serial2.println("troll");
    }

    if (buff == "train") {
      game = "";
      mySerial.println("out");
      Serial3.println("out");
      // Сообщаем башне тролля, что поезд пропущен!
      isTrainEnd = 1;
      Serial2.println("start_troll");
    }

    if (buff == "out") {
      game = "";
      mySerial.println("out");
      Serial3.println("out");
    }
    if (buff == "restart") {
      game = "";
      activePotionRoom = 0;
      potionPulsation = 0;
      OpenAll();
      RestOn();
      level = 25;
      return;
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
      Serial1.println("skin");
      Serial2.println("item_find");
      Serial3.println("item_find");
    }

    // ВСТАВКА КОПИИ БЛОКА ПРОВЕРКИ ---
    // Эта проверка сработает СРАЗУ после обработки команды пропуска, которая УСТАНОВИЛА ПОСЛЕДНИЙ флаг
    if (isPotionEnd && isDogEnd && isOwlEnd && isTrainEnd && isTrollEnd) {
      activePotionRoom = 0;
      game = "";
      helpsBankTimer = millis();
      Serial.println("material_end");
      // Теперь сбрасываем флаги
      isPotionEnd = 0;
      isDogEnd = 0;
      isOwlEnd = 0;
      isTrainEnd = 0;
      isTrollEnd = 0;

      // --- Новая логика неблокирующего перехода ---
      isFinishingMap = true;
      finishMapTimer = millis(); // Используем тот же таймер
    }
    // ---
  }

  while (Serial2.available()) {
    String buff = Serial2.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
      // Проверяем логи
      if (buff.indexOf("aluminium") != -1) {
        Serial.println("cave_search1");
      }
      if (buff.indexOf("bronze") != -1) {
        Serial.println("cave_search2");
      }
      if (buff.indexOf("copper") != -1) {
        Serial.println("cave_search3");
      }
      if (buff.indexOf("cave_end") != -1) {
        Serial.println("cave_end");
        isTrollEnd = 1;
      }
      if (buff.indexOf("cave_click") != -1) {
        Serial.println("cave_click");
      }
      if (buff.indexOf("door_cave") != -1) {
        Serial.println("door_cave");
      }
      continue;
    }
    
    // [ИЗМЕНЕНИЕ] Быстрые проверки
    if (buff == "aluminium") {
      Serial.println("cave_search1");
    } else if (buff == "bronze") {
      Serial.println("cave_search2");
    } else if (buff == "copper") {
      Serial.println("cave_search3");
    } else if (buff == "cave_end") {
      Serial.println("cave_end");
      isTrollEnd = 1;
    } else if (buff == "cave_click") {
      Serial.println("cave_click");
    } else if (buff == "door_cave") {
      Serial.println("door_cave");
    } else if (buff == "help") {
      HelpHandler("troll");
    } else if (buff == "metal") {
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      potionPulsation = 0;
      Serial.println("item_find");
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
  //digitalWrite(pinA, 0);
  //digitalWrite(pinB, 0);
  //digitalWrite(pinC, 1);
  //bool reading = !digitalRead(21);
  bool reading = !digitalReadExpander(4, board1);
  if (reading) {
    if (!potionPulsation) {
      potionPulsation = 1;
      Serial1.println("potion");
      Serial.println("item_find:potion");
      Serial2.println("item_find");
      Serial3.println("item_find");
      mySerial.println("item_find");
      goldPulsation = 0;
    }
  }

  if (goldButton.isPress()) {
    if (!goldPulsation) {
      goldPulsation = 1;
      Serial1.println("gold");
      Serial.println("item_find:gold");
      Serial2.println("item_find");
      Serial3.println("item_find");
      mySerial.println("item_find");
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

  while (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
      continue;
    }
    if (buff == "help") {
      HelpHandler("workshop");
    } else if (buff == "fire1") {
      Serial.println("fire1");
    } else if (buff == "fire2") {
      Serial.println("fire2");
    } else if (buff == "fire3") {
      Serial.println("fire3");
    } else if (buff == "fire0") {
      Serial.println("fire0");
    } else if (buff == "item_add") {
      Serial.println("item_add");
    } else if (buff == "broom") {
      Serial.println("broom");
    } else if (buff == "helmet") {
      Serial.println("helmet");
    } else if (buff == "story_35") {
      Serial.println("story_35");
      Serial1.println("item_end");
      Serial2.println("item_end");
      Serial3.println("item_end");
      mySerial.println("item_end");
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

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "open_workshop") {
      Serial1.println("open_workshop");
    }
    if (buff == "workshop") {
      Serial1.println("skip");
      /*
      Serial.println("story_35");
      Serial1.println("item_end");
      Serial2.println("item_end");
      Serial3.println("item_end");
      mySerial.println("item_end");
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
      */
    }
    if (buff == "skin") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("skin");
      Serial.println("item_find:skin");
      Serial2.println("item_find");
      Serial3.println("item_find");
      mySerial.println("item_find");
    }
    if (buff == "restart") {
      goldPulsation = 0;
      potionPulsation = 0;
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }

  while (Serial2.available()) {
    String buff = Serial2.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
      continue;
    }
    if (buff == "metal") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("metal");
      Serial.println("item_find:metal");
      Serial3.println("item_find");
      mySerial.println("item_find");
    } else if (buff == "help") {
      HelpHandler("troll");
    }
  }

  while (Serial3.available()) {
    String buff = Serial3.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
      continue;
    }
    if (buff == "crystal") {
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
      CauldronStrip.show();
      goldPulsation = 0;
      potionPulsation = 0;
      Serial1.println("crystal");
      Serial.println("item_find:crystal");
      Serial2.println("item_find");
      mySerial.println("item_find");
    } else if (buff == "help") {
      HelpHandler("knight");
    }
  }

  while (mySerial.available()) {
    String buff = mySerial.readStringUntil('\n');
    buff.trim();
    if (buff.startsWith("log:")) {
      Serial.println(buff);
    } else if (buff == "help") {
      HelpHandler("dwaf");
    }
  }
}


////игра с бутылками
void Bottles() {
  // Если активен "период охлаждения" после ошибки, выходим из функции
  if (millis() < rfidCooldownEnd) {
    // ИСПРАВЛЕНИЕ: Пока длится таймаут, поддерживаем красный свет
    CauldronMistakeFire();
    return;
  }

  // Убрали сброс флагов отсюда, он теперь в 'else' с таймером

  switch (Bottle) {
    case 0:
      SecondBottle();  // Логика в вашем коде перепутана, оставляю как есть
      break;
    case 1:
      FirstBottle();  // Логика в вашем коде перепутана, оставляю как есть
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
    // Обновляем таймер КАЖДЫЙ раз, когда видим бутылку
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
      // Неправильная бутылка
      if (FirstBottleFalse) {
        Serial.println("mistake_bottle");
        FirstBottleFalse = 0;  // "Защелкиваем" ошибку
        Bottle = 0;
        FirstBottleTrue = 1;
        SecondBottleTrue = 1;
        SecondBottleFalse = 1;
        ThirdBottleTrue = 1;
        ThirdBottleFalse = 1;
        FourBottleTrue = 1;
        FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 2000;
      }
      CauldronMistakeFire();  // Поддерживаем красный, пока бутылка в котле
    }
    myRFID.reset_search();
  } else {
    // Нет бутылки. Ждем 100мс прежде чем реагировать.
    if (millis() - Bottle1Timer >= 100) {
      // "Перевзводим" флаги, только когда бутылка убрана > 100мс
      FirstBottleFalse = 1;
      SecondBottleFalse = 1;
      ThirdBottleFalse = 1;
      FourBottleFalse = 1;

      if (FirstBottleTrue == 0) {  // Правильная бутылка была убрана
        Bottle++;                  // Переходим к шагу 2 (ThirdBottle)
        // Устанавливаем свет для *нового* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(150, 250, 0));  // yellow-green
        }
      } else {  // Мы ждем правильную бутылку (или только что убрали неправильную)
        // Восстанавливаем свет для *текущего* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(30, 0, 250));  // blue
        }
      }
      static unsigned long ledRefreshTimer1 = 0;
      if (millis() - ledRefreshTimer1 >= 30) {
         ledRefreshTimer1 = millis();
         CauldronRoomStrip.show();
         CauldronFire(); 
      }
    }
    // Если прошло < 100мс, ничего не делаем. Это фильтрует "моргания".
  }
}

///////вторая бутылка
void SecondBottle() {
  if (myRFID.search(addr)) {
    // ИСПРАВЛЕНИЕ: Обновляем таймер КАЖДЫЙ раз, когда видим бутылку
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
      // Неправильная бутылка
      if (SecondBottleFalse) {
        Serial.println("mistake_bottle");
        SecondBottleFalse = 0;  // "Защелкиваем" ошибку
        Bottle = 0;
        FirstBottleTrue = 1;
        FirstBottleFalse = 1;
        SecondBottleTrue = 1;
        ThirdBottleTrue = 1;
        ThirdBottleFalse = 1;
        FourBottleTrue = 1;
        FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 2000;
      }
      CauldronMistakeFire();  // Поддерживаем красный, пока бутылка в котле
    }
    myRFID.reset_search();
  } else {
    // Нет бутылки. Ждем 100мс прежде чем реагировать.
    if (millis() - Bottle2Timer >= 100) {
      // "Перевзводим" флаги, только когда бутылка убрана > 100мс
      FirstBottleFalse = 1;
      SecondBottleFalse = 1;
      ThirdBottleFalse = 1;
      FourBottleFalse = 1;

      if (SecondBottleTrue == 0) {  // Правильная бутылка была убрана
        Bottle++;                   // Переходим к шагу 1 (FirstBottle)
        // Устанавливаем свет для *нового* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(30, 0, 250));  // blue
        }
      } else {  // Мы ждем правильную бутылку (или только что убрали неправильную)
        // Восстанавливаем свет для *текущего* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 250, 30));  // green
        }
      }
      static unsigned long ledRefreshTimer2 = 0;
      if (millis() - ledRefreshTimer2 >= 30) {
         ledRefreshTimer2 = millis();
         CauldronRoomStrip.show();
         CauldronFire(); 
      }
    }
    // Если прошло < 100мс, ничего не делаем. Это фильтрует "моргания".
  }
}

//////третья бутылка
void ThirdBottle() {
  if (myRFID.search(addr)) {
    // Обновляем таймер КАЖДЫЙ раз, когда видим бутылку
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
      // Неправильная бутылка
      if (ThirdBottleFalse) {
        Serial.println("mistake_bottle");
        ThirdBottleFalse = 0;  // "Защелкиваем" ошибку
        Bottle = 0;
        FirstBottleTrue = 1;
        FirstBottleFalse = 1;
        SecondBottleTrue = 1;
        SecondBottleFalse = 1;
        ThirdBottleTrue = 1;
        FourBottleTrue = 1;
        FourBottleFalse = 1;
        rfidCooldownEnd = millis() + 2000;
      }
      CauldronMistakeFire();  // Поддерживаем красный, пока бутылка в котле
    }
    myRFID.reset_search();
  } else {
    // Нет бутылки. Ждем 100мс прежде чем реагировать.
    if (millis() - Bottle3Timer >= 100) {
      // "Перевзводим" флаги, только когда бутылка убрана > 100мс
      FirstBottleFalse = 1;
      SecondBottleFalse = 1;
      ThirdBottleFalse = 1;
      FourBottleFalse = 1;

      if (ThirdBottleTrue == 0) {  // Правильная бутылка была убрана
        Bottle++;                  // Переходим к шагу 3 (FourBottle)
        // Устанавливаем свет для *нового* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(250, 250, 250));  // white
        }
      } else {  // Мы ждем правильную бутылку (или только что убрали неправильную)
        // Восстанавливаем свет для *текущего* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(150, 250, 0));  // yellow-green
        }
      }
      static unsigned long ledRefreshTimer3 = 0;
      if (millis() - ledRefreshTimer3 >= 30) {
         ledRefreshTimer3 = millis();
         CauldronRoomStrip.show();
         CauldronFire(); 
      }
    }
    // Если прошло < 100мс, ничего не делаем. Это фильтрует "моргания".
  }
}

/////четвертая бутылка
void FourBottle() {
  if (myRFID.search(addr)) {
    // Обновляем таймер КАЖДЫЙ раз, когда видим бутылку
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
      // Неправильная бутылка
      if (FourBottleFalse) {
        Serial.println("mistake_bottle");
        FourBottleFalse = 0;  // "Защелкиваем" ошибку
        Bottle = 0;
        FirstBottleTrue = 1;
        FirstBottleFalse = 1;
        SecondBottleTrue = 1;
        SecondBottleFalse = 1;
        ThirdBottleTrue = 1;
        ThirdBottleFalse = 1;
        FourBottleTrue = 1;
        rfidCooldownEnd = millis() + 2000;
      }
      CauldronMistakeFire();  // Поддерживаем красный, пока бутылка в котле
    }
    myRFID.reset_search();
  } else {
    // Нет бутылки. Ждем 100мс прежде чем реагировать.
    if (millis() - Bottle4Timer >= 100) {
      // "Перевзводим" флаги, только когда бутылка убрана > 100мс
      FirstBottleFalse = 1;
      SecondBottleFalse = 1;
      ThirdBottleFalse = 1;
      FourBottleFalse = 1;

      if (FourBottleTrue == 0) {  // Правильная бутылка была убрана
        // Это был последний шаг, игра выиграна. Ничего не делаем.
      } else {  // Мы ждем правильную бутылку (или только что убрали неправильную)
        // Восстанавливаем свет для *текущего* шага
        for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(250, 250, 250));  // white
        }
        static unsigned long ledRefreshTimer4 = 0;
      if (millis() - ledRefreshTimer4 >= 30) {
           ledRefreshTimer4 = millis();
           CauldronRoomStrip.show();
           CauldronFire(); 
      }
      }
    }
    // Если прошло < 100мс, ничего не делаем. Это фильтрует "моргания".
  }
}

void BasketLesson() {
  static bool isSend;
  
  if (isTrainBasket) {
    if (!isSend) {
      Serial.println("story_59");
      isSend = 1;
    }
  }

  if (lessonSaluteActive) {
    MagicEffect();
  } else if (!snitchFlag) { 
    BasketEffectLesson();
  }

  // Чтение от Баскетбола
  while (Serial2.available()) {
    String buf = Serial2.readStringUntil('\n');
    buf.trim(); 
    if (buf.startsWith("log:")) {
      Serial.println(buf);
      // --- Перехват логов мальчика (страховка) ---
      if (buf.indexOf("Boy removed") != -1) {
         Serial.println("boy_out_lesson");
         lessonSaluteActive = false;
         snitchFlag = 1; 
         digitalWrite(Fireworks, LOW);
         strip1.clear(); strip2.clear(); strip1.show(); strip2.show();
      }
      if (buf.indexOf("Boy returned") != -1) { // Если вдруг вернется
         Serial.println("boy_in_lesson");
         if (snitchFlag == 1) lessonSaluteActive = true;
      }
      // ----------------------------------------------------
      continue;
    }
    // [ВАЖНО] Пересылка сообщений от мальчика
    if (buf == "boy_in_lesson") {
      Serial.println("boy_in_lesson");
      if (snitchFlag == 1) lessonSaluteActive = true; 
    } else if (buf == "boy_out_lesson") {
      Serial.println("boy_out_lesson");
      lessonSaluteActive = false;
      snitchFlag = 1; 
      digitalWrite(Fireworks, LOW);
      strip1.clear(); strip2.clear(); strip1.show(); strip2.show();
    } else if (buf == "lesson_goal") {
      Serial.println("lesson_goal");
    } else if (buf == "lesson_basket_done") {
      Serial.println("flying_ball");
      lessonSaluteActive = false;
      discoBallsActive = false;
      digitalWrite(Fireworks, LOW);
      snitchFlag = 0; 
      enemyTimer = millis();
      additionalTimer = millis();
      isSend = 0;
      level++;
    }
  }
  
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();

    if (buff == "start_lesson") Serial2.println("start_lesson");
    
    if (buff == "start_game_basket") {
      Serial.println("DEBUG: Received start_game_basket");
      lessonSaluteActive = false;
      digitalWrite(Fireworks, LOW);
      strip1.clear(); strip2.clear(); strip1.show(); strip2.show(); 
      snitchFlag = 0; 
      enemyTimer = millis(); additionalTimer = millis();
      Serial2.println("start_basket"); 
    }

    if (buff == "basket") {
      // ... (логика победы)
      lessonSaluteActive = false;
      discoBallsActive = true; 
      discoBallsTimer = millis();
      digitalWrite(Fireworks, LOW);
      Serial.println("win"); Serial2.println("win");
      strip1.clear(); strip2.clear(); strip1.show(); strip2.show();
      delay(1000);
      Serial.println("last_on");
      Serial1.println("firework"); delay(500); Serial1.println("firework");
      Serial2.println("firework"); delay(500); Serial2.println("firework");
      Serial3.println("firework"); delay(500); Serial3.println("firework");
      mySerial.println("firework"); delay(500); mySerial.println("firework");
      level = 20;
    }
    if (buff == "soundon") flagSound = 0;
    if (buff == "soundoff") flagSound = 1;
    
    // [ИСПРАВЛЕНИЕ] Корректный рестарт на этом уровне
    if (buff.indexOf("restart") != -1) {
      // 1. Рассылаем рестарт ВСЕМ (включая Баскет)
      Serial1.println("restart");
      Serial2.println("restart"); 
      Serial3.println("restart");
      mySerial.println("restart");
      
      // 2. Открываем дверь СОВ
      delay(50); 
      mySerial.println("open_door"); 

      // 3. Локальный сброс
      lessonSaluteActive = false;
      discoBallsActive = false;
      digitalWrite(Fireworks, LOW);
      isSend=0;
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
  }
}
////играем в баскет и передаем счет на башню сов
void Basket() {
  BasketEffect();
  // Логика Serial2 (basket3.ino)
  while (Serial2.available()) {
    String buf = Serial2.readStringUntil('\n');
    buf.trim();
    if (buf.startsWith("log:")) {
      Serial.println(buf);
      // --- Перехват логов мальчика (страховка) ---
      // Если команда потерялась, но лог пришел - реагируем на лог
      if (buf.indexOf("Boy removed") != -1) {
         snitchFlag = 1; // 1 = Пауза
         strip1.clear(); strip2.clear(); strip1.show(); strip2.show();
         Serial.println("boy_out_game");
      }
      if (buf.indexOf("Boy returned") != -1) {
         snitchFlag = 0; // 0 = Играем
         enemyTimer = millis(); 
         additionalTimer = millis();
         Serial.println("boy_in_game");
      }
      // ----------------------------------------------------
      continue;
    }
    
    // "boy_in_game" (из игры) -> Пересылаем и ЗАПУСКАЕМ/ВОЗОБНОВЛЯЕМ (snitchFlag = 0)
    if (buf == "boy_in_game") {
      snitchFlag = 0; // 0 = Анимация ВКЛ
      enemyTimer = millis(); 
      additionalTimer = millis();
      Serial.println("boy_in_game");
      
    // "boy_out_game" (из игры) -> Пересылаем и СТАВИМ ПАУЗУ (snitchFlag = 1)
    } else if (buf == "boy_out_game") {
      snitchFlag = 1; // 1 = Анимация ВЫКЛ
      // Принудительно останавливаем празднование гола (Зеленую волну)
      if (discoBallsActive) {
         discoBallsActive = false;
         digitalWrite(Fireworks, LOW); // Выключаем реле
         // Очищаем ленту от зеленого цвета
         strip1.clear(); 
         strip2.clear();
      }
      // ---------------------------------------------------------------
      enemyFlag = 0; // Сбрасываем атаку бота, если она шла
      strip1.clear(); 
      strip2.clear();
      strip1.show();
      strip2.show();
      Serial.println("boy_out_game");

    } else if (buf == "fr71nmr") {
      BotScore = "1"; 
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_1_bot"); 
      LooseSnitch();
    } else if (buf == "fr72nmr") {
      BotScore = "2"; 
      snitchFlag = 0;
      enemyTimer = millis();
      enemyFlag = 0;
      Serial.println("goal_2_bot");
      LooseSnitch();
    } else if (buf == "fr8nmr") {
      Serial.println("win");
      discoBallsActive = true;
      discoBallsTimer = millis();
      strip1.clear(); 
      strip2.clear();
      strip1.show();
      strip2.show();
      delay(1000);
      Serial.println("last_on");
      Serial1.println("firework");
      delay(500);
      Serial1.println("firework");
      Serial2.println("firework");
      delay(500);
      Serial2.println("firework");
      Serial3.println("firework");
      delay(500);
      Serial3.println("firework");
      mySerial.println("firework");
      delay(500);
      mySerial.println("firework");
      level = 20; 
    } else if (buf == "fr61nmr") {
      // snitchFlag = 0;
      // enemyTimer = millis(); 
      enemyFlag = 0;
      Serial.println("goal_1_player");
      // Включаем диско-шары ---
      discoBallsActive = true;
      discoBallsTimer = millis();
      GreenWaveEffect();
    } else if (buf == "fr62nmr") {
      // snitchFlag = 0;
      // enemyTimer = millis(); 
      enemyFlag = 0;
      Serial.println("goal_2_player");
      // Включаем диско-шары ---
      discoBallsActive = true;
      discoBallsTimer = millis();
      GreenWaveEffect();
    } else if (buf == "start_snitch") {
      snitchFlag = 0;
      enemyTimer = millis(); 
      enemyFlag = 0;
      Serial.println("start_snitch");
    } else if (buf == "fr9nmr") {
      Serial.println("DEBUG: Robot WON (fr9nmr received)"); // ЛОГ
      // Выключить диско-шары при проигрыше ---
      discoBallsActive = false; 
      digitalWrite(Fireworks, LOW);
      Serial.println("win_robot");
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
      snitchFlag = 1;
    }
  }

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "basket") {
      Serial.println("win");
      discoBallsActive = true;
      discoBallsTimer = millis();
      Serial2.println("win");
      strip1.clear();
      strip2.clear();
      strip1.show();
      strip2.show();
      delay(1000);
      Serial.println("last_on");
      Serial1.println("firework");
      delay(500);
      Serial1.println("firework");
      Serial2.println("firework");
      delay(500);
      Serial2.println("firework");
      Serial3.println("firework");
      delay(500);
      Serial3.println("firework");
      mySerial.println("firework");
      delay(500);
      mySerial.println("firework");
      level = 20;
    } else if (buff.indexOf("restart") != -1) {
      Serial1.println("restart");
      Serial2.println("restart");
      Serial3.println("restart");
      mySerial.println("restart");
      // --------------------------
      
      lessonSaluteActive = false;
      discoBallsActive = false;
      digitalWrite(Fireworks, LOW);
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
      else if (buff == "soundon") {
      flagSound = 0;
    }
      else if (buff == "soundoff") {
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
        
        // Вместо for/delay запускаем неблокирующий механизм
        isUfBlinking = true;
        ufBlinkCount = 0;
        ufBlinkTimer = millis();
        
        digitalWrite(UfHallLight, LOW);
        digitalWrite(HallLight, LOW);
        ghostState++;
      }
      break;
    /*
    // Добавляем новое состояние для ожидания третьего стука игрока
    case 5:
      // Ждем, пока игрок постучит по датчику
       if (analogRead(KnockSens) <= threshold) {
        Serial.println("punch");  // Отправляем подтверждение стука на сервер
        ghostState = 6;           // Теперь переходим к финальной загадке со стуком
        delay(300);               // Небольшая задержка для антидребезга датчика
      }
      break;
    */
    case 6:
      // --- ИЗМЕНЕНИЕ: Вся логика KnockInterval и KnockSol УДАЛЕНА ---
      // --- Теперь мы НЕМЕДЛЕННО слушаем датчик стука ---

      // Фильтрация (остается)
      if (++index > 2)
        index = 0;
      val[index] = analogRead(KnockSens);
      val_filter = middle_of_3(val[0], val[1], val[2]);
      // Serial.println(analogRead(KnockSens));

      // Проверка стука (убираем 'if (KnockState != 1)')
      if (analogRead(KnockSens) <= threshold) {
        Serial.println("punch");
        digitalWrite(KnockSol, LOW); // Оставляем выключение, на всякий случай
        digitalWrite(LibraryLight, HIGH);
        libraryDoorTimer = millis();
        level++;
      }
      break;
  }


  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "rrt3lck") {
      //OpenLock(LibraryDoor);
      digitalWrite(KnockSol, LOW);
      digitalWrite(LibraryLight, HIGH);
      level++;
    }
    if (buff == "first_clock_2") {
        Serial.println("h_clock");
        digitalWrite(UfHallLight, HIGH);
        digitalWrite(HallLight, LOW);
        ghostState=1;
    }
    if (buff == "second_clock_2") {
       Serial.println("uf_clock");
        for (int i = 0; i < 3; i++) {
          digitalWrite(UfHallLight, HIGH);
          delay(500);
          digitalWrite(UfHallLight, LOW);
          delay(500);
        }
        digitalWrite(UfHallLight, LOW);
        digitalWrite(HallLight, LOW);
        ghostState=2;
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
      level = 25;
      return;
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
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
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
        Serial.println("ghost_knock");
        ghostState = 6;
      }
      if (ghostState < 4)
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
    // Выключаем свет в библиотеке ---
    digitalWrite(LibraryLight, LOW);

    level++;
  }
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
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
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
    if (buff == "restart") {
      OpenAll();
      RestOn();
      level = 25;
      return;
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
  // state 0: Ожидание камина (Геркон 1)
  // state 1: Ожидание кубка (Геркон 2)
  static int state = 0;
  
  // 'flag' отвечает за ЛОГИКУ ЗАГАДКИ (камин должен быть нажат перед кубком)
  static bool flag = 0; 
  
  // 'fireplacePressed' отвечает за ЛОГИКУ ЗВУКА (один звук за одно нажатие)
  static bool fireplacePressed = false;

  switch (state) {
    case 0: // Ожидание камина (Геркон 1)
      
      // Проверяем геркон камина
      if (!digitalReadExpander(3, board4)) { // ГЕРКОН 1 (Камин) НАЖАТ
        if (!fireplacePressed) { // Если мы его еще не "защелкнули"
          Serial.println("fire");    // Отправляем команду на звук
          fireplacePressed = true; // "Защелкиваем" звук
          flag = 1;                // "Защелкиваем" логику (теперь можно нажать кубок)
        }
      } else { // ГЕРКОН 1 (Камин) ОТПУЩЕН
        fireplacePressed = false; // "Отщелкиваем" звук (можно нажать снова)
      }

      // Проверяем, был ли камин нажат И ЗАТЕМ отпущен, чтобы перейти к кубку
      if (digitalReadExpander(3, board4) && flag) {
        state = 1; // Переходим в состояние ожидания кубка
      }
      break;

    case 1: // Ожидание кубка (Геркон 2)

      // Проверяем геркон кубка
      if (!digitalReadExpander(2, board4)) { // ГЕРКОН 2 (Кубок) НАЖАТ
        Serial2.println("opent_basket");
        Serial.println("door_basket");
        fireFlag = 1;
        state = 0; // Сбрасываем всю логику
        flag = 0;
        fireplacePressed = false;
        level++;
      }

      // Мы также продолжаем проверять геркон камина,
      // чтобы игрок мог нажимать его для звука, пока ждет.
      if (!digitalReadExpander(3, board4)) { // ГЕРКОN 1 (Камин) НАЖАТ
        if (!fireplacePressed) {
          Serial.println("fire");
          fireplacePressed = true;
        }
      } else { // ГЕРКОН 1 (Камин) ОТПУЩЕН
        fireplacePressed = false;
      }
      break;
  }

  // --- Обработка команд Serial (остается без изменений) ---
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    // Обработчик команды мерцания ---
    if (buff == "library_flicker_start") {
      isLibraryFlickering = true;
      libraryFlickerTimer = millis();
      libraryFlickerIntervalTimer = millis();
      digitalWrite(LibraryLight, HIGH);
      // Свет уже был выключен в level 12,
      // поэтому мерцание начнется с ВКЛючения.
    }
    if (buff == "student_hide") {
      boyServo.attach(49);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }

    if (buff == "door_top") {
      Serial1.println("day_on");
      // ... (остальной код door_top) ...
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
      state = 0;
      fireplacePressed = false; // <-- Добавлен сброс
      OpenAll();
      RestOn();
      level = 25;
      return;
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
  // --- Переменные ---
  // Состояния: 0=Idle, 1=WaitR, 2=WaitL, 3=Cooldown
  static int swipeState = 0;
  static int puzzleProgress = 0;
  static unsigned long swipeTimer = 0;
  const unsigned long SWIPE_TIMEOUT = 1500;
  static unsigned long lastDebounceTimeLeft = 0;
  static unsigned long lastDebounceTimeRight = 0;
  static bool lastSteadyLeftState = HIGH;
  static bool lastSteadyRightState = HIGH;
  static bool currentLeftState = HIGH;
  static bool currentRightState = HIGH;
  const unsigned long debounceDelay = 150;
  unsigned long currentTime = millis();
  static bool initialSwitchReleased = false;

  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();

    if (buff == "spell") {
      OpenLock(HightTowerDoor);
      digitalWrite(TorchLight, HIGH);
      Serial.println("door_spell");  // Исходное сообщение
      level++;
      initialSwitchReleased = false;
      puzzleProgress = 0;
      swipeState = 0;
    }
    if (buff == "restart") {
       swipeState = 0;
       puzzleProgress = 0;
   swipeTimer = 0;
   lastDebounceTimeLeft = 0;
   lastDebounceTimeRight = 0;
   lastSteadyLeftState = HIGH;
   lastSteadyRightState = HIGH;
   currentLeftState = HIGH;
   currentRightState = HIGH;
   currentTime = millis();
   initialSwitchReleased = false;
      OpenAll();
      RestOn();
      level = 25;
      return;
    }
    if (buff == "soundon") {
      flagSound = 0;
    }
    if (buff == "soundoff") {
      flagSound = 1;
    }
  }


  // --- Чтение сенсоров с антидребезгом (без изменений) ---
  bool readingLeft = (digitalReadExpander(0, board4) == LOW);   // Пин 0 = Левый
  bool readingRight = (digitalReadExpander(1, board4) == LOW);  // Пин 1 = Правый
  // --- Антидребезг (код без изменений) ---
  if (readingLeft != lastSteadyLeftState) { lastDebounceTimeLeft = currentTime; }
  if ((currentTime - lastDebounceTimeLeft) > debounceDelay) {
    if (readingLeft != currentLeftState) { currentLeftState = readingLeft; }
  }
  lastSteadyLeftState = readingLeft;
  if (readingRight != lastSteadyRightState) { lastDebounceTimeRight = currentTime; }
  if ((currentTime - lastDebounceTimeRight) > debounceDelay) {
    if (readingRight != currentRightState) { currentRightState = readingRight; }
  }
  lastSteadyRightState = readingRight;
  bool leftPressed = currentLeftState;
  bool rightPressed = currentRightState;

  // --- Машина состояний ---
  switch (swipeState) {
    case 0:  // IDLE
      if (leftPressed && !rightPressed) {
        swipeState = 1;
        initialSwitchReleased = false;
      } else if (rightPressed && !leftPressed) {
        swipeState = 2;
        initialSwitchReleased = false;
      }
      break;

    case 1:                // WAIT_R
      if (rightPressed) {  // --- СВАЙП ВПРАВО (>) ---
        Serial.println("swipe_r");

        if (puzzleProgress == 0 || puzzleProgress == 2 || puzzleProgress == 4) {
          puzzleProgress++;

          // Отправка шагов ---
          if (puzzleProgress == 1) Serial.println("spell_step_1");
          if (puzzleProgress == 3) Serial.println("spell_step_3");
          if (puzzleProgress == 5) {
            Serial.println("spell_step_5"); // Это 5-й шаг (успех)
          }
          if (puzzleProgress == 5) {  // ПОБЕДА
            OpenLock(HightTowerDoor);
            digitalWrite(TorchLight, HIGH);
            Serial.println("door_spell");
            level++;
            puzzleProgress = 0;
          } else {  // Успешный шаг
          }
        } else {  // Неправильная последовательность
          Serial.println("swipe_wrong_sequence");
          Serial.println("spell_reset");
          puzzleProgress = 0;
        }
        swipeState = 3;  // Cooldown
        initialSwitchReleased = false;
      } else if (!leftPressed && !initialSwitchReleased) {
        initialSwitchReleased = true;
        swipeTimer = currentTime;
      } else if (initialSwitchReleased && (currentTime - swipeTimer > SWIPE_TIMEOUT)) {
        swipeState = 3;
        initialSwitchReleased = false;
      }
      break;

    case 2:               // WAIT_L
      if (leftPressed) {  // --- СВАЙП ВЛЕВО (<) ---
        Serial.println("swipe_l");

        if (puzzleProgress == 1 || puzzleProgress == 3) {
          puzzleProgress++;

          // Отправка шагов ---
          if (puzzleProgress == 2) Serial.println("spell_step_2");
          if (puzzleProgress == 4) Serial.println("spell_step_4");
        } else {  // Неправильная последовательность
          Serial.println("swipe_wrong_sequence");
          Serial.println("spell_reset");
          puzzleProgress = 0;
        }
        swipeState = 3;  // Cooldown
        initialSwitchReleased = false;
      } else if (!rightPressed && !initialSwitchReleased) {
        initialSwitchReleased = true;
        swipeTimer = currentTime;
      } else if (initialSwitchReleased && (currentTime - swipeTimer > SWIPE_TIMEOUT)) {
        swipeState = 3;
        initialSwitchReleased = false;
      }
      break;

    case 3:  // COOLDOWN
      if (!leftPressed && !rightPressed) {
        swipeState = 0;
        initialSwitchReleased = false;
      }
      break;

  }  // Конец switch

  HelpTowersHandler();
}  // Конец функции

/////нужно подуть в окно
void OpenBank() {
  // Условие для немедленного первого запуска ---
  // Проверяем, не ждем ли мы уже ответа (helpsBankTimerWaiting == false) И
  // (Это первый запуск ИЛИ прошло 30 секунд)
  bool timeToPlay = (millis() - helpsBankTimer >= 30000);

  if (!helpsBankTimerWaiting && (isBankerFirstHint || timeToPlay)) {
    int helpCounter = 0;
    helpCounter = random(0, 3); // (для 3-х вариантов a, b, c) 
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
    // НЕ СБРАСЫВАЕМ ТАЙМЕР. Вместо этого, взводим флаг ожидания.
    helpsBankTimerWaiting = true;
    // Сбрасываем флаг первого запуска ---
    isBankerFirstHint = false;
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
    helpsBankTimerWaiting = false; // [ДОБАВЛЕНО] Сброс флага при выходе с уровня
    // Восстанавливаем флаг при выходе с уровня ---
    isBankerFirstHint = true;
  }
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "story_22_done") {
      helpsBankTimer = millis(); // Вот ТЕПЕРЬ сбрасываем таймер
      helpsBankTimerWaiting = false; // Снимаем флаг ожидания
    }
    else if (buff == "student_hide") {
      boyServo.attach(49);
      digitalWrite(HallLight, HIGH);
      digitalWrite(MansardLight, HIGH);
      boyServo.write(0);
      unsigned long sTime = millis();
      while(millis() - sTime < 1000) {
         MonitorSerialBuffer(); // Просто читаем, чтобы буфер не лопнул
      }
      boyServo.detach();
    }
    else if (buff == "open_bank_door") {
      CandleStrip.setPixelColor(0, CandleStrip.Color(0, 0, 0));
      CandleStrip.show();
      Serial.println("miror");
      scrollNumber = -1;
      //digitalWrite(BankRoomLight, HIGH);
      //OpenLock(BankDoor);
      level++;
    }
    else if (buff == "restart") {
      OpenAll();
      RestOn();
      level = 25;
      helpsBankTimerWaiting = false;
      // Восстанавливаем флаг при рестарте ---
      isBankerFirstHint = true;
      return;
    }
    else if (buff == "soundon") {
      flagSound = 0;
    }
    else if (buff == "soundoff") {
      flagSound = 1;
    }
  }
  HelpTowersHandler();
}

void Scrolls() {
  // Немедленный выход после победы ---
  if (scrollNumber == 5) {
    // В этом состоянии мы ждем 4 секунды в case 5: для перехода на level 10.
    // Если мы здесь, но не в case 5:, значит, игра уже была завершена (например, через Skip),
    // или мы ждем перехода. Мы должны игнорировать герконы.

    // Но мы должны убедиться, что отрабатывает case 5: для перехода
    if (millis() - safeTimer >= 4000) {
        OpenLock(BankStashDoor); 
        level++; // Переходим на level 10 (Oven) 
    }
    // Если safeTimer еще не истек, мы просто выходим из Scrolls()
    // и игнорируем ВСЕ герконы.
    
    // Также обрабатываем входящие команды, если игра пройдена, 
    // чтобы не блокировать serial (команды open_safe, restart).
    while (Serial.available()) {
        String buff = Serial.readStringUntil('\n'); 
        buff.trim(); 
        if (buff == "open_safe") {
            OpenLock(BankStashDoor);
        }
        else if (buff == "restart") {
            OpenAll();
            RestOn();
            level = 25;
            return;
        }
        else if (buff == "soundon") {
            flagSound = 0;
        }
        else if (buff == "soundoff") {
            flagSound = 1;
        }
    }
    HelpTowersHandler(); 
    return; // Немедленно выходим, чтобы не проверять ScrollOne-Four
  }
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
      break;
    // ScrollFive() устанавливает scrollNumber = 5  и safeTimer.
    // Этот case 5: "ловит" это состояние, ждет 4 секунды и
    // наконец переключает Arduino на level 10 (Oven).
    case 5:

      break;
  }
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    Serial.println(buff);
    if (buff == "open_safe") {
      OpenLock(BankStashDoor);
      level++;
    }
    if (buff == "safe") {
      scrollNumber = 5;
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
      level = 25;
      return;
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

// ПРАВИЛЬНАЯ ПОСЛЕДОВАТЕЛЬНОСТЬ:
// Шаг 1 (ScrollOne):   Геркон 1 (Пин 2, board3)
// Шаг 2 (ScrollTwo):   Геркон 2 (Пин 0, board3)
// Шаг 3 (ScrollThree): Геркон 3 (Пин 3, board3)
// Шаг 4 (ScrollFour):  Геркон 2 (Пин 0, board3)
// Шаг 5 (ScrollFive):  Геркон 4 (Пин 1, board3)

void ScrollOne() {
  // --- ШАГ 1: ЖДЕМ ГЕРКОН 1 (Пин 2, board3) ---
  
  // --- Проверка ГЕРКОНА 1 (Правильный) ---
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) { Serial.println("safe_turn"); Scroll1On = 1; code1Timer = millis(); }
    if (Scroll1On && !Scroll11On && (millis() - code1Timer >= 2000)) {
      scrollNumber = 1; // Успех, переход на шаг 2
      Serial.println("safe_fix");
      Serial.println("safe_step_1");
      Scroll11On = 1; 
    }
  } else {
    Scroll11On = 0; Scroll1On = 0; code1Timer = millis();
  }

  // --- Проверка ГЕРКОНА 2 (Ошибка) ---
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) { Serial.println("safe_turn"); Scroll2On = 1; code2Timer = millis(); }
    if (Scroll2On && !Scroll21On && (millis() - code2Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll21On = 1;
    }
  } else {
    Scroll2On = 0; Scroll21On = 0; code2Timer = millis();
  }

  // --- Проверка ГЕРКОНА 3 (Ошибка) ---
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) { Serial.println("safe_turn"); Scroll3On = 1; code3Timer = millis(); }
    if (Scroll3On && !Scroll31On && (millis() - code3Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll31On = 1;
    }
  } else {
    Scroll3On = 0; Scroll31On = 0; code3Timer = millis();
  }
  
  // --- Проверка ГЕРКОНА 4 (Ошибка) ---
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) { Serial.println("safe_turn"); Scroll4On = 1; code4Timer = millis(); }
    if (millis() - code4Timer >= 2000) {
      if (!Scroll41On) {
        scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll41On = 1;
      }
    }
  } else {
    Scroll4On = 0; Scroll41On = 0; code4Timer = millis();
  }
}

void ScrollTwo() {
  // --- ШАГ 2: ЖДЕМ ГЕРКОН 2 (Пин 0, board3) ---

  // --- ИЗМЕНЕНИЕ: Проверка ГЕРКОНА 1 (Сброс на Шаг 1) ---
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) { Serial.println("safe_turn"); Scroll1On = 1; code1Timer = millis(); }
    if (Scroll1On && !Scroll11On && (millis() - code1Timer >= 2000)) {
      Scroll11On = 1; 
      scrollNumber = 1; // Устанавливаем Шаг 1
      Serial.println("safe_fix"); 
      Serial.println("safe_reset"); // Сначала сброс
      Serial.println("safe_step_1"); // Потом засчитываем Шаг 1
      Scroll1On = 0; code1Timer = millis(); 
    }
  } else {
    Scroll11On = 0; Scroll1On = 0; code1Timer = millis();
  }

  // --- Проверка ГЕРКОНА 2 (Правильный) ---
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) { Serial.println("safe_turn"); Scroll2On = 1; code2Timer = millis(); }
    if (Scroll2On && !Scroll21On && (millis() - code2Timer >= 2000)) {
      scrollNumber = 2; // Успех, переход на шаг 3
      Serial.println("safe_fix");
      Serial.println("safe_step_2");
      Scroll21On = 1;
    }
  } else {
    Scroll2On = 0; Scroll21On = 0; code2Timer = millis();
  }

  // --- Проверка ГЕРКОНА 3 (Ошибка) ---
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) { Serial.println("safe_turn"); Scroll3On = 1; code3Timer = millis(); }
    if (Scroll3On && !Scroll31On && (millis() - code3Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll31On = 1;
    }
  } else {
    Scroll3On = 0; Scroll31On = 0; code3Timer = millis();
  }
  
  // --- Проверка ГЕРКОНА 4 (Ошибка) ---
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) { Serial.println("safe_turn"); Scroll4On = 1; code4Timer = millis(); }
    if (Scroll4On && !Scroll41On && (millis() - code4Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll41On = 1;
    }
  } else {
    Scroll4On = 0; Scroll41On = 0; code4Timer = millis();
  }
}

void ScrollThree() {
  // --- ШАГ 3: ЖДЕМ ГЕРКОН 3 (Пин 1, board3) ---

  // --- ИЗМЕНЕНИЕ: Проверка ГЕРКОНА 1 (Сброс на Шаг 1) ---
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) { Serial.println("safe_turn"); Scroll1On = 1; code1Timer = millis(); }
    if (Scroll1On && !Scroll11On && (millis() - code1Timer >= 2000)) {
      Scroll11On = 1; 
      scrollNumber = 1; // Устанавливаем Шаг 1
      Serial.println("safe_fix"); 
      Serial.println("safe_reset"); 
      Serial.println("safe_step_1"); 
    }
  } else {
    Scroll11On = 0; Scroll1On = 0; code1Timer = millis();
  }

  // --- Проверка ГЕРКОНА 2 (Ошибка) ---
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) { Serial.println("safe_turn"); Scroll2On = 1; code2Timer = millis(); }
    if (Scroll2On && !Scroll21On && (millis() - code2Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll21On = 1;
    }
  } else {
    Scroll2On = 0; Scroll21On = 0; code2Timer = millis();
  }
  
  // --- Проверка ГЕРКОНА 3 (Правильный) ---
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) { Serial.println("safe_turn"); Scroll3On = 1; code3Timer = millis(); }
    if (Scroll3On && !Scroll31On && (millis() - code3Timer >= 2000)) {
      scrollNumber = 3; // Успех, переход на шаг 4
      Serial.println("safe_fix");
      Serial.println("safe_step_3");
      Scroll31On = 1;
    }
  } else {
    Scroll3On = 0; Scroll31On = 0; code3Timer = millis();
  }

  // --- Проверка ГЕРКОНА 4 (Ошибка) ---
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) { Serial.println("safe_turn"); Scroll4On = 1; code4Timer = millis(); }
    if (Scroll4On && !Scroll41On && (millis() - code4Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll41On = 1;
    }
  } else {
    Scroll4On = 0; Scroll41On = 0; code4Timer = millis();
  }
}

void ScrollFour() {
  // --- ШАГ 4: ЖДЕМ ГЕРКОН 2 (Пин 0, board3) ---

  // --- ИЗМЕНЕНИЕ: Проверка ГЕРКОНА 1 (Сброс на Шаг 1) ---
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) { Serial.println("safe_turn"); Scroll1On = 1; code1Timer = millis(); }
    if (Scroll1On && !Scroll11On && (millis() - code1Timer >= 2000)) {
      Scroll11On = 1; 
      scrollNumber = 1; // Устанавливаем Шаг 1
      Serial.println("safe_fix"); 
      Serial.println("safe_reset"); 
      Serial.println("safe_step_1"); 
    }
  } else {
    Scroll11On = 0; Scroll1On = 0; code1Timer = millis();
  }

  // --- Проверка ГЕРКОНА 2 (Правильный) ---
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) { Serial.println("safe_turn"); Scroll2On = 1; code2Timer = millis(); }
    if (Scroll2On && !Scroll21On && (millis() - code2Timer >= 2000)) {
      scrollNumber = 4; // Успех, переход на шаг 5
      Serial.println("safe_fix");
      Serial.println("safe_step_4");
      Scroll21On = 1;
    }
  } else {
    Scroll2On = 0; Scroll21On = 0; code2Timer = millis();
  }

  // --- Проверка ГЕРКОНА 3 (Ошибка) ---
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) { Serial.println("safe_turn"); Scroll3On = 1; code3Timer = millis(); }
    if (Scroll3On && !Scroll31On && (millis() - code3Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll31On = 1;
    }
  } else {
    Scroll3On = 0; Scroll31On = 0; code3Timer = millis();
  }

  // --- Проверка ГЕРКОНА 4 (Ошибка) ---
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) { Serial.println("safe_turn"); Scroll4On = 1; code4Timer = millis(); }
    if (Scroll4On && !Scroll41On && (millis() - code4Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll41On = 1;
    }
  } else {
    Scroll4On = 0; Scroll41On = 0; code4Timer = millis();
  }
}

void ScrollFive() {
  // --- ШАГ 5: ЖДЕМ ГЕРКОН 4 (Пин 3, board3) ---

  // --- ИЗМЕНЕНИЕ: Проверка ГЕРКОНА 1 (Сброс на Шаг 1) ---
  if (!digitalReadExpander(2, board3)) {
    if (!Scroll1On && (millis() - code1Timer >= 300)) { Serial.println("safe_turn"); Scroll1On = 1; code1Timer = millis(); }
    if (Scroll1On && !Scroll11On && (millis() - code1Timer >= 2000)) {
      Scroll11On = 1; 
      scrollNumber = 1; // Устанавливаем Шаг 1
      Serial.println("safe_fix"); 
      Serial.println("safe_reset"); 
      Serial.println("safe_step_1"); 
    }
  } else {
    Scroll11On = 0; Scroll1On = 0; code1Timer = millis();
  }
  
  // --- Проверка ГЕРКОНА 2 (Ошибка) ---
  if (!digitalReadExpander(0, board3)) {
    if (!Scroll2On && (millis() - code2Timer >= 300)) { Serial.println("safe_turn"); Scroll2On = 1; code2Timer = millis(); }
    if (Scroll2On && !Scroll21On && (millis() - code2Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll21On = 1;
    }
  } else {
    Scroll2On = 0; Scroll21On = 0; code2Timer = millis();
  }

  // --- Проверка ГЕРКОНА 3 (Ошибка) ---
  if (!digitalReadExpander(3, board3)) {
    if (!Scroll3On && (millis() - code3Timer >= 300)) { Serial.println("safe_turn"); Scroll3On = 1; code3Timer = millis(); }
    if (Scroll3On && !Scroll31On && (millis() - code3Timer >= 2000)) {
      scrollNumber = 0; Serial.println("safe_fix"); Serial.println("safe_reset"); Scroll31On = 1;
    }
  } else {
    Scroll3On = 0; Scroll31On = 0; code3Timer = millis();
  }

  // --- Проверка ГЕРКОНА 4 (Правильный - Финал) ---
  if (!digitalReadExpander(1, board3)) {
    if (!Scroll4On && (millis() - code4Timer >= 300)) { Serial.println("safe_turn"); Scroll4On = 1; code4Timer = millis(); }
    if (Scroll4On && !Scroll41On && (millis() - code4Timer >= 2000)) {
      scrollNumber = 5; // Успех, ФИНАЛ
      Serial.println("safe_end");
      GoldStrip.setPixelColor(0, GoldStrip.Color(255, 255, 0));
      GoldStrip.show();
      safeTimer = millis();
      Scroll41On = 1;
    }
  } else {
    Scroll4On = 0; Scroll41On = 0; code4Timer = millis();
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
  while (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "crystals") {
      Serial.println("cristal_up");
      OpenLock(MemoryRoomDoor);
      level++;
    } else if (cmd == "restart") {
      lastPress = 0;
      OpenAll();
      RestOn();
      level = 25;
      return;
    } else if (cmd == "soundon") {
      flagSound = 0;
      Serial.println("Sound ON");
    } else if (cmd == "soundoff") {
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
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "memory_room_end") {
      Serial.println("memory_room_end");
      for (long firstPixelHue = 0; firstPixelHue < 2 * 65536; firstPixelHue += 556) {
        
        // 1. Слушаем сервер даже во время радуги!
        if (Serial.available()) {
           String urgentBuff = Serial.readStringUntil('\n');
           urgentBuff.trim();
           if (urgentBuff.indexOf("restart") != -1) { // Если пришел рестарт - прерываем
             OpenAll();
             RestOn();
             level = 25;
             return;
           }
        }
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
      level = 25;
      return;
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
    //Serial2.println("start_lesson");
    //delay(1000);
    //Serial2.println("start_lesson");
    flag = 0;
    level++;
  }


  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff == "basket") {
      Serial.println("win");
      discoBallsActive = true;
      discoBallsTimer = millis();
      Serial2.println("win");
      level++;
    }
    if (buff == "crime") {
      Serial.println("crime_end");
      //Serial2.println("start_lesson");
      //delay(1000);
      //Serial2.println("start_lesson");
      flag = 0;
      level++;
    }
    if (buff == "restart") {
      flag=0;
      OpenAll();
      RestOn();
      level = 25;
      return;
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
  delay(300);
  digitalWrite(pin, 0);
}


void FinalPresentation() {
  digitalWrite(Fireworks, HIGH);
  MagicEffect();
}


void rainbow() {
  // Просто взводим флаг. Сама анимация будет в loop()
  isRainbowActive = true;
  rainbowHue = 0;
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
  // Сбрасываем массив active, чтобы handleLocks() не мешал открытию
  for (int i = 0; i < DOORS; i++) {
      active[i] = false;
  }
  
  for (int i = 0; i < DOORS; i++) {
    digitalWrite(doors[i], HIGH); // Включаем
    delay(300);                   
    digitalWrite(doors[i], LOW);  // Выключаем
    delay(100);                   
  }
  // ФИНАЛЬНАЯ СТРАХОВКА: Еще раз принудительно выключаем всё
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
  if (millis() - CauldronFireMistakeInterval >= random(100, 130)) {
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
          */
            // --- КОНЕЦ ИЗМЕНЕНИЯ ---

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
      */
        // --- КОНЕЦ ИЗМЕНЕНИЯ ---

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
          _levels = 3;  // Новый переход сразу на последний уровень
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
void ClearSatelliteBuffers() {
  while (Serial1.available()) Serial1.read();
  while (Serial2.available()) Serial2.read();
  while (Serial3.available()) Serial3.read();
  while (mySerial.available()) mySerial.read();
}
void RestOn() {
  // -------------------------------------------------------
  // БЛОК 1: ОДНОКРАТНАЯ ИНИЦИАЛИЗАЦИЯ
  // Выполняется только один раз при входе в режим.
  // -------------------------------------------------------
  if (!isRestInitialized) {
    // 1. Сбрасываем автоматику и закрываем двери
    for (int i = 0; i < DOORS; i++) {
      active[i] = false;
      digitalWrite(doors[i], LOW);
    }

    if (firstRun) firstRun = false;
    boyServo.detach();

    // 2. Включаем статический свет
    digitalWrite(MansardLight, HIGH);
    digitalWrite(LastTowerTopLight, HIGH);
    digitalWrite(Fireworks, LOW);
    digitalWrite(BankRoomLight, HIGH);
    digitalWrite(TorchLight, HIGH);
    digitalWrite(HallLight, HIGH);
    digitalWrite(UfHallLight, HIGH);
    digitalWrite(LibraryLight, HIGH);

    // 3. Настраиваем цвет лент (Синий)
    uint32_t restartColor = GoldStrip.Color(0, 0, 200);
    for (int i = 0; i < GoldStrip.numPixels(); i++) GoldStrip.setPixelColor(i, restartColor);
    for (int i = 0; i < CandleStrip.numPixels(); i++) CandleStrip.setPixelColor(i, restartColor);
    for (int i = 0; i < CauldronStrip.numPixels(); i++) CauldronStrip.setPixelColor(i, restartColor);
    for (int i = 0; i < CauldronRoomStrip.numPixels(); i++) CauldronRoomStrip.setPixelColor(i, restartColor);
    for (int i = 0; i < memory_Led.numPixels(); i++) memory_Led.setPixelColor(i, restartColor);
    for (int i = 0; i < strip1.numPixels(); i++) strip1.setPixelColor(i, strip1.Color(0, 0, 0));
    for (int i = 0; i < strip2.numPixels(); i++) strip2.setPixelColor(i, strip2.Color(0, 0, 0));

    // 4. Обновляем ленты (ЭТО ОТКЛЮЧАЕТ ПРЕРЫВАНИЯ, НО ТЕПЕРЬ ТОЛЬКО 1 РАЗ)
    CandleStrip.show(); CauldronStrip.show(); CauldronRoomStrip.show();
    memory_Led.show(); GoldStrip.show(); strip1.show(); strip2.show();

    // 5. Сброс переменных игровой логики
    _presentTimer; _stages = 0; _present = 0; lightBr = 0;
    iterator = 0; symbolFade = 1; flagSalut = 1; _levels = 0; _stones = 0;
    symbolBrightness = 0; symbolFadeTimer = 0; fadeWhiteTimer = 0;
    rainbowCycles = 0; rainbowCycleCycles = 0; repeatCount = 0;
    FirstFlag = 0; SecondFlag = 0; ThirdFlag = 0; FourFlag = 0;
    KnockState = 0; FirstBottleTrue = 1; FirstBottleFalse = 1;
    SecondBottleTrue = 1; SecondBottleFalse = 1; ThirdBottleTrue = 1;
    ThirdBottleFalse = 1; FourBottleTrue = 1; FourBottleFalse = 1;
    flagSound = 1; FireInterval = 0; CauldronFireInterval = 0;
    CauldronFireMistakeInterval = 0; CauldronFireTrueInterval = 0;
    KnockInterval = 0; KnockIntervalLow = 0; Bottle1Timer = 0;
    Bottle2Timer = 0; Bottle3Timer = 0; Bottle4Timer = 0;
    scrollNumber = 0; Bottle = 0; BotScore = ""; OwlScore = 0;
    NumKnock = 0; doorFlags = 1; flagStory = 1;
    helpsBankTimerWaiting = false; isBankerFirstHint = true;
    enemyTimer = 0; additionalTimer = 0; cometTimer1 = 0;
    direction = 1; headPosition = 0; enemyFlag = 0;
    cometLength = 5; cometPosition = -cometLength;
    clickFlag1 = 0; clickFlag2 = 0; clickFlag3 = 0; clickFlag4 = 0;
    clickFlag11 = 0; clickFlag22 = 0; clickFlag31 = 0; clickFlag41 = 0;
    snitchFlag = 1; stepsFlag = 0; centralTowerGameFlag = 0; fireFlag = 0;
    code1Timer = 0; code2Timer = 0; code3Timer = 0; code4Timer = 0;
    libraryDoorTimer = 0; Scroll1On = 0; Scroll2On = 0; Scroll3On = 0;
    Scroll4On = 0; Scroll11On = 0; Scroll21On = 0; Scroll31On = 0;
    Scroll41On = 0; isPotionEnd = 0; isDogEnd = 0; isOwlEnd = 0;
    isTrainEnd = 0; isTrollEnd = 0; isTrainBasket = 0; ghostState = 0;
    isStartDoorOpenedGlobal = false;
    
    // Ставим флаг, что инициализация завершена.
    // Больше в этот блок мы не зайдем, пока не выйдем из уровня 25.
    isRestInitialized = true; 
  }

  // -------------------------------------------------------
  // БЛОК 2: ЦИКЛИЧЕСКАЯ ОБРАБОТКА (БЫСТРАЯ)
  // Здесь нет strip.show(), поэтому прерывания работают,
  // и Serial отлично принимает команды.
  // -------------------------------------------------------
  
  handleIdleBoySensor();

  while (Serial.available()) {
    ClearSatelliteBuffers(); // Чистим входящий мусор от башен
    
    String buff = Serial.readStringUntil('\n');
    buff.trim();

    // ДОБАВЛЕНО: Логирование входящей команды от сервера
    if (buff.length() > 0) {
      Serial.print("log:main:RestOn: Received command: ");
      Serial.println(buff);
      delay(10); // Задержка для разделения лога и возможной пересылки
    }
    // КОНЕЦ

    if (buff.indexOf("restart") != -1) {
      Serial1.println("restart"); delay(20);
      Serial2.println("restart"); delay(20);
      Serial3.println("restart"); delay(20);
      mySerial.println("restart"); delay(50); 
      mySerial.println("restart");
      
      OpenAll(); 
      
      // [ВАЖНО] Если нажали рестарт, находясь в рестарте - 
      // сбрасываем флаг, чтобы проинициализировать (моргнуть светом) снова.
      isRestInitialized = false; 
      return;
    }

    if (buff.indexOf("ready") != -1) {
       _dataQueue = 0; _towerTimer = 0; _towerCounter = 0; doorEvent = 0;
       mansardEvent = 0; libraryEvent = 0; galet1Event = 0; galet2Event = 0;
       galet3Event = 0; galet4Event = 0; galet5Event = 0; sealEvent = 0;
       sealSpaceEvent = 0; finalEvent = 0; crimeEvent = 0; safeEvent = 0;
       bugTimerScroll = 0;
       for (int i = 0; i < DOORS; i++) active[i] = false;

       Serial1.println("ready"); Serial2.println("ready");
       Serial3.println("ready"); mySerial.println("ready");

       if (digitalRead(startDoorPin)) Serial.println("open_door");
       if (!digitalRead(galetSwitchesPin)) Serial.println("galet_on");
       if (digitalReadExpander(4, board4)) Serial.println("cristal_up");
       if (digitalReadExpander(7, board1)) Serial.println("boy_out");
       if (digitalReadExpander(5, board3)) Serial.println("lib_door");
       if (digitalReadExpander(1, board2)) Serial.println("crime_open");
       if (digitalReadExpander(3, board3) && digitalReadExpander(0, board3) && digitalReadExpander(1, board3) && digitalReadExpander(2, board3)) Serial.println("safe_open");
      
       // Выключаем свет для проверки
       digitalWrite(MansardLight, LOW); digitalWrite(LastTowerTopLight, LOW);
       digitalWrite(Fireworks, LOW); digitalWrite(BankRoomLight, LOW);
       digitalWrite(TorchLight, LOW); digitalWrite(HallLight, LOW);
       digitalWrite(UfHallLight, LOW); digitalWrite(LibraryLight, LOW);
       for (int s = 0; s < STRIPS; s++) { strips[s]->clear(); strips[s]->show(); }
      
       boyStateInitialized = false;
       readyListenTimer = millis();
       level = 26; 
       return;
    }

    // Обработка открытия дверей
    if (buff == "open_mansard_door") OpenDoor(MansardDoor);
    else if (buff == "open_crime_door") OpenDoor(CrimeDoor);
    else if (buff == "open_bank_door") OpenDoor(BankDoor);
    else if (buff == "open_potion_door") OpenDoor(PotionsRoomDoor);
    else if (buff == "open_owl_door") {
       mySerial.println("open_door"); 
       delay(50);
       mySerial.println("open_door"); 
    }
    else if (buff == "open_dog_door") Serial3.println("open_door");
    else if (buff == "open_low_tower_door") OpenDoor(HightTowerDoor);
    else if (buff == "open_high_tower_door") OpenDoor(HightTowerDoor2);
    else if (buff == "open_library_door") OpenDoor(LibraryDoor);
    else if (buff == "open_workshop_door") Serial1.println("open_door");
    else if (buff == "open_safe_door") OpenDoor(BankStashDoor);
    else if (buff == "open_memory_door") OpenDoor(MemoryRoomDoor);
    else if (buff == "open_basket_door") Serial2.println("open_door");
    else if (buff == "open_mine_door") Serial2.println("open_mine_door");
  }
}


// --- ДОБАВЛЕНО: Функция для прослушивания ответов от башен ---
// ПРИЧИНА: Необходимо было создать выделенное состояние, в котором главная плата
// активно слушает все Serial порты от башен и пересылает их сообщения на сервер.
// Это решает проблему, когда сообщения от башен терялись, так как плата
// не слушала их в состоянии ожидания (PowerOn/RestOn).
void ListenForReady() {
  // Обработка экстренного рестарта во время проверки
  while (Serial.available()) {
    String buff = Serial.readStringUntil('\n');
    buff.trim();
    if (buff.indexOf("restart") != -1) {
       level = 0; // Сброс в PowerOn
       return;
    }
  }

  // 1. Проверяем, не истекло ли время прослушивания
  if (millis() - readyListenTimer > READY_LISTEN_DURATION) {
    Serial.println("ready_check_finished");
    level = 0;
    return;
  }

  // 2. Пока время не вышло, слушаем все порты и пересылаем сообщения
  // (Код пересылки остается без изменений, он был верен)
  if (Serial1.available()) {
    String buff = Serial1.readStringUntil('\n'); buff.trim();
    if (buff.length() > 0) Serial.println(buff);
  }
  if (Serial2.available()) {
    String buff = Serial2.readStringUntil('\n'); buff.trim();
    if (buff.length() > 0) Serial.println(buff);
  }
  if (Serial3.available()) {
    String buff = Serial3.readStringUntil('\n'); buff.trim();
    if (buff.length() > 0) Serial.println(buff);
  }
  if (mySerial.available()) {
    String buff = mySerial.readStringUntil('\n'); buff.trim();
    if (buff.length() > 0) Serial.println(buff);
  }
}

// Функция для проверки геркона "мальчика" в режиме ожидания (PowerOn/RestOn) ---
void handleIdleBoySensor() {
  // Проверяем не чаще, чем раз в 100 мс для стабильности
  if (millis() - boyCheckTimer >= 100) { 
    boyCheckTimer = millis();
    
    // Пин 7 на board1 - это геркон "мальчика"
    bool currentBoyState = digitalReadExpander(7, board1);

    // Инициализация при первом запуске
    if (!boyStateInitialized) {
      lastBoyState = currentBoyState;
      boyStateInitialized = true;
      return; // Выходим, чтобы не отправлять сообщение при первом чтении
    }

    // Если состояние изменилось
    if (currentBoyState != lastBoyState) {
      if (currentBoyState) {
        Serial.println("boy_out"); // Мальчик ВЫНУТ (геркон разомкнут)
      } else {
        Serial.println("boy_in");  // Мальчик ВСТАВЛЕН (геркон замкнут)
      }
      lastBoyState = currentBoyState; // Сохраняем новое состояние
    }
  }
}

void Restart() {
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
      
      // snitchFlag == 0 (FALSE) -> Игрок на месте (Игра активна)
      // snitchFlag == 1 (TRUE)  -> Игрок ушел (Игра на паузе)

      if (snitchFlag) { 
        // Игрок ушел с платформы (snitchFlag == 1).
        // Атака бота отменяется.
        enemyFlag = 0;
        enemyTimer = millis();
        additionalTimer = millis();
      } else { 
        // Игрок на платформе (snitchFlag == 0), но не поймал красный мяч.
        // Засчитываем гол боту!
        Serial.println("goal_1_bot"); // звук
        Serial2.println("start_basket_robot"); // Отправляем гол        
        LooseSnitch(); // Показываем красную вспышку

        // СБРАСЫВАЕМ ФЛАГИ (это было пропущено в оригинале)
        enemyFlag = 0;
        enemyTimer = millis(); // Сбрасываем таймер *следующего* красного мяча
        additionalTimer = millis(); // Сбрасываем таймер *этой* атаки
      }
    }

    if (millis() - cometTimer1 >= 1000) {
      interval = interval + random(-4, 4);
      if (interval < 2) {
        interval = 1;
      }
      if (enemyFlag) {
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
  for(int i = 0; i<=152;i++){
    if(i != 65){
      strip2.setPixelColor(i, strip2.Color(0, 0, 0));
    }
    strip1.setPixelColor(i, strip1.Color(0, 0, 0));
  }
  //strip1.clear();
  //strip2.clear();

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
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch1");
        } else {
          Serial.println("catch1");
        }

        Serial2.println("start_basket");
        CatchSnitch();
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
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch2");
        } else {
          Serial.println("catch2");
        }
        Serial2.println("start_basket");
        CatchSnitch();
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
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch3");
        } else {
          Serial.println("catch3");
        }
        Serial2.println("start_basket");
        CatchSnitch();
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
        snitchFlag = 1;
        isTrainBasket = 1;
        if (enemyFlag) {
          Serial.println("enemy_catch4");
        } else {
          Serial.println("catch4");
        }
        Serial2.println("start_basket");
        CatchSnitch();
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
  delay(1); // Эта задержка в 1 мс важна для стабилизации
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

void handleRainbow() {
  if (!isRainbowActive) return;
  if (millis() - rainbowTimer >= 20) {
    rainbowTimer = millis();
    for (int i = 0; i < CauldronStrip.numPixels(); i++) {
      int pixelHue = rainbowHue + (i * 65536L / CauldronStrip.numPixels());
      CauldronStrip.setPixelColor(i, CauldronStrip.gamma32(CauldronStrip.ColorHSV(pixelHue)));
      CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.gamma32(CauldronStrip.ColorHSV(pixelHue)));
    }
    CauldronStrip.show();
    CauldronRoomStrip.show();
    
    rainbowHue += 556;
    // Условие выхода (примерно 2 полных цикла)
    if (rainbowHue >= 2 * 65536) {
       isRainbowActive = false;
       
       // Финальный цвет комнаты (Синий)
       for (int i = 0; i <= 12; i++) {
          CauldronRoomStrip.setPixelColor(i, CauldronRoomStrip.Color(0, 0, 250));
       }
       
       // --- ДОБАВЛЕНО: Финальный цвет котла (Фиолетовый) ---
       // (128, 0, 128) - это классический фиолетовый (Purple)
       CauldronStrip.setPixelColor(0, CauldronStrip.Color(128, 0, 128));
       
       CauldronRoomStrip.show();
       CauldronStrip.show(); // Применяем цвет котла
    }
  }
}

void LooseSnitch() {
  // анимация теперь длится ровно 5 секунд.
  
  // 1. Включаем красный цвет
  for (int i = 0; i <= 160; i++) {
    strip2.setPixelColor(i, strip2.Color(255, 0, 0));
    strip1.setPixelColor(i, strip1.Color(255, 0, 0));
  }
  strip1.show();
  strip2.show();
  
  // 2. Эффект растворения (Fade Out) - 5 секунд
  // 5000 мс / 50 мс задержки = 100 шагов
  for (int step = 0; step < 100; step++) {
    if (Serial.available()) {
        String b = Serial.readStringUntil('\n');
        // Если пришел рестарт или важная команда - выходим
        if (b.indexOf("restart") != -1) { 
           OpenAll(); RestOn(); level=25; return; 
        }
     }
     // В каждом шаге гасим 1% случайных диодов или уменьшаем яркость
     // Для эффекта "отделения" лучше гасить рандомно
     for (int k=0; k < 4; k++) { // Гасим по 4 диода за такт на каждой ленте
        int r = random(0, 161);
        strip1.setPixelColor(r, 0);
        strip2.setPixelColor(r, 0);
     }
     
     // Дополнительно плавно снижаем общую яркость красного
     // (опционально, если хотите просто гашение - уберите это)
     
     strip1.show();
     strip2.show();
     delay(50); 
  }
  
  // 3. Финал - все черное
  strip1.clear();
  strip2.clear();
  strip1.show();
  strip2.show();
}
// Эффект зеленой волны со вспышками]
void GreenWaveEffect() {
  static uint16_t wavePhase = 0;
  static unsigned long lastWaveUpdate = 0;

  // Обновляем каждые 30 мс
  if (millis() - lastWaveUpdate < 30) return;
  lastWaveUpdate = millis();

  // Фон: Зеленый 30%
  for (int i = 0; i <= 160; i++) {
     strip1.setPixelColor(i, strip1.Color(0, 75, 0));
     strip2.setPixelColor(i, strip2.Color(0, 75, 0));
  }

  int waveWidth = 10; 
  int numLeds = 160; 
  
  for (int w = 0; w < 2; w++) {
    int center = (wavePhase + (w * (numLeds / 2))) % numLeds;
    for (int k = -waveWidth/2; k <= waveWidth/2; k++) {
       int pos = center + k;
       if (pos < 0) pos += numLeds;
       if (pos >= numLeds) pos -= numLeds;
       
       strip1.setPixelColor(pos, strip1.Color(0, 255, 0));
       strip2.setPixelColor(pos, strip2.Color(0, 255, 0));
       
       if (random(0, 10) > 7) {
          strip1.setPixelColor(pos, strip1.Color(255, 255, 255));
          strip2.setPixelColor(pos, strip2.Color(255, 255, 255));
       }
    }
  }
  
  strip1.show();
  strip2.show();
  
  wavePhase++;
  if (wavePhase >= numLeds) wavePhase = 0;
}

void MonitorSerialBuffer() {
  static unsigned long lastWarningTime = 0;
  // Размер буфера Arduino Mega = 64 байта.
  int bytesWaiting = Serial.available();

  // Если буфер почти полон (более 50 байт из 64)
  if (bytesWaiting > 50) {
    
    // 1. Читаем всё содержимое, чтобы освободить место
    String trash = "";
    while (Serial.available() > 0) {
      char c = Serial.read();
      trash += c;
      // Ограничиваем чтение, чтобы не зависнуть в цикле, если данные льются потоком
      if (trash.length() > 60) break; 
    }

    // 2. Сообщаем в лог, что именно мы удалили (для отладки)
    // Шлем предупреждение не чаще раза в 500 мс
    if (millis() - lastWarningTime > 500) {
      lastWarningTime = millis();
      Serial.print("log:warn:BUFFER CLEARED! Removed: ");
      Serial.println(trash); 
    }
  }
}
