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

//! @brief  HSV to RGB
extern uint32_t hsv2rgb(const uint8_t h, const uint8_t s, const uint8_t v);

}  // namespace rgb
}  // namespace unit
}  // namespace m5

#endif
