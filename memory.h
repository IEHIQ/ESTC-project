#ifndef MEMORY_H
#define MEMORY_H
#pragma once

#include <stdint.h>
#include "colors.h"

/**
 * @brief Initializes memory parameters
 */
void init_memory();

/**
 * @brief Saves HSB color into nvm
 *
 * @param color Color to be saved
 */
void save_HSB(const HSB *color);

/**
 * @brief Loads last saved HSB color from nvm
 *
 * @param color Variable to store loaded color
 */
void load_HSB(HSB *color);


#endif