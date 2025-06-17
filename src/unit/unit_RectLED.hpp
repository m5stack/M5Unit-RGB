/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_RectLED.hpp
  @brief Rectangular array LED unit
*/
#ifndef M5_UNIT_RGB_UNIT_RECT_LED_HPP
#define M5_UNIT_RGB_UNIT_RECT_LED_HPP

#include "unit_LED.hpp"
#include "unit_SK6812.hpp"
#include "unit_WS2812.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitRectLED
  @brief Rectangular array LED unit
  @tparam WIDTH Number of LEDs in horizontal direction
  @tparam HEIGHT Number of LEDs in vertical direction
  @tparam BaseClass Parent LED chip class
*/
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
class UnitRectLED : public BaseClass {
    static_assert(WIDTH > 0, "WIDTH must be greater than 0");
    static_assert(HEIGHT > 0, "HEIGHT must be greater than 0");
    static_assert(std::is_base_of<UnitLED, BaseClass>::value, "BaseClass must derive from UnitLED");
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRectLED, 0x00);

public:
    using BaseClass::BaseClass;
    UnitRectLED() : BaseClass(WIDTH * HEIGHT)
    {
    }
    static constexpr uint8_t LED_WIDTH{WIDTH};
    static constexpr uint8_t LED_HEIGHT{HEIGHT};

    inline virtual uint8_t maxRotation() const
    {
        // 0:0 1:90 2:180 3:270
        return 4;
    }

protected:
    virtual uint16_t index_of(const uint16_t n) const override
    {
        uint8_t x = n % WIDTH;
        uint8_t y = n / WIDTH;
        uint8_t new_x{}, new_y{};

        switch (this->rotation()) {
            case 1:  // 90°
                new_x = HEIGHT - 1 - y;
                new_y = x;
                break;
            case 2:  // 180°
                new_x = WIDTH - 1 - x;
                new_y = HEIGHT - 1 - y;
                break;
            case 3:  // 270°
                new_x = y;
                new_y = WIDTH - 1 - x;
                break;
            default:  // 0°
                new_x = x;
                new_y = y;
                break;
        }
        return new_y * WIDTH + new_x;
    }
};

///@cond
using m5::utility::mmh3::operator"" _mmh3;
// class UnitRectLED
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const char UnitRectLED<WIDTH, HEIGHT, BaseClass>::name[] = "UnitRectLED";
using m5::utility::mmh3::operator"" _mmh3;
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const types::uid_t UnitRectLED<WIDTH, HEIGHT, BaseClass>::uid{"UnitRectLED"_mmh3};
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const types::attr_t UnitRectLED<WIDTH, HEIGHT, BaseClass>::attr{types::attribute::AccessGPIO};
///@endcond

}  // namespace unit
}  // namespace m5
#endif
