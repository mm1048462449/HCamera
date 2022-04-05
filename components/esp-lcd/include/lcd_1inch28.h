#ifndef LCD_1INCH28_H_INCLUDED
#define LCD_1INCH28_H_INCLUDED

/*****************************************************************************
 * | File      	:   LCD_1inch28.h
 * | Author      :   Waveshare team
 * | Function    :   Hardware underlying interface
 * | Info        :
 *                Used to shield the underlying layers of each master
 *                and enhance portability
 *----------------
 * |	This version:   V1.0
 * | Date        :   2020-12-06
 * | Info        :   Basic version
 * 
 * ---------------
 * |PIN DESC.
 * |VCC :电源正(3.3/5V输入)
 * |GND :电源地
 * |DIN :SPI数据输入(MOSI) 13
 * |CLK :SPI时钟输入	   14
 * |CS  :片选(低电平有效)  15/由外部i2c设备扩展gpio控制
 * |DC  :数据/命令(低电平表示命令，高电平表示数据) 2/由外部i2c设备扩展gpio控制
 * |RST :复位(低电平有效)  由外部i2c设备扩展gpio控制
 * |BL  :背光(用不了引脚不够)
 *
 ******************************************************************************/
#include <stdint.h>

#include <stdlib.h> //itoa()
#include <stdio.h>
#include "lcd_typedef.h"
#include "lcd_gpio.h"
#include "lcd_spi.h"
#include "lcd_system.h"
#include "i2c_slave.h"

#define LCD_1IN28_HEIGHT 240
#define LCD_1IN28_WIDTH 240

#define HORIZONTAL 0
#define VERTICAL 1

#define LCD_1IN28_CS_0 LCD_CS_0//(lcd_cs_set(0))
#define LCD_1IN28_CS_1 LCD_CS_1//(lcd_cs_set(1))

#define LCD_1IN28_RST_0 LCD_RST_0//(lcd_rst_set(0))
#define LCD_1IN28_RST_1 LCD_RST_1//(lcd_rst_set(1))

#define LCD_1IN28_DC_0 LCD_DC_0//(lcd_dc_set(0))
#define LCD_1IN28_DC_1 LCD_DC_1//(lcd_dc_set(1))

typedef struct
{
	UWORD WIDTH;
	UWORD HEIGHT;
	UBYTE SCAN_DIR;
} LCD_1IN28_ATTRIBUTES;
extern LCD_1IN28_ATTRIBUTES LCD_1IN28;

typedef struct
{
	UBYTE disp_flush;
	UBYTE disp_play;
	UWORD *disp_buf;
	UWORD disp_size;
	UWORD disp_row;
} LCD_1IN28_DISP;
extern LCD_1IN28_DISP LCD_Disp;
/********************************************************************************
function:
			Macro definition variable name
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir);
void LCD_1IN28_Clear(UWORD Color);
void LCD_1IN28_Clear_Fast(UWORD *pcolor);
void LCD_1IN28_Display(UWORD *Image);
void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN28_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void LCD_1IN28_DrawPaint(UWORD x, UWORD y, UWORD Color);
void LCD_1IN28_SetBackLight(UWORD Value);

void LCD_1IN28_Reset(void);

#endif // LCD_1INCH28_H_INCLUDED
