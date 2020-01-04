#include <EEPROM.h>

#include "canvas.hpp"

//#define INIT
#define INITIAL 2
#define SLEEP_PERIOD_MIN 60
#define MULTIPLIER 6

ADC_MODE(ADC_VCC);

void init_spi() {
  pinMode(CS_PIN  , OUTPUT);
  pinMode(RST_PIN , OUTPUT);
  pinMode(DC_PIN  , OUTPUT);
  pinMode(BUSY_PIN,  INPUT);
  SPI.begin();
}

void setup(void) {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  init_spi();
  EEPROM.begin(4);
#ifdef INIT
  EEPROM.write(0, INITIAL);
  EEPROM.commit();
#endif
}

void loop(void) {
  int i = EEPROM.read(0);
  EEPROM.write(0, i + 1);
  EEPROM.commit();
  unsigned long start = millis();
  if (i % MULTIPLIER == 0)
  {
    Canvas *canvas = new Canvas();
    canvas->draw();
    delete canvas;
  }
  WiFi.disconnect();
  ESP.deepSleep((SLEEP_PERIOD_MIN * 60 * 1000 - (millis() - start)) * 1000);
}
