#ifndef _LCD_SYSTEM_H
#define _LCD_SYSTEM_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp32/rom/ets_sys.h"

static void DEV_Delay_ms(uint16_t t) {
    vTaskDelay( t / portTICK_PERIOD_MS );
}

static void Delay_Us(uint16_t t){
    ets_delay_us(t);
}

#endif