#include "PWM.h"

void PWM_init(PWM_config *config)
{
    config->PWM_delay = BLINK_DELAY_MCS / 100;
    config->current_DC = 100;
    config->PWM_step = 10000 / PWM_FREQ;
    config->DC_delta = -1;
    config->DC_times[0] = config->PWM_step * config->current_DC;
    config->DC_times[1] = 0;
    config->DC_times_index = 0;
    config->last_DC_change_time = BLINK_DELAY_MCS;
}

void toggle_DC_times_index(PWM_config *config)
{
    config->DC_times_index = ~(config->DC_times_index) & 1;
}

void PWM_set_blink(PWM_config *config)
{
    config->DC_delta *= -1;
    config->last_DC_change_time = BLINK_DELAY_MCS;
}

void change_DC(PWM_config *config, int32_t *time) 
{
    config->last_DC_change_time = *time;
    config->current_DC += config->DC_delta;
    config->DC_times[0] = config->PWM_step * config->current_DC; 
    config->DC_times[1] = config->PWM_step * (100 - config->current_DC);
}

void PWM_LED_toggle(PWM_config *PWM_config, LEDs_config *LEDs_config)
{
    LED_toggle(&(LEDs_config->LEDs[LEDs_config->current_LED]));
    toggle_DC_times_index(PWM_config);
}

int32_t get_current_DC_time(PWM_config *PWM_config)
{
    return PWM_config->DC_times[PWM_config->DC_times_index];
}
