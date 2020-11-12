#include "influxdb.h"

InfluxDB::InfluxDB(String location, String hostname, int port, String database) {
  _location = location;
  _hostname = hostname;
  _port = port;
  _db = database;
}

bool InfluxDB::writeLine(String line) {
  // curl -v "http://docker.skynet:8086/api/v2/write?bucket=skynet" --data-raw "temperature,location=mock C=0"
  http.begin(wifi, "http://" + _hostname + ":" + _port + "/api/v2/write?bucket=" + _db);
  int status = http.POST(line);
  http.end();

  return status == 204;
}

bool InfluxDB::writeTemperature(float temp_C) {
  return writeLine("temperature,location=" + _location + " C=" + temp_C);
}

bool InfluxDB::writeHumidity(float relative_humidity) {
  return writeLine("humidity,location=" + _location + " RH=" + relative_humidity);
}
