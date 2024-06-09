#include "include/server.h"

static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static const char *TAG = "SERVER";

// Create a queue to hold camera frames
QueueHandle_t frame_queue;

// This task retrieves frames from the camera and puts them into the queue
void camera_task(void *pvParameters)
{
    while (true)
    {
        camera_fb_t *frame = esp_camera_fb_get();
        if (!frame)
        {
            continue;
        }

        // Put the frame into the queue, but don't block if the queue is full
        xQueueSend(frame_queue, &frame, 0);

        vTaskDelay(10 / portTICK_RATE_MS);
    }
}

mime_map_t mime_map[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".gif", "image/gif"},
    {NULL, NULL} // End of list marker
};

/**
 * @brief Stream handler for HTTP server to stream video feed from camera module
 *
 * @param req HTTP request
 */
esp_err_t stream_handler(httpd_req_t *req)
{
    camera_fb_t *frame = NULL;
    esp_err_t response = ESP_OK;
    size_t jpg_buf_len;
    uint8_t *jpg_buf;
    char *part_buf[64];

    if ((response = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE)) != ESP_OK)
    {
        return response;
    }

    while (true)
    {
        // Retrieve a frame from the queue in a non-blocking manner
        if (xQueueReceive(frame_queue, &frame, 0) != pdTRUE)
        {
            // No frame was available, so continue with the next iteration of the loop
            continue;
        }

        jpg_buf_len = frame->len;
        jpg_buf = frame->buf;

        response = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, jpg_buf_len);

        response = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        response = httpd_resp_send_chunk(req, (const char *)jpg_buf, jpg_buf_len);

        esp_camera_fb_return(frame);

        if (response != ESP_OK)
        {
            break;
        }
    }

    return response;
}

/**
 * @brief Index handler for HTTP server
 *
 * @param req HTTP request
 */
esp_err_t web_server(httpd_req_t *req)
{
    const char *filepath = req->uri;

    char fullpath[1024];
    sprintf(fullpath, "%s%s", PREFIX, filepath);

    FILE *file = fopen(fullpath, "r");
    if (file == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    const char *ext = strrchr(filepath, '.');
    if (ext == NULL)
    {
        ESP_LOGE(TAG, "No file extension found");
        return ESP_FAIL;
    }

    const char *mime_type = NULL;
    for (mime_map_t *m = mime_map; m->extension != NULL; m++)
    {
        if (strcmp(ext, m->extension) == 0)
        {
            mime_type = m->mime_type;
            break;
        }
    }

    if (mime_type == NULL)
    {
        ESP_LOGE(TAG, "Unsupported file type: %s", ext);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, mime_type);

    char buffer[CHUNK_SIZE];
    while (1)
    {
        size_t bytes_read = fread(buffer, 1, CHUNK_SIZE, file);
        if (bytes_read > 0)
        {
            esp_err_t ret = httpd_resp_send_chunk(req, buffer, bytes_read);
            if (ret != ESP_OK)
            {
                fclose(file);
                return ret;
            }
        }
        else
        {
            break;
        }
    }

    fclose(file);
    return httpd_resp_send_chunk(req, NULL, 0); // Send an empty chunk to signal the end of the response
}

/**
 * @brief Reboot handler for HTTP server
 *
 * @param req HTTP request
 */
esp_err_t reboot_handler(httpd_req_t *req)
{
    esp_restart();
    return ESP_OK;
}

/**
 * @brief Status handler for HTTP server
 *
 * @param req HTTP request
 */
esp_err_t status_handler(httpd_req_t *req)
{
    // return status of free heap memory and other system parameters
    char response[1024];

    sprintf(response, "{\"free_heap\": %lu, \"min_free_heap\": %lu, \"max_alloc_heap\": %u, \"min_alloc_heap\": %u, \"total_alloc_heap\": %d}",
            esp_get_free_heap_size(), esp_get_minimum_free_heap_size(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT), heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT), heap_caps_get_total_size(MALLOC_CAP_8BIT));

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));

    return ESP_OK;
}

httpd_uri_t uri_index = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = web_server,
    .user_ctx = NULL};

httpd_uri_t uri_stream = {
    .uri = "/stream",
    .method = HTTP_GET,
    .handler = stream_handler,
    .user_ctx = NULL};

httpd_uri_t uri_reboot = {
    .uri = "/reboot",
    .method = HTTP_GET,
    .handler = reboot_handler,
    .user_ctx = NULL};

httpd_uri_t uri_status = {
    .uri = "/status",
    .method = HTTP_GET,
    .handler = status_handler,
    .user_ctx = NULL};

/**
 * @brief Setup HTTP server to stream video feed from camera module and control the camera
 *
 * @return httpd_handle_t HTTP server handler
 */
httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    httpd_handle_t server_handler = NULL;

    frame_queue = xQueueCreate(10, sizeof(camera_fb_t *));

    xTaskCreate(camera_task, "camera_task", 8096, NULL, 5, NULL);

    if (httpd_start(&server_handler, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server_handler, &uri_stream);
        httpd_register_uri_handler(server_handler, &uri_reboot);
        httpd_register_uri_handler(server_handler, &uri_status);

        // Register the index handler last so that it is the default handler
        httpd_register_uri_handler(server_handler, &uri_index);
    }

    return server_handler;
}