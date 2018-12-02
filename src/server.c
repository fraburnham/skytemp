#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "user_config.h"
#include "HTTPServer.h"
#include "HTTPTypes.h"

bool ICACHE_FLASH_ATTR
start_server() {
  // move some of this to a server config fn or something
  // maybe a `start_server` (it could take the routes and set them up)
  static esp_tcp http_connection = {
    .local_port = 80,
  };
  
  static struct espconn esp_connection = {
    .type = ESPCONN_TCP,
    .proto.tcp = &http_connection,
    .state = ESPCONN_NONE
  };

  if(espconn_regist_connectcb(&esp_connection, &webserver_listen) != 0) {
    if(LOG) {
       os_printf("Failed to register connect callback!\n");
    }
    return false;
  }
  
  if(espconn_accept(&esp_connection) != 0) {
    if(LOG) {
      os_printf("Failed to setup espconn!\n");
    }
    return false;
  }

  return true;
}

// might move this out to `routes.c`
