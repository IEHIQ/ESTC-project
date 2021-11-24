#ifndef MODES_H
#define MODES_H
#pragma once
#include "app_timer.h"

#include "colors.h"
#include "PWM.h"

#define MODES_COUNT 4

/* HSB delta values */
#define HUE_STEP 1
#define SATURATION_STEP 1
#define BRIGHTNESS_STEP 1

#define ZERO 0
#define HUE_STEP_DELAY_TICKS APP_TIMER_TICKS(10) //Blink time: 360 * 10 = 3600 ms
#define SATURATION_STEP_DELAY_TICKS APP_TIMER_TICKS(10) //Blink time: 100 * 10 = 1000 ms
#define BRIGHTNESS_STEP_DELAY_TICKS APP_TIMER_TICKS(5) //Blink time: 100 * 5 = 500 ms

/* LED color changing modes */
typedef enum
{
    OFF,
    HUE,
    SATURATION,
    BRIGHTNESS
} modes;

/* LED color changing mode config */
typedef struct
{
    modes mode;
    HSB_delta delta;
} mode_config;

/* Array for storing all modes values */
extern const mode_config MODE_CONFIGS[MODES_COUNT];

extern modes current_mode;

/* Reverses delta (multiplies all HSB values by -1) */
void reverse(HSB_delta *color_delta);

/* Switches to next mode (...off -> changing hue -> changing saturation -> changing brightness -> off...) */
void next_mode(HSB *color, HSB_delta *color_delta);

#endif
