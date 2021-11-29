#ifndef MODES_H
#define MODES_H
#pragma once
#include "app_timer.h"

#include "colors.h"

/* HSB color values steps */
#define HUE_STEP        3
#define SATURATION_STEP 1
#define BRIGHTNESS_STEP 1

/* HSB color values changing steps delays */
#define HSB_STEP_DELAY_MS    20
#define HSB_STEP_DELAY_TICKS APP_TIMER_TICKS(HSB_STEP_DELAY_MS)

/* LED 0 value step */
#define LED_0_STEP 5

/* LED 0 values changing delays for different modes */
#define HUE_MODE_STEP_DELAY_MS        8
#define SATURATION_MODE_STEP_DELAY_MS 2

#define HUE_MODE_STEP_DELAY_TICKS        APP_TIMER_TICKS(HUE_MODE_STEP_DELAY_MS)
#define SATURATION_MODE_STEP_DELAY_TICKS APP_TIMER_TICKS(SATURATION_MODE_STEP_DELAY_MS)

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
    HSB hsb_delta;
} mode_config;

/**
 * @brief Returns current LED color changing mode
 *
 * @return Current mode
 */
modes get_current_mode();

/**
 * @brief Returns current LED color changing mode config
 *
 * @return Current mode config
 */
mode_config get_current_mode_config();

/**
 * @brief Switches to next mode in cycles
 * (...off -> changing hue -> changing saturation -> changing brightness -> off...)
 */
void next_mode();

#endif
