#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "DHT.h"

#define LOG                      true
#define BIT_RATE_9600            9600

#define ADJUSTED_MIN_TEMP_C      200  // 20.0C
#define ADJUSTED_MAX_TEMP_C      250  // 25.0C
#define TEMP_CHECK_PERIOD_MINS   5
#define TEMP_TASK_PRIO           0
#define TEMP_TASK_QUEUE_LEN      1
#define TIMER_PERIOD_MS          60000

#define AIRCON_PIN               BIT12
#define DATA_PIN                 5  // BIT5
#define FAN_PIN                  BIT14
#define HEAT_PIN                 BIT13

static os_timer_t temp_check_os_timer;
os_event_t temp_task_queue[TEMP_TASK_QUEUE_LEN];

int periods = 0;

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

void ICACHE_FLASH_ATTR
temp_actions(os_event_t *events) {
  DHTData *data = (DHTData*)read_dht(DATA_PIN);

  if(LOG) {
    os_printf("Temp: %d.%d\t", data->temp, data->temp_frac);
    os_printf("Humidity: %d.%d\n", data->humidity, data->humidity_frac);
  }

  int adjusted_temp = (data->temp * 10) + data->temp_frac;

  // consider requiring a second read 5s later that must also be low before triggering
  // maybe do both reads and avg the values then compare

  if(adjusted_temp > ADJUSTED_MAX_TEMP_C) {
    start_aircon_cycle();
  } else if (adjusted_temp < ADJUSTED_MIN_TEMP_C) {
    start_heat_cycle();
  } else {
    end_climate_cycle();
  }
}

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void) {
  return 0;
}

void ICACHE_FLASH_ATTR
user_init() {
  if(LOG) {
    uart_init(BIT_RATE_9600, BIT_RATE_9600);
  }

  gpio_init();
  // setup temp sensor pins (find PERIPHS_IO_MUX in eagle_soc.h)
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);  // dht

  // setup hvac control pins
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14); // fan
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12); // a/c
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13); // heat

  // start periodic temp check
  os_timer_disarm(&temp_check_os_timer);
  os_timer_setfn(&temp_check_os_timer, (os_timer_func_t *)temp_check_timer, NULL);
  os_timer_arm(&temp_check_os_timer, TIMER_PERIOD_MS, 1);

  // setup task to listen for data ready from timer
  system_os_task(temp_actions, TEMP_TASK_PRIO, temp_task_queue, TEMP_TASK_QUEUE_LEN);
}
