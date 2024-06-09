#ifndef WIFI_H_
#define WIFI_H_

#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_log.h"

#define MAXIMUM_RETRY 5

typedef struct {
    char *ip;
    bool connected;
    bool failed;
    int retry_num;
    void (*on_connected)(void);
    void (*on_failed)(void);
} WIFI_t;

void wifi_connect(WIFI_t *wifi);

#endif /* WIFI_H_ */