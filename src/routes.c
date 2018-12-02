#include "string.h"
#include "osapi.h"
#include "c_types.h"
#include "HTTPTypes.h"
#include "HTTPServer.h"
#include "DHT.h"
#include "temp.h"

Response * ICACHE_FLASH_ATTR
status_route(Request *request) {
  DHTData *data = dht_last_read();

  char *body_fmt = "<html>"
    "<h3>Temp C: %d.%d</h3><br />"
    "<h3>Relative Humidity %: %d.%d</h3><br /><br />"
    "Current min temp: %d, max: %d<br />"
    "<form action='/temps'>"
    "Enter temps in deci degrees C (C*10)<br />"
    "Min: <input type='text' name='min' /><br />"
    "Max: <input type='text' name='max' /><br />"
    "<input type='submit' value='Update' />"
    "</form>"
    "</html>";
  static char body[512];

  if(data) {
    os_sprintf(body, body_fmt, data->temp, data->temp_frac, data->humidity,
	       data->humidity_frac, min_temp(), max_temp());
  } else {
    os_sprintf(body, body_fmt, 0, 0, 0, 0, min_temp(), max_temp());
  }

  static Response response = {
    .status = 200,
    .body = body,
    .content_type = "text/html"
  };
  
  return &response;
}

Response * ICACHE_FLASH_ATTR
adjust_temps_route(Request *request) {
  KeyValuePair *params = request->params;
  int min = ADJUSTED_MIN_TEMP_C, max = ADJUSTED_MAX_TEMP_C; // not really ideal, but crash resistant for now
  
  while(params && params->key) {
    if(strcmp(params->key, "min") == 0) {
      min = atoi(params->value);
    } else if(strcmp(params->key, "max") == 0) {
      max = atoi(params->value);
    }
    
    params++;
  }

  adjust_temps(min, max);

  char *body_fmt = "<html>New temp degrees dC min: %d max: %d</html>";
  static char body[64];

  os_sprintf(body, body_fmt, min, max);

  static Response response = {
    .status = 200,
    .body = body,
    .content_type = "text/html"
  };

  return &response;
}

// add enable/disable routes for fan/heat/ac (start w/ fan)

Route routes[] = {
  {
    .method = GET,
    .path = "/",
    .route_fn = status_route
  },
  {
    .method = GET,
    .path = "/temps",
    .route_fn = adjust_temps_route
  }
};

int route_count = 2;
