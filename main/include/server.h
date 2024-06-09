#ifndef SERVER_H_
#define SERVER_H_

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_camera_helper.h"

typedef struct {
    const char *extension;
    const char *mime_type;
} mime_map_t;

#define FILE_PATH_MAX 512
#define PREFIX "/sdcard/"
#define FULL_PATH_MAX (FILE_PATH_MAX + sizeof(PREFIX))

#define CHUNK_SIZE 512

#define PART_BOUNDARY "123456789000000000000987654321"
httpd_handle_t setup_server(void);

#endif /* SERVER_H_ */