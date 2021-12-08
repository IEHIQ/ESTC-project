#include "nrf_gpio.h"
#include "app_timer.h"
#include "nrf_log.h"

#include "button.h"

/* Defining button timer */
APP_TIMER_DEF(btn_timer);

static uint32_t button_pin;

static void empty_event() {};

/* Button events */
static event on_hold_start;
static event on_hold_end;
static event on_double_click;

/* Indicates that the next button press will (or will not) be a double click. */
static bool is_double_click;

static void button_timer_handler(void *context)
{
    NRF_LOG_DEBUG("Handler");
    is_double_click = false;
    if (!nrf_gpio_pin_read(button_pin))
    {
        NRF_LOG_DEBUG("Holding!");
        on_hold_start();
    }
};

/**
 * @brief Configures button timer
 *
 */
static void init_button_timer()
{
    app_timer_create(&btn_timer, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);
}

/*
    Button click event.
*/
static void click(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (!nrf_gpio_pin_read(button_pin))
    {
        NRF_LOG_DEBUG("Pressed!");
        if (!is_double_click)
        {
            is_double_click = true;
            app_timer_start(btn_timer, DOUBLE_CLICK_DELAY_TICKS, NULL);
        }
        else
        {
            NRF_LOG_DEBUG("Double click!");
            is_double_click = false;
            on_double_click();
            app_timer_stop(btn_timer);
        }
    }
    else
    {
        NRF_LOG_DEBUG("Released!");
        on_hold_end();
    }
}

void init_button()
{
    on_hold_start = empty_event;
    on_hold_end = empty_event;
    on_double_click = empty_event;
    is_double_click = false;

    nrfx_gpiote_init();
    init_button_timer();
    button_pin = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(button_pin, NRF_GPIO_PIN_PULLUP);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(button_pin, &button_config, click);
    nrfx_gpiote_in_event_enable(button_pin, true);
}

void set_hold_start_event(event hold_start_event)
{
    on_hold_start = hold_start_event;
}

void set_hold_end_event(event hold_end_event)
{
    on_hold_end = hold_end_event;
}

void set_double_click_event(event double_click_event)
{
    on_double_click = double_click_event;
}