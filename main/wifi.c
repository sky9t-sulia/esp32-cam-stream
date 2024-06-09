#include "include/wifi.h"

static const char *TAG = "WIFI";

/**
 * @brief Event handler for WIFI_EVENT_STA_START event
 * @param arg WIFI_t struct
 * @param event_base Event base
 * @param event_id Event ID
 * @param event_data Event data
 */
void event_start(void *arg, esp_event_base_t event_base,
                 int32_t event_id, void *event_data)
{
    esp_wifi_connect();
}

/**
 * @brief Event handler for WIFI_EVENT_STA_DISCONNECTED event
 * @param arg WIFI_t struct
 * @param event_base Event base
 * @param event_id Event ID
 * @param event_data Event data
 */
void event_disconnect(void *arg, esp_event_base_t event_base,
                      int32_t event_id, void *event_data)
{
    WIFI_t *wifi = (WIFI_t *)arg;

    if (wifi->retry_num < MAXIMUM_RETRY)
    {
        esp_wifi_connect();
        wifi->retry_num++;
        ESP_LOGI(TAG, "Retry to connect to the AP");

        return;
    }

    wifi->failed = true;

    if (wifi->ip != NULL)
    {
        free(wifi->ip);
    }

    ESP_LOGI(TAG, "Connect to the AP failed");
}

/**
 * @brief Event handler for IP_EVENT_STA_GOT_IP event
 * @param arg WIFI_t struct
 * @param event_base Event base
 * @param event_id Event ID
 * @param event_data Event data
 */
void event_connect(void *arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data)
{
    WIFI_t *wifi = (WIFI_t *)arg;

    ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
    ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
    wifi->ip = (char *)malloc(16);
    sprintf(wifi->ip, IPSTR, IP2STR(&event->ip_info.ip));

    wifi->retry_num = 0;
    wifi->connected = true;
}

/**
 * @brief Connect to the WiFi network
 * @param wifi WIFI_t struct
 */
void wifi_connect(WIFI_t *wifi)
{
    wifi->connected = false;
    wifi->failed = false;

    
    // Initialize NVS
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
    esp_event_handler_instance_t instance_disconnect;
    esp_event_handler_instance_t instance_connect;

    // Register event handler for WIFI_EVENT_STA_START event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            WIFI_EVENT_STA_START,
            &event_start,
            wifi,
            &instance_start));

    // Register event handler for WIFI_EVENT_STA_DISCONNECTED event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            WIFI_EVENT_STA_DISCONNECTED,
            &event_disconnect,
            wifi,
            &instance_disconnect));

    // Register event handler for IP_EVENT and STA_GOT_IP event
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_connect,
            wifi,
            &instance_connect));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi Initialized");
}