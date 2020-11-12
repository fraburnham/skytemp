#include "Arduino.h"
#include "wifi.h"

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.println("\nConnecting WiFi...");
  Serial.print("Success: ");
  Serial.println(initWiFi());
}

void loop()
{
  delay(1000);
}
