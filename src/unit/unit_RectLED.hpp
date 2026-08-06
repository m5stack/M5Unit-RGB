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
    //! @brief Default constructor (uses WIDTH x HEIGHT pixels)
    UnitRectLED() : BaseClass(WIDTH * HEIGHT)
    {
    }
    static constexpr uint8_t LED_WIDTH{WIDTH};
    static constexpr uint8_t LED_HEIGHT{HEIGHT};

    /*!
      @brief Logical width considering rotation (0/180: WIDTH, 90/270: HEIGHT)
      @return Width in pixels
     */
    inline virtual uint16_t width() const override
    {
        return (this->rotation() & 1) ? HEIGHT : WIDTH;
    }
    /*!
      @brief Logical height considering rotation (0/180: HEIGHT, 90/270: WIDTH)
      @return Height in pixels
     */
    inline virtual uint16_t height() const override
    {
        return (this->rotation() & 1) ? WIDTH : HEIGHT;
    }

    /*!
      @brief Get the max rotation (4 steps: 0/90/180/270 deg)
      @return 4
     */
    inline virtual uint8_t maxRotation() const override
    {
        // 0:0 1:90 2:180 3:270
        return 4;
    }

protected:
    virtual uint16_t index_of(const uint16_t n) const override
    {
        switch (this->rotation()) {
            case 1: {  // 90° CW (logical grid: HEIGHT x WIDTH)
                uint8_t x = n % HEIGHT;
                uint8_t y = n / HEIGHT;
                return x * WIDTH + (WIDTH - 1 - y);
            }
            case 2: {  // 180° (logical grid: WIDTH x HEIGHT)
                uint8_t x = n % WIDTH;
                uint8_t y = n / WIDTH;
                return (HEIGHT - 1 - y) * WIDTH + (WIDTH - 1 - x);
            }
            case 3: {  // 270° CW (logical grid: HEIGHT x WIDTH)
                uint8_t x = n % HEIGHT;
                uint8_t y = n / HEIGHT;
                return (HEIGHT - 1 - x) * WIDTH + y;
            }
            default: {  // 0°
                return n;
            }
        }
    }
};

///@cond
using m5::utility::mmh3::operator"" _mmh3;
// class UnitRectLED
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const char UnitRectLED<WIDTH, HEIGHT, BaseClass>::name[] = "UnitRectLED";
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const types::uid_t UnitRectLED<WIDTH, HEIGHT, BaseClass>::uid{"UnitRectLED"_mmh3};
template <const uint8_t WIDTH, const uint8_t HEIGHT, class BaseClass>
const types::attr_t UnitRectLED<WIDTH, HEIGHT, BaseClass>::attr{types::attribute::AccessGPIO};
///@endcond

}  // namespace unit
}  // namespace m5
#endif
