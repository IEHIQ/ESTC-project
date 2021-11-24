#ifndef LED_H
#define LED_H
#pragma once

#include <stdint.h>
#include "nrf_gpio.h"

#define LED_R NRF_GPIO_PIN_MAP(0, 8)
#define LED_G NRF_GPIO_PIN_MAP(1, 9)
#define LED_B NRF_GPIO_PIN_MAP(0, 12)

#endif