#ifndef COLORS_H
#define COLORS_H
#pragma once

#include <stdint.h>
#include <math.h>

#define MAX_HUE 360
#define MAX_SATURATION 100
#define MAX_BRIGHTNESS 100

/* Normalized RGB */
typedef struct
{
    float R; //[0-1]
    float G; //[0-1]
    float B; //[0-1]
} N_RGB;

/*
    Extended RGB for PWM
    (apparently the range of the PWM duty cycle
    in PWM driver exceeds the range of the RGB value)
*/
typedef struct
{
    uint16_t R;
    uint16_t G;
    uint16_t B;
} RGB_16;

typedef struct
{
    uint16_t H; //0-360
    uint8_t S; //0-100
    uint8_t B; //0-100
} HSB;

/* HSB delta struct for HSB color changing (with possible negative values) */
typedef struct
{
    int8_t H;
    int8_t S;
    int8_t B;
} HSB_delta;

/* Colors */
extern RGB_16 pwm_rgb_color;
extern HSB_delta hsb_delta;
extern HSB hsb_color;

/* Translates HSB to normalized RGB */
void HSB_to_N_RGB(HSB *in, N_RGB *out);

/* Translates HSB to extended RGB */
void HSB_to_RGB_16(HSB *in, RGB_16 *out, uint16_t factor);

/* Addes *addenum HSB values to the corresponding *color HSB values */
void add_hsb(HSB *color, HSB_delta *addenum);

/*
    Same *delta to *color addition with extra step of reversing delta
    when one of *color values reaches its HSB min/max range.
*/
void add_delta(HSB *color, HSB_delta *delta);

#endif