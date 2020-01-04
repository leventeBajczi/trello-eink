#ifndef ADAPTER_HPP
#define ADAPTER_HPP

#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

#include "secrets.h"
#include "data.hpp"

const char* ssid = SSID;
const char* password = PWD;


void connect_wifi()
{
  if(WiFi.status() != WL_CONNECTED)
  {
    WiFi.hostname("eink");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
  }
}

struct TimeRequest{
  short y = 0;
  byte m = 0;
  byte d = 0;
  byte h = 0;
  byte mi = 0;
  short httpCode;
  TimeRequest(){}
  TimeRequest(const TimeRequest& obj)
  {
    y = obj.y;
    m = obj.m;
    d = obj.d;
    h = obj.h;
    mi = obj.mi;
    httpCode = obj.httpCode;
  }
};

class Time
{
private:
    static const char* connection;
    TimeRequest time;
    TimeRequest utc;
    static Time* instance;
public:
    Time()
    {
        connect_wifi();
        HTTPClient http;
        http.begin(connection);
        time.httpCode = http.GET();
        if(time.httpCode == 200)
        {
            const char* response = http.getString().c_str();
            DynamicJsonDocument timejson(512);
            deserializeJson(timejson, response);
            char* datetime = strdup(timejson["datetime"]);
            time.y  = atoi(strtok (datetime,"-T:"));
            time.m  = atoi(strtok(NULL,"-T:"));
            time.d  = atoi(strtok(NULL,"-T:"));
            time.h  = atoi(strtok(NULL,"-T:"));
            time.mi = atoi(strtok(NULL,"-T:"));
            free(datetime);
            datetime = strdup(timejson["utc_datetime"]);
            utc.y  = atoi(strtok (datetime,"-T:"));
            utc.m  = atoi(strtok(NULL,"-T:"));
            utc.d  = atoi(strtok(NULL,"-T:"));
            utc.h  = atoi(strtok(NULL,"-T:"));
            utc.mi = atoi(strtok(NULL,"-T:"));
            free(datetime);
        }  
        http.end();
        instance = this;
    }
    static const Time* getInstance() { return instance; }
    operator const char*() const noexcept
    {
        if(!time.httpCode) return "Unknown error occured!";
        if(time.httpCode != 200) return "Response code: " + time.httpCode;

        char tmp[40];
        sprintf(tmp, "Last update on: %04d. %02d. %02d. %02d:%02d", time.y, time.m, time.d, time.h, time.mi);
        return tmp;
    } 
    int getDiff(const char* date) const
    {
        char* datetime = strdup(date);
        short y  = atoi(strtok (datetime,"-T:")) - utc.y;
        short m  = atoi(strtok(NULL,"-T:")) - utc.m;
        short d  = atoi(strtok(NULL,"-T:")) - utc.d;
        short h  = atoi(strtok(NULL,"-T:")) - utc.h;
        short mi = atoi(strtok(NULL,"-T:")) - utc.mi;
        free(datetime);
        if(y > 0 || m > 0) return 31*24*60;
        return mi+(h + d*24)*60;
    }
};

struct Card
{
  char* timestr = nullptr;
  char* text = nullptr;
  int time = -1;
  Card()
  {
  }
  ~Card()
  {
      free(text);
      free(timestr);
  }
  Card(const Card& obj) :
      text(strdup(obj.text)),
      time(obj.time),
      timestr(strdup(obj.timestr))
  {
  }
  Card(Card&& obj) :
      text(obj.text),
      time(obj.time),
      timestr(obj.timestr)
  {
    obj.text = nullptr;
    obj.timestr = nullptr;
  }
  Card& operator= (Card c)
  {
    swap(text, c.text);
    swap(time, c.time);
    swap(timestr, c.timestr);
    return *this;
  }
  static int cmp(const void* a, const void* b)
  {
      if(((const Card*)a)->time == -1) return 1;
      if(((const Card*)b)->time == -1) return -1;
      return( ((const Card*)a)->time - ((const Card*)b)->time );
  }
};

class CardList
{
private:
    Card* cards;
    short all_cards;
    static const String base;
    static const String lists_start;
    static const String cards_start;
    static const String todo;
    static const String doing;
    static const String lists_end;
    static const String cards_end;
    static const String tail;
public:
    CardList(bool is_todo) :
        cards(nullptr),
        all_cards(-1)
    {
        connect_wifi();
        HTTPClient* lists_http = new HTTPClient;
        lists_http->begin(base + lists_start + (is_todo ? todo : doing) + lists_end + tail, "a");
        short httpCode;
        if((httpCode = lists_http->GET()) == 200)
        {
            DynamicJsonDocument cardlist(512);
            deserializeJson(cardlist, lists_http->getString());
            lists_http->end();
            delete lists_http;
            JsonArray array = cardlist.as<JsonArray>();
            all_cards = array.size();
            cards = new Card[all_cards];
            int i = 0;
            for(JsonVariant v : array) {
                HTTPClient* cards_http = new HTTPClient;
                cards_http->begin(base + cards_start + String{(const char*)v["id"]} + cards_end + tail, "a");
                if((httpCode = cards_http->GET()) == 200)
                { 
                  DynamicJsonDocument cards_doc(512);
                  deserializeJson(cards_doc, cards_http->getString().c_str());
                  cards_http->end();
                  delete cards_http;
                  cards[i].text = strdup(cards_doc["name"]);
                  if(!cards_doc["due"].isNull()) cards[i++].time = Time::getInstance()->getDiff(cards_doc["due"]);
                  else                           cards[i++].time = -1;
                }
                else
                {
                  cards_http->end();
                  delete cards_http;
                }
            }  
        }
        else
        {
            lists_http->end();
            delete lists_http;
        }
        qsort(cards, all_cards, sizeof(Card), Card::cmp);
    }

    ~CardList()
    {
        delete [] cards;
    }
    CardList(const CardList& obj) :
        cards(new Card[obj.all_cards]),
        all_cards(obj.all_cards)
    {
        std::copy(obj.cards, obj.cards+all_cards, cards);
    }
    CardList(CardList&& obj) :
        cards(obj.cards),
        all_cards(obj.all_cards)
    {
        obj.cards = nullptr;
    }
    CardList& operator= (CardList c)
    {
      all_cards = c.all_cards;
      swap(cards, c.cards);
      return *this;
    }
    const char* get_text(byte i) const noexcept
    {
        if(i >= all_cards) return NULL;
        return cards[i].text;
    }

    const char* get_time(byte i) const noexcept
    {
        if(i >= all_cards) return NULL;
        if(cards[i].timestr) return cards[i].timestr;
        int time = cards[i].time;
        if(time == -1) return cards[i].timestr = strdup("");
        if(time < 0) return cards[i].timestr = strdup("!");
        if(time >= 10*24*60) return cards[i].timestr = strdup("+");
        if(time >= 24*60)
        {
            char ret[2];
            sprintf(ret, "%1d", time/24/60);
            return cards[i].timestr = strdup(ret);
        }
        char ret[5];
        sprintf(ret, "%02d:%02d", time/60, time%60);
        return cards[i].timestr = strdup(ret);
    }
};

class Battery
{
private:
    static const byte MEASUREMENTS = 4;
    static const short MIN_VCC = 1998;
    static const short MAX_VCC = 2496;
    const byte percentage;
public:
    Battery() : 
      percentage{retrieve_percentage()}
    {
    }
    const byte retrieve_percentage() const{
        double measurement = 0;
        for(int i = 0; i < MEASUREMENTS; ++i)
        {
            measurement += ESP.getVcc()*1.0/MEASUREMENTS;
            delay(50);
        }
        return 100*(measurement-MIN_VCC)/(MAX_VCC-MIN_VCC);
    }
    operator const char*() const noexcept {
        char vcc[5];
        sprintf(vcc, "%3d%%", (int)percentage);
        return vcc;
    }
 
};
Time* Time::instance = nullptr;
const char* Time::connection = "http://worldtimeapi.org/api/timezone/CET";
const String CardList::base = "https://api.trello.com/1/";
const String CardList::lists_start = "lists/";
const String CardList::cards_start = "cards/";
const String CardList::todo = "5d922cac78768f6bba3b3aec";
const String CardList::doing= "5d924c8a99ef7f3573ee785c";
const String CardList::lists_end = "/cards?fields=id&";
const String CardList::cards_end = "?fields=name,due&";
const String CardList::tail = "key="KEY"&token="TOKEN;
#endif
