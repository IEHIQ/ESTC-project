#include "LED.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

const uint8_t blink_counts[LEDS_COUNT] = {6, 5, 8, 7};

const uint32_t LED_gpio_pins[LEDS_COUNT] = 
{
    NRF_GPIO_PIN_MAP(0, 6),
    NRF_GPIO_PIN_MAP(0, 8),
    NRF_GPIO_PIN_MAP(1, 9), 
    NRF_GPIO_PIN_MAP(0, 12)
};

void LEDs_init(LEDs_config *config)
{   
    for (int i = 0; i < LEDS_COUNT; i++) 
    {
        nrf_gpio_cfg_output(LED_gpio_pins[i]);
        nrf_gpio_pin_write(LED_gpio_pins[i], 1);
    }
    config->current_LED = 0;
    config->blinks_left = blink_counts[config->current_LED] * 2;
}

void LED_toggle(uint8_t *LED_num) 
{
    nrf_gpio_pin_toggle(LED_gpio_pins[*LED_num]);
}

void LED_set_blink(LEDs_config *config)
{
    config->blinks_left -= 1;
    if (config->blinks_left == 0) 
    {
        config->current_LED = (config->current_LED + 1) % LEDS_COUNT;
        config->blinks_left = blink_counts[config->current_LED] * 2;
    }
}