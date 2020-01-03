#ifndef ADAPTER_HPP
#define ADAPTER_HPP

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "Router";
const char* password = "19670227";

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

class Card
{
private:
    const char* text;
    const char* time;
public:
    Card(byte i) :
      text{retrieve_text()},
      time{retrieve_time(i)}
    {
    }
    const char* retrieve_text() const noexcept { return "Informatikai rendszertervezes vizsga"; }
    const char* retrieve_time(const byte i) const noexcept {
      switch (i)
      {
           case 0: return "2"; 
           case 1: return "11"; 
           default: return "08:30"; 
      }
    }
    const char* get_text() const noexcept
    {
        return text;
    }

    const char* get_time() const noexcept
    {
        return time;
    }
};

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
    const TimeRequest time;
public:
    Time() :
      time{retrieve_time()}
    {
    }
    const TimeRequest retrieve_time() const
    {  
        connect_wifi();
        HTTPClient http;
        TimeRequest ret;
        http.begin(connection);
        ret.httpCode = http.GET();
        if(ret.httpCode == 200)
        {
            const char* response = http.getString().c_str();
            DynamicJsonDocument timejson(512);
            deserializeJson(timejson, response);
            char* datetime = strdup(timejson["datetime"]);
            ret.y  = atoi(strtok (datetime,"-T:"));
            ret.m  = atoi(strtok(NULL,"-T:"));
            ret.d  = atoi(strtok(NULL,"-T:"));
            ret.h  = atoi(strtok(NULL,"-T:"));
            ret.mi = atoi(strtok(NULL,"-T:"));
            free(datetime);
        }  
        http.end();
        return ret;
    }
    operator const char*() const noexcept{
        if(!time.httpCode) return "Unknown error occured!";
        if(time.httpCode != 200) return "Response code: " + time.httpCode;

        char tmp[40];
        sprintf(tmp, "Last update on: %04d. %02d. %02d. %02d:%02d", time.y, time.m, time.d, time.h, time.mi);
        return tmp;
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
const char* Time::connection = "http://worldtimeapi.org/api/timezone/CET";
#endif
