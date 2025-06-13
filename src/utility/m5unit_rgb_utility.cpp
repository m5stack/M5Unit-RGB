/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file m5unit_rgb_utility.cpp
  @brief Utilities for M5Unit-RGB
*/
#include "m5unit_rgb_utility.hpp"

namespace m5 {
namespace unit {
namespace rgb {

uint32_t hsv2rgb(const uint8_t h, const uint8_t s, const uint8_t v)
{
    uint8_t region    = h / 43;  // 0-5
    uint8_t remainder = (h % 43) * 6;

    uint8_t p = (v * (255 - s)) >> 8;
    uint8_t q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    uint8_t t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
    uint8_t r{}, g{}, b{};

    switch (region) {
        case 0:  // Red to Yellow
            r = v;
            g = t;
            b = p;
            break;
        case 1:  // Yellow to Green
            r = q;
            g = v;
            b = p;
            break;
        case 2:  // Green to Cyan
            r = p;
            g = v;
            b = t;
            break;
        case 3:  // Cyan to Blue
            r = p;
            g = q;
            b = v;
            break;
        case 4:  // Blue to Magenta
            r = t;
            g = p;
            b = v;
            break;
        default:  // Magenta to Red
            r = v;
            g = p;
            b = q;
            break;
    }
    // WRGB 32bits: W==0
    return (uint32_t(r) << 16) | (uint32_t(g) << 8) | uint32_t(b);
}

}  // namespace rgb
}  // namespace unit
}  // namespace m5
