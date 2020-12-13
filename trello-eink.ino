#include <EEPROM.h>
#include "canvas.hpp"
#include <GDBStub.h>
//#define RESET

#define SLEEP_PERIOD_MIN 60
#define CYCLES 1

#ifdef RESET
#define CURRENT_CYCLES 0
#endif

ADC_MODE(ADC_VCC);
unsigned long start;
char i;

void init_spi() {
    pinMode(CS_PIN  , OUTPUT);
    pinMode(RST_PIN , OUTPUT);
    pinMode(DC_PIN  , OUTPUT);
    pinMode(BUSY_PIN,  INPUT);
    SPI.begin();
}

volatile int ManualStart = 0;

void setup(void) {
    start = millis();
    EEPROM.begin(4);
    Serial.begin(9600);
    gdbstub_init();
#ifdef RESET
    Serial.begin(9600);
    EEPROM.write(0, CURRENT_CYCLES);
    EEPROM.commit();
    Serial.printf("Written %d at 0\n", CURRENT_CYCLES);
#else
    i = EEPROM.read(0);
    if(i%CYCLES)
    {
        Serial.printf("Going to sleep, cycle count is: %d\n", i);
        EEPROM.write(0, i+1);
        EEPROM.commit();
        ESP.deepSleep((SLEEP_PERIOD_MIN * 60 * 1000 - (millis() - start)) * 1000);
    }
    EEPROM.write(0, 1);
    EEPROM.commit();
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    init_spi();
#endif
}

void loop(void) {
  if(ManualStart) {
#ifdef RESET
    Serial.println(".");
    delay(100);
#else
    Serial.println("Creating canvas");
    Canvas *canvas = new Canvas();
    canvas->draw();
    delete canvas;
    WiFi.disconnect();
    EEPROM.write(0, i+1);
    EEPROM.commit();
    ESP.deepSleep((SLEEP_PERIOD_MIN * 60 * 1000 - (millis() - start)) * 1000);
#endif
  }
}
