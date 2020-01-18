#include <EEPROM.h>
#include "canvas.hpp"

#define SLEEP_PERIOD_MIN 60

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
}

void loop(void) {
    unsigned long start = millis();
    Canvas *canvas = new Canvas();
    canvas->draw();
    delete canvas;
    WiFi.disconnect();
    ESP.deepSleep((SLEEP_PERIOD_MIN * 60 * 1000 - (millis() - start)) * 1000);
}
