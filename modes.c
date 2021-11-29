#include "modes.h"
#define MODES_COUNT 4

static modes current_mode = OFF;
static const mode_config MODE_CONFIGS[MODES_COUNT] =
{
    {OFF,        {0, 0, 0}},
    {HUE,        {HUE_STEP, 0, 0}},
    {SATURATION, {0, SATURATION_STEP, 0}},
    {BRIGHTNESS, {0, 0, BRIGHTNESS_STEP}}
};

modes get_current_mode()
{
    return current_mode;
}

mode_config get_current_mode_config()
{
    return MODE_CONFIGS[current_mode];
}

void next_mode()
{
    current_mode = (current_mode + 1) % 4;
}