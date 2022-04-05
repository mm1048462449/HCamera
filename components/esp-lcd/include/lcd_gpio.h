#ifndef _LCD_GPIO_H
#define _LCD_GPIO_H

#include "driver/gpio.h"

#define LCD_CS_PIN 15
#define LCD_DC_PIN 2
#define LCD_RST_PIN 0

#define LCD_PIN_SET ((1<<LCD_CS_PIN)|(1<<LCD_DC_PIN)/*|(1<<LCD_RST_PIN)*/)

static void lcd_gpio_init()
{
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = LCD_PIN_SET;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

static void lcd_cs_set(uint8_t state)
{
    gpio_set_level(LCD_CS_PIN, state);
}

static void lcd_dc_set(uint8_t state)
{
    gpio_set_level(LCD_DC_PIN, state);
}

static void lcd_rst_set(uint8_t state)
{
    // gpio_set_level(LCD_RST_PIN, state);
}

#endif