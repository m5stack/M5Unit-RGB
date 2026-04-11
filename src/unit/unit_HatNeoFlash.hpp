/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_HatNeoFlash.hpp
  @brief Hat NeoFlash for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_HAT_NEOFLASH_HPP
#define M5_UNIT_RGB_UNIT_HAT_NEOFLASH_HPP

#include "unit_RectLED.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::HatNeoFlash
  @brief Hat NeoFlash (18x7 LED)
*/
class HatNeoFlash : public UnitRectLED<18, 7, UnitWS2812> {
    M5_UNIT_COMPONENT_HPP_BUILDER(HatNeoFlash, 0x00);

public:
    HatNeoFlash() : UnitRectLED<18, 7, UnitWS2812>()
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

}  // namespace unit
}  // namespace m5
#endif
