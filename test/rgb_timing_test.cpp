/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Tests for the data line timing converted to RMT ticks (no hardware).
  Placed directly under test/, so they run on the host (test_native) and on the boards (test_<Unit>_<Board>)
*/
#include <gtest/gtest.h>
#include <unit/unit_LED_data.hpp>

using namespace m5::unit::rgb;

namespace {

// Limits common to the SK6812, WS2812B, WS2812C-2020 and WS2812E-V5 datasheets (ns)
constexpr uint32_t T0H_MIN{250}, T0H_MAX{380};
constexpr uint32_t T1H_MIN{650}, T1H_MAX{750};
constexpr uint32_t T0L_MIN{750}, T0L_MAX{1000};
constexpr uint32_t T1L_MIN{580}, T1L_MAX{600};
constexpr uint32_t RESET_MIN{280000};
constexpr uint32_t DURATION_MAX{32767};  // 15-bit RMT duration field

// Actual duration (ns) of the ticks at the resolution
double ns_of(const uint32_t ticks, const uint32_t resolution_hz)
{
    return ticks * 1e9 / resolution_hz;
}

// Ticks computed at calc_hz, sent at actual_hz, must stay within the datasheet limits
void check_limits(const uint32_t calc_hz, const uint32_t actual_hz)
{
    const double t0h = ns_of(ticks_from_ns(timing::T0H_NS, calc_hz), actual_hz);
    const double t0l = ns_of(ticks_from_ns(timing::T0L_NS, calc_hz), actual_hz);
    const double t1h = ns_of(ticks_from_ns(timing::T1H_NS, calc_hz), actual_hz);
    const double t1l = ns_of(ticks_from_ns(timing::T1L_NS, calc_hz), actual_hz);
    EXPECT_GE(t0h, T0H_MIN);
    EXPECT_LE(t0h, T0H_MAX);
    EXPECT_GE(t0l, T0L_MIN);
    EXPECT_LE(t0l, T0L_MAX);
    EXPECT_GE(t1h, T1H_MIN);
    EXPECT_LE(t1h, T1H_MAX);
    EXPECT_GE(t1l, T1L_MIN);
    EXPECT_LE(t1l, T1L_MAX);

    const uint32_t reset_ticks = ticks_from_ns(timing::RESET_NS, calc_hz);
    EXPECT_GE(ns_of(reset_ticks, actual_hz), RESET_MIN);
    EXPECT_LE(reset_ticks, DURATION_MAX);
}

}  // namespace

TEST(RgbTiming, TicksFromNs)
{
    EXPECT_EQ(ticks_from_ns(0, 20000000), 0u);
    EXPECT_EQ(ticks_from_ns(350, 0), 0u);
    EXPECT_EQ(ticks_from_ns(50, 20000000), 1u);
    EXPECT_EQ(ticks_from_ns(74, 20000000), 1u);  // 1.48 rounds down
    EXPECT_EQ(ticks_from_ns(75, 20000000), 2u);  // 1.5 rounds up
    EXPECT_EQ(ticks_from_ns(300000, 32000000), 9600u);
}

// At 20 MHz (tick_ns 50) the ticks match the long-standing fixed items {7,1,16,0} / {14,1,12,0} / 6000,
// so the waveform is unchanged
TEST(RgbTiming, SameTicksAt20MHz)
{
    constexpr uint32_t hz{20000000};
    EXPECT_EQ(ticks_from_ns(timing::T0H_NS, hz), 7u);
    EXPECT_EQ(ticks_from_ns(timing::T0L_NS, hz), 16u);
    EXPECT_EQ(ticks_from_ns(timing::T1H_NS, hz), 14u);
    EXPECT_EQ(ticks_from_ns(timing::T1L_NS, hz), 12u);
    EXPECT_EQ(ticks_from_ns(timing::RESET_NS, hz), 6000u);
}

// Resolutions the RMT gives: 20 MHz (tick_ns 50), 32 MHz (ESP32-H2, tick_ns 31) and 40 MHz (tick_ns 25)
TEST(RgbTiming, WithinDatasheetLimits)
{
    for (const uint32_t hz : {20000000u, 32000000u, 40000000u}) {
        SCOPED_TRACE(hz);
        check_limits(hz, hz);
    }
}

// ESP32-H2 with M5UnitUnified older than 0.6.4: the ticks are computed at the requested 1e9 / 31 Hz,
// but the RMT runs at 32 MHz
TEST(RgbTiming, WithinDatasheetLimitsH2Fallback)
{
    check_limits(1000000000u / 31, 32000000u);
}
