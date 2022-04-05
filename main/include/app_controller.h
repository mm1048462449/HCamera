#ifndef _APP_CONTROLER_H
#define _APP_CONTROLER_H

#include <stdlib.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <esp_err.h>
#include "quirc.h"
#include "quirc_internal.h"
#include "app_qrcode.h"
#include "i2c_slave.h"
#include "app_config.h"

// TASK ID
typedef enum control_flag
{
    CF_NONE = -1,
    CF_CAPTURE,
    CF_VIDEO,
    CF_STREAM,
    CF_CONFIG,
    CF_QRCODE,
} app_control_flag;

typedef enum control_type
{
    VIDEO_SIZE = 0,
    XCLK,
    QUALITY,
    BRIGHTNESS,
    CONTRAST,
    SATURATION,
    SPECIAL_EFFECT,
    AWB,
    AWB_GAIN,
    WB_MODE,
    AEC_SENSOR,
    AEC_DSP,
    AE_LEVEL,
    AGC,
    GAINCEILING,
    BPC,
    WPC,
    RAW_GMA,
    LENC,
    H_MIRROR,
    V_FLIP,
    DCW,
    COLOR_BAR,
    LED_INTENSITY,
    FACE_DETECTION,
} app_control_type;

typedef struct camera_buf_struct
{
    uint8_t *buf;
    size_t buf_len;
    size_t buf_w;
    size_t buf_h;
} camera_buf_t; //摄像头帧数据

typedef struct camera_config_struct
{
    int cfg_type;
    int val;
} camera_cfg_t; //配置参数

typedef struct control_param_struct
{
    camera_buf_t *buf_t;
    camera_cfg_t *cfg_t;
    char qrcode[128];
} control_param_t;

typedef void (*c_handler)(control_param_t *bt);

typedef struct control_handle_struct
{
    uint8_t flag;
    uint8_t id;
    TaskHandle_t xHandle;
    c_handler handler;
} control_handle_t;

esp_err_t app_controller_main();
void controller_reg_handler(const control_handle_t *control_handler);
void controller_start_task(uint8_t id);
void controller_stop_task(uint8_t id);
void controller_sound_task(void);

#endif