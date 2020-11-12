#include "Arduino.h"
#include "wifi.h"
#include "config.h"
#include "sensor.h"

Sensor* dht;

void setup()
{
  Serial.begin(9600);
  delay(10);

  dht = new Sensor(DHT_PIN);
  
  Serial.println("\nConnecting WiFi...");
  Serial.print("Success: ");
  Serial.println(initWiFi());
}

// need to get away from loop into interrupts for lower power consumption
void loop()
{
  delay(60000);

  if(dht->read(DHT_RETRIES)) {
    Serial.print("Relative Humidity: ");
    Serial.print(dht->relative_humidity());
    Serial.println("%");

    Serial.print("Temperature: ");
    Serial.print(dht->temperature());
    Serial.println("C");
  }
}
