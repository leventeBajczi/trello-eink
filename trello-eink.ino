#include "epd.h"
#include "geometry.h"
#include "adapter.h"

ADC_MODE(ADC_VCC);
const int MIN_VCC = 1998;
const int MAX_VCC = 2496;

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

#define MEASUREMENTS 3

void draw_status_line()
{
  char* text = get_last_update();
  for(int i = 0; i < 400*300/8; ++i)
  {
    black[i] = 255;
    yellow[i] = 255;
  } 
  draw_rectangle(black, 0, 0, 399, 22, true, true);
  draw_textbox(black, text, 0, 2, 350, 22);
  double measurement = 0;
  for(int i = 0; i < MEASUREMENTS; ++i)
  {
    measurement += ESP.getVcc()*1.0/MEASUREMENTS;
    delay(50);
  }
  char vcc[5];
  sprintf(vcc, "%3d%%", (int)100*(ESP.getVcc()-MIN_VCC)/(MAX_VCC-MIN_VCC));
  draw_textbox(black, vcc, 358, 2, 398, 22);
    
}

void draw_todo()
{
  draw_rectangle(black, 0, 22, 199, 299, true, true);
  draw_textbox(black, "ToDo", 66, 30, 132, 70, false, false, 2);
  for(int i = 0; i < 3; ++i)
  {
    char* text = get_text(i);
    if(text == NULL) break;
    draw_rectangle(black, 5, 75+75*i, 194, 75+75*i+70);
    draw_rectangle(yellow, 5, 75+75*i,  194, 75+75*i+70);
    draw_textbox(yellow, text, 5, 75+75*i, 194, 75+75*i+70, true);
    char* time = get_time(i);
    int length;
    if((length = strlen(time)) == 1)
    {
      draw_circle(yellow, 182, 75+75*i+58, 10, true);
      draw_circle(black, 182, 75+75*i+58, 10, true);
      draw_textbox(black, time, 177, 75+75*i+49, 188, 75+75*i+77);
    }
    else if(strchr(time, ':'))
    {
      draw_rectangle(yellow, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68, true);
      draw_rectangle(black, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68, true);
      draw_textbox(black, time, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68);
    }
  }
}

void draw_doing()
{
  draw_rectangle(black, 200, 22, 399, 299, true, true);
  draw_textbox(black, "Doing", 256, 30, 338, 70, false, false, 2);
  for(int i = 0; i < 3; ++i)
  {
    char* text = get_text(i);
    if(text == NULL) break;
    draw_rectangle(black, 205, 75+75*i, 394, 75+75*i+70);
    draw_rectangle(yellow, 205, 75+75*i,  394, 75+75*i+70);
    draw_textbox(yellow, text, 205, 75+75*i, 394, 75+75*i+70, true); 
  }
}

void setup(void) {
  Serial.begin(9600);
  init_spi();
  for(int i = 0; i < 400*300/8; ++i)
  {
    black[i] = 255;
    yellow[i] = 255;
  }
  init_client();
}

void loop(void) {
    
  draw_status_line();
  draw_todo();
  draw_doing();
  show_image();
  delay(1000);
}
