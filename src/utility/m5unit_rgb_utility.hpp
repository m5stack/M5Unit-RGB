/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file m5unit_rgb_utility.hpp
  @brief Utilities for M5Unit-RGB
*/
#ifndef M5_UNIT_RGB_UTILITY_M5UNIT_RGB_UTILITY_HPP
#define M5_UNIT_RGB_UTILITY_M5UNIT_RGB_UTILITY_HPP
#include <cstdint>

namespace m5 {
namespace unit {
namespace rgb {

//! @brief HSV to RGB
uint32_t hsv2rgb(const uint8_t h, const uint8_t s, const uint8_t v);

/*!
  @brief Rounded 8-bit scale (divide-by-255)
  @param a Value to scale (0-255)
  @param b Scale factor (0-255, where 255 = 100%)
  @return `(a * b + 127) / 255`. Maps `scale8(255, 255) == 255` exactly (`>> 8` would give 254).
 */
inline uint8_t scale8(const uint8_t a, const uint8_t b)
{
    return static_cast<uint8_t>((static_cast<uint16_t>(a) * b + 127) / 255);
}

}  // namespace rgb
}  // namespace unit
}  // namespace m5

#endif
