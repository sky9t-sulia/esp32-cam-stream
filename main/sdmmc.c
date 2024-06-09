#include "include/sdmmc.h"

static const char *TAG = "SDMMC";

esp_err_t init_sdmmc()
{
    sdmmc_card_t *card;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .max_files = 30,
        .format_if_mount_failed = false,
        .allocation_unit_size = 0,
        .disk_status_check_enable = false
    };

    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.flags |= SDMMC_HOST_FLAG_1BIT;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    slot_config.width = 1;
    slot_config.clk = CONFIG_SD_MMC_CLK;
    slot_config.cmd = CONFIG_SD_MMC_CMD;
    slot_config.d0 = CONFIG_SD_MMC_DAT0;


    esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "ERROR: %s", esp_err_to_name(ret));
        }
        else if (ret == ESP_ERR_INVALID_STATE)
        {
            ESP_LOGE(TAG, "ERROR: %s", esp_err_to_name(ret));
        }
        else
        {
            ESP_LOGE(TAG, "ERROR: %s", esp_err_to_name(ret));
        }
    }

    return ret;
}