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


#define LEDS_COUNT 4
#define BLINK_DELAY 500

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
 * @brief Struct for storing LED port and pin values
 */
typedef struct 
{
    uint32_t port;
    uint32_t pin;
} LED;


/**
 * @brief Procedure for initialization an array with LED pin numbers
 */
void init_leds(uint32_t *leds) 
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
void toggle_led(uint32_t *led) 
{
    nrf_gpio_pin_toggle(*led);
}

/**
 * @brief Procedure for initialization button pin number
 */
void init_button(uint32_t *button) 
{
    *button = NRF_GPIO_PIN_MAP(1, 6);
    nrf_gpio_cfg_input(*button, NRF_GPIO_PIN_PULLUP);
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    /* Starting logs. */
    logs_init();
    NRF_LOG_INFO("Starting up the LED blinking project with USB logging.");

    /* Variable for counting LED blinking delay when button is pressed. */
    unsigned short counter_ms = 0;
    /* Variable for storing the rest of LED blinking delay after button was released. */
    unsigned short left_ms = 0;

    /* Configure button. */
    uint32_t button;
    init_button(&button);

    /* Configure LEDs. */
    uint32_t leds[LEDS_COUNT];
    init_leds(leds);
    uint8_t current_led = 0;
    

    /* My board ID is #6587. */
    const uint8_t blink_counts[LEDS_COUNT] = {6, 5, 8, 7};
    uint8_t blinks_left = blink_counts[current_led] * 2;

    /* Toggle LEDs when button is pressed. */
    while (true)
    {
        NRF_LOG_INFO("Millisecond counter : %d", counter_ms);

        if (!nrf_gpio_pin_read(button)) 
        {
            if (counter_ms >= BLINK_DELAY - left_ms) 
            {
                left_ms = 0;
                counter_ms = 0;
                toggle_led(&leds[current_led]);
                blinks_left -= 1;
                if (blinks_left == 0) 
                {
                    current_led = (current_led + 1) % LEDS_COUNT;
                    blinks_left = blink_counts[current_led] * 2;
                }
            }

            nrf_delay_ms(1);
            counter_ms += 1;
        }
        else 
        {
            if (left_ms != counter_ms)
                left_ms = counter_ms;
        }

        LOG_BACKEND_USB_PROCESS();
        NRF_LOG_PROCESS();
    }
    
}

/**
 *@}
 **/
