#ifndef CONFIG_H
#define CONFIG_H

const int DHT_PIN = 5;
const int DHT_RETRIES = 5;
const String LOCATION = "living-room";
const String INFLUX_HOST = "docker.skynet";
const int INFLUX_PORT = 8086;
const String INFLUX_DB = "skynet";
const int READ_DELAY_uS = 90000;

#endif
