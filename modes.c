#include "modes.h"

modes current_mode = OFF;
const mode_config MODE_CONFIGS[MODES_COUNT] =
{
    {OFF,        {0, 0, 0}},
    {HUE,        {HUE_STEP, 0, 0}},
    {SATURATION, {0, SATURATION_STEP, 0}},
    {BRIGHTNESS, {0, 0, BRIGHTNESS_STEP}}
};

void reverse(HSB_delta *color_delta)
{
    color_delta->H *= -1;
    color_delta->S *= -1;
    color_delta->B *= -1;
}

void next_mode(HSB *color, HSB_delta *color_delta)
{
    current_mode = (current_mode + 1) % 4;
    *color_delta = MODE_CONFIGS[current_mode].delta;
    if (current_mode == OFF)
    {
        pwm_stop();
    }
    else if (current_mode == HUE)
    {
        pwm_start();
    }

}