#ifndef WIFI_HANDLER_H_
#define WIFI_HANDLER_H_

#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"

extern int wifi_connect_status;
void connect_wifi(void);

#endif /* WIFI_HANDLER_H_ */