#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_config.h"
#include "DHT.h"

#define TIMER_PERIOD_MS          5000
#define BIT_RATE_9600            9600
#define DATA_PIN                 BIT5

static os_timer_t temp_check_os_timer;

void temp_check_timer(void *arg) {
  DHTData *data = (DHTData*)read_dht(DATA_PIN);
  
  os_printf("Temp: %d.%d\n", data->temp, data->temp_frac);
  os_printf("RH: %d.%d\n", data->humidity, data->humidity_frac);

  // temp adjustment logic will go in here (and call out to relay.h or something
  // relay.h needs to know about the fan, a/c, and heater (rather I'll need multiple relays or something)
  // for now I should set up leds (or the logic analyzer) and just watch the output
}

uint32 ICACHE_FLASH_ATTR user_rf_cal_sector_set(void) {
  return 0;
}

void ICACHE_FLASH_ATTR user_init() {
  uart_init(BIT_RATE_9600, BIT_RATE_9600);

  gpio_init();
  // setup temp sensor pins
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);  // data
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);  // trigger
  gpio_output_set(BIT4, 0, BIT4, 0);

  // start periodic temp check
  os_timer_disarm(&temp_check_os_timer);
  os_timer_setfn(&temp_check_os_timer, (os_timer_func_t *)temp_check_timer, NULL);
  os_timer_arm(&temp_check_os_timer, TIMER_PERIOD_MS, 1);
}
