#ifndef WIFI_HANDLER_H_
#define WIFI_HANDLER_H_

#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_log.h"
#include "freertos/event_groups.h"

extern int wifi_connect_status;
void connect_wifi(void);

#endif /* WIFI_HANDLER_H_ */