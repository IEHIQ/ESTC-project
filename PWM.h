#ifndef PWM_H
#define PWM_H
#pragma once

#include <stddef.h>
#include "LED.h"

#define PWM_FREQ 1000

typedef struct
{
    /* DC - duty cycle. */

    /* Time in microseconds during which DC value should not change. */
    uint32_t PWM_delay;
    /* 
    1% of time of each PWM cycle.
    (10 000 - 1% of a second in microseconds)
    */
    uint32_t PWM_step;
    /* Current DC value (0-100%). */
    uint32_t current_DC;
    /* The "direction" towards which DC value will change (increment/decrement). */
    int8_t DC_delta;
    /* Array with t_on, and t_off values in microseconds for current DC state. */
    int32_t DC_times[2];
    /* Current DC time index (t_on or t_off). */
    size_t DC_times_index;
    /* Timestamp of last DC value change. */
    int32_t last_DC_change_time;

} PWM_config;

/**
 * @brief Peocedure for PWM config initialization
 */
void PWM_init(PWM_config *config);

/**
 * @brief Procedure for toggling DC_times_index value of a PWM config
 */
void toggle_DC_times_index(PWM_config *config);

/**
 * @brief Procedure for switching values of a PWM config for next blink phase (lighting or fading)
 */
void PWM_set_blink(PWM_config *config);

/**
 * @brief Procedure for changing DC value of PWM config
 */
void change_DC(PWM_config *config, int32_t *time);

/**
 * @brief Procedure for toggling current LED and toggle DC_times_index PWM config parameter
 */
void PWM_LED_toggle(PWM_config *PWM_config, LEDs_config *LEDs_config);

/**
 * @brief Function that returns current_DC_time parameter value  
 */
int32_t get_current_DC_time(PWM_config *PWM_config);

#endif