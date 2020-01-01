#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "alphabet.h"

byte *black  = new byte[400*300/8]; 
byte *yellow = new byte[400*300/8]; 

void draw_textbox(byte* data, char* str, int x0, int y0, int x1, int y1, bool inverse = false)
{
  int offset_x = x0;
  int offset_y = y0;
  char help[] = " ";
  for(int i = 0; i < strlen(str); ++i)
  {
    if(offset_y + HEIGHT + 2 > y1) return; 
    if(str[i] == ' ' &&
      (
        (strchr(str+i+1, ' ') && offset_x + (strchr(str+i+1, ' ')-(str+i))*(WIDTH+2) > x1)) ||
        ((!strchr(str+i+1, ' ') && offset_x + strlen(str+i)*(WIDTH+2) > x1))
      )
    {
      
      offset_x = x0;
      offset_y += HEIGHT+2;
      continue;
    }
    help[0] = str[i];
    struct character* _char = get_data(help);
    for(int i = 0; i < (_char -> data_length); ++i) {
      if(inverse)
        data[((offset_y + _char->data[i].y)*400 + (offset_x + _char->data[i].x))/8] |=  (128 >> (byte)(((offset_y + _char->data[i].y)*400 + (offset_x + _char->data[i].x)) % 8));
      else
        data[((offset_y + _char->data[i].y)*400 + (offset_x + _char->data[i].x))/8] &= ~(128 >> (byte)(((offset_y + _char->data[i].y)*400 + (offset_x + _char->data[i].x)) % 8));  
    }
    offset_x+=WIDTH+2;
  }
}

void draw_rectangle(byte* data, int x0, int y0, int x1, int y1, bool inverse = false)
{
  for(int i = x0; i<x1; ++i)
  {
    for(int j = y0; j<y1; ++j)
    {
      if(inverse)
        data[(j*400 + i)/8] |= (128 >> (byte)((j*400 + i) % 8));
      else
        data[(j*400 + i)/8] &= ~(128 >> (byte)((j*400 + i) % 8));
    }
  }
}
#endif
