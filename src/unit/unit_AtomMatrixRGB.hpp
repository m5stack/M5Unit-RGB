/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_AtomMatrixRGB.hpp
  @brief Atom Matrix RGB for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_ATOM_MATRIX_RGB_HPP
#define M5_UNIT_RGB_UNIT_ATOM_MATRIX_RGB_HPP

#include "unit_RectLED.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitAtomMatrixRGB
  @brief For AtomMatrix RGB (5x5 LED)
*/
class UnitAtomMatrixRGB : public UnitRectLED<5, 5, UnitWS2812> {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitAtomMatrixRGB, 0x00);

public:
    UnitAtomMatrixRGB() : UnitRectLED<5, 5, UnitWS2812>()
    {
        auto ccfg = component_config();
        component_config(ccfg);
    }
    virtual bool begin() override;
};

}  // namespace unit
}  // namespace m5
#endif
