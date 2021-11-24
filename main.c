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

#include "nrfx_pwm.h"
#include "app_timer.h"

/**
 * @brief Procedure for logs initialization.
 */
void logs_init()
{
    ret_code_t ret = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(ret);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/* Timer definition. */
APP_TIMER_DEF(led_timer);

/* Empty pointer stub. */
void * stub = NULL;

/**
 * @brief LED color changing handler.
 */
void led_timer_handler(void *context)
{
    add_delta(&hsb_color, &hsb_delta);
    HSB_to_RGB_16(&hsb_color, &pwm_rgb_color, MAX_DC);
    *pwm_r = pwm_rgb_color.R;
    *pwm_g = pwm_rgb_color.G;
    *pwm_b = pwm_rgb_color.B;

    //NRF_LOG_INFO("Current RGB (%d, %d, %d)", *pwm_r, *pwm_g, *pwm_b);
    //NRF_LOG_INFO("Current HSB (%d, %d, %d)", hsb_color.H, hsb_color.S, hsb_color.B);
    //NRF_LOG_INFO("Current delta (%d, %d, %d)", hsb_delta.H, hsb_delta.S, hsb_delta.B);
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
    switch (current_mode)
    {
        case HUE:
            app_timer_start(led_timer, HUE_STEP_DELAY_TICKS, NULL);
            break;

        case SATURATION:
            app_timer_start(led_timer, SATURATION_STEP_DELAY_TICKS, NULL);
            break;

        case BRIGHTNESS:
            app_timer_start(led_timer, BRIGHTNESS_STEP_DELAY_TICKS, NULL);
            break;

        default:
            break;
    }
    //NRF_LOG_INFO("Hold started");
}

/**
 * @brief Event that triggers when button holding ends.
 * (Stops color changing timer.)
 */
void on_btn_hold_end()
{
    app_timer_stop(led_timer);
    //NRF_LOG_INFO("Hold ended");
}

/**
 * @brief Event that triggers when double click happens.
 * (Switches to the next color changing mode.)
 */
void on_btn_double_click()
{
    //NRF_LOG_INFO("Double click");
    next_mode(&hsb_color, &hsb_delta);
}

/* Configure color changing timer. */
void led_timer_init()
{
    app_timer_create(&led_timer, APP_TIMER_MODE_REPEATED, led_timer_handler);
    on_hold_start = on_btn_hold_start;
    on_hold_end = on_btn_hold_end;
    on_double_click = on_btn_double_click;
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Starting logs. */
    logs_init();

    /* Configure PWM. */
    pwm_init();

    /* Configure button. */
    button_init();

    /* Configure LED timer. */
    led_timer_init();

    /* Toggle LEDs according to current state. */
    while (true)
    {
        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
}

/**
 *@}
 **/
