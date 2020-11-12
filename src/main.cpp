#include "Arduino.h"
#include "wifi.h"
#include "config.h"
#include "sensor.h"
#include "influxdb.h"

Sensor* dht;
InfluxDB* idb;

void setup()
{
  Serial.begin(9600);
  delay(10);

  dht = new Sensor(DHT_PIN);
  idb = new InfluxDB(LOCATION, INFLUX_HOST, INFLUX_PORT, INFLUX_DB);
  
  Serial.println("\nConnecting WiFi...");
  Serial.print("Success: ");
  Serial.println(initWiFi());
}

// need to get away from loop into interrupts for lower power consumption
void loop()
{
  delay(READ_DELAY_uS);

  if(dht->read(DHT_RETRIES)) {
    Serial.print("Relative Humidity: ");
    Serial.print(dht->relative_humidity());
    Serial.println("%");

    Serial.print("Temperature: ");
    Serial.print(dht->temperature());
    Serial.println("C");

    Serial.print("Storing temp success: ");
    Serial.println(idb->writeTemperature(dht->temperature()));

    Serial.print("Storing humidity success: ");
    Serial.println(idb->writeHumidity(dht->relative_humidity()));
  }
}
