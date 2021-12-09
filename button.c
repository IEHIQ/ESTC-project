#include "nrf_gpio.h"
#include "app_timer.h"
#include "nrf_log.h"

#include "button.h"

/* Defining button timers */
APP_TIMER_DEF(click_timer);
APP_TIMER_DEF(antibounce_timer);

static uint32_t button_pin;

static void empty_event() {};

/* Button events */
static btn_evt_t on_hold_start;
static btn_evt_t on_hold_end;
static btn_evt_t on_double_click;

/* Indicates that the next button press will (or will not) be a double click. */
static bool is_double_click;
/* Indicates that if contact antibounce delay timer is still running */
static bool antibounce;

static void click_timer_handler(void *context)
{
    NRF_LOG_DEBUG("Handler");
    is_double_click = false;
    if (!nrf_gpio_pin_read(button_pin))
    {
        NRF_LOG_DEBUG("Holding!");
        on_hold_start();
    }
}

static void antibounce_timer_handler(void *context)
{
    antibounce = false;
    NRF_LOG_DEBUG("Antibounce ended");
}

/**
 * @brief Configures button timers
 */
static void init_button_timers()
{
    app_timer_create(&click_timer, APP_TIMER_MODE_SINGLE_SHOT, click_timer_handler);
    app_timer_create(&antibounce_timer, APP_TIMER_MODE_SINGLE_SHOT, antibounce_timer_handler);
}

/*
    Button click event.
*/
static void click(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    if (!antibounce)
    {
        if (!nrf_gpio_pin_read(button_pin))
        {
            NRF_LOG_DEBUG("Antibounce");
            antibounce = true;
            app_timer_start(antibounce_timer, ANTIBOUNCE_DELAY_TICKS, NULL);

            if (!is_double_click)
            {
                is_double_click = true;
                app_timer_start(click_timer, DOUBLE_CLICK_DELAY_TICKS, NULL);
            }
            else
            {
                NRF_LOG_DEBUG("Double click!");
                is_double_click = false;
                on_double_click();
                app_timer_stop(click_timer);
            }
        }
        else
        {
            NRF_LOG_DEBUG("Released!");
            on_hold_end();
        }
    }
}

void init_button()
{
    on_hold_start = empty_event;
    on_hold_end = empty_event;
    on_double_click = empty_event;
    is_double_click = false;
    antibounce = false;

    nrfx_gpiote_init();
    init_button_timers();
    button_pin = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(button_pin, NRF_GPIO_PIN_PULLUP);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(button_pin, &button_config, click);
    nrfx_gpiote_in_event_enable(button_pin, true);
}

void set_hold_start_event(btn_evt_t hold_start_event)
{
    on_hold_start = hold_start_event;
}

void set_hold_end_event(btn_evt_t hold_end_event)
{
    on_hold_end = hold_end_event;
}

void set_double_click_event(btn_evt_t double_click_event)
{
    on_double_click = double_click_event;
}