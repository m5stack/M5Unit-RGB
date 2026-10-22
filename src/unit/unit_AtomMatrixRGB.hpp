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
  @brief ATOM Matrix built-in 5x5 RGB LED (not an external unit)
  @warning Do not set brightness too high. Excessive brightness can generate high temperatures
  and damage the LEDs and acrylic panel. Recommended brightness is 20 or less.
*/
class UnitAtomMatrixRGB : public UnitRectLED<5, 5, UnitWS2812> {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitAtomMatrixRGB, 0x00);

public:
    //! @brief Constructor (5x5 = 25 LEDs)
    UnitAtomMatrixRGB() : UnitRectLED<5, 5, UnitWS2812>()
    {
    }
    /*!
      @brief Begin the unit
      @return True if successful
     */
    virtual bool begin() override;
};

}  // namespace unit
}  // namespace m5
#endif
