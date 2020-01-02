#ifndef ADAPTER_H
#define ADAPTER_H
#define HTTP_DEBUG

#include <ArduinoJson.h>
#include <RestClient.h>

RestClient client = RestClient("worldtimeapi.org");

void init_client()
{
  client.begin("Router", "19670227");
}
  
char* get_last_update()
{
  Serial.println(ESP.getFreeHeap());
  String response;
  int httpCode;
  if ((httpCode = client.get("/api/timezone/CET", &response)) == 200)
  {
    StaticJsonDocument<512> timejson;
    deserializeJson(timejson, response);
    char* ret = "Last update on: 0000. 00. 00. 00:00";
    int y, m, d, h, mi;
    sscanf(timejson["datetime"], "%d-%d-%dT%d:%d", &y, &m, &d, &h, &mi);
    sprintf(ret, "Last update on: %04d. %02d. %02d. %02d:%02d", y, m, d, h, mi);
    return ret;
  }
  else
  {
    char* ret = "Time server response code: 000";
    sprintf(ret, "Time server response code: %03d", httpCode);
    return ret;
  }
}

char* get_text(int i)
{
  return "Informatikai rendszertervezes vizsga";
}

char* get_time(int i)
{
  switch (i)
  {
    case 0: return "2";
    case 1: return "11";
    case 2: return "08:30";
  }
}

#endif
