#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <stdint.h>
#include "nrf_gpiote.h"
#include "nrfx_gpiote.h"

#define BUTTON_PORT 1
#define BUTTON_PIN 6
#define DOUBLE_CLICK_DELAY_MS 250
#define DOUBLE_CLICK_DELAY_TICKS APP_TIMER_TICKS(DOUBLE_CLICK_DELAY_MS)

/* Button state type */
typedef enum
{
    RELEASED,
    PRESSED
} btn_state_t;

/* Function poitner for button events */
typedef void (*event)();

/* Events */
extern event on_hold_start;
extern event on_hold_end;
extern event on_double_click;

/**
 * @brief Procedure for initialization button pin, GPIOTE, timer
 */
void button_init();

#endif