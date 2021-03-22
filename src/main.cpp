#include "Arduino.h"
#include "wifi.h"
#include "config.h"
#include "influxdb.h"
#include "ESP8266TimerInterrupt.h"
#include "DHT.h"

volatile int trigger_count = 0;
volatile bool data_ready = false;
volatile bool data_error = false;
volatile float temp_C = 0;
volatile float relative_humidity = 0;

DHT11 sensor;
ESP8266Timer timer;
InfluxDB* idb;

void ICACHE_RAM_ATTR sensor_timer_handler() {
  trigger_count++;

  if(trigger_count >= READ_DELAY_S) {
    data_ready = false;
    trigger_count = 0;
    sensor.read();
  }
}

void ICACHE_RAM_ATTR handle_read(float _relative_humidity, float _temp_C) {
  relative_humidity = _relative_humidity;
  temp_C = _temp_C;
  data_ready = true;
}

void ICACHE_RAM_ATTR handle_error(int err) {
  data_error = true;
}

void setup()
{
  if(DEBUG) {
    Serial.begin(9600);
    delay(10);
  }

  sensor.setup(DHT_PIN);
  sensor.onData(handle_read);
  sensor.onError(handle_error);

  if(DEBUG) {
    Serial.println("\nConnecting WiFi...");
  }

  int wifi_result = initWiFi(); // make fn naming consistent...

  if(DEBUG) {
    Serial.printf("Success: %s\n", wifi_result ? "yes" : "no");
  }

  idb = new InfluxDB(LOCATION, INFLUX_HOST, INFLUX_PORT, INFLUX_DB);
  
  // timer stops every 1s and the ISR will sort out how many s need to pass
  if(timer.attachInterruptInterval(1000000, sensor_timer_handler) && DEBUG) {
    Serial.println("Timer interrupt attached");
  } else {
    idb->writeError("Timer interrupt failed to attach!");
    
    if (DEBUG) {
      Serial.println("Timer interrupt failed to attach!");
    }
  }
}

// need to get away from loop into interrupts for lower power consumption
void loop() {
  if(DEBUG) {
    Serial.printf("data_ready: %s\tdata_error: %s\ttrigger_count: %d\n", (data_ready ? "true" : "false"), (data_error ? "true" : "false"), trigger_count);
  }
  
  if(data_ready) {
    data_ready = false;

    if(DEBUG) {
      Serial.printf("Relative Humidity: %f\n", relative_humidity);
      Serial.printf("Temperature: %f\n", temp_C);
    }

    bool idb_temp_success = idb->writeTemperature(temp_C);
    bool idb_rh_success = idb->writeHumidity(relative_humidity);

    if(DEBUG) {
      Serial.printf("Storing temp success: %d\n", idb_temp_success);
      Serial.printf("Storing humidity success: %d\n", idb_rh_success);
    }
  }

  if(data_error) {
    data_error = false;

    idb->writeError("Failed to read climate data");
    
    if(DEBUG) {
      Serial.println("Failed to read climate data");
    }
  }

  delay(1000);
}
