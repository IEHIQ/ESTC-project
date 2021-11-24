#include "nrf_gpio.h"
#include "app_timer.h"
#include "nrf_log.h"

#include "button.h"

/* Defining button timer */
APP_TIMER_DEF(btn_timer);

void empty_event() {};

event on_hold_start = empty_event;
event on_hold_end = empty_event;
event on_double_click = empty_event;

/* Current state of button. */
btn_state_t button_state = RELEASED;

/* Indicates that the next button press will (or will not) be a double click. */
bool is_double_click = false;

/*
    Indicates that double click was registered
    in current button press-release cycle.
 */
bool was_double_click = false;

/* Changes button state. */
void toggle_button_state()
{
    button_state = (button_state + 1) % 2;
}

void button_timer_handler(void *context)
{
    //NRF_LOG_INFO("Handler");
    *(bool *)context = false;
};

/* Configure button timer. */
void button_timer_init()
{
    app_timer_init();
    app_timer_create(&btn_timer, APP_TIMER_MODE_SINGLE_SHOT, button_timer_handler);
}

/*
    Button click event.
    When button gets pressed and double click was not initiated, button holding state begins

    When button gets pressed and double click was initiated (and double click delay still not ended),
        double click happens

    When button gets released, button holding state ends

    When button gets released and button press
        that was before this release did not triggered double click,
        initiates new double click timer delay
*/
void click(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    toggle_button_state();

    if (button_state == PRESSED)
    {
        //NRF_LOG_INFO("Pressed!");

        if (!is_double_click)
        {
            //NRF_LOG_INFO("Holding!");
            on_hold_start();
        }
        else
        {
            //NRF_LOG_INFO("Double click!");
            on_double_click();
            was_double_click = true;
            is_double_click = false;
            app_timer_stop(btn_timer);
        }
    }
    else
    {
        //NRF_LOG_INFO("Released!");
        on_hold_end();
        if (!was_double_click)
        {
            is_double_click = true;
            app_timer_start(btn_timer, DOUBLE_CLICK_DELAY_TICKS, &is_double_click);
        }
        was_double_click = false;
    }
}

void button_init()
{
    nrfx_gpiote_init();
    button_timer_init();
    uint32_t button;
    button = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

    nrfx_gpiote_in_config_t button_config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(button, &button_config, click);
    nrfx_gpiote_in_event_enable(button, true);
}