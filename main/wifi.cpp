#include "include/wifi.h"
#include <cstring>

#define MAXIMUM_RETRY 10

WiFiConnect::WiFiConnect(const char *ssid, const char *password)
{
    this->ssid = ssid;
    this->password = password;
    this->retry_num = 0;

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_start;
    esp_event_handler_instance_t instance_connected;
    esp_event_handler_instance_t instance_disconnect;

    // Register the start event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT, WIFI_EVENT_STA_START, &WiFiConnect::handle_start, this, &instance_start
        )
    );

    // Register the connect event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT, IP_EVENT_STA_GOT_IP, &WiFiConnect::handle_connect, this, &instance_connected
        )
    );

    // Register the disconnect event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &WiFiConnect::handle_disconnect, this, &instance_disconnect
        )
    );
}

void WiFiConnect::connect()
{
    wifi_config_t wifi_config;

    assert(ssid[0] != '\0');
    assert(password[0] != '\0');

    strncpy((char*)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, password, sizeof(wifi_config.sta.password));
    
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA_PSK;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(tag, "Trying to connect to AP");
}

void WiFiConnect::handle_start(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    WiFiConnect *wifi = static_cast<WiFiConnect *>(arg);

    ESP_LOGI(wifi->tag, "Start connecting to AP");
    esp_wifi_connect();
}

void WiFiConnect::handle_connect(void *arg, esp_event_base_t event_base,
                                int32_t event_id, void *event_data)
{
    WiFiConnect *wifi = static_cast<WiFiConnect *>(arg);
    ESP_LOGI(wifi->tag, "Connected to AP");

    // Reset the retry number
    wifi->retry_num = 0;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(wifi->tag, "Got IP:" IPSTR, IP2STR(&event->ip_info.ip));

    char ip[16];
    sprintf(ip, IPSTR, IP2STR(&event->ip_info.ip));
    wifi->connected = true;
    wifi->trigger_connect(ip);
}

void WiFiConnect::handle_disconnect(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
{
    WiFiConnect *wifi = static_cast<WiFiConnect *>(arg);
    ESP_LOGI(wifi->tag, "Disconnected from AP");

    if (wifi->retry_num < MAXIMUM_RETRY)
    {
        esp_wifi_connect();
        wifi->retry_num++;
        ESP_LOGI(wifi->tag, "Retry... %d", wifi->retry_num);
    }
    else
    {
        wifi->connected = false;
        wifi->trigger_disconnect("Can't connect to AP. Retry limit reached");
    }

    ESP_LOGI(wifi->tag, "Connect to the AP failed");
}

void WiFiConnect::trigger_connect(const char *ip)
{
    if (on_connect != NULL)
    {
        on_connect(ip);
    }
    else
    {
        ESP_LOGI(tag, "on_connect callback is not set");
    }
}

void WiFiConnect::trigger_disconnect(const char *message)
{
    if (on_disconnect != NULL)
    {
        on_disconnect(message);
    }
    else
    {
        ESP_LOGI(tag, "on_disconnect callback is not set");
    }
}

void WiFiConnect::set_retry(int retry)
{
    retry_num = retry;
}