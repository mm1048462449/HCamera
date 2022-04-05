#ifndef _LED_H
#define _LED_H

#include <stdint.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void esp_led_init(int pin);
void esp_led_on();
void esp_led_off();
void esp_led_switch(int n, int td); //n:次数，td:time delay

#endif