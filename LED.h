#ifndef LED_H
#define LED_H
#pragma once

#include <stdint.h>
#include "colors.h"
#include "nrf_gpio.h"

/* Pins values configuration for LEDs */
#define LED_0 NRF_GPIO_PIN_MAP(0, 6)
#define LED_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_B NRF_GPIO_PIN_MAP(0, 12)

/**
 * @brief Configures LEDs
 */
void init_leds();

/**
 * @brief Starts LED 0 timer
 *
 * @param delay Time in ticks that passes to app_timer_start function
 */
void start_led0(const uint32_t delay);

/**
 * @brief Starts LED 1 timer
 *
 * @param delay Time in ticks that passes to app_timer_start function
 */
void start_led1(const uint32_t delay);

/**
 * @brief Stops LED 0 timer
 */
void stop_led0();

/**
 * @brief Stops LED 1 timer
 */
void stop_led1();

/**
 * @brief Turns off LED 0 (and stops its timer so LED wouldn't blink)
 */
void turn_off_led0();

/**
 * @brief Turns on LED 0 (and stops its timer so LED wouldn't blink)
 */
void turn_on_led0();

/**
 * @brief Turns off LED 1 (and stops its timer so LED wouldn't blink)
 */
void turn_off_led1();

/**
 * @brief Sets the hsb delta parameter
 *
 * @param new_delta New delta value
 */
void set_hsb_delta(const HSB new_delta);

/**
 * @brief Returns current HSB color value
 *
 * @return Current HSB color value
 */
HSB get_current_hsb();


/**
 * @brief Sets current HSB color value with *color value
 *
 * @param new_delta New HSB color value
 */
void set_current_hsb(const HSB *color);


/**
 * @brief Returns current RGB color value
 *
 * @return Current RGB color value
 */
RGB_16 get_current_rgb();

/**
 * @brief Saves current HSB color into nvm
 */
void save_current_hsb();

#endif