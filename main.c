/**
 * Copyright (c) 2014 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"

#include "nordic_common.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "nrf_log_backend_usb.h"

#include "app_usbd.h"
#include "app_usbd_serial_num.h"

#include "nrfx_systick.h"

#include "nrf_gpiote.h"
#include "nrfx_gpiote.h"


#define LEDS_COUNT 4
#define BLINK_DELAY 500

#define BLINK_DELAY_MCS 500000
#define PWM_FREQ 1000

#define DOUBLE_CLICK_DELAY 100000

/**
 * @brief Struct for storing LED port and pin values
 */
typedef struct 
{
    uint32_t port;
    uint32_t pin;
} LED;


/**
 * @brief Procedure for logs initialization.
 */
void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief Procedure for initialization an array with LED pin numbers
 */
void LEDs_init(uint32_t *leds) 
{
    const LED pins_n_ports[] = 
    {
        {0, 6},
        {0, 8},
        {1, 9}, 
        {0, 12}
    };
    
    for (int i = 0; i < LEDS_COUNT; i++) 
    {
        leds[i] = NRF_GPIO_PIN_MAP(pins_n_ports[i].port, pins_n_ports[i].pin);
        nrf_gpio_cfg_output(leds[i]);
        nrf_gpio_pin_write(leds[i], 1);
    } 
}

/**
 * @brief Procedure-wrapper for toggling LEDs
 */
void LED_toggle(uint32_t *led) 
{
    nrf_gpio_pin_toggle(*led);
}

/* The time remaining until the next click for double-click to get detected. */
uint32_t mcs_dclick_time_left = 0;
/* Current state of LEDs behaviour. */
bool is_blinking = 0;

/* Double click event handler. */
void dclick(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
    if (mcs_dclick_time_left > 0)
    {
        //NRF_LOG_INFO("Double click!");
        is_blinking = !is_blinking;
        mcs_dclick_time_left = 0;
    }
    else
        mcs_dclick_time_left = DOUBLE_CLICK_DELAY;
}

/**
 * @brief Procedure for initialization button pin number and GPIOTE
 */
void button_init() 
{
    nrfx_gpiote_init();
    uint32_t button;
    button = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(button, NRF_GPIO_PIN_PULLUP);

    nrfx_gpiote_in_config_t button_config;
    button_config.sense = NRF_GPIOTE_POLARITY_LOTOHI;
    button_config.pull = NRF_GPIO_PIN_PULLUP;

    nrfx_gpiote_in_init(button, &button_config, dclick);
    nrfx_gpiote_in_event_enable(button, true);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Starting logs. */
    logs_init();
    NRF_LOG_INFO("Starting up the LED blinking project with USB logging.");

    /* Variable for counting time button is pressed. */
    int32_t mcs_counter = BLINK_DELAY_MCS;

    /* Variable for counting time when button is released. */
    int32_t mcs_counter_static = 0;

    /* Configure button. */
    button_init();

    /* Configure LEDs. */
    uint32_t LEDs[LEDS_COUNT];
    LEDs_init(LEDs);
    uint8_t current_LED = 0;
    
    /* My board ID is #6587. */
    const uint8_t blink_counts[LEDS_COUNT] = {6, 5, 8, 7};
    uint8_t blinks_left = blink_counts[current_LED] * 2;


    /* Configure timer. */
    nrfx_systick_init();
    nrfx_systick_state_t time_state = {0};
    nrfx_systick_get(&time_state);

    /* DC - duty cycle. */

    /* Time in microseconds during which DC value should not change. */
    uint32_t PWM_delay = BLINK_DELAY_MCS / 100;
    /* Current DC value (0-100%). */
    uint32_t current_DC = 100;

    /* 
    1% of time of each PWM cycle.
    (10 000 - 1% of a second in microseconds)
    */
    uint32_t PWM_step = 10000 / PWM_FREQ;

    /* The "direction" towards which DC value will change (increment/decrement). */
    int8_t DC_delta = -1;

    /* Array with t_on, and t_off values in microseconds for current DC state. */
    int32_t DC_times[2] = {PWM_step * current_DC, PWM_step * (100 - current_DC)};
    /* Current DC time index (t_on or t_off). */
    size_t DC_times_index = 0;

    /* Timestamp of last DC value change. */
    int32_t last_DC_change_time = BLINK_DELAY_MCS;


    /* Toggle LEDs when button is pressed. */
    while (true)
    {
        if (nrfx_systick_test(&time_state, 1))
        {
            nrfx_systick_get(&time_state);
            mcs_counter_static++;

            if (mcs_dclick_time_left > 0)
                mcs_dclick_time_left--;

            if (mcs_counter_static >= DC_times[DC_times_index])
            {
                mcs_counter_static = 0;
                LED_toggle(&LEDs[current_LED]);
                DC_times_index = ~DC_times_index & 1;
            }

            if (is_blinking) 
            {
                mcs_counter--;

                if (mcs_counter <= 0)
                {
                    //NRF_LOG_INFO("Counter zeroed.");
                    last_DC_change_time = BLINK_DELAY_MCS;
                    mcs_counter = BLINK_DELAY_MCS;

                    DC_delta *= -1;

                    blinks_left -= 1;
                    if (blinks_left == 0) 
                    {
                        current_LED = (current_LED + 1) % LEDS_COUNT;
                        blinks_left = blink_counts[current_LED] * 2;
                    }
                }

                if (mcs_counter <= last_DC_change_time - PWM_delay)
                {
                    //NRF_LOG_INFO("DC changed.");
                    last_DC_change_time = mcs_counter;
                    current_DC += DC_delta;

                    DC_times[0] = PWM_step * current_DC; 
                    DC_times[1] = PWM_step * (100 - current_DC);
                    mcs_counter_static = DC_times[DC_times_index]; 
                }
            }
        }

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
