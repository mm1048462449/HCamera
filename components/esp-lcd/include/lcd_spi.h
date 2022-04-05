#ifndef _LCD_SPI_H
#define _LCD_SPI_H

#include <stdio.h>
#include <string.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define DMA_ENABLE 1

#define DMA_CHAN SPI_DMA_CH_AUTO
#define PIN_NUM_MISO 12
#define PIN_NUM_MOSI 13
#define PIN_NUM_CLK 14
#define PIN_NUM_LCD_CS -1


spi_device_handle_t spi_lcd_device; // lcd spi句柄

static void lcd_spi_init()
{
    esp_err_t ret;
    int transfer_size = SOC_SPI_MAXIMUM_BUFFER_SIZE;
#if DMA_ENABLE
    transfer_size = 60 * 240 * 2;
#endif
    //配置SPI总线参数
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,      //设置主机输入从机输出引脚(接收数据引脚)
        .mosi_io_num = PIN_NUM_MOSI,      //设置主机输出从机输入引脚(发送数据引脚)
        .sclk_io_num = PIN_NUM_CLK,       //设置时钟引脚
        .quadwp_io_num = -1,              //不使用wp信号线
        .quadhd_io_num = -1,              //不使用hd信号线
        .max_transfer_sz = transfer_size, //一次性最大传输字节个数;默认为SOC_SPI_MAXIMUM_BUFFER_SIZE(启用DMA时需要设置为0)
    };

    //配置SPI数据传输参数(可以新建多个这种结构体变量,相当于挂载SPI设备,一个SPI最多挂载3个设备,需要设置不同的片选引脚)
    spi_device_interface_config_t devcfg_lcd = {
        .address_bits = 0,                  //不需要地址数据
        .clock_speed_hz = 80 * 1000 * 1000, //频率40M
        .command_bits = 0,                  //不需要命令数据
        .mode = 0,                          // SPI模式(0,1,2,3)
        .spics_io_num = PIN_NUM_LCD_CS,     //设置CS引脚
        .queue_size = 8,                    //传输(事务)队列大小
    };

    //初始化配置SPI总线(配置HSPI);最后设置为0为不启用DMA
#if DMA_ENABLE
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, DMA_CHAN);
#else
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, 0);
#endif
    ESP_ERROR_CHECK(ret);

    //把传输参数配置进SPI总线
    ret = spi_bus_add_device(HSPI_HOST, &devcfg_lcd, &spi_lcd_device);
    ESP_ERROR_CHECK(ret);
}

static void lcd_spi_write(const uint8_t data)
{
    esp_err_t ret;
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));                              // Zero out the transaction
    t.length = 8;                                          // Command is 8 bits
    t.tx_buffer = &data;                                   // The data is the cmd itself
    t.user = (void *)0;                                    // D/C needs to be set to 0
    ret = spi_device_polling_transmit(spi_lcd_device, &t); // Transmit!
    assert(ret == ESP_OK);                                 // Should have had no issues.
}

static void lcd_spi_write_pack(const uint16_t *data, int len)
{
    esp_err_t ret;
    spi_transaction_t t;
    if (len == 0)
        return;                                            // no need to send anything
    memset(&t, 0, sizeof(t));                              // Zero out the transaction
    t.length = len * 2 * 8;                                // Len is in bytes, transaction length is in bits.
    t.tx_buffer = data;                                    // Data
    t.user = (void *)1;                                    // D/C needs to be set to 1
    ret = spi_device_polling_transmit(spi_lcd_device, &t); // Transmit!
    assert(ret == ESP_OK);                                 // Should have had no issues.
}

#endif