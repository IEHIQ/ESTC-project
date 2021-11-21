#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <stdint.h>
#include "nrf_gpiote.h"
#include "nrfx_gpiote.h"

#define BUTTON_PORT 1
#define BUTTON_PIN 6
#define DOUBLE_CLICK_DELAY 100000

/**
 * @brief Procedure for initialization button pin number and GPIOTE
 */
void button_init();

/**
 * @brief Double click event handler
 */
void dclick(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);

#endif