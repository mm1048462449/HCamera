#include "app_config.h"

#define STORAGE_NAMESPACE "hcamera_storage"
#define TAG_READ "[NVS_READ] "
#define TAG_WRITE "[NVS_WRITE] "

#define KEY "config_st"

app_config_t app_config;

static void config_default_init()
{
    app_config.sound_state = 0;
    app_config.wifi_state = 1;
    app_config.camera_state = 0;

    if (app_config_write() == ESP_OK)
    {
        ESP_LOGI(TAG_WRITE, "configuration reset successful");
    }
    else
    {
        ESP_LOGI(TAG_WRITE, "configuration reset failed");
    }
}

esp_err_t app_config_read()
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG_READ, "%s", "nvs open error");
        return err;
    }

    size_t required_size = 0;
    err = nvs_get_blob(nvs_handle, KEY, NULL, &required_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
        return err;

    ESP_LOGI(TAG_READ, "config size: %d", required_size);
    err = nvs_get_blob(nvs_handle, KEY, &app_config, &required_size);
    if (err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGI(TAG_READ, "%s", "key doesn't exist");
        return err;
    }
    else if (err == ESP_ERR_NVS_INVALID_HANDLE)
    {
        ESP_LOGI(TAG_READ, "%s", "handle has been closed or is NULL");
        return err;
    }
    else if (err == ESP_ERR_NVS_INVALID_NAME)
    {
        ESP_LOGI(TAG_READ, "%s", "name doesn't satisfy constraints");
        return err;
    }
    else if (err == ESP_ERR_NVS_INVALID_LENGTH)
    {
        ESP_LOGI(TAG_READ, "%s", "length is not sufficient to store data");
        return err;
    }
    else
    {
        ESP_LOGI(TAG_READ, "%s", "read successful");
    }

    return ESP_OK;
}

esp_err_t app_config_write()
{
    nvs_handle_t nvs_handle;
    esp_err_t err;

    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG_READ, "%s", "nvs open error");
        return err;
    }

    err = nvs_set_blob(nvs_handle, KEY, &app_config, sizeof(app_config_t));
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG_WRITE, "%s", "write failed");
        return err;
    }
    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGI(TAG_WRITE, "%s", "commit failed");
        return err;
    }

    nvs_close(nvs_handle);
    return ESP_OK;
}

void app_config_init()
{
    if (app_config_read() != ESP_OK)
    {
        config_default_init();
    }

    ESP_LOGI(TAG_READ, "sound: %d", app_config.sound_state);
    ESP_LOGI(TAG_READ, "bl: %d", app_config.wifi_state);
    ESP_LOGI(TAG_READ, "camera: %d", app_config.camera_state);
}