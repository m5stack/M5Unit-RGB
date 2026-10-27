/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_LED_data.hpp
  @brief Color type and data line timing for M5Unit-RGB

  Kept apart from unit_LED.hpp so the native (host) tests can include it
  without pulling in M5UnitComponent.
*/
#ifndef M5_UNIT_RGB_UNIT_LED_DATA_HPP
#define M5_UNIT_RGB_UNIT_LED_DATA_HPP

#include "../utility/m5unit_rgb_utility.hpp"
#include <cstdint>

namespace m5 {
namespace unit {

/*!
  @namespace rgb
  @brief For M5Unit-RGB
 */
namespace rgb {

//! @brief WRGB 32bits
using wrgb32_color_t = uint32_t;

/*!
  @union Color
  @brief WRGB 32bits
*/
union Color {
    /*!
      @brief Constructor from packed WRGB
      @param c WRGB 32bits (default 0)
     */
    explicit constexpr Color(const wrgb32_color_t c = 0) : wrgb{c}
    {
    }
    /*!
      @brief Constructor from RGB / RGBW components
      @param rr Red
      @param gg Green
      @param bb Blue
      @param ww White (default 0)
     */
    constexpr Color(const uint8_t rr, const uint8_t gg, const uint8_t bb, const uint8_t ww = 0)
        : b{bb}, g{gg}, r{rr}, w{ww}
    {
    }
    /*!
      @brief Convert to packed WRGB
      @return WRGB 32bits
     */
    constexpr operator wrgb32_color_t() const
    {
        return wrgb;
    }
    /*!
      @brief Make Color from HSV
      @param h Hue
      @param s Saturation
      @param v Value
      @return Color
     */
    static Color fromHSV(const uint8_t h, const uint8_t s, const uint8_t v)
    {
        return Color(hsv2rgb(h, s, v));
    }

    wrgb32_color_t wrgb{};  //!< WRGB 32bits
    struct {
        uint8_t b;  //!< Blue
        uint8_t g;  //!< Green
        uint8_t r;  //!< Red
        uint8_t w;  //!< White
    };
    uint8_t a[4];  //!< Array
};
/*!
  @brief Compare two Colors
  @param lhs Left-hand side
  @param rhs Right-hand side
  @return True if equal
 */
constexpr bool operator==(const Color& lhs, const Color& rhs)
{
    return lhs.wrgb == rhs.wrgb;
}

/*!
  @brief Convert a duration to RMT ticks
  @param ns Duration (ns)
  @param resolution_hz RMT resolution (Hz)
  @return Ticks, rounded to the nearest (not clamped to the 15-bit RMT duration)
 */
constexpr uint32_t ticks_from_ns(const uint32_t ns, const uint32_t resolution_hz)
{
    return static_cast<uint32_t>((static_cast<uint64_t>(ns) * resolution_hz + 500000000ULL) / 1000000000ULL);
}

/*!
  @namespace timing
  @brief Data line timing shared by the SK6812 and WS2812 units (ns)
  @details Within the SK6812, WS2812B, WS2812C-2020 and WS2812E-V5 datasheets, so both kinds can share one daisy chain
 */
namespace timing {
constexpr uint32_t T0H_NS{350};       //!< 0 code, high
constexpr uint32_t T0L_NS{800};       //!< 0 code, low
constexpr uint32_t T1H_NS{700};       //!< 1 code, high
constexpr uint32_t T1L_NS{600};       //!< 1 code, low
constexpr uint32_t RESET_NS{300000};  //!< Reset (latch), WS2812C-2020 needs > 280 us
}  // namespace timing

}  // namespace rgb
}  // namespace unit
}  // namespace m5
#endif
