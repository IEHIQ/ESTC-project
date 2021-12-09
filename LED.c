#include "LED.h"

#include "utils.h"
#include "PWM.h"
#include "modes.h"
#include "app_timer.h"
#include "nrf_log.h"
#include "memory.h"

/* LED_0 timer definition. */
APP_TIMER_DEF(led0_timer);

/* LED_1 timer definition. */
APP_TIMER_DEF(led1_timer);

/* Consts for black colors */
static const uint16_t zero = 0;
static const RGB_16 black = {0, 0, 0};

static HSB current_hsb;
static HSB current_hsb_delta;

static RGB_16 current_rgb;

/* Current and delta colors (duty cycles) value for led_0*/
static uint16_t current_led0_color;
static int16_t current_led0_color_delta;

/* Function poitner for changing HSB color (increase or decrease) */
typedef void (*hsb_modifyer)(HSB*, const HSB*);
static hsb_modifyer apply_hsb_delta;

/**
 * @brief Loads last HSB color from nvm
 */
void load_last_hsb()
{
    load_HSB(&current_hsb);
    HSB_to_RGB_16(&current_hsb, &current_rgb, MAX_DC);
}

/**
 * @brief Saves current HSB color into nvm
 */
void save_current_hsb()
{
    save_HSB(&current_hsb);
}

/**
 * @brief Configures all LEDs parameters
 */
void init_leds_config()
{
    current_hsb = (HSB){0, 100, 100};
    load_last_hsb();
    current_hsb_delta = (HSB){0, 0 ,0};

    current_led0_color = 0;
    current_led0_color_delta = LED_0_STEP;
    apply_hsb_delta = hsb_add;
}

void set_current_hsb(const HSB *color)
{
    current_hsb = *color;
    HSB_to_RGB_16(&current_hsb, &current_rgb, MAX_DC);
    set_pwm_led_1(&current_rgb);
}

HSB get_current_hsb()
{
    return current_hsb;
}

void set_current_rgb(const RGB_16 *color)
{
    current_rgb = *color;
    set_pwm_led_1(&current_rgb);
}

RGB_16 get_current_rgb()
{
    return current_rgb;
}

void set_hsb_delta(const HSB new_delta)
{
    current_hsb_delta = new_delta;
}

/**
 * @brief Checks current blink phase and changes HSB modifying function
 * (add or subtract delta on each timer cycle) if needed
 */
void checkset_blink_phase()
{
    modes mode = get_current_mode();
    if ((mode == HUE        && current_hsb.H == 0) ||
        (mode == SATURATION && current_hsb.S == 0) ||
        (mode == BRIGHTNESS && current_hsb.B == 0))
    {
        apply_hsb_delta = hsb_add;
    }
    else if ((mode == HUE        && current_hsb.H == MAX_HUE)        ||
             (mode == SATURATION && current_hsb.S == MAX_SATURATION) ||
             (mode == BRIGHTNESS && current_hsb.B == MAX_BRIGHTNESS))
    {
        apply_hsb_delta = hsb_subtract;
    }
}

/**
 * @brief LED 1 color changing handler
 */
void led1_timer_handler(void *context)
{
    apply_hsb_delta(&current_hsb, &current_hsb_delta);
    checkset_blink_phase();
    HSB_to_RGB_16(&current_hsb, &current_rgb, MAX_DC);
    set_pwm_led_1(&current_rgb);
}

/**
 * @brief Configures LED 1 color changing timer
 */
static void init_led1_timer()
{
    app_timer_create(&led1_timer, APP_TIMER_MODE_REPEATED, led1_timer_handler);
}

/**
 * @brief LED 0 color changing handler
 */
void led0_timer_handler(void *context)
{
    current_led0_color = clamp(current_led0_color + current_led0_color_delta, 0, MAX_DC);
    if (current_led0_color == 0 || current_led0_color == MAX_DC)
    {
        current_led0_color_delta *= -1;
    }
    set_pwm_led_0(current_led0_color);
}

/**
 * @brief Configures LED 0 color changing timer
 */
static void init_led0_timer()
{
    app_timer_create(&led0_timer, APP_TIMER_MODE_REPEATED, led0_timer_handler);
}

void init_leds()
{
    init_leds_config();
    init_led0_timer();
    init_led1_timer();
}

void start_led0(const uint32_t delay)
{
    app_timer_start(led0_timer, delay, NULL);
}

void start_led1(const uint32_t delay)
{
    app_timer_start(led1_timer, delay, NULL);
}

void stop_led0()
{
    app_timer_stop(led0_timer);
}

void stop_led1()
{
    app_timer_stop(led1_timer);
}

void turn_off_led0()
{
    stop_led0();
    set_pwm_led_0(zero);
}

void turn_on_led0()
{
    stop_led0();
    set_pwm_led_0(MAX_DC);
}

void turn_off_led1()
{
    stop_led1();
    set_pwm_led_1(&black);
}
