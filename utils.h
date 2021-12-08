#ifndef UTILS_H
#define UTILS_H
#pragma once

#include <stdint.h>

/**
 * @brief Clamps value between min and max values
 *
 * @param value Value to be clamped
 * @param min Minimum possible value
 * @param max Maximum possible value
 * @return int32_t Clamped value
 */
int32_t clamp(const int32_t value, const int32_t min, const int32_t max);

#endif