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
#include "m5_utility/button_status.hpp"

namespace m5 {
namespace unit {
/*!
  @class m5::unit::UnitNeco
  @brief Neco LED Unit
  @note Button related functions are available from m5::utility::button::Status
  @warning Button status cannot be acquired by daisy chain or connection via PbHub
*/
class UnitNeco : public UnitWS2812, public m5::utility::button::Status {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitNeco, 0x00);

public:
    //! @brief Constructor (35 LEDs)
    UnitNeco() : UnitWS2812(35 /* 35 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
    /*!
      @brief Begin the unit
      @return True if successful
     */
    virtual bool begin() override;
    /*!
      @brief Update the button status
      @param force Ignored (the button is read on every call)
     */
    virtual void update(const bool force = false) override;

    /*!
      @brief Whether the button is usable on the current wiring
      @return True if the RX pin was successfully configured as InputPullup at begin()
      @note Determined at begin(); false if RX pin is unassigned or cannot support InputPullup
      (e.g. ESP32 classic GPIO 34-39 are input-only with no internal pull-up).
     */
    inline bool buttonAvailable() const
    {
        return _btn_available;
    }

private:
    bool _btn_available{false};
};

}  // namespace unit
}  // namespace m5
#endif
