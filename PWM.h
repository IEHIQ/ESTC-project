#ifndef PWM_H
#define PWM_H
#pragma once

#include <stddef.h>
#include "LED.h"
#include "nrfx_pwm.h"

#define MAX_DC 1023

/* Pointers for each subLED (for convenience) */
extern uint16_t * pwm_r;
extern uint16_t * pwm_g;
extern uint16_t * pwm_b;

/* Configures PWM */
void pwm_init();

/* Stops PWM playback */
void pwm_stop();

/* Starts PWM playback */
void pwm_start();

#endif