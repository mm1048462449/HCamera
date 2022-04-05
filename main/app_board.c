/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2017 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS products only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "string.h"
#include "sdkconfig.h"
#include "app_board.h"
#include "app_camera.h"
#include "app_wifi.h"
#include "app_config.h"
#include "led.h"
#include "lcd_1inch28.h"
#include "lcd_gui.h"
#include "i2c_slave.h"
#include "app_controller.h"

void app_board_main(void)
{
    //初始化摄像头
    //初始化NVS存储
    app_camera_main();
    //初始化i2c总线设备
    slave_init();

    //初始化LCD
    LCD_1IN28_Init(0);
    LCD_1IN28_Clear(BLACK);

    //初始化配置
    app_config_init();

    if (app_config.wifi_state)
    {
        //初始化WIFI
#ifdef CONFIG_ESP_SMAERT_CONFIG
        initialise_wifi();
#else
        app_wifi_main();
#endif
    }
}