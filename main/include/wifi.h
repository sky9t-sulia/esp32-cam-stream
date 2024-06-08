#ifndef WIFI_H_
#define WIFI_H_

#include <nvs_flash.h>
#include "esp_wifi.h"
#include "esp_log.h"

class WiFiConnect
{
    private:
        const char *tag = "WIFI";
        const char *ssid;
        const char *password;
        int retry_num;
        bool connected;

        void trigger_connect(const char *ip);
        void trigger_disconnect(const char *message);

        static void handle_start(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
        static void handle_connect(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
        static void handle_disconnect(void *arg, esp_event_base_t event_base,
                                    int32_t event_id, void *event_data);
    public:
        WiFiConnect(const char *ssid, const char *password);
        void (*on_connect)(const char *ip);
        void (*on_disconnect)(const char *message);
        void set_retry(int retry);
        void connect();
};

#endif /* WIFI_H_ */