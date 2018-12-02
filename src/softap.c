#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "user_config.h"

bool ICACHE_FLASH_ATTR
start_softap() {
  if(!wifi_set_opmode(SOFTAP_MODE)) {
    if(LOG) {
      os_printf("Failed to set wifi opmode!!\n");
    }
    return false;
  }
  
  struct softap_config config = {  // all of the config should be passable or overrideable
    .authmode = AUTH_WPA2_PSK,
    .password = SOFTAP_PASS,
    .ssid = SOFTAP_SSID,
    .ssid_len = os_strlen(SOFTAP_SSID),
    .channel = 2,
    .max_connection = 1
  };
  
  if(!wifi_softap_set_config(&config)) {
    if(LOG) {
      os_printf("Failed to set softap config!\n");
    }
    return false;
  }

  return true;
}

