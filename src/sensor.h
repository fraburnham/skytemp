#ifndef SENSOR_H
#define SENSOR_H

#include "DHT.h"

class Sensor {
private:
  DHT* dht;
  float _relative_humidity;
  float _temp_C;

public:
  Sensor(int);
  bool read(int);
  float relative_humidity();
  float temperature();
};

#endif
