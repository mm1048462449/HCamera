#ifndef _PCF8575_H
#define _PCF8575_H

#include <stdio.h>
#include "esp_log.h"

#define  bit(b) (1 << (b))

typedef enum PIN_MODE{
    PCF_OUTPUT=0,
    PCF_INPUT
}pcf8575_pinmode_t;

typedef enum PIN_STATE{
    PCF_LOW=0,
    PCF_HIGH
}pcf8575_pinstate_t;

void pcf8575_begin(void);
void pcf8575_pinmode(uint8_t pin, uint8_t mode);
void pcf8575_write(uint8_t pin, uint8_t value);
uint8_t pcf8575_read(uint8_t pin);

#endif