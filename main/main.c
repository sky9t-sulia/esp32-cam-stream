#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_camera.h"
#include "esp_camera_helper.h"

#include "include/wifi.h"
#include "include/server.h"
#include "include/sdmmc.h"
#include "ssd1306.h"
#include <string.h>

static WIFI_t wifi;
SSD1306_t dev;

void ui_connect_task(void *pvParameter)
{
    SSD1306_t dev = *(SSD1306_t *)pvParameter;

    ssd1306_clear_screen(&dev, false);
    ssd1306_contrast(&dev, 0xff);

    int delay = 60 * 5; // 5 minutes
    int ssid_length = strlen(CONFIG_WIFI_SSID);
    int previous_state = -1; // -1 indicates no previous state

    while (true)
    {
        int current_state = wifi.connected ? 1 : (wifi.failed ? 2 : 0);

        if (current_state != previous_state)
        {
            ssd1306_clear_screen(&dev, false);

            if (current_state == 0)
            {
                ssd1306_display_text(&dev, 0, "Connecting to", 13, false);
                ssd1306_display_text(&dev, 1, CONFIG_WIFI_SSID, ssid_length, false);
                ssd1306_display_text(&dev, 2, "Please wait", 11, false);
            } 
            else if (current_state == 1)
            {
                ssd1306_display_text(&dev, 0, "Connected to", 12, false);
                ssd1306_display_text(&dev, 1, CONFIG_WIFI_SSID, ssid_length, false);
                ssd1306_display_text(&dev, 3, wifi.ip, strlen(wifi.ip), false);
            }
            else if (current_state == 2)
            {
                ssd1306_display_text(&dev, 0, "WiFi Fail.", 10, false);
                ssd1306_display_text(&dev, 2, CONFIG_WIFI_SSID, ssid_length, false);
            }

            previous_state = current_state;
        }

        delay--;

        // Clear the screen after 5 minutes to prevent burn-in
        // and to save power
        if (delay == 0)
        {
            ssd1306_clear_screen(&dev, false);
            break;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void server_task(void *pvParameter)
{
    while (true)
    {
        if (wifi.connected)
        {
            setup_server();
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void setup_task(void *pvParameter)
{
    // Initialize the oled display
    i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&dev, 128, 32);

    // Initialize the camera
    if (init_camera() != ESP_OK)
    {
        ssd1306_clear_screen(&dev, false);
        ssd1306_display_text(&dev, 0, "Camera fail", 11, false);
        vTaskDelete(NULL);
    }

    // Set the default camera configuration
    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, FRAMESIZE_SVGA);
    sensor->set_pixformat(sensor, PIXFORMAT_JPEG);
    sensor->set_quality(sensor, 5);

    // Initialize the SD card
    if (init_sdmmc() != ESP_OK)
    {
        ssd1306_clear_screen(&dev, false);
        ssd1306_display_text(&dev, 0, "SD Card fail", 12, false);
        vTaskDelete(NULL);
    }

    while (true)
    {
        if (wifi.connected)
        {
            xTaskCreate(&server_task, "server_task", 8192 * 4, NULL, 5, NULL);
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    vTaskDelete(NULL);
}

void app_main()
{
    // Create a task to setup the camera, sd card, and server
    xTaskCreate(&setup_task, "setup_task", 8192 * 2, NULL, 5, NULL);

    // Initialize wifi
    wifi_connect(&wifi);

    // Create a task to display the connection status
    xTaskCreate(&ui_connect_task, "ui_connect_task", 4096, &dev, 5, NULL);
}