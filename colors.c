#include "colors.h"
#include "utils.h"

void HSB_to_N_RGB(const HSB *in, NRGB *out)
{
    float hue_f = (float)in->H / 60.0f;
    float sat_f = (float)in->S / 100.0f;
    float bri_f = (float)in->B / 100.0f;

    float c = sat_f * bri_f;
    float x = c * (1 - fabs(fmod(hue_f, 2.0f) - 1));
    float m = bri_f - c;

    float nr = 0.0f;
    float ng = 0.0f;
    float nb = 0.0f;

    if (0.0f <= hue_f && hue_f < 1.0f)
    {
        nr = c;
        ng = x;
    }
    else if (1.0f <= hue_f && hue_f < 2.0f)
    {
        nr = x;
        ng = c;
    }
    else if (2.0f <= hue_f && hue_f < 3.0f)
    {
        ng = c;
        nb = x;
    }
    else if (3.0f <= hue_f && hue_f < 4.0f)
    {
        ng = x;
        nb = c;
    }
    else if (4.0f <= hue_f && hue_f < 5.0f)
    {
        nr = x;
        nb = c;
    }
    else if (5.0f <= hue_f && hue_f < 6.0f)
    {
        nr = c;
        nb = x;
    }

    out->R = nr + m;
    out->G = ng + m;
    out->B = nb + m;
}

void HSB_to_RGB_16(const HSB *in, RGB_16 *out, const uint16_t factor)
{
    NRGB nrgb;
    HSB_to_N_RGB(in, &nrgb);

    out->R = (uint16_t)(nrgb.R * (float)factor);
    out->G = (uint16_t)(nrgb.G * (float)factor);
    out->B = (uint16_t)(nrgb.B * (float)factor);
}

void hsb_add(HSB *color, const HSB *addenum)
{
    color->H = clamp(color->H + addenum->H, 0, MAX_HUE);
    color->S = clamp(color->S + addenum->S, 0, MAX_SATURATION);
    color->B = clamp(color->B + addenum->B, 0, MAX_BRIGHTNESS);
}

void hsb_subtract(HSB *color, const HSB *subtrahend)
{
    color->H = clamp(color->H - subtrahend->H, 0, MAX_HUE);
    color->S = clamp(color->S - subtrahend->S, 0, MAX_SATURATION);
    color->B = clamp(color->B - subtrahend->B, 0, MAX_BRIGHTNESS);
}

uint32_t HSB_to_word(const HSB *color)
{
    return (uint32_t)((color->H<<16) + (color->S<<8) + color->B);
}

void word_to_HSB(uint32_t word, HSB *color)
{
    color->B = (uint8_t)(word & 255);
    word >>= 8;
    color->S = (uint8_t)(word & 255);
    word >>= 8;
    color->H = (uint16_t)(word & 65535);
}

void RGB_8_to_HSB(const RGB_8 *rgb, HSB *hsb)
{
    float nr = (float)rgb->R / 255.0f;
    float ng = (float)rgb->G / 255.0f;
    float nb = (float)rgb->B / 255.0f;

    float cmax = fmax(fmax(nr, ng), nb);
    float cmin = fmin(fmin(nr, ng), nb);
    float delta = cmax - cmin;

    float hue_f = 0.0f;
    float sat_f = 0.0f;
    float bri_f = 0.0f;

    if (delta == 0.0f)
    {
        hue_f = 0;
    }
    else if (cmax == nr)
    {
        hue_f = ((ng - nb) / delta) * 60.0f;
    }
    else if (cmax == ng)
    {
        hue_f = ((nb - nr) / delta + 2) * 60.0f;
    }
    else if (cmax == nb)
    {
        hue_f = ((nr - ng) / delta + 4) * 60.0f;
    }

    if (cmax == 0)
    {
        sat_f = 0;
    }
    else
    {
        sat_f = delta / cmax;
    }

    bri_f = cmax;

    if (hue_f < 0)
    {
        hue_f +=360;
    }

    hsb->H = hue_f;
    hsb->S = sat_f * 100;
    hsb->B = bri_f * 100;
}

void RGB_8_to_RGB_16(const RGB_8 *rgb_8, RGB_16 *rgb_16, const uint16_t max_color)
{
    rgb_16->R = (uint16_t)((float)rgb_8->R / 255.0f * (float)max_color);
    rgb_16->G = (uint16_t)((float)rgb_8->G / 255.0f * (float)max_color);
    rgb_16->B = (uint16_t)((float)rgb_8->B / 255.0f * (float)max_color);
}