/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Puzzle.hpp
  @brief Puzzle unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_PUZZLE_HPP
#define M5_UNIT_RGB_UNIT_PUZZLE_HPP

#include "unit_RectLED.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitPuzzle
  @brief Puzzle Unit (8x8 LED)
*/
class UnitPuzzle : public UnitRectLED<8, 8, UnitWS2812> {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitPuzzle, 0x00);

public:
    UnitPuzzle() : UnitRectLED<8, 8, UnitWS2812>()
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

}  // namespace unit
}  // namespace m5
#endif
