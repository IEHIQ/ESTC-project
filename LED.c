#include "LED.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"

const uint8_t blink_counts[LEDS_COUNT] = {6, 5, 8, 7};

const LED pins_n_ports[] = 
{
    {0, 6},
    {0, 8},
    {1, 9}, 
    {0, 12}
};

void LEDs_init(LEDs_config *config)
{   
    for (int i = 0; i < LEDS_COUNT; i++) 
    {
        config->LEDs[i] = NRF_GPIO_PIN_MAP(pins_n_ports[i].port, pins_n_ports[i].pin);
        nrf_gpio_cfg_output(config->LEDs[i]);
        nrf_gpio_pin_write(config->LEDs[i], 1);
    }
    config->current_LED = 0;
    config->blinks_left = blink_counts[config->current_LED] * 2;
}

void LED_toggle(uint32_t *led) 
{
    nrf_gpio_pin_toggle(*led);
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