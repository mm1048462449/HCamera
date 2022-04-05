#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H

#include "esp_log.h"
#include "nvs_flash.h"

typedef struct AppConfigStruct
{
    uint8_t sound_state : 1;
    uint8_t wifi_state : 1;
    uint8_t camera_state : 3;
} app_config_t;

extern app_config_t app_config;

esp_err_t app_config_read(void);
esp_err_t app_config_write(void);
void app_config_init(void);

#endif