#ifndef TEMP_H
#define TEMP_H

#include "os_type.h"
#include "user_config.h"
#include "DHT.h"

static os_timer_t temp_check_os_timer;
extern os_event_t temp_task_queue[TEMP_TASK_QUEUE_LEN];

extern void temp_check_timer(void *);
extern int min_temp();
extern int max_temp();
extern void adjust_temps(int, int);
extern void temp_actions(os_event_t *);
extern DHTData *dht_last_read();

#endif
