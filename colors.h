#ifndef COLORS_H
#define COLORS_H
#pragma once

#include <stdint.h>
#include <math.h>

#define MAX_HUE 360
#define MAX_SATURATION 100
#define MAX_BRIGHTNESS 100

/**
 * @brief Normalized RGB struct
 */
typedef struct
{
    float R; //[0-1]
    float G; //[0-1]
    float B; //[0-1]
} NRGB;

/**
 * @brief Extended RGB for PWM
 */
typedef struct
{
    uint16_t R;
    uint16_t G;
    uint16_t B;
} RGB_16;

/**
 * @brief HSB color struct
 */
typedef struct
{
    uint16_t H; //0-360
    uint8_t S; //0-100
    uint8_t B; //0-100
} HSB;

/**
 * @brief Translates HSB to normalized RGB
 *
 * @param in Initial color
 * @param out Translated color
 */
void HSB_to_N_RGB(const HSB *in, NRGB *out);

/*  */
/**
 * @brief Translates HSB to extended RGB
 *
 * @param in Initial color
 * @param out Translated color
 * @param factor Maximum value of translated color (normalized RGB multiplier basically)
 */
void HSB_to_RGB_16(const HSB *in, RGB_16 *out, const uint16_t factor);

/**
 * @brief Adds *addenum HSB values to the corresponding *color HSB values
 * and clamps result between HSB color scheme bounaries
 *
 * @param color initial color
 * @param addenum color values of which will be added from initial color values
 */
void hsb_add(HSB *color, const HSB *addenum);

/**
 * @brief Subtracts *subtrahend HSB values from the corresponding *color HSB values
 * and clamps result between HSB color scheme bounaries
 *
 * @param color initial color
 * @param subtrahend color values of which will be subtracted from initial color
 */
void hsb_subtract(HSB *color, const HSB *subtrahend);

#endif
