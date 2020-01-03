#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "alphabet.h"

void draw_outline(byte* data, int x0, int y0, int x1, int y1, bool inverse = false)
{
  for(int j = y0; j<=y1; ++j)
    {
      if(inverse)
      {
         data[(j*400+x0)/8] |= (128 >> (byte)((j*400+x0) % 8));
         data[(j*400+x1)/8] |= (128 >> (byte)((j*400+x1) % 8)); 
      }
      else
      {
         data[(j*400+x0)/8] &= ~(128 >> (byte)((j*400+x0) % 8));
         data[(j*400+x1)/8] &= ~(128 >> (byte)((j*400+x1) % 8)); 
      }
    }
  for(int i = x0; i<=x1; ++i)
  {
    if(inverse)
      {
         data[(y0*400+i)/8] |= (128 >> (byte)((y0*400+i) % 8));
         data[(y1*400+i)/8] |= (128 >> (byte)((y1*400+i) % 8)); 
      }
      else
      {
         data[(y0*400+i)/8] &= ~(128 >> (byte)((y0*400+i) % 8));
         data[(y1*400+i)/8] &= ~(128 >> (byte)((y1*400+i) % 8)); 
      } 
  }
}

void draw_textbox(byte* data, const char* str, int x0, int y0, int x1, int y1, bool inverse = false, bool outline = false, int size = 1, int spacing_horizontal = 2, int spacing_vertical = 2, int padding_left = 2, int padding_right = 2, int padding_top = 2, int padding_bottom = 2)
{
  int offset_x = x0+padding_left;
  int offset_y = y0+padding_top;
  char help[] = " ";
  for(int i = 0; i < strlen(str); ++i)
  {
    if(offset_y + HEIGHT*size + padding_bottom > y1) 
      if(outline) draw_outline(data, x0, y0, x1, y1, !inverse);
      else return; 
    if((i == 0 || str[i] == ' ') &&
      (
        (strchr(str+i+1, ' ') && offset_x + (strchr(str+i+1, ' ')-(str+i))*(WIDTH*size+spacing_horizontal) > x1-padding_right+spacing_horizontal)) ||
        ((!strchr(str+i+1, ' ') && offset_x + strlen(str+i)*(WIDTH*size+spacing_horizontal) > x1-padding_right+spacing_horizontal))
      )
    {
      
      offset_x = x0+padding_left;
      offset_y += HEIGHT*size+spacing_vertical;
      continue;
    }
    help[0] = str[i];
    struct character* _char = get_data(help);
    for(int i = 0; i < (_char -> data_length); ++i) {
      if(inverse)
        for(int s1 = 0; s1 < size; ++s1)
        {
          for(int s2 = 0; s2 < size; ++s2)
          {
            data[((offset_y + _char->data[i].y*size+s1)*400 + (offset_x + _char->data[i].x*size+s2))/8] |=  (128 >> (byte)(((offset_y + _char->data[i].y*size+s1)*400 + (offset_x + _char->data[i].x*size+s2)) % 8));    
          }
        }
      else
        for(int s1 = 0; s1 < size; ++s1)
        {
          for(int s2 = 0; s2 < size; ++s2)
          {
            data[((offset_y + _char->data[i].y*size+s1)*400 + (offset_x + _char->data[i].x*size+s2))/8] &=  ~(128 >> (byte)(((offset_y + _char->data[i].y*size+s1)*400 + (offset_x + _char->data[i].x*size+s2)) % 8));    
          }
        } 
    }
    offset_x+=WIDTH*size+spacing_horizontal;
  }
  if(outline) draw_outline(data, x0, y0, x1, y1, !inverse);
}

void draw_rectangle(byte* data, int x0, int y0, int x1, int y1, bool inverse = false, bool outline = false)
{
  for(int i = x0; i<=x1; ++i)
  {
    for(int j = y0; j<=y1; ++j)
    {
      if(inverse)
        data[(j*400 + i)/8] |= (128 >> (byte)((j*400 + i) % 8));
      else
        data[(j*400 + i)/8] &= ~(128 >> (byte)((j*400 + i) % 8));
    }
  }
  if(outline) draw_outline(data, x0, y0, x1, y1, !inverse);
}

void draw_circle(byte* data, int x, int y, int r, bool inverse = false)
{
  for(int i = x-r+1; i<=x+r-1; ++i)
  {
    for(int j = y-r+1; j<=y+r-1; ++j)
    {
      if((i-x)*(i-x)+(j-y)*(j-y) > r*r) continue;
      if(inverse)
        data[(j*400 + i)/8] |= (128 >> (byte)((j*400 + i) % 8));
      else
        data[(j*400 + i)/8] &= ~(128 >> (byte)((j*400 + i) % 8));
    }
  }
}

#endif
