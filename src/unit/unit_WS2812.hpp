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
    inline virtual gpio::m5_rmt_item_t rmt_item_one() override
    {
        return gpio::m5_rmt_item_t{{14, 1, 12, 0}};
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_zero() override
    {
        return gpio::m5_rmt_item_t{{7, 1, 16, 0}};
    }
    inline virtual gpio::m5_rmt_item_t rmt_item_reset() override
    {
        //        return gpio::m5_rmt_item_t{{1000, 0, 0, 0}};  // 50us
        return gpio::m5_rmt_item_t{{1600, 0, 0, 0}};  // Same as SK6812 (Measures for SK/WS mixed daisy chain)
    }
};

}  // namespace unit
}  // namespace m5
#endif
