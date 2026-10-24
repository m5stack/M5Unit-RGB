/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_WS2812.hpp
  @brief WS2812 unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_WS2812_HPP
#define M5_UNIT_RGB_UNIT_WS2812_HPP

#include "unit_LED.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitWS2812
  @brief WS2812 Unit
*/
class UnitWS2812 : public UnitLED {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitWS2812, 0x00);

public:
    /*!
      @brief Constructor
      @param num_of_pixels Number of pixels
     */
    explicit UnitWS2812(const uint16_t num_of_pixels) : UnitLED(num_of_pixels, rgb::Order::grb24)
    {
    }
    //! @brief Destructor
    virtual ~UnitWS2812()
    {
    }

protected:
    virtual gpio::adapter_config_t rmt_config() override;
    // Timing in ns (rgb::timing), converted to ticks at the actual RMT resolution.
    // Same as SK6812 so SK6812 and WS2812 units can share one daisy chain
    inline virtual gpio::m5_rmt_item_t rmt_item_one() override
    {
        return rmt_item_from_ns(rgb::timing::T1H_NS, rgb::timing::T1L_NS);
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_zero() override
    {
        return rmt_item_from_ns(rgb::timing::T0H_NS, rgb::timing::T0L_NS);
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_reset() override
    {
        return rmt_item_low_from_ns(rgb::timing::RESET_NS);
    }
};

}  // namespace unit
}  // namespace m5
#endif
