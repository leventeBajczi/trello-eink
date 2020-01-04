#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "adapter.hpp"
#include "geometry.h"
#include "epd.h"
#include "data.hpp"


class StatusBar
{
private:
    const Time lastRefreshed;
    const Battery battery;
public:
    void black(DataWrapper<SDEF, NDEF> black) const{
        draw_rectangle(black, 0, 0, 399, 22, true, true);
        draw_textbox(black, lastRefreshed, 0, 2, 350, 22);
        draw_textbox(black, battery, 358, 2, 398, 22);
    } 
    void yellow(DataWrapper<SDEF, NDEF> yellow) const {} 

};

class ToDo
{
private:
    const CardList cardList;
public:
    ToDo() : 
        cardList(true)
    {}
    void black(DataWrapper<SDEF, NDEF> black) const {
        draw_rectangle(black, 0, 22, 199, 299, true, true);
        draw_textbox(black, "ToDo", 66, 30, 132, 70, false, false, 2);

        for(int i = 0; i < 3; ++i)
        {
            const char* text = cardList.get_text(i);
            if(text == NULL) break;
            draw_rectangle(black, 5, 75+75*i, 194, 75+75*i+70);
            const char* time = cardList.get_time(i);
            int length;
            if((length = strlen(time)) == 1)
            {
                draw_circle(black, 182, 75+75*i+58, 10, true);
                draw_textbox(black, time, 177, 75+75*i+49, 188, 75+75*i+77);
            }
            else if(strchr(time, ':'))
            {
                draw_rectangle(black, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68, true);
                draw_textbox(black, time, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68);
            }
        }

    } 
    void yellow(DataWrapper<SDEF, NDEF> yellow) const {

        for(int i = 0; i < 3; ++i)
        {
            const char* text = cardList.get_text(i);
            if(text == NULL) break;
                draw_rectangle(yellow, 5, 75+75*i,  194, 75+75*i+70);
                draw_textbox(yellow, text, 5, 75+75*i, 194, 75+75*i+70, true);
            const char* time = cardList.get_time(i);
            int length;
            if((length = strlen(time)) == 1)
            {
                draw_circle(yellow, 182, 75+75*i+58, 10, true);
            }
            else if(strchr(time, ':'))
            {
                draw_rectangle(yellow, 192-length*(WIDTH+2)-2, 75+75*i+50, 192, 75+75*i+68, true);
            }
        }

    } 
};

class Doing
{
private:
    const CardList cardList;
public:
    Doing() :
        cardList(false)
    {}
    void black(DataWrapper<SDEF, NDEF> black) const{
        draw_rectangle(black, 200, 22, 399, 299, true, true);
        draw_textbox(black, "Doing", 256, 30, 338, 70, false, false, 2);

        for(int i = 0; i < 3; ++i)
        {
            const char* text = cardList.get_text(i);
            if(text == NULL) break;
            draw_rectangle(black, 205, 75+75*i, 394, 75+75*i+70);
            const char* time = cardList.get_time(i);
            int length;
            if((length = strlen(time)) == 1)
            {
                draw_circle(black, 382, 75+75*i+58, 10, true);
                draw_textbox(black, time, 377, 75+75*i+49, 388, 75+75*i+77);
            }
            else if(strchr(time, ':'))
            {
                draw_rectangle(black, 392-length*(WIDTH+2)-2, 75+75*i+50, 392, 75+75*i+68, true);
                draw_textbox(black, time, 392-length*(WIDTH+2)-2, 75+75*i+50, 392, 75+75*i+68);
            }
        }

    } 
    void yellow(DataWrapper<SDEF, NDEF> yellow) const{

        for(int i = 0; i < 3; ++i)
        {
            const char* text = cardList.get_text(i);
            if(text == NULL) break;
                draw_rectangle(yellow, 205, 75+75*i,  394, 75+75*i+70);
                draw_textbox(yellow, text, 205, 75+75*i, 394, 75+75*i+70, true);
            const char* time = cardList.get_time(i);
            int length;
            if((length = strlen(time)) == 1)
            {
                draw_circle(yellow, 382, 75+75*i+58, 10, true);
            }
            else if(strchr(time, ':'))
            {
                draw_rectangle(yellow, 392-length*(WIDTH+2)-2, 75+75*i+50, 392, 75+75*i+68, true);
            }
        }

    } 
};


class Canvas
{
private:
    Data<SDEF, NDEF>* data;
    const StatusBar *statusbar;
    const ToDo *todo;
    const Doing *doing;
public:
    Canvas() :
      statusbar(new StatusBar{}),
      todo(new ToDo{}),
      doing(new Doing{}),
      data(nullptr)
    {
    }
    ~Canvas()
    {
      delete doing;
      delete todo;
      delete statusbar;
    }
    Canvas(const Canvas& obj) :
      statusbar{new StatusBar{*obj.statusbar}},
      todo{new ToDo{*obj.todo}},
      doing{new Doing{*obj.doing}}
    {
    }
    Canvas(Canvas&& obj) noexcept :
      statusbar{obj.statusbar},
      todo{obj.todo},
      doing{obj.doing}
    {
      obj.statusbar = nullptr;
      obj.todo = nullptr;
      obj.doing = nullptr;
    }
    Canvas& operator=(Canvas obj) noexcept
    {
      swap(statusbar, obj.statusbar);
      swap(todo, obj.todo);
      swap(doing, obj.doing);
      return *this;
    }
    
    void draw() {
        EPD_Init_4in2b();

        data = new Data<SDEF, NDEF>;
        DataWrapper<SDEF, NDEF> dw(data);
        for(int i = 0; i < 400*300/8; ++i)
        {
            (dw)[i] = 255;
        } 
        statusbar->black(dw);
        todo->black(dw);
        doing->black(dw);
        for(int i = 0; i < 400*300/8; ++i)
            EPD_loadA((dw)[i]);

        EPD_SendCommand(0x13);
        delay(2);

        for(int i = 0; i < 400*300/8; ++i)
        {
            (dw)[i] = 255;
        }
        statusbar->yellow(dw);
        todo->yellow(dw);
        doing->yellow(dw);
        for(int i = 0; i < 400*300/8; ++i)
            EPD_loadA((dw)[i]);

        EPD_showB();

        delete data;

    } 
};

#endif
