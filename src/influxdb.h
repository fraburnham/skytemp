#ifndef INFLUXDB_H
#define INFLUXDB_H

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

class InfluxDB {
 private:
  String _location;
  String _hostname;
  int _port;
  String _db;

  WiFiClient wifi;
  HTTPClient http;
  
  bool writeLine(String);

 public:
  InfluxDB(String, String, int, String);
  bool writeTemperature(float);
  bool writeHumidity(float);
};

#endif
