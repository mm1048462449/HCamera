#include "i2c_slave.h"

void slave_init(void)
{
    //初始化相关引脚
    pcf8575_pinmode(PIN_KEY_LEFT, PCF_INPUT);
    pcf8575_pinmode(PIN_KEY_RIGHT, PCF_INPUT);
    pcf8575_pinmode(PIN_KEY_ENTER, PCF_INPUT);

    // pcf8575_pinmode(PIN_SD_CS, PCF_OUTPUT);

    pcf8575_pinmode(PIN_LCD_CS, PCF_OUTPUT);
    pcf8575_pinmode(PIN_LCD_DC, PCF_OUTPUT);
    pcf8575_pinmode(PIN_LCD_RST, PCF_OUTPUT);

    pcf8575_pinmode(PIN_BELL, PCF_OUTPUT);

    //开启i2c通信
    pcf8575_begin();

    //设置spi设备
    slave_spi_set(SPI_LCD);
    BELL_OFF;
}

uint8_t slave_read_key(void)
{
    uint8_t key = M_KEY_NONE;

    if (pcf8575_read(PIN_KEY_LEFT) > 0)
        key = M_KEY_LEFT;
    else if (pcf8575_read(PIN_KEY_RIGHT) > 0)
        key = M_KEY_RIGHT;
    else if (pcf8575_read(PIN_KEY_ENTER) > 0)
        key = M_KEY_ENTER;

    return key;
}

void slave_spi_set(spi_type_t type)
{
    // SD_CS_1;
    // LCD_CS_1;

    if (type == SPI_LCD)
    {
        LCD_CS_0;
    }
    else if (type == SPI_SD)
    {
        SD_CS_0;
    }
}
