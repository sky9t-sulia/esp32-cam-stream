#include <stdio.h>
#include "wifi_connect.h"
#include "httpd_handler.h"
#include "esp_camera.h"
#include "esp_camera_helper.h"

void app_main(void)
{
    if (init_camera() != ESP_OK) {
        vTaskDelete(NULL);
    }
    
    sensor_t *sensor = esp_camera_sensor_get();

    // max resolution for ov2640
    sensor->set_framesize(sensor, FRAMESIZE_SVGA);
    sensor->set_pixformat(sensor, PIXFORMAT_JPEG);
    sensor->set_quality(sensor, 10);

    connect_wifi();
    if (wifi_connect_status) {
        setup_server();
    }
}