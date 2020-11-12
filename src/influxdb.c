// this will know how to write to influxdb
// a top level fn will get called w/ a temp and humidity value (probs in a diff file)
// and fns in here will be used to write to influxdb

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "influxdb.h"

const char** metric_to_name = {"temperature", "humidity"};
const char** metric_to_field = {"C", "RH"};

bool writeInflux(char* line) {
  return false;
}

bool writeMetric(MetricType metric, int value) {
  // build a string like
  // <metric> location=<DEVICE_LOCATION> <metric-field-name>=<value>
  // and make an http request to influx db

  char* line_fmt = "%s location=%s %s=%d";
  static char line[128];
  
  os_sprintf(line,
	     line_fmt,
	     metric_to_name[metric],
	     DEVICE_LOCATION,
	     metric_to_field[metric],
	     value);
  
  return writeInflux(line);bbmaster
}
