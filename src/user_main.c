#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"
#include "user_config.h"
#include "driver/uart.h"
#include "temp.h"  // this seems like a layer crossing sign

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void) {
  return 0;
}

void ICACHE_FLASH_ATTR
post_init_setup() {
  if(!start_server()) { return; }
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

  // config connection w/ wifi ap
  start_station();

  // setup callback to init server
  system_init_done_cb(&post_init_setup);  // lots of setup should move here probably
}
