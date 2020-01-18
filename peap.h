#ifndef PEAP_H
#define PEAP_H

/*
 * This header requires a modified umm_malloc implementation! 
 * Disable poison checking.
 */

#include <Arduino.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include <wpa2_enterprise.h>


bool connect() {
  wifi_set_opmode(STATION_MODE);
  struct station_config wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  wifi_station_set_config(&wifi_config);
  wifi_station_dhcpc_start();
  wifi_station_clear_cert_key();
  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));
  wifi_station_set_enterprise_ca_cert((byte*)ca_cert, strlen(ca_cert));
  wifi_station_connect();
  Ticker timer;
  bool connected = true;
  timer.attach(5, [&](){connected = false;});
  while (WiFi.status() != WL_CONNECTED) {
    delay(10);
  }
  timer.detach();
  return connected;
}

void connectBlock(){
  while(!connect()){ 
    delay(100);
  }
}

#endif