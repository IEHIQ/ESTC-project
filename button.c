#include "nrf_gpio.h"

#include "button.h"

/* The time remaining until the next click for double-click to get detected. */
uint32_t mcs_dclick_time_left = 0;
/* Current state of LEDs behaviour. */
bool is_blinking = 0;

void dclick(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
    if (mcs_dclick_time_left > 0)
    {
        //NRF_LOG_INFO("Double click!");
        is_blinking = !is_blinking;
        mcs_dclick_time_left = 0;
    }
    else
    {
        mcs_dclick_time_left = DOUBLE_CLICK_DELAY_MCS;
    }
}

void button_init() 
{
    nrfx_gpiote_init();
    uint32_t button;
    button = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(button, &button_config, dclick);
    nrfx_gpiote_in_event_enable(button, true);
}