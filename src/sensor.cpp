#include "sensor.h"

Sensor::Sensor(int data_pin) { // dht11 assumed for now
  dht = new DHT(data_pin, DHT11, 0);
}

bool Sensor::read(int retries) {
  for(int i=0; i<retries; i++) {
    _relative_humidity = dht->readHumidity();
    _temp_C = dht->readTemperature();

    if(!isnan(_relative_humidity) || !isnan(_temp_C)) return true;
    delay(5000);
  }

  return false;
}

float Sensor::relative_humidity() { return _relative_humidity; }

float Sensor::temperature() { return _temp_C; }

