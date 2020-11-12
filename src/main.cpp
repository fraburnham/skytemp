#include "Arduino.h"
#include "DHT.h"
#include "wifi.h"
#include "config.h"

DHT dht(DHT_PIN, DHT_TYPE);

void setup()
{
  Serial.begin(9600);
  delay(10);
  Serial.println("\nConnecting WiFi...");
  Serial.print("Success: ");
  Serial.println(initWiFi());
}

// need to get away from loop into interrupts for lower power consumption
void loop()
{
  delay(10000);

  float relative_humidity = dht.readHumidity();
  float temp_C = dht.readTemperature();

  Serial.print("Relative Humidity: ");
  Serial.print(relative_humidity);
  Serial.println("%");

  Serial.print("Temperature: ");
  Serial.print(temp_C);
  Serial.println("C");
}
