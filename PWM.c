#include "PWM.h"

/* PWM configure params */
static nrfx_pwm_t pwm_instance = NRFX_PWM_INSTANCE(0);
static nrfx_pwm_config_t pwm_config = NRFX_PWM_DEFAULT_CONFIG;
static nrf_pwm_values_individual_t pwm_values_individual[] = {{ 0, 0, 0, 0 }};
static nrf_pwm_values_t pwm_values;
static nrf_pwm_sequence_t pwm_sequence;

uint16_t * pwm_r = &pwm_values_individual[0].channel_1;
uint16_t * pwm_g = &pwm_values_individual[0].channel_2;
uint16_t * pwm_b = &pwm_values_individual[0].channel_3;

/* Configures PWM sequence */
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

void pwm_init()
{
    pwm_config.load_mode = NRF_PWM_LOAD_INDIVIDUAL;

    pwm_config.output_pins[0] = NRFX_PWM_PIN_NOT_USED;
    pwm_config.output_pins[1] = LED_R | NRFX_PWM_PIN_INVERTED;
    pwm_config.output_pins[2] = LED_G | NRFX_PWM_PIN_INVERTED;
    pwm_config.output_pins[3] = LED_B | NRFX_PWM_PIN_INVERTED;

    pwm_values.p_individual = pwm_values_individual;
    nrfx_pwm_init(&pwm_instance, &pwm_config, pwm_handler);
    pwm_sequence_init();
}

void pwm_stop()
{
    nrfx_pwm_stop(&pwm_instance, false);
}

void pwm_start()
{
    nrfx_pwm_simple_playback(&pwm_instance, &pwm_sequence, 1, NRFX_PWM_FLAG_LOOP);
}