#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoOTA.h>
#include "epd.h"
#include "geometry.h"

ADC_MODE(ADC_VCC);
const int MIN_VCC = 1998;
const int MAX_VCC = 2496;

const char* ssid = "Router";
const char* password = "19670227";

void connect_wifi() {
  WiFi.hostname("eink-display");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(10); }
}

void init_spi() {
  pinMode(CS_PIN  , OUTPUT);
  pinMode(RST_PIN , OUTPUT);
  pinMode(DC_PIN  , OUTPUT);
  pinMode(BUSY_PIN,  INPUT);
  SPI.begin();
}

void show_image() {
  EPD_Init_4in2b();

  for(int i = 0; i < 400*300/8; ++i)
    EPD_loadA(black[i]);

  int code = 0x13;
  EPD_SendCommand(code);
  delay(2);
  
  for(int i = 0; i < 400*300/8; ++i)
    EPD_loadA(yellow[i]);

  EPD_showB();
}

char *str1 = "Far out in the uncharted backwaters of the unfashionable end of the Western Spiral arm of the Galaxy lies a small unregarded yellow sun. Orbiting this at a distance of roughly ninety-eight million miles is an utterly insignificant little blue-green planet whose ape-descended life forms are so amazingly primitive that they still think digital watches are a pretty neat idea.";
char *str2 = "The man in black fled across the desert, and the gunslinger followed.";

void setup(void) {
  init_spi();
  for(int i = 0; i < 400*300/8; ++i)
  {
    black[i] = 255;
    yellow[i] = 255;
  }
  draw_textbox(black, str2, 10, 20, 390, 60);
  draw_rectangle(yellow, 10, 100, 390, 290);
  draw_textbox(yellow, str1, 10, 100, 390, 290, true); 
}

void loop(void) {
  connect_wifi();
  delay(1000);
  WiFi.disconnect();
  char vcc[5];
  sprintf(vcc, "%3d%%", (int)100*(ESP.getVcc()-MIN_VCC)/(MAX_VCC-MIN_VCC));
  draw_rectangle(black, 362, 2, 398, 18, true);
  draw_textbox(black, vcc, 362, 2, 398, 18);
  show_image();
}
