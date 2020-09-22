#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "user_config.h"
#include "secret.h"

bool ICACHE_FLASH_ATTR
start_station() {
  if(!wifi_set_opmode(STATION_MODE)) {
    if(LOG) {
      os_printf("Failed to set wifi opmode!!\n");
    }
    return false;
  }

  struct station_config config;
  // ssid and pass are expected to be defined in the .gitignored `secret.h`
  os_memcpy(&config.ssid, ssid, 32);
  os_memcpy(&config.password, pass, 64);  // oh no big passwords

  if(!wifi_station_set_config(&config)) {  // connection is handled automagically if this is called in `user_init`
    if(LOG) {
      os_printf("Failed to set station config!\n");
    }
    return false;
  }

  return true;
}
