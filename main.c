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

#include "LED.h"
#include "button.h"
#include "PWM.h"

/* The time remaining until the next click for double-click to get detected. */
extern uint32_t mcs_dclick_time_left;
/* Current state of LEDs behaviour. */
extern bool is_blinking;

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
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Starting logs. */
    logs_init();

    /* Variable for counting time when button is pressed. */
    int32_t mcs_counter = BLINK_DELAY_MCS;

    /* Variable for counting time when button is released. */
    int32_t mcs_counter_static = 0;

    /* Configure button. */
    button_init();

    /* Configure LEDs. */
    LEDs_config LEDs_config;
    LEDs_init(&LEDs_config);

    /* Configure PWM. */
    PWM_config PWM_config;
    PWM_init(&PWM_config);

    /* Configure timer. */
    nrfx_systick_init();
    nrfx_systick_state_t time_state = {0};
    nrfx_systick_get(&time_state);


    /* Toggle LEDs according to current state. */
    while (true)
    {
        if (nrfx_systick_test(&time_state, 1))
        {
            nrfx_systick_get(&time_state);
            mcs_counter_static++;

            if (mcs_dclick_time_left > 0)
                mcs_dclick_time_left--;

            if (mcs_counter_static >= get_current_DC_time(&PWM_config))
            {
                mcs_counter_static = 0;
                PWM_LED_toggle(&PWM_config, &LEDs_config);
            }

            if (is_blinking) 
            {
                mcs_counter--;

                if (mcs_counter <= 0)
                {
                    //NRF_LOG_INFO("Counter zeroed.");
                    mcs_counter = BLINK_DELAY_MCS;
                    PWM_set_blink(&PWM_config);
                    LED_set_blink(&LEDs_config);
                }

                if (mcs_counter <= PWM_config.last_DC_change_time - PWM_config.PWM_delay)
                {
                    //NRF_LOG_INFO("DC changed.");
                    mcs_counter_static = get_current_DC_time(&PWM_config);
                    change_DC(&PWM_config, &mcs_counter); 
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
