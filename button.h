#ifndef BUTTON_H
#define BUTTON_H
#pragma once

#include <stdint.h>
#include "nrf_gpiote.h"
#include "nrfx_gpiote.h"

#define BUTTON_PORT 1
#define BUTTON_PIN 6
#define DOUBLE_CLICK_DELAY_MS 500
#define DOUBLE_CLICK_DELAY_TICKS APP_TIMER_TICKS(DOUBLE_CLICK_DELAY_MS)
#define ANTIBOUNCE_DELAY_MS 100
#define ANTIBOUNCE_DELAY_TICKS APP_TIMER_TICKS(ANTIBOUNCE_DELAY_MS)

/* Function poitner for button events */
typedef void (*btn_evt_t)();

/**
 * @brief Procedure for initialization button pin, GPIOTE, timer
 */
void init_button();

/**
 * @brief On_hold_start event setter
 *
 * @param hold_start_event Event that will be executed when button hold will start
 */
void set_hold_start_event(btn_evt_t hold_start_event);

/**
 * @brief On_hold_end event setter
 *
 * @param hold_end_event Event that will be executed when button hold will end
 */
void set_hold_end_event(btn_evt_t hold_end_event);

/**
 * @brief On_double_click event setter
 *
 * @param double_click_event Event that will be executed when double click will happen
 */
void set_double_click_event(btn_evt_t double_click_event);


#endif