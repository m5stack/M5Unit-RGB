/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Hex.hpp
  @brief Hex,NeoHex unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_HEX_HPP
#define M5_UNIT_RGB_UNIT_HEX_HPP

#include "unit_SK6812.hpp"
#include "unit_WS2812.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitHexLED37
  @brief Unit comprising 37 LED hexagons
  @details Base class for UnitHEX, UnitNeoHex
  @tparam BaseClass Parent LED chip class
*/
template <class BaseClass>
class UnitHexLED37 : public BaseClass {
    static_assert(std::is_base_of<UnitLED, BaseClass>::value, "BaseClass must derive from UnitLED");
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitHexLED37, 0x00);

public:
    using BaseClass::BaseClass;
    UnitHexLED37() : BaseClass(37)
    {
    }
    virtual uint16_t index_of(const uint16_t n) const override;

    inline virtual uint8_t maxRotation() const
    {
        // 0:0 1:60 2:120 3:180 4:240 5:300
        return 6;
    }
};

/*!
  @class m5::unit::UnitHex
  @brief Hex Unit
*/
class UnitHex : public UnitHexLED37<UnitWS2812> {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitHex, 0x00);

public:
    UnitHex() : UnitHexLED37<UnitWS2812>()
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitNeoHex
  @brief NeoHex Unit
*/
class UnitNeoHex : public UnitHexLED37<UnitSK6812GRB> {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitNeoHex, 0x00);

public:
    UnitNeoHex() : UnitHexLED37<UnitSK6812GRB>()
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

}  // namespace unit
}  // namespace m5
#endif
