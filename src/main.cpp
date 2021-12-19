#include <Arduino.h>

#include "FastLED.h"
#include "GyverButton.h"

// using TMode = void(*)(void);
// TMode modes[] = {&rainbow1, &rainbow2, &red, &green, &blue, &random1};
// void loop() {
//   static uint64_t time = millis();
//   if (time + 15 < millis()) {
//     modes[mode]();
//     time = millis();
//   }
// }

/*
  Скетч создан на основе FASTSPI2 EFFECTS EXAMPLES автора teldredge
  (www.funkboxing.com) А также вот этой статьи
  https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/#cylon
  Доработан, переведён и разбит на файлы 2017 AlexGyver
*/

#define LED_COUNT 64
#define LED_DT 2

int max_bright = 128;

GButton but1(4);
GButton but2(6);

volatile uint16_t ledMode = 0;
const uint16_t ledModeMox = 39;

// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
int BOTTOM_INDEX = 0;  // светодиод начала отсчёта
int TOP_INDEX = int(LED_COUNT / 2);
int EVENODD = LED_COUNT % 2;
struct CRGB leds[LED_COUNT];
int ledsX[LED_COUNT][3];  //-ARRAY FOR COPYING WHATS IN THE LED STRIP CURRENTLY
                          //(FOR CELL-AUTOMATA, MARCH, ETC)

int thisdelay = 20;  //-FX LOOPS DELAY VAR
int thisstep = 10;   //-FX LOOPS DELAY VAR
int thishue = 0;     //-FX LOOPS DELAY VAR
int thissat = 255;   //-FX LOOPS DELAY VAR

int thisindex = 0;
int thisRED = 0;
int thisGRN = 0;
int thisBLU = 0;

int idex = 0;             //-LED INDEX (0 to LED_COUNT-1
int ihue = 0;             //-HUE (0-255)
int ibright = 0;          //-BRIGHTNESS (0-255)
int isat = 0;             //-SATURATION (0-255)
int bouncedirection = 0;  //-SWITCH FOR COLOR BOUNCE (0-1)
float tcount = 0.0;       //-INC VAR FOR SIN LOOPS
int lcount = 0;           //-ANOTHER COUNTING VAR
// ---------------СЛУЖЕБНЫЕ ПЕРЕМЕННЫЕ-----------------
void change_mode(int newmode);

#include "effects.h"

void change_mode(int newmode) {
  thissat = 255;
  switch (newmode) {
    case 0:
      thisdelay = 60;
      break;  //---STRIP RAINBOW FADE
    case 1:
      thisdelay = 20;
      thisstep = 10;
      break;  //---RAINBOW LOOP
    case 2:
      thisdelay = 20;
      break;  //---RANDOM BURST
    case 3:
      thisdelay = 20;
      thishue = 0;
      break;  //---CYLON v1
    case 4:
      thisdelay = 80;
      thishue = 0;
      break;  //---CYLON v2
    case 5:
      thisdelay = 40;
      thishue = 0;
      break;  //---POLICE LIGHTS SINGLE
    case 6:
      thisdelay = 40;
      thishue = 0;
      break;  //---POLICE LIGHTS SOLID
    case 7:
      thishue = 160;
      thissat = 50;
      break;  //---STRIP FLICKER
    case 8:
      thisdelay = 15;
      thishue = 0;
      break;  //---PULSE COLOR BRIGHTNESS
    case 9:
      thisdelay = 30;
      thishue = 0;
      break;  //---PULSE COLOR SATURATION
    case 10:
      thisdelay = 60;
      thishue = 180;
      break;  //---VERTICAL SOMETHING
    case 11:
      thisdelay = 100;
      break;  //---CELL AUTO - RULE 30 (RED)
    case 12:
      thisdelay = 80;
      break;  //---MARCH RANDOM COLORS
    case 13:
      thisdelay = 80;
      break;  //---MARCH RWB COLORS
    case 14:
      thisdelay = 60;
      thishue = 95;
      break;  //---RADIATION SYMBOL
    case 15:
      thisdelay = 60;
      thishue = 95;
      break;  //---RADIATION SYMBOL
    case 16:
      thisdelay = 60;
      thishue = 95;
      break;  //---RADIATION SYMBOL
    case 17:
      thisdelay = 35;
      thishue = 180;
      break;  //---SIN WAVE BRIGHTNESS
    case 18:
      thisdelay = 100;
      thishue = 0;
      break;  //---POP LEFT/RIGHT
    case 19:
      thisdelay = 100;
      thishue = 180;
      break;  //---QUADRATIC BRIGHTNESS CURVE
    case 20:
      thisdelay = 100;
      thishue = 180;
      break;  //---QUADRATIC BRIGHTNESS CURVE
    case 21:
      thisdelay = 50;
      thisstep = 15;
      break;  //---VERITCAL RAINBOW
    case 22:
      thisdelay = 50;
      break;  //---PACMAN
    case 23:
      thisdelay = 35;
      break;  //---RANDOM COLOR POP
    case 24:
      thisdelay = 25;
      thishue = 0;
      break;  //---EMERGECNY STROBE
    case 25:
      thisdelay = 100;
      thishue = 0;
      break;  //---RGB PROPELLER
    case 26:
      thisdelay = 100;
      thishue = 0;
      break;  //---KITT
    case 27:
      thisdelay = 100;
      thishue = 95;
      break;  //---MATRIX RAIN
    case 28:
      thisdelay = 15;
      break;  //---NEW RAINBOW LOOP
    case 29:
      thisdelay = 50;
      break;  // colorWipe
    case 30:
      thisdelay = 50;
      break;  // CylonBounce
    case 31:
      thisdelay = 15;
      break;  // Fire
    case 32:
      thisdelay = 20;
      break;  // rainbowCycle
    case 33:
      thisdelay = 10;
      break;  // rainbowTwinkle
    case 34:
      thisdelay = 50;
      break;  // RunningLights
    case 35:
      thisdelay = 0;
      break;  // Sparkle
    case 36:
      thisdelay = 30;
      break;  // SnowSparkle
    case 37:
      thisdelay = 50;
      break;  // theaterChase
    case 38:
      thisdelay = 50;
      break;  // theaterChaseRainbow
    case 39:
      thisdelay = 100;
      break;  // Strobe
  }
  bouncedirection = 0;
  one_color_all(0, 0, 0);
  ledMode = newmode;
}

void setup() {
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(3, LOW);
  digitalWrite(5, LOW);
  but1.setTickMode(true);
  but2.setTickMode(true);

  LEDS.setBrightness(max_bright);
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);
  one_color_all(0, 255, 0);
  LEDS.show();
  randomSeed(analogRead(0));
  change_mode(ledMode);
}

void loop() {
  but1.tick();
  but2.tick();
  switch (ledMode) {
    case 0:
      rainbow_fade();
      break;  // плавная смена цветов всей ленты
    case 1:
      rainbow_loop();
      break;  // крутящаяся радуга
    case 2:
      random_burst();
      break;  // случайная смена цветов
    case 3:
      color_bounce();
      break;  // бегающий светодиод
    case 4:
      color_bounceFADE();
      break;  // бегающий паровозик светодиодов
    case 5:
      ems_lightsONE();
      break;  // вращаются красный и синий
    case 6:
      ems_lightsALL();
      break;  // вращается половина красных и половина синих
    case 7:
      flicker();
      break;  // случайный стробоскоп
    case 8:
      pulse_one_color_all();
      break;  // пульсация одним цветом
    case 9:
      pulse_one_color_all_rev();
      break;  // пульсация со сменой цветов
    case 10:
      fade_vertical();
      break;  // плавная смена яркости по вертикали (для кольца)
    case 11:
      rule30();
      break;  // безумие красных светодиодов
    case 12:
      random_march();
      break;  // безумие случайных цветов
    case 13:
      rwb_march();
      break;  // белый синий красный бегут по кругу (ПАТРИОТИЗМ!)
    case 14:
      radiation();
      break;  // пульсирует значок радиации
    case 15:
      color_loop_vardelay();
      break;  // красный светодиод бегает по кругу
    case 16:
      white_temps();
      break;  // бело синий градиент (?)
    case 17:
      sin_bright_wave();
      break;  // тоже хрень какая то
    case 18:
      pop_horizontal();
      break;  // красные вспышки спускаются вниз
    case 19:
      quad_bright_curve();
      break;  // полумесяц
    case 20:
      flame();
      break;  // эффект пламени
    case 21:
      rainbow_vertical();
      break;  // радуга в вертикаьной плоскости (кольцо)
    case 22:
      pacman();
      break;  // пакман
    case 23:
      random_color_pop();
      break;  // безумие случайных вспышек
    case 24:
      ems_lightsSTROBE();
      break;  // полицейская мигалка
    case 25:
      rgb_propeller();
      break;  // RGB пропеллер
    case 26:
      kitt();
      break;  // случайные вспышки красного в вертикаьной плоскости
    case 27:
      matrix();
      break;  // зелёненькие бегают по кругу случайно
    case 28:
      new_rainbow_loop();
      break;  // крутая плавная вращающаяся радуга
    case 29:
      colorWipe(0x00, 0xff, 0x00, thisdelay);
      colorWipe(0x00, 0x00, 0x00, thisdelay);
      break;  // плавное заполнение цветом
    case 30:
      CylonBounce(0xff, 0, 0, 4, 10, thisdelay);
      break;  // бегающие светодиоды
    case 31:
      Fire(55, 120, thisdelay);
      break;  // линейный огонь
    case 32:
      rainbowCycle(thisdelay);
      break;  // очень плавная вращающаяся радуга
    case 33:
      TwinkleRandom(20, thisdelay, 1);
      break;  // случайные разноцветные включения (1 - танцуют все, 0 -
              // случайный 1 диод)
    case 34:
      RunningLights(0xff, 0xff, 0x00, thisdelay);
      break;  // бегущие огни
    case 35:
      Sparkle(0xff, 0xff, 0xff, thisdelay);
      break;  // случайные вспышки белого цвета
    case 36:
      SnowSparkle(0x10, 0x10, 0x10, thisdelay, random(100, 1000));
      break;  // случайные вспышки белого цвета на белом фоне
    case 37:
      theaterChase(0xff, 0, 0, thisdelay);
      break;  // бегущие каждые 3 (ЧИСЛО СВЕТОДИОДОВ ДОЛЖНО БЫТЬ КРАТНО 3)
    case 38:
      theaterChaseRainbow(thisdelay);
      break;  // бегущие каждые 3 радуга (ЧИСЛО СВЕТОДИОДОВ ДОЛЖНО БЫТЬ КРАТНО
              // 3)
    case 39:
      Strobe(0xff, 0xff, 0xff, 10, thisdelay, 1000);
      break;  // стробоскоп
  }
}
