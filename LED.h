#ifndef LED_H
#define LED_H
#pragma once

#include <stdint.h>

#define LEDS_COUNT 4
#define BLINK_DELAY 500
#define BLINK_DELAY_MCS 100000

extern const uint32_t LED_gpio_pins[LEDS_COUNT];

/**
 * @brief Struct for storing LEDs config
 */
typedef struct 
{
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
void LED_toggle(uint8_t *LED_num);

/**
 * @brief Procedure for switching values of a LED config for next blink phase (lighting or fading)
 */
void LED_set_blink(LEDs_config *config);

#endif