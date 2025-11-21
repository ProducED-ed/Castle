#include <Arduino.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "GyverButton.h"

// --- NASTROYKA PINOV ---
const int PADLOCK_REED_PIN = A5;  // Pin gerkona "navesnoy zamok" (D12)
const int CAPSULE_REED_PIN = 11;  // Pin gerkona "kapsula" na dveri (D11)
const int DOOR_LOCK_PIN = 10;     // Pin upravleniya rele zamka "dver" (D10)
const int LIGHTING_LED_PIN = 9;   // Pin svetodioda "osveshchenie" (D9) // PWM pin dlya plavnogo vklyucheniya
const int KNIGHT_REED_PIN = 13;   // Pin gerkona "rycar" (D13)
const int CAGE_REED_PIN = A0;     // Pin gerkona "kletka" (A0)
const int VIBRO_MOTOR_PIN = 6;    // Pin upravleniya rele vibromotora (D6)
const int END_REED_PIN = A1;      // Pin gerkona "end" (A1)
const int CAGE_LOCK_PIN = 5;      // Pin upravleniya rele zamka "kletka" (D5)
const int LED_STRIP_PIN = 4;      // Pin dlya 12V svetodiodnoy lenty cherez rele (D4)

// Novye piny
const int CRYSTAL_REED_PIN = A2;  // Pin gerkona "kristall" (A2)
const int CRYSTAL_LIGHT_PIN = 3;  // Pin osveshcheniya "kristalla" (D3) // PWM pin dlya plavnogo vklyucheniya/vyklyucheniya

// Dobavlennye piny
const int ROSE_REED_PIN = A3;      // Pin gerkona "roza" (A3)
const int FLAG_IR_SENSOR_PIN = 7;  // Pin IK datchika "flag" (A4)
const int ROOF_LIGHTING_PIN = 2;   // Pin osveshcheniya kryshi (D2)

// --- NASTROYKA APPARATNOGO UART DLYA AUDIO-MODULYA (D0/RX, D1/TX) ---
// Piny 0 (RX) i 1 (TX) ispolzuyutsya dlya svyazi s audio-modulem/kompyuterom.

// --- NASTROYKA UART SOOBSHCHENIY DLYA AUDIO-MODULYA ---
const char MSG_CHAIN_FALL[] PROGMEM = "door_dog";
const char MSG_DOBBY[] PROGMEM = "help";  // Izmeneno na odin file dobby.mp3
const char MSG_GROWL[] PROGMEM = "dog_growl";
const char MSG_SLEEP[] PROGMEM = "dog_sleep";
const char MSG_STORY_20A[] PROGMEM = "story_20_a";
const char MSG_ST0RY_20B[] PROGMEM = "story_20_b";
const char MSG_STORY_20C[] PROGMEM = "story_20_c";
const char MSG_LOCK_CLICK[] PROGMEM = "dog_lock";

// Novye UART soobshcheniya
const char MSG_CRYSTAL[] PROGMEM = "crystal";  // Izmeneno s "part_crystal"
const char MSG_START[] PROGMEM = "start";
const char MSG_MAP_DOG[] PROGMEM = "key";
const char MSG_MAP_FISH[] PROGMEM = "fish";
const char MSG_MAP_OWL[] PROGMEM = "owl";
const char MSG_MAP_TRAIN[] PROGMEM = "train";
const char MSG_MAP_OUT[] PROGMEM = "out";
const char MSG_RESTART[] PROGMEM = "restart";
const char MSG_SKIP_DOG[] PROGMEM = "skip_dog";
const char MSG_SKIP_PADLOCK[] PROGMEM = "skip_padlock";
const char MSG_STOP_CRYSTAL[] PROGMEM = "stop_crystal";
const char MSG_READY[] PROGMEM = "ready";      // Novoe soobshchenie ready
const char MSG_DOG_NRD[] PROGMEM = "dog_nrd";  // Novoe soobshchenie dog_nrd

// Dobavlennye UART soobshcheniya
const char MSG_ROSE[] PROGMEM = "galet_on";
const char MSG_NROSE[] PROGMEM = "galet_off";
const char MSG_FLAG2[] PROGMEM = "flag3_off";
const char MSG_NFLAG2[] PROGMEM = "flag3_on";
const char MSG_OWL_DWARF[] PROGMEM = "light_on";
const char MSG_NOWL_DWARF[] PROGMEM = "light_off";

// Novye soobshcheniya dlya upravleniya kristalom
const char MSG_GOLD[] PROGMEM = "item_find";
const char MSG_POTION[] PROGMEM = "item_find";
const char MSG_SKIN[] PROGMEM = "item_find";
const char MSG_METAL[] PROGMEM = "item_find";
const char MSG_PARTH_OFF[] PROGMEM = "item_end";
const char MSG_ITEM_FIND[] PROGMEM = "item_find";  // Soobshchenie pri starte pulsacii

// Novye soobshcheniya dlya upravleniya osveshcheniem
const char MSG_DAY_ON[] PROGMEM = "day_on";
const char MSG_DAY_OFF[] PROGMEM = "day_off";
const char MSG_LIGHT_ON[] PROGMEM = "light_on";
const char MSG_LIGHT_OFF[] PROGMEM = "light_off";

const char *const STORY_SOUNDS[] PROGMEM = {
  MSG_STORY_20A,
  MSG_ST0RY_20B,
  MSG_STORY_20C
};
const int NUM_STORY_SOUNDS = 3;

// --- SOSTOYANIYA KVESTA ---
enum QuestState {
  STATE_WAITING_FOR_START,
  STATE_IN_PROGRESS,
  STATE_GAME_FINISHED,
  STATE_RESTARTING  // Promezhutochnoe sostoyanie dlya sbrosa
};
QuestState currentQuestState = STATE_WAITING_FOR_START;

// --- SISTEMNYE PEREMENNYE (FLAGI SOSTOYANIYA) ---
bool isPadlockActivated = false;
bool isDoorOpened = false;
bool isDoorClosedAfterOpen = false;
unsigned long doorCloseTime = 0;
const long DOOR_REOPEN_DELAY = 5000;
unsigned long doorLockActiveTime = 0;
bool doorLockEngaged = false;  // Flag, chto zamok dveri otkryt
unsigned long timer1;
unsigned long timer2;

// Dlya gerkona "rycar"
bool lastKnightReedState = HIGH;

// Dlya gerkona "kletka" i kolesa
bool lastCageReedState = HIGH;
unsigned long lastCageReedActivationTime = 0;
bool _restartFlag = 0;
bool _restartGalet = 0;
bool isCageReedInitialized = false;
bool isFastSpinning = false;               // Flag, ukazyvayushchiy na aktivnoe bystroe vrashchenie
unsigned long lastFastSpinEffectTime = 0;  // Vremya poslednego obnaruzheniya bystrogo vrashcheniya
bool wasFastSpinningActive = false;        // Flag, ukazyvayushchiy, bylo li bystroe vrashchenie hot raz v tekushchey faze igry
int initialCageReedState = -1;
unsigned long lastCageReedUnactivatedTime = 0;  // Vremya, kogda gerkon "kletka" stal razomknut (HIGH)
unsigned long lastStorySoundPlayTime = 0;       // Dlya periodicheskih zvukov pri neaktivnosti
bool periodicStorySoundActive = false;
unsigned long lastFastSpinStorySoundTime = 0;  // Peremennaya dlya taymera periodicheskogo zvuka vo vremya bystrogo vrashcheniya
bool periodicFastSpinSoundActive = false;      // Flag dlya aktivacii periodicheskih zvukov vo vremya bystrogo vrashcheniya
unsigned long lastBlinkTime_LEDStrip = 0;
const long BLINK_INTERVAL_FAST = 100;
#define MAX_UART_MESSAGE_LENGTH 32
char receivedUartMessageBuffer[MAX_UART_MESSAGE_LENGTH];
byte receivedUartMessageIndex = 0;
unsigned long cageReedLowStartTime = 0;  // Vremya, kogda gerkon "kletka" stal zamknut (LOW)
bool cageReedWasLow = false;             // Flag, chto gerkon "kletka" byl zamknut
unsigned long lastVibroPulseTime = 0;
bool vibroPulseState = false;  // Upravlyaet odnovremenno vibro i osveshcheniem (dlya pulsacii)
const int VIBRO_ON_DURATIONS[] = { 100, 150, 200, 620 };
const int VIBRO_OFF_DURATIONS[] = { 300, 600, 450, 380 };
const int NUM_VIBRO_PATTERNS = 4;
int currentVibroOnDuration = 0;
int currentVibroOffDuration = 0;

// Dlya neblokiruyushchego otkrytiya lokera "kletka" pri starte kvesta
unsigned long cageLockInitialOpenTime = 0;
bool cageLockInitialOpenActive = false;
const long CAGE_LOCK_INITIAL_OPEN_DURATION = 500;  // 0.5 sekundy

// Dlya neblokiruyushchego otkrytiya lokera "kletka" pri "end"
unsigned long cageLockEndOpenTime = 0;
bool cageLockEndOpenActive = false;
const long CAGE_LOCK_END_OPEN_DURATION = 500;  // 0.5 sekundy
bool endReedTriggeredWithFastSpin = false;     // Novyy flag dlya otslezhivaniya srabatyvaniya "end" pri aktivnom bystrom vrashchenii

// Dlya neblokiruyushchego otkrytiya lokerov pri restart
unsigned long doorLockRestartTime = 0;
bool doorLockRestartActive = false;
unsigned long cageLockRestartTime = 0;
bool cageLockRestartActive = false;
const long RESTART_LOCK_DURATION = 500;  // 0.5 sekundy

// Dlya gerkona "kristall"
bool lastCrystalReedState = HIGH;
bool isCrystalPulsating = false;  // NOVYY FLAG: Upravlyaet pulsatsiey kristalla
bool isCrystalLockedOff = false;  // NOVYY FLAG: Blokiruet kristall do perezapuska

// Dlya gerkona "end"
bool lastEndReedState = HIGH;

// Dlya plavnogo vklyucheniya svetodioda osveshcheniya
const int LIGHTING_FADE_INTERVAL = 10;  // Interval mezhdu shagami dimmirovaniya v ms
const int LIGHTING_FADE_STEP = 5;       // shag izmeneniya yarkosti (0-255)
int currentLightingBrightness = 0;
unsigned long lastLightingFadeTime = 0;
bool lightingFadingIn = false;

// Dlya plavnogo vklyucheniya/vyklyucheniya osveshcheniya CRYSTAL_LIGHT_PIN
const int CRYSTAL_FADE_INTERVAL = 18;  // Interval mezhdu shagami dimmirovaniya v ms (5ms dlya 255 shagov)
const int CRYSTAL_FADE_STEP = 5;       // shag izmeneniya yarkosti (0-255)
int currentCrystalBrightness = 0;
unsigned long lastCrystalFadeTime = 0;
bool crystalFadingIn = false;
bool crystalFadingOut = false;

// Dlya morganiya osveshcheniya pri dog_nrd
unsigned long lastBlinkLightingTime = 0;
const long BLINK_LIGHTING_INTERVAL = 250;  // Interval morganiya v ms
bool lightingBlinkActive = false;
bool currentLightingBlinkState = false;  // Flag dlya otslezhivaniya tekushchego sostoyaniya morganiya (VKL/VYKL)

// Novye flagi dlya pinov A3 i A4
bool lastRoseReedState = HIGH;
bool lastFlagSensorState = HIGH;

GButton galetButton(ROSE_REED_PIN);
GButton flagButton(FLAG_IR_SENSOR_PIN);

bool isActive;
bool hasSentReadyLog = false;

void sendLog(String message) {
  Serial.println("log:dog:" + message);
}

void putWheelToSleep();
void resetQuestState();
void activateEndStage();
void smoothTurnOnLighting();
void smoothTurnOnCrystal();
void smoothTurnOffCrystal();
void setup();
void loop();

void putWheelToSleep() {
  // Proveryaem, deystvitelno li chto-to aktivno, prezhde chem "usyplyat"
  if (isFastSpinning || wasFastSpinningActive || digitalRead(VIBRO_MOTOR_PIN) == HIGH || digitalRead(LED_STRIP_PIN) == HIGH || periodicStorySoundActive || periodicFastSpinSoundActive) {
    Serial.println((__FlashStringHelper *)MSG_SLEEP);
    sendLog("Putting wheel to sleep.");
    isFastSpinning = false;
    wasFastSpinningActive = false;
    periodicFastSpinSoundActive = false;
    digitalWrite(VIBRO_MOTOR_PIN, LOW);
    analogWrite(LIGHTING_LED_PIN, 255);  // Vklyuchaem osveshchenie postoyanno
    currentLightingBrightness = 255;
    lightingFadingIn = false;  // Otklyuchaem plavnoe vklyuchenie
    vibroPulseState = false;   // Sbros sostoyaniya pulsacii dlya vibro i sveta
    digitalWrite(LED_STRIP_PIN, LOW);
    periodicStorySoundActive = false;  // Otklyuchaem periodicheskie zvuki neaktivnosti
    lastCageReedUnactivatedTime = 0;   // Sbros taymerov dlya spyachki
    cageReedLowStartTime = 0;
    cageReedWasLow = false;
  } else {
  }
}

void resetQuestState() {
  sendLog("Resetting tower to initial state.");
  isPadlockActivated = false;
  isDoorOpened = false;
  isDoorClosedAfterOpen = false;
  doorCloseTime = 0;
  doorLockActiveTime = 0;
  doorLockEngaged = false;
  lastKnightReedState = HIGH;
  lastCageReedState = HIGH;
  lastCageReedActivationTime = 0;
  isCageReedInitialized = false;

  // Pri sbrose kvesta, koleso dolzhno byt "spyashchim"
  isFastSpinning = false;
  lastFastSpinEffectTime = 0;
  wasFastSpinningActive = false;
  initialCageReedState = -1;
  lastCageReedUnactivatedTime = 0;
  lastStorySoundPlayTime = 0;
  periodicStorySoundActive = false;
  lastFastSpinStorySoundTime = 0;
  periodicFastSpinSoundActive = false;
  lastBlinkTime_LEDStrip = 0;
  cageReedLowStartTime = 0;
  cageReedWasLow = false;
  lastVibroPulseTime = 0;
  vibroPulseState = false;
  cageLockInitialOpenTime = 0;
  cageLockInitialOpenActive = false;
  cageLockEndOpenTime = 0;
  cageLockEndOpenActive = false;
  endReedTriggeredWithFastSpin = false;
  doorLockRestartActive = false;
  cageLockRestartActive = false;

  lastCrystalReedState = HIGH;
  isCrystalPulsating = false;  // Sbros flaga pulsacii
  isCrystalLockedOff = false;  // Sbros flaga blokirovki

  // Obnovlyaem lastEndReedState do tekushchego fakticheskogo sostoyaniya END_REED_PIN pri sbrose
  lastEndReedState = digitalRead(END_REED_PIN);

  // Otklyuchaem vse vyhody, vklyuchaya LIGHTING_LED_PIN i CRYSTAL_LIGHT_PIN
  digitalWrite(DOOR_LOCK_PIN, LOW);
  digitalWrite(VIBRO_MOTOR_PIN, LOW);
  digitalWrite(CAGE_LOCK_PIN, LOW);
  digitalWrite(LED_STRIP_PIN, LOW);
  analogWrite(LIGHTING_LED_PIN, 0);
  currentLightingBrightness = 0;
  lightingFadingIn = false;
  lightingBlinkActive = false;
  currentLightingBlinkState = false;
  analogWrite(CRYSTAL_LIGHT_PIN, 0);
  currentCrystalBrightness = 0;
  crystalFadingIn = false;
  crystalFadingOut = false;

  // Otklyuchaem osveshchenie kryshi pri sbrose
  digitalWrite(ROOF_LIGHTING_PIN, LOW);
}

void activateEndStage() {
  if (currentQuestState == STATE_IN_PROGRESS) {
    // Uslovie dlya otkrytiya lokera 'kletka': Bystroe vrashchenie aktivno I usloviya dlya spyashchego sostoyaniya NE vypolneny (dver otkryta).
    // Eto uslovie teper ispolzuet isFastSpinning, tak kak ono otrazhaet tekushchuyu aktivnost
    if (isFastSpinning && digitalRead(CAPSULE_REED_PIN) == HIGH) {
      // Deystviya pri BYSTROM vrashchenii (prolete mimo zamka)
      // Pobedy net, soobshcheniy ne otpravlyaem. Prosto otkryvaem zamok kletki.
      digitalWrite(CAGE_LOCK_PIN, HIGH);
      cageLockEndOpenTime = millis();
      cageLockEndOpenActive = true;
      endReedTriggeredWithFastSpin = true;
    } else {
      // Deystviya pri MEDLENNOM vrashchenii (USPESHNOE zavershenie)
      // Esli bystrogo vrashcheniya ne bylo ili usloviya dlya spyashchego rezhima vypolneny (dver zakryta)

      // PEREMESHCHENO SYUDA: Otpravlyaem soobshchenie o pobede
      // ИЗМЕНЕНО: Добавлено логирование перед командой
      sendLog("Dog game finished. Sending dog_lock. dog_complete");
      delay(10);
      // КОНЕЦ
      Serial.println((__FlashStringHelper *)MSG_LOCK_CLICK);
      sendLog("Game finished successfully (lock_click).");

      // PEREMESHCHENO SYUDA: Vklyuchaem kristall
      smoothTurnOnCrystal();

      // Zavershaem igru nemedlenno
      currentQuestState = STATE_GAME_FINISHED;
      putWheelToSleep();  // Prinuditelno perevodim koleso v spyashchee sostoyanie, vyklyuchaya vse effekty
      endReedTriggeredWithFastSpin = false;
    }
  } else {
    switch (currentQuestState) {
      case STATE_WAITING_FOR_START:
        Serial.println(F("STATE_WAITING_FOR_START"));
        break;
      case STATE_GAME_FINISHED:
        Serial.println(F("STATE_GAME_FINISHED"));
        break;
      case STATE_RESTARTING:
        Serial.println(F("STATE_RESTARTING"));
        break;
      default:
        Serial.println(F("UNKNOWN"));
        break;
    }
  }
}

// Funkciya plavnogo vklyucheniya svetodioda osveshcheniya
void smoothTurnOnLighting() {
  lightingFadingIn = true;
  lastLightingFadeTime = millis();
}

// Funkciya dlya zapuska plavnogo vklyucheniya sveta kristalla
void smoothTurnOnCrystal() {
  crystalFadingOut = false;
  crystalFadingIn = true;
  lastCrystalFadeTime = millis();
}

// Funkciya dlya zapuska plavnogo vyklyucheniya sveta kristalla
void smoothTurnOffCrystal() {
  crystalFadingIn = false;
  crystalFadingOut = true;
  lastCrystalFadeTime = millis();
}

void setup() {
  Serial.begin(9600);

  pinMode(PADLOCK_REED_PIN, INPUT_PULLUP);
  pinMode(CAPSULE_REED_PIN, INPUT_PULLUP);
  pinMode(KNIGHT_REED_PIN, INPUT_PULLUP);
  pinMode(CAGE_REED_PIN, INPUT_PULLUP);
  pinMode(END_REED_PIN, INPUT_PULLUP);
  pinMode(CRYSTAL_REED_PIN, INPUT_PULLUP);

  // Nastroyka novyh pinov kak INPUT_PULLUP
  // pinMode(ROSE_REED_PIN, INPUT_PULLUP);
  // pinMode(FLAG_IR_SENSOR_PIN, INPUT_PULLUP);

  galetButton.setDebounce(50);       // nastroyka antidrebezga (po umolchaniyu 80 ms)
  galetButton.setTimeout(300);       // nastroyka taymauta na uderzhanie (po umolchaniyu 500 ms)
  galetButton.setClickTimeout(600);  // nastroyka taymauta mezhdu klikami (po umolchaniyu 300 ms)
  galetButton.setType(HIGH_PULL);
  galetButton.setDirection(NORM_OPEN);

  flagButton.setDebounce(50);       // nastroyka antidrebezga (po umolchaniyu 80 ms)
  flagButton.setTimeout(300);       // nastroyka taymauta na uderzhanie (po umolchaniyu 500 ms)
  flagButton.setClickTimeout(600);  // nastroyka taymauta mezhdu klikami (po umolchaniyu 300 ms)
  flagButton.setType(LOW_PULL);
  flagButton.setDirection(NORM_OPEN);

  pinMode(DOOR_LOCK_PIN, OUTPUT);
  pinMode(LIGHTING_LED_PIN, OUTPUT);
  pinMode(VIBRO_MOTOR_PIN, OUTPUT);
  pinMode(CAGE_LOCK_PIN, OUTPUT);
  pinMode(LED_STRIP_PIN, OUTPUT);
  pinMode(CRYSTAL_LIGHT_PIN, OUTPUT);

  // Nastroyka novogo pina kak OUTPUT
  pinMode(ROOF_LIGHTING_PIN, OUTPUT);

  // Iznachalno vse vyhody vyklyucheny.
  digitalWrite(DOOR_LOCK_PIN, LOW);
  analogWrite(LIGHTING_LED_PIN, 0);
  digitalWrite(VIBRO_MOTOR_PIN, LOW);
  digitalWrite(CAGE_LOCK_PIN, LOW);
  digitalWrite(LED_STRIP_PIN, LOW);
  analogWrite(CRYSTAL_LIGHT_PIN, 0);
  digitalWrite(ROOF_LIGHTING_PIN, LOW);  // Ubedimsya, chto osveshchenie kryshi vyklyucheno

  memset(receivedUartMessageBuffer, 0, MAX_UART_MESSAGE_LENGTH);
  receivedUartMessageIndex = 0;

  randomSeed(analogRead(A0));

  // Inicializaciya lastEndReedState realnym tekushchim sostoyaniem
  lastEndReedState = digitalRead(END_REED_PIN);

  // Inicializaciya sostoyaniy novyh datchikov
  lastRoseReedState = digitalRead(ROSE_REED_PIN);
  lastFlagSensorState = digitalRead(FLAG_IR_SENSOR_PIN);
}


void CheckState() { // ИСПРАВЛЕНО: опечатка в имени функции
  // Проверяем состояние геркона "роза" (pin A3)
  if (!digitalRead(A3)) { // Если геркон активен (LOW)
    if (!_restartGalet) {    // И если мы еще не отправляли сообщение
      Serial.println("galet_on");
      sendLog("Rose sensor activated (galet_on).");
      _restartGalet = 1;     // Устанавливаем флаг, что сообщение отправлено
    }
  } else {                   // Если геркон неактивен (HIGH)
    if (_restartGalet) {     // И если мы ранее отправляли сообщение "on"
      Serial.println("galet_off");
      sendLog("Rose sensor deactivated (galet_off).");
      _restartGalet = 0;    // Сбрасываем флаг
    }
  }

  // Проверяем состояние ИК-датчика "флаг" (pin 7)
  // Для flagButton используется LOW_PULL, поэтому активное состояние - HIGH
  if (digitalRead(7)) { // Если флаг на месте (HIGH)
    if (!_restartFlag) {    // И если мы еще не отправляли сообщение
      Serial.println("flag3_on");
      sendLog("Flag sensor activated (flag3_on).");
      _restartFlag = 1;     // Устанавливаем флаг
    }
  } else {                  // Если флага нет (LOW)
    if (_restartFlag) {     // И если мы ранее отправляли сообщение "on"
      Serial.println("flag3_off");
      sendLog("Flag sensor deactivated (flag3_off).");
      _restartFlag = 0;   // Сбрасываем флаг
    }
  }
}
void loop() {
  static QuestState previousState = STATE_RESTARTING;
  if (currentQuestState != previousState) {
    String stateName = "UNKNOWN";
    if (currentQuestState == STATE_WAITING_FOR_START) stateName = "WAITING_FOR_START";
    else if (currentQuestState == STATE_IN_PROGRESS) stateName = "IN_PROGRESS";
    else if (currentQuestState == STATE_GAME_FINISHED) stateName = "GAME_FINISHED";
    else if (currentQuestState == STATE_RESTARTING) stateName = "RESTARTING";
    sendLog("State changed to " + stateName);
    previousState = currentQuestState;
  }

  static unsigned long last_start_ping = 0;
  unsigned long currentMillis = millis();

  if (!isActive && currentMillis - last_start_ping >= 500L) {
    last_start_ping = currentMillis;
    if (currentQuestState == STATE_WAITING_FOR_START || currentQuestState == STATE_RESTARTING) {
      resetQuestState();
      currentQuestState = STATE_IN_PROGRESS;
      if (digitalRead(CAPSULE_REED_PIN) == HIGH) {
        Serial.println((__FlashStringHelper *)MSG_DOG_NRD);
        sendLog("Door is not ready (dog_nrd).");
        lightingBlinkActive = true;
        currentLightingBlinkState = true;
        analogWrite(LIGHTING_LED_PIN, 255);
        lastBlinkLightingTime = currentMillis;
      } else {
        digitalWrite(CAGE_LOCK_PIN, HIGH);
        cageLockInitialOpenTime = currentMillis;
        cageLockInitialOpenActive = true;
        lightingBlinkActive = false;
        currentLightingBlinkState = false;
        analogWrite(LIGHTING_LED_PIN, 0);
        currentLightingBrightness = 0;
        lightingFadingIn = false;
      }
    }
  }

  // --- OBRABOTKA VHODYASHCHIH SOOBSHCHENIY PO UART (vsegda aktivna) ---
  while (Serial.available()) {
    char inChar = (char)Serial.read();

    if (inChar == '\r') {
      // Ignoriruem simvol vozvrata karetki
    } else if (inChar == '\n') {
      if (receivedUartMessageIndex > 0) {
        receivedUartMessageBuffer[receivedUartMessageIndex] = '\0';
        sendLog("Received command: " + String(receivedUartMessageBuffer));
        // --- NACHALO BLOKA OBRABOTKI KOMAND ---
        if (strcmp_P(receivedUartMessageBuffer, MSG_MAP_DOG) == 0) {
          isActive = 1;
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_MAP_OWL) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_MAP_FISH) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_MAP_TRAIN) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_MAP_OUT) == 0) {
          isActive = 0;
        } else if (strcmp_P(receivedUartMessageBuffer, PSTR("open_door")) == 0) {
          digitalWrite(10, HIGH);  // Включаем светодиод на пине 10
          delay(500);              // Ждем 500 мс
          digitalWrite(10, LOW);   // Выключаем светодиод
        } 
        else if (strcmp_P(receivedUartMessageBuffer, PSTR("check_state")) == 0) {
          _restartFlag = 0;
          _restartGalet = 0;
          CheckState();
        }
        else if (strcmp_P(receivedUartMessageBuffer, MSG_RESTART) == 0) {
          hasSentReadyLog = false;
          currentQuestState = STATE_RESTARTING;
          resetQuestState();
          digitalWrite(10, HIGH);  // Включаем светодиод на пине 10
          delay(500);              // Ждем 500 мс
          digitalWrite(10, LOW);
          _restartFlag = 0;
          _restartGalet = 0;
          CheckState();

          digitalWrite(DOOR_LOCK_PIN, HIGH);
          doorLockRestartTime = currentMillis;
          doorLockRestartActive = true;
          digitalWrite(CAGE_LOCK_PIN, HIGH);
          cageLockRestartTime = currentMillis;
          cageLockRestartActive = true;
          analogWrite(LIGHTING_LED_PIN, 255);
          currentLightingBrightness = 255;
          lightingFadingIn = false;
          lightingBlinkActive = false;
          currentLightingBlinkState = false;
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_SKIP_DOG) == 0) {
          if (currentQuestState == STATE_IN_PROGRESS) {
            wasFastSpinningActive = false;
            activateEndStage();
          }
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_SKIP_PADLOCK) == 0) {
          if (!isPadlockActivated) {
            isPadlockActivated = true;
            Serial.println((__FlashStringHelper *)MSG_CHAIN_FALL);
            digitalWrite(DOOR_LOCK_PIN, HIGH);
            doorLockActiveTime = currentMillis;
            doorLockEngaged = true;
            initialCageReedState = digitalRead(CAGE_REED_PIN);
            if (currentQuestState == STATE_WAITING_FOR_START) {
              currentQuestState = STATE_IN_PROGRESS;
            }
          }
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_STOP_CRYSTAL) == 0) {
          if (currentQuestState == STATE_GAME_FINISHED) {
            smoothTurnOffCrystal();
          }
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_READY) == 0) {
          if (!hasSentReadyLog) {
            sendLog("Checking initial sensor states.");
            hasSentReadyLog = true;
          }
          // ---------------------------------------------------------------------------------
          // ИЗМЕНЕНО: Добавлена принудительная проверка состояния при команде "ready".
          // ПРИЧИНА: Чтобы башня сообщала о уже активных датчиках (например,
          // оставленном флаге) сразу после перезапуска сервера, а не только при
          // изменении их состояния.
          // ---------------------------------------------------------------------------------
          resetQuestState();
          _restartFlag = 0;
          _restartGalet = 0;
          CheckState(); // <-- ДОБАВЛЕНО: Немедленная проверка состояния
          currentQuestState = STATE_WAITING_FOR_START;
          if (digitalRead(CAPSULE_REED_PIN) == HIGH) {
            Serial.println((__FlashStringHelper *)MSG_DOG_NRD);
            lightingBlinkActive = true;
            currentLightingBlinkState = true;
            analogWrite(LIGHTING_LED_PIN, 255);
            lastBlinkLightingTime = currentMillis;
          } else {
            lightingBlinkActive = false;
            currentLightingBlinkState = false;
            analogWrite(LIGHTING_LED_PIN, 255);
            currentLightingBrightness = 255;
          }
        }
        // Obrabotka novyh UART soobshcheniy dlya osveshcheniya kryshi
        else if (strcmp_P(receivedUartMessageBuffer, MSG_OWL_DWARF) == 0) {
          digitalWrite(ROOF_LIGHTING_PIN, HIGH);
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_NOWL_DWARF) == 0) {
          digitalWrite(ROOF_LIGHTING_PIN, LOW);
        }
        // NOVYY BLOK: Obrabotka komand dlya ostanovki pulsacii kristalla
        // которое он сам вызвал, отправляя "crystal".
        else if (strcmp_P(receivedUartMessageBuffer, MSG_GOLD) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_POTION) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_SKIN) == 0 || strcmp_P(receivedUartMessageBuffer, MSG_METAL) == 0) {
          if (currentQuestState == STATE_GAME_FINISHED) {
            isCrystalPulsating = false; // Остановка пульсации
            smoothTurnOnCrystal();      // Включение постоянного света
          }
        }
        // NOVYY BLOK: Obrabotka komandy dlya polnogo vyklyucheniya kristalla
        else if (strcmp_P(receivedUartMessageBuffer, MSG_PARTH_OFF) == 0) {
          if (currentQuestState == STATE_GAME_FINISHED) {
            isCrystalLockedOff = true;   // Blokiruem gerkon
            isCrystalPulsating = false;  // Ostanavlivaem pulsaciyu
            smoothTurnOffCrystal();      // I vyklyuchaem svet
          }
        }
        // NOVYY BLOK: Upravlenie osveshcheniem
        else if (strcmp_P(receivedUartMessageBuffer, MSG_DAY_ON) == 0) {
          if (isPadlockActivated) {  // Proveryaem, byl li zamok uzhe otkryt
            analogWrite(LIGHTING_LED_PIN, 255);
            currentLightingBrightness = 255;
            lightingFadingIn = false;
            lightingBlinkActive = false;
          }
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_DAY_OFF) == 0) {
          if (isPadlockActivated) {  // Proveryaem, byl li zamok uzhe otkryt
            analogWrite(LIGHTING_LED_PIN, 0);
            currentLightingBrightness = 0;
            lightingFadingIn = false;
            lightingBlinkActive = false;
          }
        }
        // NOVYY BLOK: Upravlenie osveshcheniem kryshi
        else if (strcmp_P(receivedUartMessageBuffer, MSG_LIGHT_ON) == 0) {
          digitalWrite(ROOF_LIGHTING_PIN, HIGH);
        } else if (strcmp_P(receivedUartMessageBuffer, MSG_LIGHT_OFF) == 0) {
          digitalWrite(ROOF_LIGHTING_PIN, LOW);
        }
        // On ispol'zuetsya dlya OSTANOVKI pul'sacii, kogda vybran DRUGOY predmet.
        else if (strcmp_P(receivedUartMessageBuffer, MSG_ITEM_FIND) == 0) {
          if (currentQuestState == STATE_GAME_FINISHED) {
            if (isCrystalPulsating) {
              isCrystalPulsating = false; // Ostanavlivaem pul'saciyu
              smoothTurnOffCrystal();     // i plavno gasim svet
            }
          }
        }

        // Sbros bufera dlya sleduyushchego soobshcheniya
        receivedUartMessageIndex = 0;
        memset(receivedUartMessageBuffer, 0, MAX_UART_MESSAGE_LENGTH);
      }
    } else {
      if (receivedUartMessageIndex < (MAX_UART_MESSAGE_LENGTH - 1)) {
        receivedUartMessageBuffer[receivedUartMessageIndex] = inChar;
        receivedUartMessageIndex++;
      }
    }
  }

  // --- GLAVNYY KONTROL SOSTOYANIYA KVESTA ---
  if (currentQuestState == STATE_WAITING_FOR_START || currentQuestState == STATE_RESTARTING) {
    if (doorLockRestartActive && (currentMillis - doorLockRestartTime >= RESTART_LOCK_DURATION)) {
      digitalWrite(DOOR_LOCK_PIN, LOW);
      doorLockRestartActive = false;
    }
    if (cageLockRestartActive && (currentMillis - cageLockRestartTime >= RESTART_LOCK_DURATION)) {
      digitalWrite(CAGE_LOCK_PIN, LOW);
      cageLockRestartActive = false;
    }
    if (lightingBlinkActive) {
      if (digitalRead(CAPSULE_REED_PIN) == LOW) {
        lightingBlinkActive = false;
        currentLightingBlinkState = false;
        analogWrite(LIGHTING_LED_PIN, 255);
        currentLightingBrightness = 255;
      } else {
        if (currentMillis - lastBlinkLightingTime >= BLINK_LIGHTING_INTERVAL) {
          lastBlinkLightingTime = currentMillis;
          currentLightingBlinkState = !currentLightingBlinkState;
          analogWrite(LIGHTING_LED_PIN, currentLightingBlinkState ? 255 : 0);
        }
      }
    }
    if (currentQuestState == STATE_IN_PROGRESS || currentQuestState == STATE_WAITING_FOR_START) {
      galetButton.tick();
      if (galetButton.isPress()) {
        Serial.println("galet_on");
      }
      if (galetButton.isRelease()) {
        Serial.println("galet_off");
      }
      flagButton.tick();
      if (flagButton.isPress()) {
        Serial.println("flag3_on");
      }
      if (flagButton.isRelease()) {
        Serial.println("flag3_off");
      }
    }
    return;
  }
  if (currentQuestState == STATE_IN_PROGRESS && lightingBlinkActive && digitalRead(CAPSULE_REED_PIN) == HIGH) {
    if (currentMillis - lastBlinkLightingTime >= BLINK_LIGHTING_INTERVAL) {
      lastBlinkLightingTime = currentMillis;
      currentLightingBlinkState = !currentLightingBlinkState;
      analogWrite(LIGHTING_LED_PIN, currentLightingBlinkState ? 255 : 0);
    }
  } else if (currentQuestState == STATE_IN_PROGRESS && lightingBlinkActive && digitalRead(CAPSULE_REED_PIN) == LOW) {
    lightingBlinkActive = false;
    currentLightingBlinkState = false;
    analogWrite(LIGHTING_LED_PIN, 255);
    currentLightingBrightness = 255;
  }
  if (currentQuestState == STATE_IN_PROGRESS || currentQuestState == STATE_GAME_FINISHED) {
    int currentRoseReedState = digitalRead(ROSE_REED_PIN);
    if (lastRoseReedState == HIGH && currentRoseReedState == LOW) {
      Serial.println((__FlashStringHelper *)MSG_ROSE);
      sendLog("Rose sensor activated (galet_on).");
    } else if (lastRoseReedState == LOW && currentRoseReedState == HIGH) {
      Serial.println((__FlashStringHelper *)MSG_NROSE);
      sendLog("Rose sensor deactivated (galet_off).");
    }
    lastRoseReedState = currentRoseReedState;
    int currentFlagSensorState = digitalRead(FLAG_IR_SENSOR_PIN);
    if (lastFlagSensorState == LOW && currentFlagSensorState == HIGH) {
      Serial.println((__FlashStringHelper *)MSG_NFLAG2);
      sendLog("Flag sensor activated (flag3_on).");
    } else if (lastFlagSensorState == HIGH && currentFlagSensorState == LOW) {
      Serial.println((__FlashStringHelper *)MSG_FLAG2);
      sendLog("Flag sensor deactivated (flag3_off).");
    }
    lastFlagSensorState = currentFlagSensorState;
  }
  if (cageLockInitialOpenActive && (currentMillis - cageLockInitialOpenTime >= CAGE_LOCK_INITIAL_OPEN_DURATION)) {
    digitalWrite(CAGE_LOCK_PIN, LOW);
    cageLockInitialOpenActive = false;
  }
  if (cageLockEndOpenActive && (currentMillis - cageLockEndOpenTime >= CAGE_LOCK_END_OPEN_DURATION)) {
    digitalWrite(CAGE_LOCK_PIN, LOW);
    cageLockEndOpenActive = false;
    if (endReedTriggeredWithFastSpin) {
    }
  }
  if (doorLockEngaged && (currentMillis - doorLockActiveTime >= 500)) {
    digitalWrite(DOOR_LOCK_PIN, LOW);
    doorLockEngaged = false;
  }
  if (!isPadlockActivated && digitalRead(PADLOCK_REED_PIN) == LOW && isActive) {
    isPadlockActivated = true;
    Serial.println((__FlashStringHelper *)MSG_CHAIN_FALL);
    sendLog("Padlock opened (chain_fall).");
    digitalWrite(DOOR_LOCK_PIN, HIGH);
    doorLockActiveTime = currentMillis;
    doorLockEngaged = true;
    initialCageReedState = digitalRead(CAGE_REED_PIN);
  }
  if (isPadlockActivated && !isDoorOpened && digitalRead(CAPSULE_REED_PIN) == HIGH) {
    isDoorOpened = true;
    isDoorClosedAfterOpen = false;
    smoothTurnOnLighting();
    if (!endReedTriggeredWithFastSpin) {
      putWheelToSleep();
    } else {
      periodicStorySoundActive = false;
      lastCageReedUnactivatedTime = 0;
    }
  }
  if (lightingFadingIn && currentLightingBrightness < 255) {
    if (currentMillis - lastLightingFadeTime >= LIGHTING_FADE_INTERVAL) {
      currentLightingBrightness += LIGHTING_FADE_STEP;
      if (currentLightingBrightness > 255)
        currentLightingBrightness = 255;
      analogWrite(LIGHTING_LED_PIN, currentLightingBrightness);
      lastLightingFadeTime = currentMillis;
      if (currentLightingBrightness == 255) {
        lightingFadingIn = false;
      }
    }
  }
  if (isDoorOpened && !isDoorClosedAfterOpen && digitalRead(CAPSULE_REED_PIN) == LOW) {
    isDoorClosedAfterOpen = true;
    doorCloseTime = currentMillis;
    if (!endReedTriggeredWithFastSpin) {
      putWheelToSleep();
    } else {
      periodicStorySoundActive = false;
      lastCageReedUnactivatedTime = 0;
    }
  }
  if (isDoorClosedAfterOpen && (currentMillis - doorCloseTime >= DOOR_REOPEN_DELAY)) {
    digitalWrite(DOOR_LOCK_PIN, HIGH);
    doorLockActiveTime = currentMillis;
    doorLockEngaged = true;
    isDoorClosedAfterOpen = false;
  }
  int currentKnightReedState = digitalRead(KNIGHT_REED_PIN);
  if (lastKnightReedState == HIGH && currentKnightReedState == LOW) {
    Serial.println((__FlashStringHelper *)MSG_DOBBY);
    sendLog("Knight sensor activated (help).");
  }
  lastKnightReedState = currentKnightReedState;
  int currentCageReedState = digitalRead(CAGE_REED_PIN);
  if (isPadlockActivated && currentQuestState != STATE_GAME_FINISHED) {
    if (!isCageReedInitialized && initialCageReedState != -1) {
      if ((initialCageReedState == LOW && currentCageReedState == HIGH) || (initialCageReedState == HIGH && currentCageReedState == LOW)) {
        isCageReedInitialized = true;
        lastCageReedActivationTime = currentMillis;
      }
    }
    if (isCageReedInitialized) {
      if (lastCageReedState == HIGH && currentCageReedState == LOW) {
        unsigned long pulseDuration = currentMillis - lastCageReedActivationTime;
        lastCageReedActivationTime = currentMillis;
        if (pulseDuration < 2000 && pulseDuration > 0) {
          if (!isFastSpinning) {
            isFastSpinning = true;
            wasFastSpinningActive = true;
            periodicFastSpinSoundActive = true;
            lastFastSpinStorySoundTime = currentMillis;
            Serial.println((__FlashStringHelper *)MSG_GROWL);
            sendLog("Fast spinning detected (dog_growl).");
            int firstRandomIndex = random(NUM_STORY_SOUNDS);
            char firstBuffer[MAX_UART_MESSAGE_LENGTH];
            strcpy_P(firstBuffer, (PGM_P)pgm_read_word(&(STORY_SOUNDS[firstRandomIndex])));
            vibroPulseState = true;
            digitalWrite(VIBRO_MOTOR_PIN, HIGH);
            analogWrite(LIGHTING_LED_PIN, 255);
            lastVibroPulseTime = currentMillis;
            int patternIndex = random(NUM_VIBRO_PATTERNS);
            currentVibroOnDuration = VIBRO_ON_DURATIONS[patternIndex];
            currentVibroOffDuration = VIBRO_OFF_DURATIONS[patternIndex];
            digitalWrite(LED_STRIP_PIN, HIGH);
            lastFastSpinEffectTime = currentMillis;
            delay(500);
            Serial.println(firstBuffer);
          }
          periodicStorySoundActive = false;
          lastCageReedUnactivatedTime = 0;
          cageReedLowStartTime = 0;
          cageReedWasLow = false;
        }
      }
      lastCageReedState = currentCageReedState;
      if (isFastSpinning || (endReedTriggeredWithFastSpin && (digitalRead(VIBRO_MOTOR_PIN) == HIGH || digitalRead(LED_STRIP_PIN) == HIGH))) {
        if (vibroPulseState) {
          if (currentMillis - lastVibroPulseTime >= currentVibroOnDuration) {
            digitalWrite(VIBRO_MOTOR_PIN, LOW);
            analogWrite(LIGHTING_LED_PIN, 0);
            vibroPulseState = false;
            lastVibroPulseTime = currentMillis;
            int patternIndex = random(NUM_VIBRO_PATTERNS);
            currentVibroOnDuration = VIBRO_ON_DURATIONS[patternIndex];
            currentVibroOffDuration = VIBRO_OFF_DURATIONS[patternIndex];
          }
        } else {
          if (currentMillis - lastVibroPulseTime >= currentVibroOffDuration) {
            digitalWrite(VIBRO_MOTOR_PIN, HIGH);
            analogWrite(LIGHTING_LED_PIN, 255);
            vibroPulseState = true;
            lastVibroPulseTime = currentMillis;
            int patternIndex = random(NUM_VIBRO_PATTERNS);
            currentVibroOnDuration = VIBRO_ON_DURATIONS[patternIndex];
            currentVibroOffDuration = VIBRO_OFF_DURATIONS[patternIndex];
          }
        }
        if (currentMillis - lastBlinkTime_LEDStrip >= BLINK_INTERVAL_FAST) {
          lastBlinkTime_LEDStrip = currentMillis;
          digitalWrite(LED_STRIP_PIN, !digitalRead(LED_STRIP_PIN));
        }
        if (periodicFastSpinSoundActive && (currentMillis - lastFastSpinStorySoundTime >= 12000)) {
          int randomIndex = random(NUM_STORY_SOUNDS);
          char buffer[MAX_UART_MESSAGE_LENGTH];
          strcpy_P(buffer, (PGM_P)pgm_read_word(&(STORY_SOUNDS[randomIndex])));
          Serial.println(buffer);
          lastFastSpinStorySoundTime = currentMillis;
        }
      }
      if (!isFastSpinning) {
        if (digitalRead(CAPSULE_REED_PIN) == HIGH) {
          if (currentCageReedState == HIGH) {
            if (lastCageReedUnactivatedTime == 0) {
              lastCageReedUnactivatedTime = currentMillis;
            }
            if (currentMillis - lastCageReedUnactivatedTime >= 12000 && !periodicStorySoundActive) {
              periodicStorySoundActive = true;
              lastStorySoundPlayTime = currentMillis;
            }
          } else {
            lastCageReedUnactivatedTime = 0;
            if (periodicStorySoundActive) {
              periodicStorySoundActive = false;
            }
          }
        } else {
          lastCageReedUnactivatedTime = 0;
          if (periodicStorySoundActive) {
            periodicStorySoundActive = false;
          }
        }
        /* if (periodicStorySoundActive && (currentMillis - lastStorySoundPlayTime >= 10000)) {
          int randomIndex = random(NUM_STORY_SOUNDS);
          char buffer[MAX_UART_MESSAGE_LENGTH];
          strcpy_P(buffer, (PGM_P)pgm_read_word(&(STORY_SOUNDS[randomIndex])));
          lastStorySoundPlayTime = currentMillis;
        } */
      }
      bool shouldGoToSleepCheck = false;
      if (currentCageReedState == LOW) {
        if (!cageReedWasLow) {
          cageReedLowStartTime = currentMillis;
          cageReedWasLow = true;
        }
        if (currentMillis - cageReedLowStartTime >= 12000) {
          shouldGoToSleepCheck = true;
        }
      } else {
        cageReedWasLow = false;
        cageReedLowStartTime = 0;
      }
      if (digitalRead(CAPSULE_REED_PIN) == LOW) {
        shouldGoToSleepCheck = true;
      }
      if (shouldGoToSleepCheck) {
        putWheelToSleep();
      }
    }
  }
  int currentEndReedState = digitalRead(END_REED_PIN);
  if (lastEndReedState == HIGH && currentEndReedState == LOW) {
    if (currentQuestState == STATE_IN_PROGRESS) {
      if (endReedTriggeredWithFastSpin && !isFastSpinning) {
        currentQuestState = STATE_GAME_FINISHED;
        putWheelToSleep();
        endReedTriggeredWithFastSpin = false;
      } else {
        activateEndStage();
      }
    }
  }
  lastEndReedState = currentEndReedState;

  // --- ETAP 9: Gerkon "kristall" (tolko posle zaversheniya igry) ---
  if (currentQuestState == STATE_GAME_FINISHED && !isCrystalLockedOff) {
    int currentCrystalReedState = digitalRead(CRYSTAL_REED_PIN);
    if (lastCrystalReedState == HIGH && currentCrystalReedState == LOW) {
      Serial.println((__FlashStringHelper *)MSG_CRYSTAL);
      sendLog("Crystal sensor activated (crystal).");
      isCrystalPulsating = true;
      //Serial.println((__FlashStringHelper *)MSG_ITEM_FIND); // Otpravlyaem soobshchenie pri starte pulsacii
    }
    lastCrystalReedState = currentCrystalReedState;
  }

  // --- Plavnoe vklyuchenie/vyklyuchenie osveshcheniya kristalla (CRYSTAL_LIGHT_PIN) ---
  if (crystalFadingIn && currentCrystalBrightness < 255) {
    if (currentMillis - lastCrystalFadeTime >= CRYSTAL_FADE_INTERVAL) {
      currentCrystalBrightness += CRYSTAL_FADE_STEP;
      if (currentCrystalBrightness > 255)
        currentCrystalBrightness = 255;
      analogWrite(CRYSTAL_LIGHT_PIN, currentCrystalBrightness);
      lastCrystalFadeTime = currentMillis;
      if (currentCrystalBrightness == 255) {
        crystalFadingIn = false;
      }
    }
  } else if (crystalFadingOut && currentCrystalBrightness > 0) {
    if (currentMillis - lastCrystalFadeTime >= CRYSTAL_FADE_INTERVAL) {
      currentCrystalBrightness -= CRYSTAL_FADE_STEP;
      if (currentCrystalBrightness < 0)
        currentCrystalBrightness = 0;
      analogWrite(CRYSTAL_LIGHT_PIN, currentCrystalBrightness);
      lastCrystalFadeTime = currentMillis;
      if (currentCrystalBrightness == 0) {
        crystalFadingOut = false;
      }
    }
  }

  // NOVYY BLOK: Logika pulsacii kristalla
  if (isCrystalPulsating) {
  // 1. Logika PULSACII (kogda flag isCrystalPulsating = true)
  if (currentMillis - lastCrystalFadeTime >= CRYSTAL_FADE_INTERVAL) { // Ispol'zuem sushchestvuyushchie konstanty
    lastCrystalFadeTime = currentMillis;

    if (crystalFadingIn) { // Ispol'zuem 'crystalFadingIn' kak napravlenie (true = vverkh)
      currentCrystalBrightness += CRYSTAL_FADE_STEP;
      if (currentCrystalBrightness >= 255) {
        currentCrystalBrightness = 255;
        crystalFadingIn = false; // Menyaem napravlenie na "vniz"
      }
    } else {
      currentCrystalBrightness -= CRYSTAL_FADE_STEP;
      if (currentCrystalBrightness <= 0) {
        currentCrystalBrightness = 0;
        crystalFadingIn = true; // Menyaem napravlenie na "vverkh"
      }
    }
    analogWrite(CRYSTAL_LIGHT_PIN, currentCrystalBrightness);
  }
  // Sbros flaga FadingOut, on ne ispol'zuetsya v etom rezhime
  crystalFadingOut = false;

} else if (crystalFadingIn) {
  // 2. Logika plavnogo VKLYUCHENIYA (kogda NE pulsaciya, napr. komanda 'skin')
  if (currentMillis - lastCrystalFadeTime >= CRYSTAL_FADE_INTERVAL) {
    currentCrystalBrightness += CRYSTAL_FADE_STEP;
    if (currentCrystalBrightness > 255)
      currentCrystalBrightness = 255;
    analogWrite(CRYSTAL_LIGHT_PIN, currentCrystalBrightness);
    lastCrystalFadeTime = currentMillis;
    if (currentCrystalBrightness == 255) {
      crystalFadingIn = false; // Zavershili
    }
  }
} else if (crystalFadingOut) {
  // 3. Logika plavnogo VIKLYUCHENIYA (kogda NE pulsaciya)
  if (currentMillis - lastCrystalFadeTime >= CRYSTAL_FADE_INTERVAL) {
    currentCrystalBrightness -= CRYSTAL_FADE_STEP;
    if (currentCrystalBrightness < 0)
      currentCrystalBrightness = 0;
    analogWrite(CRYSTAL_LIGHT_PIN, currentCrystalBrightness);
    lastCrystalFadeTime = currentMillis;
    if (currentCrystalBrightness == 0) {
      crystalFadingOut = false; // Zavershili
      crystalFadingIn = true; // Sbrasyvaem napravlenie pulsacii na "vverkh" dlya sleduyushchego raza
    }
  }
}
}
