#include "led.h"

int led_pin;

void esp_led_init(int pin)
{
    led_pin = pin;
    gpio_config_t LED_cof;
	LED_cof.pin_bit_mask = (1<<pin);
	LED_cof.mode = GPIO_MODE_OUTPUT;
	LED_cof.pull_up_en = 0;
	LED_cof.pull_down_en = 0;
	LED_cof.intr_type = 0;

    if (gpio_config(&LED_cof) == ESP_OK) {    //配置GPIO并判断是否配置成功
		printf("ESP_GPIO_COF_OK\n");
	}
	else {                                                                                                                                                          
        printf("ESP_ERR_INVALID_ARG\n");
	}
}
void esp_led_on()
{
    gpio_set_level(led_pin, 1);
}
void esp_led_off()
{
    gpio_set_level(led_pin, 0);
}
void esp_led_switch(int n, int td)
{
    while (n--)
    {
        esp_led_on();
        vTaskDelay(td / portTICK_PERIOD_MS);
        esp_led_off();
        vTaskDelay(td / portTICK_PERIOD_MS);
    }
}