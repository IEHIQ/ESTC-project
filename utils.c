#include "utils.h"

int32_t clamp(const int32_t value, const int32_t min, const int32_t max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}