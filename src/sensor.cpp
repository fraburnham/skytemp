#include "sensor.h"

Sensor::Sensor(int data_pin) { // dht11 assumed for now
  dht = new DHT(data_pin, DHT11, 0);
}

bool Sensor::read(int retries) { // skipping retries for a bit. The speed of them may be hurting the device. Iirc the docs suggest waiting 60+s between reads
  _relative_humidity = dht->readHumidity();
  _temp_C = dht->readTemperature();

  if(!isnan(_relative_humidity) || !isnan(_temp_C)) return true;

  return false;
}

float Sensor::relative_humidity() { return _relative_humidity; }

float Sensor::temperature() { return _temp_C; }

