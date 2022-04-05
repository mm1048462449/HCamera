#ifndef _I2C_SLAVE_H
#define _I2C_SLAVE_H

#include <stdio.h>
#include <pcf8575.h>

#define PIN_KEY_LEFT 0
#define PIN_KEY_RIGHT 1
#define PIN_KEY_ENTER 2

#define PIN_SD_CS 3

#define PIN_LCD_CS 4
#define PIN_LCD_DC 5
#define PIN_LCD_RST 6

#define PIN_BELL 8

#define SD_CS_0 pcf8575_write(PIN_SD_CS, PCF_LOW);
#define SD_CS_1 pcf8575_write(PIN_SD_CS, PCF_HIGH);

#define LCD_CS_0 pcf8575_write(PIN_LCD_CS, PCF_LOW);
#define LCD_CS_1 pcf8575_write(PIN_LCD_CS, PCF_HIGH);

#define LCD_DC_0 pcf8575_write(PIN_LCD_DC, PCF_LOW);
#define LCD_DC_1 pcf8575_write(PIN_LCD_DC, PCF_HIGH);

#define LCD_RST_0 pcf8575_write(PIN_LCD_RST, PCF_LOW);
#define LCD_RST_1 pcf8575_write(PIN_LCD_RST, PCF_HIGH);

#define BELL_ON pcf8575_write(PIN_BELL, PCF_LOW);
#define BELL_OFF pcf8575_write(PIN_BELL, PCF_HIGH);

typedef enum SPI_TYPE{
    SPI_LCD,
    SPI_SD
}spi_type_t;

typedef enum KEY_TYPE{
    M_KEY_NONE = 0,
    M_KEY_LEFT,
    M_KEY_RIGHT,
    M_KEY_ENTER
}key_type_t;

void slave_init(void);
uint8_t slave_read_key(void);
void slave_spi_set(spi_type_t type);

#endif