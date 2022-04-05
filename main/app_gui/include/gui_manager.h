#ifndef _GUI_MANAGER_H
#define _GUI_MANAGER_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "esp_timer.h"
#include "lv_inc.h"
#include "lv_libs.h"
#include "lcd_1inch28.h"
#include "gui_handler.h"
#include "gui_inc.h"

#include "gui_main.h"
#include "gui_camera.h"
#include "gui_picture.h"
#include "gui_game.h"
#include "gui_setting.h"
#include "gui_about.h"

void app_gui_main();
void app_gui_exit();

#endif