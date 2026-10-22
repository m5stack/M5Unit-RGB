/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_SK6812.hpp
  @brief SK6812 unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_SK6812_HPP
#define M5_UNIT_RGB_UNIT_SK6812_HPP

#include "unit_LED.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitSK6812
  @brief SK6812 base Unit
*/
class UnitSK6812 : public UnitLED {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitSK6812, 0x00);

protected:
    UnitSK6812(const uint16_t num_of_pixels, const rgb::Order order) : UnitLED(num_of_pixels, order)
    {
    }

public:
    //! @brief Destructor
    virtual ~UnitSK6812()
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
        // Same as WS2812 so SK6812 and WS2812 units can share one daisy chain
        return gpio::m5_rmt_item_t{{6000, 0, 0, 0}};  // 300 us (WS2812C-2020 needs > 280 us)
    }
};

/*!
  @class m5::unit::UnitSK6812GRB
  @brief SK6812GRB Unit
*/
class UnitSK6812GRB : public UnitSK6812 {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitSK6812GRB, 0x00);

public:
    /*!
      @brief Constructor
      @param num_of_pixels Number of pixels
     */
    explicit UnitSK6812GRB(const uint16_t num_of_pixels) : UnitSK6812(num_of_pixels, rgb::Order::grb24)
    {
    }
    //! @brief Destructor
    virtual ~UnitSK6812GRB()
    {
    }
};

/*!
  @class m5::unit::UnitSK6812GRBW
  @brief SK6812GRBW Unit
*/
class UnitSK6812GRBW : public UnitSK6812 {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitSK6812GRBW, 0x00);

public:
    /*!
      @brief Constructor
      @param num_of_pixels Number of pixels
     */
    explicit UnitSK6812GRBW(const uint16_t num_of_pixels) : UnitSK6812(num_of_pixels, rgb::Order::grbw32)
    {
    }
    //! @brief Destructor
    virtual ~UnitSK6812GRBW()
    {
    }
};

}  // namespace unit
}  // namespace m5
#endif
