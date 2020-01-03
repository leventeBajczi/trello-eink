#include "geometry.hpp"

ADC_MODE(ADC_VCC);

void init_spi() {
  pinMode(CS_PIN  , OUTPUT);
  pinMode(RST_PIN , OUTPUT);
  pinMode(DC_PIN  , OUTPUT);
  pinMode(BUSY_PIN,  INPUT);
  SPI.begin();
}

void setup(void) {
  init_spi();
}

void loop(void) {
  Canvas *canvas = new Canvas();
  canvas->draw();
  delete canvas;
  delay(1000);  
}
