#ifndef HTTPD_HANDLER_H_
#define HTTPD_HANDLER_H_

#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_camera_helper.h"

#define PART_BOUNDARY "123456789000000000000987654321"
httpd_handle_t setup_server(void);

#endif /* HTTPD_HANDLER_H_ */