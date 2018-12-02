#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "user_config.h"
#include "DHT.h"
#include "temp.h"

static os_timer_t temp_check_os_timer;
os_event_t temp_task_queue[TEMP_TASK_QUEUE_LEN];

int periods = 0;
DHTData *_last_read;

int _adjusted_max_temp_c = ADJUSTED_MAX_TEMP_C;
int _adjusted_min_temp_c = ADJUSTED_MIN_TEMP_C;

void
temp_check_timer(void *arg) {
  periods++;

  if(periods == TEMP_CHECK_PERIOD_MINS) {
    periods = 0;
    system_os_post(TEMP_TASK_PRIO, 0, NULL);
  }
}

void ICACHE_FLASH_ATTR
start_heat_cycle() {
  if(LOG) {
    os_printf("Starting heat cycle\n");
  }

  gpio_output_set(HEAT_PIN, AIRCON_PIN, HEAT_PIN|AIRCON_PIN, 0);
}

void ICACHE_FLASH_ATTR
start_aircon_cycle() {
  if(LOG) {
    os_printf("Starting air cycle\n");
  }
  
  gpio_output_set(AIRCON_PIN, HEAT_PIN, AIRCON_PIN|HEAT_PIN, 0);
}

void ICACHE_FLASH_ATTR
end_climate_cycle() {
  if(LOG) {
    os_printf("Ending climate cycle\n");
  }

  gpio_output_set(0, AIRCON_PIN|HEAT_PIN, AIRCON_PIN|HEAT_PIN, 0);
}

int ICACHE_FLASH_ATTR
min_temp() {
  return _adjusted_min_temp_c;
}

int ICACHE_FLASH_ATTR
max_temp() {
  return _adjusted_max_temp_c;
}

void ICACHE_FLASH_ATTR
adjust_temps(int new_adjusted_min, int new_adjusted_max) {
  if(LOG) {
    os_printf("Adjusting temp to min: %d and max: %d\n", new_adjusted_min, new_adjusted_max);
  }

  _adjusted_max_temp_c = new_adjusted_max;
  _adjusted_min_temp_c = new_adjusted_min;
}

void ICACHE_FLASH_ATTR
temp_actions(os_event_t *events) {
  DHTData *data = (DHTData*)read_dht(DATA_PIN);
  _last_read = data;

  if(LOG) {
    os_printf("Temp: %d.%d\t", data->temp, data->temp_frac);
    os_printf("Humidity: %d.%d\n", data->humidity, data->humidity_frac);
  }

  int adjusted_temp = (data->temp * 10) + data->temp_frac;

  // consider requiring a second read 5s later that must also be low before triggering
  // maybe do both reads and avg the values then compare

  if(adjusted_temp > max_temp()) {
    start_aircon_cycle();
  } else if (adjusted_temp < min_temp()) {
    start_heat_cycle();
  } else {
    end_climate_cycle();
  }
}

// should move pin setup here

DHTData *  ICACHE_FLASH_ATTR
dht_last_read() {
  // this should offer some protection by returning a copy
  return _last_read;
}
