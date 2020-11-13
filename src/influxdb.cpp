#include "influxdb.h"

InfluxDB::InfluxDB(String location, String hostname, int port, String database) {
  _location = location;
  _hostname = hostname;
  _port = port;
  _db = database;
}

bool InfluxDB::writeLine(String line) {
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

bool InfluxDB::writeError(String err) {
  return writeLine("error,location=" + _location + " message=\"" + err + "\"");
}
