#ifndef _APP_CLIENT_H
#define _APP_CLIENT_H

#include "app_wifi.h"
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_client.h"
// #include "esp_event_loop.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "app_config.h"

enum CLIENT_TASK_TYPE
{
    TASK_FILE_LIST = 0,
    TASK_FILE_UPLOAD,
    TASK_FILE_DOWNLOAD
};

typedef struct RequestCallbackStruct{
    void(*list_txt)(uint8_t index, const char* file_name); //文件列表更新回调
    void(*list_info)(); //页数信息更新回调
    void(*req_finish)(bool state); //请求完成回调//state: true:finish; false:error
    void(*req_prog)(float percent); //下载进度
}req_cb_t;

typedef struct FileRequestStruct
{
    const char* url;
    const char* dir;
    int pn;
    int pi;
    int task_type;
    char file[64];
    uint8_t *file_buf;
    size_t file_buf_len;
}file_req_t;

typedef struct FileResponseStruct
{
    const char* dir; //目录
    int page_num; //页数
    int page_count; //总页数
    int page_item; //每页条数
    uint8_t *file_buf;
    int file_len;
}file_res_t;

extern req_cb_t client_req_cb;
extern file_req_t file_req;
extern file_res_t file_res;

int client_check_connected(void);
void client_task_begin(int task_type);

#endif