#ifndef CONFIG_H
#define CONFIG_H

const bool DEBUG = true;
const int DHT_PIN = 5;
const int DHT_RETRIES = 5;
const String LOCATION = "living-room";
const String INFLUX_HOST = "docker.skynet";
const int INFLUX_PORT = 8086;
const String INFLUX_DB = "skynet";
const int READ_DELAY_S = 60;

#endif
