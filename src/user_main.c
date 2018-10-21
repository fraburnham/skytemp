#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "DHT.h"

#define LOG                      true
#define ADJUSTED_MIN_TEMP_C      195 // 19.5C ~67F
#define ADJUSTED_MAX_TEMP_C      245 // 24.5C ~76F
#define TIMER_PERIOD_MS          5000
#define BIT_RATE_9600            9600
#define DATA_PIN                 BIT5
#define TEMP_TASK_PRIO           0
#define TEMP_TASK_QUEUE_LEN      1

static os_timer_t temp_check_os_timer;

os_event_t temp_task_queue[TEMP_TASK_QUEUE_LEN];

void
temp_check_timer(void *arg) {
  system_os_post(TEMP_TASK_PRIO, 0, NULL);
}

void ICACHE_FLASH_ATTR
temp_actions(os_event_t *events) {
  DHTData *data = (DHTData*)read_dht(DATA_PIN);

  if(LOG) {
    os_printf("Temp: %d.%d\t", data->temp, data->temp_frac);
    os_printf("Humidity: %d.%d\n", data->humidity, data->humidity_frac);
  }

  int adjusted_temp = (data->temp * 10) + data->temp_frac;

  if(adjusted_temp > ADJUSTED_MAX_TEMP_C) {
    if(LOG) {
      os_printf("Would activate A/C\n");
    }
  } else if (adjusted_temp < ADJUSTED_MIN_TEMP_C) {
    if(LOG) {
      os_printf("Would activate heat\n");
    }
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
  // setup temp sensor pins
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);  // data
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);  // trigger
  gpio_output_set(BIT4, 0, BIT4, 0);

  // start periodic temp check
  os_timer_disarm(&temp_check_os_timer);
  os_timer_setfn(&temp_check_os_timer, (os_timer_func_t *)temp_check_timer, NULL);
  os_timer_arm(&temp_check_os_timer, TIMER_PERIOD_MS, 1);

  // setup task to listen for data ready from timer
  system_os_task(temp_actions, TEMP_TASK_PRIO, temp_task_queue, TEMP_TASK_QUEUE_LEN);
}
