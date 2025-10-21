#include <Arduino.h>
#include <FastLED.h>  // Включаем библиотеку FastLED
#include "GyverButton.h"

unsigned long previousLockerTime = 0;       // Время последнего открытия
const unsigned long lockerInterval = 8000;  // Интервал 8 секунд
const unsigned long lockerDuration = 500;   // Длительность открытия 0.5 секунды
bool lockerActive = false;                  // Флаг активности открытия
bool owlCommandReceived = 0;
int state = 0;
int owl = 0;

// --- Пины для устройств ---
const int PIN_LOKER_DOOR = 23;  // Локер "дверь" (через реле)
const int PIN_LED_ROOM = 12;    // Светодиод "освещение комнаты" (ШИМ-совместимый)
const int PIN_LED_WINDOW = 21;  // Освещение окна (ШИМ-совместимый для мгновенного включения/выключения)
const int LED_PIN_TILES = 3;    // АДРЕСНАЯ ЛЕНТА ДЛЯ ПЛИТОК - ПЕРЕНАЗНАЧЕН НА ПИН 3
const int NUM_TILE_LEDS = 4;    // Количество светодиодов на ленте для плиток

CRGB tileLeds[NUM_TILE_LEDS];  // Массив для хранения цветов светодиодов плиток

// --- Пины для плиток с подсветкой ---
GButton PIN_HERKON_BOAT(30);    // НОВЫЙ: Геркон "лодка"
GButton PIN_IR_FLAG(27);        // НОВЫЙ: ИК Датчик "Флаг"
GButton PIN_HERKON_OWA(29);     // Геркон "сова"
GButton PIN_HERKON_DWARF(31);   // Геркон "гном"
GButton PIN_HERKON_TILE1(A4);   // Геркон плитка 1
GButton PIN_HERKON_TILE2(A6);   // Геркон плитка 2
GButton PIN_HERKON_TILE3(A8);   // Геркон плитка 3
GButton PIN_HERKON_TILE4(A10);  // Геркон плитка 4
GButton PIN_HERKON_TILE0(5);    // Геркон плитка 0



// --- Пины для левого галетного переключателя ---
// Позиции: D46=2, D44=5, D42=4, D40=3
GButton PIN_DIAL_LEFT_POS2(46);
GButton PIN_DIAL_LEFT_POS3(44);  // Это 5-я позиция на схеме
GButton PIN_DIAL_LEFT_POS4(42);  // Это 4-я позиция на схеме
GButton PIN_DIAL_LEFT_POS5(40);  // Это 3-я позиция на схеме

// --- Пины для правого галетного переключателя ---
// Позиции: D32=1, D34=2, D36=3, D38=4
GButton PIN_DIAL_RIGHT_POS1(32);
GButton PIN_DIAL_RIGHT_POS2(34);
GButton PIN_DIAL_RIGHT_POS3(36);
GButton PIN_DIAL_RIGHT_POS4(38);

unsigned long owlEndTime = 0;
unsigned long rainbowStartTime = 0;
const unsigned long OWL_END_DELAY = 2000;      // Ждем 2 секунды после owl_end
const unsigned long RAINBOW_DURATION = 10000;  // Радуга на 10 секунд
uint8_t gHue = 0;                              // Для радужной анимации// Для радужной анимации

bool skipCommand = false;  // Флаг для команды skip
bool rainbowActive = false;
bool tile0State = 0;
bool tile1State = 0;
bool tile2State = 0;
bool tile3State = 0;
bool tile4State = 0;
// Глобальные переменные для отслеживания решения плиток
bool tile1Solved = false;
bool tile2Solved = false;
bool tile3Solved = false;
bool tile4Solved = false;
bool allTilesSolved = false;
bool _restartFlag;
bool _restartGalet;
bool F;
bool fireworkActive = false; // Флаг для фейерверка

void handleSkipCommand() {
  if (skipCommand) {
    if (!rainbowActive) {
      // Первый вызов после команды skip
      rainbowActive = true;
      rainbowStartTime = millis();
      // Зажигаем все зеленым
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
      }
      FastLED.show();
      Serial1.println("owl_end");
    }

    // Запускаем радужную анимацию
    rainbowAnimation();

    // Проверяем закончилось ли время радуги
    if (millis() - rainbowStartTime >= RAINBOW_DURATION) {
      rainbowActive = false;
      skipCommand = false;
      // Оставляем зеленый цвет
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
      }
      FastLED.show();
      //Serial1.println("Rainbow finished");
    }
  }
}

// Функция радужной анимации
void rainbowAnimation() {
  fill_rainbow(tileLeds, NUM_TILE_LEDS, gHue, 7);
  gHue++;
  FastLED.show();
}

void setbutton(GButton &buttonN) {
  buttonN.setDebounce(10);      // настройка антидребезга (по умолчанию 80 мс)
  buttonN.setTimeout(10);       // настройка таймаута на удержание (по умолчанию 500 мс)
  buttonN.setClickTimeout(10);  // настройка таймаута между кликами (по умолчанию 300 мс)
  buttonN.setType(HIGH_PULL);
  buttonN.setDirection(NORM_OPEN);
  buttonN.setTickMode(AUTO);
}

void controlLocker() {
  unsigned long currentTime = millis();

  if (!lockerActive) {
    // Режим ожидания - проверяем интервал 8 секунд
    if (currentTime - previousLockerTime >= lockerInterval) {
      digitalWrite(PIN_LOKER_DOOR, HIGH);
      lockerActive = true;
      previousLockerTime = currentTime;  // Запоминаем время открытия
      //Serial1.println("Locker OPEN");
    }
  } else {
    // Замок открыт - проверяем время до закрытия
    if (currentTime - previousLockerTime >= lockerDuration) {
      digitalWrite(PIN_LOKER_DOOR, LOW);
      lockerActive = false;
      previousLockerTime = currentTime;  // Сбрасываем таймер для нового цикла
                                         // Serial1.println("Locker CLOSE");
    }
  }
}


void handleSerial1Commands() {
  if (Serial1.available()) {
    String command = Serial1.readStringUntil('\n');  // Читаем до LF
    command.trim();                                  // Удаляем CR и пробелы

    // Удаляем CR если он есть в конце
    if (command.endsWith("\r")) {
      command.remove(command.length() - 1);
    }

    if (command == "day_on") {
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
        digitalWrite(PIN_LED_ROOM, HIGH);
        digitalWrite(PIN_LED_WINDOW, HIGH);
      }
      FastLED.show();
    }

    if (command == "day_off") {
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Black;
        digitalWrite(PIN_LED_ROOM, 0);
        digitalWrite(PIN_LED_WINDOW, 0);
      }
      FastLED.show();
    }

    if (command == "owl") {
      state = 0;
      owlCommandReceived = true;
      F = false;
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Black;
        digitalWrite(PIN_LED_ROOM, 0);
        digitalWrite(PIN_LED_WINDOW, 0);
      }
      FastLED.show();
    }

    if (command == "out") {
      //  state = 0;
      owlCommandReceived = false;
    }

    if (command == "owl_door") {
      F = true;  // Устанавливаем флаг
      Serial1.println("door_owl");
      Serial.println("door_owl");
      state = 1;
    }

    if (command == "open_door") {
      digitalWrite(PIN_LOKER_DOOR, HIGH);
      delay(100);
      digitalWrite(PIN_LOKER_DOOR, LOW);
    }

    if (command == "firework") {

      fireworkActive = true;

      FastLED.clear(); // Очищаем светодиоды перед запуском

      FastLED.show();

    }

    if (command == "skip") {
      skipCommand = true;
      rainbowActive = true;
      rainbowStartTime = millis();

      tile1Solved = true;
      tile2Solved = true;
      tile3Solved = true;
      tile4Solved = true;
      owl = 6;
      digitalWrite(PIN_LED_ROOM, HIGH);
      digitalWrite(PIN_LED_WINDOW, HIGH);
      state = 2;

      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
      }
      FastLED.show();
    }

    if (command == "ready") {
      _restartFlag = 0;
      _restartGalet = 0;
    }

    if (command == "restart") {
      // Сброс всех переменных
      skipCommand = false;
      fireworkActive = false;
      rainbowActive = false;
      tile0State = false;
      tile1State = false;
      tile2State = false;
      tile3State = false;
      tile4State = false;
      tile1Solved = false;
      tile2Solved = false;
      tile3Solved = false;
      tile4Solved = false;
      lockerActive = false;
      owlCommandReceived = false;
      state = 0;
      owl = 0;
      F = false;
      if(!digitalRead(30) && !_restartGalet){
        Serial1.println("galet_on");
        _restartGalet = 1;
        delay(500);
      }
    if(digitalRead(30) && _restartGalet){
      Serial1.println("galet_off");
      _restartGalet = 0;
      delay(500);
    }


    if(digitalRead(27) && !_restartFlag){
      Serial1.println("flag4_on");
      _restartFlag = 1;
      delay(500);
    }
    if(_restartFlag && !digitalRead(27)){
      Serial1.println("flag4_off");
      delay(500);
      _restartFlag = 0;
    }
    digitalWrite(PIN_LOKER_DOOR, HIGH);
      delay(100);
      digitalWrite(PIN_LOKER_DOOR, LOW);
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
        digitalWrite(PIN_LED_ROOM, HIGH);
        digitalWrite(PIN_LED_WINDOW, HIGH);
      }
      FastLED.show();
    }

    if (command == "start") {
      // Сброс всех переменных
      skipCommand = false;
      rainbowActive = false;
      tile0State = false;
      tile1State = false;
      tile2State = false;
      tile3State = false;
      tile4State = false;
      tile1Solved = false;
      tile2Solved = false;
      tile3Solved = false;
      tile4Solved = false;
      lockerActive = false;
      owlCommandReceived = false;
      state = 0;
      _restartFlag = 0;
      _restartGalet = 0;
      owl = 0;
      F = false;
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Black;
        digitalWrite(PIN_LED_ROOM, 0);
        digitalWrite(PIN_LED_WINDOW, 0);
      }
      FastLED.show();
    }
  }
}


// Функция проверки кнопки совы
void checkOwlButton() {

  if (owlCommandReceived && PIN_HERKON_OWA.isHold()) {
    if (!F) {
      F = true;  // Устанавливаем флаг
      Serial.println("door_owl");
      delay(1000);
      Serial1.println("door_owl");
      delay(1000);
      state = 1;
    }
  } else if (!PIN_HERKON_OWA.isHold()) {
    F = false;  // Сбрасываем флаг когда кнопка отпущена
  }
}

void handleFlagSensorSimple() {
  if (PIN_IR_FLAG.isPress()) {
    Serial1.println("flag4_off");
  }
  if (PIN_IR_FLAG.isRelease()) {
    Serial1.println("flag4_on");
  }
}

void handleBOATSensorSimple() {
  if (PIN_HERKON_BOAT.isPress()) {
    Serial1.println("galet_on");
  }
  if (PIN_HERKON_BOAT.isRelease()) {
    Serial1.println("galet_off");
  }
}

void HELP() {
  if (PIN_HERKON_DWARF.isPress()) {
    Serial1.println("help");
  }
}

void handleTileLeds() {

  if (skipCommand) {
    return;
  }

  // ЕСЛИ РАДУГА АКТИВНА - ТОЛЬКО АНИМАЦИЯ И ВЫХОД
  if (rainbowActive) {
    rainbowAnimation();

    // ПРОВЕРЯЕМ ВРЕМЯ РАДУГИ ОТ НАЧАЛА АНИМАЦИИ
    if (millis() - rainbowStartTime >= RAINBOW_DURATION) {
      rainbowActive = false;
      // Возвращаем зеленый цвет на все плитки
      for (int i = 0; i < NUM_TILE_LEDS; i++) {
        tileLeds[i] = CRGB::Green;
      }
      FastLED.show();
      //Serial1.println("Rainbow finished");
      owl = 6;
    }
    return;  // ВЫХОДИМ ИЗ ФУНКЦИИ
  }

  // ПЛИТКА 1
  if (!tile1Solved && PIN_HERKON_TILE1.isHold() && PIN_DIAL_LEFT_POS3.isHold() && PIN_DIAL_RIGHT_POS2.isHold()) {
    tileLeds[0] = CRGB::Green;
    tile1Solved = true;
    owl++;
    //digitalWrite(PIN_LED_WINDOW, 0);

    if (owl < 4) {
      Serial1.println("owl_flew");
    }
    delay(10);
  }

  // ПЛИТКА 2
  if (!tile2Solved && PIN_HERKON_TILE2.isHold() && PIN_DIAL_LEFT_POS4.isHold() && PIN_DIAL_RIGHT_POS3.isHold()) {
    tileLeds[1] = CRGB::Green;
    tile2Solved = true;
    owl++;
    //digitalWrite(PIN_LED_WINDOW, 0);

    if (owl < 4) {
      Serial1.println("owl_flew");
    }
    delay(10);
  }

  // ПЛИТКА 3
  if (!tile3Solved && PIN_HERKON_TILE3.isHold() && PIN_DIAL_LEFT_POS2.isHold() && PIN_DIAL_RIGHT_POS1.isHold()) {
    tileLeds[2] = CRGB::Green;
    tile3Solved = true;
    owl++;
    //digitalWrite(PIN_LED_WINDOW, 0);

    if (owl < 4) {
      Serial1.println("owl_flew");
    }
    delay(10);
  }

  // ПЛИТКА 4
  if (!tile4Solved && PIN_HERKON_TILE4.isHold() && PIN_DIAL_LEFT_POS5.isHold() && PIN_DIAL_RIGHT_POS4.isHold()) {
    tileLeds[3] = CRGB::Green;
    tile4Solved = true;
    owl++;
    //digitalWrite(PIN_LED_WINDOW, 0);
    if (owl < 4) {
      Serial1.println("owl_flew");
    }
    delay(10);
  }

  // ПРОВЕРКА ВСЕХ ПЛИТОК И ЗАПУСК РАДУГИ
  if (owl == 4 && !rainbowActive) {
    //delay(1000);
    Serial1.println("owl_end");
    owl++;
    owlEndTime = millis();
  }

  // ЗАПУСК РАДУГИ ЧЕРЕЗ 2 СЕКУНДЫ ПОСЛЕ owl_end
  if (owl == 5 && !rainbowActive && millis() - owlEndTime >= OWL_END_DELAY) {
    rainbowActive = true;
    rainbowStartTime = millis();
    //Serial1.println("Rainbow started");
  }

  // ОБЫЧНАЯ ЛОГИКА ПОДСВЕТКИ (только для нерешенных плиток)
  // Плитка 1 -> Светодиод 0

  if (PIN_HERKON_TILE0.isHold()) {
    if (!tile0State) {
      Serial1.println("light_off");
      tile0State = true;

      digitalWrite(PIN_LED_WINDOW, 0);
    }
  } else {
    if (tile0State) {
      Serial1.println("light_on");
      tile0State = false;
      digitalWrite(PIN_LED_WINDOW, 1);
    }
  }

  if (!tile1Solved) {
    if (PIN_HERKON_TILE1.isHold()) {
      tileLeds[0] = CRGB::Yellow;
      if (!tile1State) {
        // Serial1.println("light_on");
        tile1State = true;
        //digitalWrite(PIN_LED_WINDOW, 1);
      }
    } else {
      tileLeds[0] = CRGB::Black;
      if (tile1State) {
        // Serial1.println("light_off");
        tile1State = false;
        // digitalWrite(PIN_LED_WINDOW, 0);
      }
    }
  }

  // Плитка 2 -> Светодиод 1
  if (!tile2Solved) {
    if (PIN_HERKON_TILE2.isHold()) {
      tileLeds[1] = CRGB::Yellow;
      if (!tile2State) {
        // Serial1.println("light_on");
        tile2State = true;
        // digitalWrite(PIN_LED_WINDOW, 1);
      }
    } else {
      tileLeds[1] = CRGB::Black;
      if (tile2State) {
        //Serial1.println("light_off");
        tile2State = false;
        //digitalWrite(PIN_LED_WINDOW, 0);
      }
    }
  }

  // Плитка 3 -> Светодиод 2
  if (!tile3Solved) {
    if (PIN_HERKON_TILE3.isHold()) {
      tileLeds[2] = CRGB::Yellow;
      if (!tile3State) {
        //Serial1.println("light_on");
        tile3State = true;
        //digitalWrite(PIN_LED_WINDOW, 1);
      }
    } else {
      tileLeds[2] = CRGB::Black;
      if (tile3State) {
        // Serial1.println("light_off");
        tile3State = false;
        // digitalWrite(PIN_LED_WINDOW, 0);
      }
    }
  }

  // Плитка 4 -> Светодиод 3
  if (!tile4Solved) {
    if (PIN_HERKON_TILE4.isHold()) {
      tileLeds[3] = CRGB::Yellow;
      if (!tile4State) {
        //Serial1.println("light_on");
        tile4State = true;
        //digitalWrite(PIN_LED_WINDOW, 1);
      }
    } else {
      tileLeds[3] = CRGB::Black;
      if (tile4State) {
        //Serial1.println("light_off");
        tile4State = false;
        //digitalWrite(PIN_LED_WINDOW, 0);
      }
    }
  }

  FastLED.show();
}

void setup() {
  Serial1.begin(9600);
  Serial.begin(9600);

  FastLED.addLeds<WS2812B, LED_PIN_TILES, GRB>(tileLeds, NUM_TILE_LEDS);
  FastLED.setBrightness(100);
  FastLED.clear();
  FastLED.show();

  pinMode(PIN_LOKER_DOOR, OUTPUT);
  pinMode(PIN_LED_ROOM, OUTPUT);
  pinMode(PIN_LED_WINDOW, OUTPUT);

  setbutton(PIN_HERKON_BOAT);
  setbutton(PIN_IR_FLAG);

  setbutton(PIN_HERKON_OWA);
  setbutton(PIN_HERKON_DWARF);
  setbutton(PIN_HERKON_TILE1);
  setbutton(PIN_HERKON_TILE2);
  setbutton(PIN_HERKON_TILE3);
  setbutton(PIN_HERKON_TILE4);
  setbutton(PIN_HERKON_TILE0);

  setbutton(PIN_DIAL_LEFT_POS2);
  setbutton(PIN_DIAL_LEFT_POS3);
  setbutton(PIN_DIAL_LEFT_POS4);
  setbutton(PIN_DIAL_LEFT_POS5);
  setbutton(PIN_DIAL_RIGHT_POS1);
  setbutton(PIN_DIAL_RIGHT_POS2);
  setbutton(PIN_DIAL_RIGHT_POS3);
  setbutton(PIN_DIAL_RIGHT_POS4);
}


void loop() {
  // Главный state machine
  if (fireworkActive) {
    handleFirework();
  } else {
  switch (state) {
    case 1:
      if (skipCommand) {
        handleSkipCommand();
      } else {
        handleTileLeds();  // Обычная логика плиток
      }
      digitalWrite(PIN_LED_ROOM, HIGH);
      break;
    case 2:

      break;
  }
  }
  if (state > 0) controlLocker();

  // Общие функции, работающие в любом состоянии
  FastLED.show();
  delay(10);
  handleSerial1Commands();   // Проверяем команды
  checkOwlButton();  
    handleFlagSensorSimple();  //проверяем флаг
    handleBOATSensorSimple();  //проверяем лодку
    HELP();  
                    //просим подсказку
  //digitalWrite(PIN_LED_WINDOW, 0);
}

// Адаптированная функция фейерверка для owls.ino (FastLED)
void handleFirework() {
  if (!fireworkActive) return;

  static unsigned long lastFireworkTime = 0;
  // Уменьшаем кол-во взрывов для 4 светодиодов
  static const int MAX_EXPLOSIONS = 2; 
  // Уменьшаем макс. радиус взрыва
  static const int FIREWORK_RADIUS = 2; 
  static struct Explosion {
    int phase;
    CRGB color;
    int center;
    unsigned long startTime;
  } explosions[MAX_EXPLOSIONS];


  // Создаем новые взрывы (реже, т.к. светодиодов мало)
  if (millis() - lastFireworkTime >= 1000) { // [изменено с 600]
    lastFireworkTime = millis();
    // Ищем свободный слот для нового взрыва
    for (int i = 0; i < MAX_EXPLOSIONS; i++) {
      if (explosions[i].phase == 0) {
        // Красивые цвета салюта
        CRGB niceColors[] = {
          CRGB(255, 100, 50),   // Оранжевый
          CRGB(100, 255, 100),  // Светло-зеленый
          CRGB(100, 100, 255),  // Светло-синий
          CRGB(255, 255, 100),  // Светло-желтый
          CRGB(255, 100, 255),  // Розовый
          CRGB(100, 255, 255)   // Бирюзовый
        };
        explosions[i].color = niceColors[random(6)];
        // Центр взрыва - случайный из 4 светодиодов
        explosions[i].center = random(NUM_TILE_LEDS); // 
        explosions[i].startTime = millis();
        explosions[i].phase = 1;
        break;
      }
    }
  }

  // Обрабатываем все активные взрывы
  for (int e = 0; e < MAX_EXPLOSIONS; e++) {
    if (explosions[e].phase > 0) {
      unsigned long elapsed = millis() - explosions[e].startTime;
      if (elapsed < 500) {
        // Фаза расширения
        float progress = (float)elapsed / 500.0;
        int radius = progress * FIREWORK_RADIUS; // Используем новый радиус

        for (int i = 0; i < NUM_TILE_LEDS; i++) { // 
          int distance = abs(i - explosions[e].center);
          if (distance <= radius) {
            float intensity = 1.0 - (float)distance / radius;
            // Применяем цвет с масштабированной яркостью
            tileLeds[i] = explosions[e].color; // 
            tileLeds[i].fadeToBlackBy(255 - (intensity * 255)); // 
          }
        }
      } else if (elapsed < 1000) {
        // Фаза затухания
        float fadeProgress = (float)(elapsed - 500) / 500.0;
        uint8_t fadeAmount = fadeProgress * 255;

        for (int i = 0; i < NUM_TILE_LEDS; i++) { // 
          int distance = abs(i - explosions[e].center);
          if (distance <= FIREWORK_RADIUS) { // Используем новый радиус
            // Применяем цвет и сразу затухаем
            tileLeds[i] = explosions[e].color; // 
            tileLeds[i].fadeToBlackBy(fadeAmount); // 
          }
        }
      } else {
        // Завершаем взрыв
        explosions[e].phase = 0;
        for (int i = 0; i < NUM_TILE_LEDS; i++) { // 
          int distance = abs(i - explosions[e].center);
          if (distance <= FIREWORK_RADIUS) { // Используем новый радиус
            tileLeds[i] = CRGB::Black; // 
          }
        }
      }
    }
  }

  // Плавное затухание всех светодиодов
  EVERY_N_MILLISECONDS(20) {
    for (int i = 0; i < NUM_TILE_LEDS; i++) { // 
      if (tileLeds[i] != CRGB::Black) { // 
        tileLeds[i].fadeToBlackBy(8); // 
      }
    }
  }

  FastLED.show();
}
