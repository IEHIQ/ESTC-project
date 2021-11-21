#ifndef LED_H
#define LED_H
#pragma once

#include <stdint.h>

#define LEDS_COUNT 4
#define BLINK_DELAY 500
#define BLINK_DELAY_MCS 100000

/**
 * @brief Struct for storing LED port and pin values
 */
typedef struct 
{
    uint32_t port;
    uint32_t pin;
} LED;

/**
 * @brief Struct for storing LEDs config
 */
typedef struct 
{
    uint32_t LEDs[LEDS_COUNT];
    uint8_t current_LED;
    uint8_t blinks_left;
} LEDs_config;

/**
 * @brief Procedure for initialization an array with LED pin numbers
 */
void LEDs_init(LEDs_config *config);

/**
 * @brief Procedure-wrapper for toggling LEDs
 */
void LED_toggle(uint32_t *led);

/**
 * @brief Procedure for switching values of a LED config for next blink phase (lighting or fading)
 */
void LED_set_blink(LEDs_config *config);

#endif