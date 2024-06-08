#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "include/wifi.h"
#include "esp_camera.h"
#include "esp_camera_helper.h"

static const char *TAG = "ESP32_CAM";

const char *current_ip;

void display(void *pvParameter)
{
    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
}

void connect(void *pvParameter)
{
    WiFiConnect wifi(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    wifi.set_retry(5);

    wifi.on_connect = [](const char *ip) {
        current_ip = ip;

        ESP_LOGI(TAG, "Connected to AP. Got IP: %s", ip);
    };

    wifi.on_disconnect = [](const char *message) {
        ESP_LOGI(TAG, "Disconnected from AP. Reason: %s", message);
    };

    wifi.connect();

    while (true)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    
    vTaskDelete(NULL);
}

extern "C" void app_main()
{
    xTaskCreate(&connect, "wifi", 4096, NULL, 5, NULL);
    xTaskCreate(&display, "display", 4096, NULL, 5, NULL);
}