#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "gui_manager.h"
#include "app_board.h"
#include "app_controller.h"
#include "app_client.h"
#include "app_config.h"

static const char *TAG = "[APP] ";

TaskHandle_t testHandler = NULL;
uint8_t task_flag = 1;
uint8_t task_run = 1;

void test_task(void *args)
{
    while (task_run)
    {
        printf("test task state: %d\n", eTaskGetState(testHandler));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    task_flag = 0;
    vTaskDelete(testHandler);
}

void test_func()
{
    printf("create test task0!\n");
    printf("test task0 flag: %d\n", task_flag);

    xTaskCreate(&test_task,
                "TEST_TASK0",
                4096,
                &task_flag,
                tskIDLE_PRIORITY,
                &testHandler);

    vTaskDelay((15 * 1000) / portTICK_PERIOD_MS);
    task_run = 0;

    while (true)
    {
        if (eTaskGetState(testHandler) == eDeleted)
        {
            printf("test task0 deleted!\n");
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    printf("test task0 flag: %d\n", task_flag);

    /////

    task_flag = 1;
    task_run = 1;
    printf("create test task1!\n");
    printf("test task1 flag: %d\n", task_flag);

    xTaskCreate(&test_task,
                "TEST_TASK1",
                4096,
                &task_flag,
                tskIDLE_PRIORITY,
                &testHandler);

    vTaskDelay((15 * 1000) / portTICK_PERIOD_MS);
    task_run = 0;

    while (true)
    {
        if (eTaskGetState(testHandler) == eDeleted)
        {
            printf("test task1 deleted!\n");
            break;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    printf("test task1 flag: %d\n", task_flag);

    return;
}

void app_main(void)
{
    control_handle_t capture_handle = {
        .id = CF_CAPTURE,
        .flag = 0,
        .handler = camera_capture_handler};

    control_handle_t video_handle = {
        .id = CF_VIDEO,
        .flag = 0,
        .handler = camera_video_handler};

    control_handle_t config_handle = {
        .id = CF_CONFIG,
        .flag = 0,
        .handler = camera_config_handler};

    control_handle_t qrcode_handle = {
        .id = CF_QRCODE,
        .flag = 0,
        .handler = camera_qrcode_handler};

    if (app_controller_main() == ESP_OK)
    {
        controller_reg_handler(&capture_handle);
        controller_reg_handler(&video_handle);
        controller_reg_handler(&config_handle);
        controller_reg_handler(&qrcode_handle);
    }

    app_board_main();
    app_gui_main();

    //摄像头配置
    controller_start_task(CF_CONFIG);
    vTaskDelay(200 / portTICK_RATE_MS);
    controller_start_task(CF_CONFIG);

    //开始运行
    ESP_LOGI(TAG, "Startup...");
    ESP_LOGI(TAG, "Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "IDF version: %s", esp_get_idf_version());
}
