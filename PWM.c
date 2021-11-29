#include "PWM.h"
#include "LED.h"

/* PWM config parameters */
static nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
static nrf_pwm_values_individual_t pwm_values_individual = { 0, 0, 0, 0 };
static nrf_pwm_values_t pwm_values;
static nrf_pwm_sequence_t pwm_sequence;

/**
 * @brief Configures pwm_config
 */
void pwm_config_init()
{
    pwm_config.load_mode = NRF_PWM_LOAD_INDIVIDUAL;

    pwm_config.output_pins[0] = LED_0 | NRFX_PWM_PIN_INVERTED;
    pwm_config.output_pins[1] = LED_R | NRFX_PWM_PIN_INVERTED;
    pwm_config.output_pins[2] = LED_G | NRFX_PWM_PIN_INVERTED;
    pwm_config.output_pins[3] = LED_B | NRFX_PWM_PIN_INVERTED;
}

/**
 * @brief Configures pwm_sequence
 */
void pwm_sequence_init()
{
    pwm_sequence.values = pwm_values;
    pwm_sequence.length = NRF_PWM_VALUES_LENGTH(pwm_values_individual);
    pwm_sequence.repeats = 0;
    pwm_sequence.end_delay = 0;
}

void pwm_handler(nrfx_pwm_evt_type_t event_type)
{
    //NRF_LOG_INFO("PWM");
}

void init_pwm()
{
    pwm_config_init();
    pwm_values.p_individual = &pwm_values_individual;
    nrfx_pwm_init(&pwm_instance, &pwm_config, pwm_handler);
    pwm_sequence_init();
}

void stop_pwm()
{
    nrfx_pwm_stop(&pwm_instance, false);
}

void start_pwm()
{
    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}

void set_pwm_led_0(const uint16_t color)
{
    pwm_values_individual.channel_0 = color;
}

void set_pwm_led_1(const RGB_16 *color)
{
    pwm_values_individual.channel_1 = color->R;
    pwm_values_individual.channel_2 = color->G;
    pwm_values_individual.channel_3 = color->B;
}