#ifndef SD_MMC_H_
#define SD_MMC_H_

#include "driver/gpio.h"
#include "driver/sdmmc_host.h"
#include <esp_vfs_fat.h>

esp_err_t init_sdmmc();

#endif /* SD_MMC_H_ */