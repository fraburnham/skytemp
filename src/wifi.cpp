#include <ESP8266WiFi.h>
#include "secret.h"
#include "wifi.h"

bool initWiFi() {
  WiFi.begin(ssid, password);

  for(int i=0; i<WIFI_CONNECT_MAX_RETRIES; i++) {
    delay(1000);
    
    if(WiFi.status() == WL_CONNECTED) return true;
  }

  return false;
}
