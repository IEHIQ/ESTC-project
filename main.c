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

#include "button.h"
#include "LED.h"
#include "colors.h"
#include "PWM.h"
#include "modes.h"
#include "memory.h"
#include "USB.h"


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
 * @brief Event that triggers when button holding begins.
 * (Launches specific color changing timer according to current mode.)
 */
void on_btn_hold_start()
{
    /*
        For some reason timer works correctly only with <#define> delays
        (with variables handler triggers only once).
        Perhaps i misunderstood something here
    */
   start_led1(HSB_STEP_DELAY_TICKS);
    NRF_LOG_DEBUG("Hold started");
}

/**
 * @brief Event that triggers when button holding ends.
 * (Stops color changing timer.)
 */
void on_btn_hold_end()
{
    stop_led1();
    NRF_LOG_DEBUG("Hold ended");
}

/**
 * @brief Event that triggers when double click happens.
 * (Switches to the next color changing mode.)
 */
void on_btn_double_click()
{
    stop_led0();
    next_mode();
    set_hsb_delta(get_current_mode_config().hsb_delta);
    switch (get_current_mode())
    {
        case HUE:
            start_led0(HUE_MODE_STEP_DELAY_TICKS);
            break;
        case SATURATION:
            start_led0(SATURATION_MODE_STEP_DELAY_TICKS);
            break;
        case BRIGHTNESS:
            turn_on_led0();
            break;
        default:
            turn_off_led0();
            save_current_hsb();
            break;
    }
}

/**
 * @brief Sets button events
 */
void set_button_events()
{
    set_hold_start_event(on_btn_hold_start);
    set_hold_end_event(on_btn_hold_end);
    set_double_click_event(on_btn_double_click);
}

void on_rgb_command(void *context)
{
    RGB_8 new_rgb =
    {
        *((int32_t *)(context)),
        *((int32_t *)(context + sizeof(int32_t))),
        *((int32_t *)(context + sizeof(int32_t) * 2))
    };
    NRF_LOG_DEBUG("Color set to (%d, %d, %d)", new_rgb.R, new_rgb.G, new_rgb.B);
    HSB new_hsb;
    RGB_8_to_HSB(&new_rgb, &new_hsb);
    set_current_hsb(&new_hsb);
}

void set_usb_events()
{
    set_rgb_com_event(on_rgb_command);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Init app_timer */
    app_timer_init();
    /* Starting logs. */
    logs_init();
    /* Init memory params*/
    init_memory();
    /* Configure LEDs */
    init_leds();
    /* Configure PWM. */
    init_pwm();
    /* Configure button. */
    init_button();
    set_button_events();
    /* Start PWM playback */
    start_pwm();

    init_usb();
    set_usb_events();

    while (true)
    {
        while (app_usbd_event_queue_process())
        {
            /* Nothing to do */
        }

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
