/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Tests for scale8() and Color (no hardware).
  Placed directly under test/, so they run on the host (test_native) and on the boards (test_<Unit>_<Board>)
*/
#include <gtest/gtest.h>
#include <unit/unit_LED_data.hpp>
#include <utility/m5unit_rgb_utility.hpp>

using m5::unit::rgb::Color;
using m5::unit::rgb::scale8;

TEST(RgbColor, Scale8)
{
    EXPECT_EQ(scale8(0, 255), 0);
    EXPECT_EQ(scale8(1, 255), 1);
    EXPECT_EQ(scale8(128, 255), 128);
    EXPECT_EQ(scale8(254, 255), 254);
    EXPECT_EQ(scale8(255, 255), 255);
    EXPECT_EQ(scale8(255, 0), 0);
    EXPECT_EQ(scale8(255, 1), 1);
    EXPECT_EQ(scale8(255, 128), 128);
    EXPECT_EQ(scale8(255, 254), 254);
    EXPECT_EQ(scale8(0, 0), 0);
    EXPECT_EQ(scale8(0, 128), 0);
    EXPECT_EQ(scale8(128, 0), 0);
    EXPECT_EQ(scale8(128, 128), 64);
    EXPECT_EQ(scale8(100, 100), 39);
}

TEST(RgbColor, FromHSV)
{
    Color r = Color::fromHSV(0, 255, 255);
    EXPECT_EQ(r.r, 255);
    EXPECT_EQ(r.g, 0);
    EXPECT_EQ(r.b, 0);

    Color g = Color::fromHSV(85, 255, 255);
    EXPECT_GT(g.g, g.r);
    EXPECT_GT(g.g, g.b);

    Color b = Color::fromHSV(170, 255, 255);
    EXPECT_GT(b.b, b.r);
    EXPECT_GT(b.b, b.g);

    Color w = Color::fromHSV(0, 0, 255);
    EXPECT_EQ(w.r, 255);
    EXPECT_EQ(w.g, 255);
    EXPECT_EQ(w.b, 255);

    Color k = Color::fromHSV(0, 255, 0);
    EXPECT_EQ(k.r, 0);
    EXPECT_EQ(k.g, 0);
    EXPECT_EQ(k.b, 0);

    for (uint16_t hue : {0u, 43u, 85u, 128u, 170u, 213u}) {
        Color gray = Color::fromHSV(static_cast<uint8_t>(hue), 0, 128);
        EXPECT_EQ(gray.r, 128) << "hue=" << hue;
        EXPECT_EQ(gray.g, 128) << "hue=" << hue;
        EXPECT_EQ(gray.b, 128) << "hue=" << hue;
    }

    Color w1 = Color::fromHSV(0, 0, 1);
    EXPECT_EQ(w1.r, 1);
    EXPECT_EQ(w1.g, 1);
    EXPECT_EQ(w1.b, 1);
    Color w254 = Color::fromHSV(0, 0, 254);
    EXPECT_EQ(w254.r, 254);
    EXPECT_EQ(w254.g, 254);
    EXPECT_EQ(w254.b, 254);
}
