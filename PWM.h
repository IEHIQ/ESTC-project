#ifndef PWM_H
#define PWM_H
#pragma once

#include <stddef.h>
#include "colors.h"
#include "nrfx_pwm.h"

#define MAX_DC 1000

/**
 * @brief Configures PWM
 */
void init_pwm();

/**
 * @brief Stops PWM playback
 */
void stop_pwm();

/**
 * @brief Starts PWM playback
 */
void start_pwm();

/**
 * @brief Sets duty cycle value for LED 0
 *
 * @param color Value of duty cycle
 */
void set_pwm_led_0(const uint16_t color);

/**
 * @brief Sets duty cycle values for each of RGB LED channels
 *
 * @param color Values for each of RGB LED channels
 */
void set_pwm_led_1(const RGB_16 *color);

#endif