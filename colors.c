#include "colors.h"
#include "modes.h"

RGB_16 pwm_rgb_color = {0, 0, 0};
HSB hsb_color = {0, 50, 50};
HSB_delta hsb_delta = {0, 0, 0};

void HSB_to_N_RGB(HSB *in, N_RGB *out)
{
    float _h = (float)in->H / 60.0f;
    float _s = (float)in->S / 100.0f;
    float _b = (float)in->B / 100.0f;

    float c = _s * _b;
    float x = c * (1 - fabs(fmod(_h, 2.0f) - 1));
    float m = _b - c;

    float _red = 0.0f;
    float _gre = 0.0f;
    float _blu = 0.0f;

    if (0.0f <= _h && _h < 1.0f)
    {
        _red = c;
        _gre = x;
    }
    else if (1.0f <= _h && _h < 2.0f)
    {
        _red = x;
        _gre = c;
    }
    else if (2.0f <= _h && _h < 3.0f)
    {
        _gre = c;
        _blu = x;
    }
    else if (3.0f <= _h && _h < 4.0f)
    {
        _gre = x;
        _blu = c;
    }
    else if (4.0f <= _h && _h < 5.0f)
    {
        _red = x;
        _blu = c;
    }
    else if (5.0f <= _h && _h < 6.0f)
    {
        _red = c;
        _blu = x;
    }

    out->R = _red + m;
    out->G = _gre + m;
    out->B = _blu + m;
}

void HSB_to_RGB_16(HSB *in, RGB_16 *out, uint16_t factor)
{
    N_RGB nrgb;
    HSB_to_N_RGB(in, &nrgb);

    out->R = (uint16_t)(nrgb.R * (float)factor);
    out->G = (uint16_t)(nrgb.G * (float)factor);
    out->B = (uint16_t)(nrgb.B * (float)factor);
}

void add_hsb(HSB *color, HSB_delta *addenum)
{
    color->H += addenum->H;
    color->H %= (MAX_HUE + 1);

    color->S += addenum->S;
    color->S %= (MAX_SATURATION + 1);

    color->B += addenum->B;
    color->B %= (MAX_BRIGHTNESS + 1);
}

void add_delta(HSB *color, HSB_delta *delta)
{
    add_hsb(color, delta);

    if (
        (current_mode == HUE        && (color->H == 0 || color->H == MAX_HUE))        ||
        (current_mode == SATURATION && (color->S == 0 || color->S == MAX_SATURATION)) ||
        (current_mode == BRIGHTNESS && (color->B == 0 || color->B == MAX_BRIGHTNESS))
       )
    {
        reverse(delta);
    }
}