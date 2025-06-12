/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Neco.hpp
  @brief Neco LED unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_NECO_HPP
#define M5_UNIT_RGB_UNIT_NECO_HPP

#include "unit_WS2812.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitNeco
  @brief Neco LED Unit
*/
class UnitNeco : public UnitWS2812 {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitNeco, 0x00);

public:
    UnitNeco() : UnitWS2812(35 /* 35 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
    virtual bool begin() override;
    virtual void update(const bool force = false) override;

    ///@note State is not updated until update() is processed
    ///@warning Not available for connection via PbHub
    ///@name Button
    ///@{
    inline bool isPressed() const
    {
        return _press;
    }
    inline bool wasPressed() const
    {
        return _wasPressed;
    }
    inline bool isReleased() const
    {
        return !_press;
    }
    inline bool wasReleased() const
    {
        return _wasReleased;
    }
    ///@}

protected:
    bool _press{}, _wasPressed{}, _wasReleased{};
};

}  // namespace unit
}  // namespace m5
#endif
