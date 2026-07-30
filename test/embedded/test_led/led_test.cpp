/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitLED
*/
#include <gtest/gtest.h>
#include <Wire.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <M5UnitUnifiedRGB.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <set>

using namespace m5::unit::googletest;
using namespace m5::unit;
using namespace m5::unit::rgb;

constexpr uint16_t NUM_OF_PIXELS{4};
constexpr Color black{};

template <class U>
class TestLED : public GPIOComponentTestBase<U> {
protected:
    virtual U* get_instance() override
    {
        auto ptr = new U(NUM_OF_PIXELS);
        return ptr;
    }
};

using TestSK6812GRB = TestLED<UnitSK6812GRB>;
using TestWS2812    = TestLED<UnitWS2812>;

namespace testing {
namespace internal {

template <>
std::string GetTypeName<UnitSK6812GRB>()
{
    return "SK6812GRB";
}

template <>
std::string GetTypeName<UnitWS2812>()
{
    return "WS2812";
}

}  // namespace internal
}  // namespace testing

namespace {
__attribute__((unused)) void print_pixels(const UnitLED* u)
{
    for (uint16_t i = 0; i < u->numPixels(); ++i) {
        auto c  = u->color(i);
        auto ac = u->absoluteColor(i);
        M5_LOGI("I[%02u]:%3u,%3u,%3u A[%02u]:%3u,%3u,%3u", i, c.r, c.g, c.b, i, ac.r, ac.g, ac.b);
    }
}

}  // namespace

//
// Test inner pixel status
//
void basic_test(UnitLED* unit)
{
    EXPECT_EQ(unit->numPixels(), NUM_OF_PIXELS);
    EXPECT_EQ(unit->brightness(), 10);
    EXPECT_FALSE(unit->hasWhite());

    EXPECT_EQ(unit->order(), Order::grb24);
    EXPECT_EQ(unit->offsetR(), 1);
    EXPECT_EQ(unit->offsetG(), 0);
    EXPECT_EQ(unit->offsetB(), 2);
    EXPECT_LT(unit->offsetW(), 0);

    // Brightness
    unit->setBrightness(12);
    EXPECT_EQ(unit->brightness(), 12);

    // setPixelColor, [], color, absoluteColor, pixels, rotate
    EXPECT_TRUE(unit->setPixelColor(0, Color(1, 2, 3)));
    EXPECT_TRUE(unit->setPixelColor(1, 4, 5, 6));
    EXPECT_TRUE(unit->setPixelColor(2, 7, 8, 9, 255));
    EXPECT_TRUE(unit->setPixelColor(3, (10U << 16) | (11U << 8) | 12U));
    EXPECT_FALSE(unit->setPixelColor(4, black));
    EXPECT_FALSE(unit->setPixelColor(65535, black));

    auto& u = *unit;
    EXPECT_EQ(u[0], unit->color(0));
    EXPECT_EQ(u[1], unit->color(1));
    EXPECT_EQ(u[2], unit->color(2));
    EXPECT_EQ(u[3], unit->color(3));
    EXPECT_EQ(unit->color(4), black);
    EXPECT_EQ(unit->color(65535), black);

    EXPECT_EQ(u[0], unit->absoluteColor(0));
    EXPECT_EQ(u[1], unit->absoluteColor(1));
    EXPECT_EQ(u[2], unit->absoluteColor(2));
    EXPECT_EQ(u[3], unit->absoluteColor(3));
    EXPECT_EQ(unit->absoluteColor(4), black);
    EXPECT_EQ(unit->absoluteColor(65535), black);

    EXPECT_EQ(u[0], Color(1, 2, 3));
    EXPECT_EQ(u[1], Color(4, 5, 6));
    EXPECT_EQ(u[2], Color(7, 8, 9));
    EXPECT_EQ(u[3], Color(10, 11, 12));

    //
    u[0] = u[1];
    u[1] = Color(13, 14, 15);
    u[2] = u[3];
    u[3] = Color(16, 17, 18);

    EXPECT_EQ(u[0], Color(4, 5, 6));
    EXPECT_EQ(u[1], Color(13, 14, 15));
    EXPECT_EQ(u[2], Color(10, 11, 12));
    EXPECT_EQ(u[3], Color(16, 17, 18));

    // Rotation 180 deg: [0,1,2,3] => [3,2,1,0]
    //    print_pixels(unit.get());

    EXPECT_EQ(unit->rotation(), 0);
    unit->setRotation(1);
    EXPECT_EQ(unit->rotation(), 1);

    //    print_pixels(unit.get());

    EXPECT_EQ(u[0], Color(4, 5, 6));
    EXPECT_EQ(u[1], Color(13, 14, 15));
    EXPECT_EQ(u[2], Color(10, 11, 12));
    EXPECT_EQ(u[3], Color(16, 17, 18));

    EXPECT_EQ(unit->absoluteColor(0), Color(16, 17, 18));
    EXPECT_EQ(unit->absoluteColor(1), Color(10, 11, 12));
    EXPECT_EQ(unit->absoluteColor(2), Color(13, 14, 15));
    EXPECT_EQ(unit->absoluteColor(3), Color(4, 5, 6));

    // Reset rotation for subsequent tests
    unit->setRotation(0);
    EXPECT_EQ(unit->rotation(), 0);
    EXPECT_EQ(unit->absoluteColor(0), Color(4, 5, 6));
    EXPECT_EQ(unit->absoluteColor(3), Color(16, 17, 18));
}

void fill_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    constexpr Color G(0, 255, 0);
    constexpr Color B(0, 0, 255);
    auto& u = *unit;

    unit->fill(R, 0, 1);
    unit->fill(G, 1, 2);
    unit->fill(B, 2);

    EXPECT_EQ(u[0], R);
    EXPECT_EQ(u[1], G);
    EXPECT_EQ(u[2], B);
    EXPECT_EQ(u[3], B);

    unit->fillRainbow();
    {
        std::vector<Color> v;
        for (uint16_t i = 0; i < unit->numPixels(); i++) {
            v.push_back(unit->color(i));
        }
        std::set<Color> s(v.begin(), v.end());
        EXPECT_EQ(v.size(), s.size());
    }

    unit->clear();
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    unit->fillGradation(R, B);
    {
        std::vector<Color> v;
        for (uint16_t i = 0; i < unit->numPixels(); i++) {
            v.push_back(unit->color(i));
        }
        std::set<Color> s(v.begin(), v.end());
        EXPECT_EQ(v.size(), s.size());
    }

    // fill boundary: count=0 fills to end
    unit->clear();
    unit->fill(R, 0, 0);
    EXPECT_EQ(u[0], R);
    EXPECT_EQ(u[1], R);
    EXPECT_EQ(u[2], R);
    EXPECT_EQ(u[3], R);

    // fill boundary: count=0 from midpoint
    unit->clear();
    unit->fill(G, 2, 0);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], G);
    EXPECT_EQ(u[3], G);

    // fill boundary: out of range
    unit->clear();
    EXPECT_FALSE(unit->fill(R, 5, 1));

    // fillRandom
    unit->clear();
    EXPECT_TRUE(unit->fillRandom());
    {
        bool any_nonblack = false;
        for (uint16_t i = 0; i < unit->numPixels(); i++) {
            if (unit->color(i).wrgb != 0) {
                any_nonblack = true;
                break;
            }
        }
        EXPECT_TRUE(any_nonblack);
    }
}

void shift_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    constexpr Color G(0, 255, 0);
    constexpr Color B(0, 0, 255);
    constexpr Color W(255, 255, 255);

    auto& u = *unit;

    // SL
    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);

    unit->shiftLeft(2);
    EXPECT_EQ(u[0], B);
    EXPECT_EQ(u[1], W);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    unit->shiftLeft();
    EXPECT_EQ(u[0], W);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);
    unit->shiftLeft(4);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);
    unit->shiftLeft(65535);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    // SR
    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);

    unit->shiftRight(2);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], R);
    EXPECT_EQ(u[3], G);

    unit->shiftRight();
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], R);

    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);
    unit->shiftRight(4);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);
    unit->shiftRight(65535);
    EXPECT_EQ(u[0], black);
    EXPECT_EQ(u[1], black);
    EXPECT_EQ(u[2], black);
    EXPECT_EQ(u[3], black);

    // RL
    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);

    unit->rotateLeft(2);
    EXPECT_EQ(u[0], B);
    EXPECT_EQ(u[1], W);
    EXPECT_EQ(u[2], R);
    EXPECT_EQ(u[3], G);

    unit->rotateLeft();
    EXPECT_EQ(u[0], W);
    EXPECT_EQ(u[1], R);
    EXPECT_EQ(u[2], G);
    EXPECT_EQ(u[3], B);

    unit->rotateLeft(4);
    EXPECT_EQ(u[0], W);
    EXPECT_EQ(u[1], R);
    EXPECT_EQ(u[2], G);
    EXPECT_EQ(u[3], B);

    unit->rotateLeft(4 * 100 + 1);
    EXPECT_EQ(u[0], R);
    EXPECT_EQ(u[1], G);
    EXPECT_EQ(u[2], B);
    EXPECT_EQ(u[3], W);

    // RR
    unit->setPixelColor(0, R);
    unit->setPixelColor(1, G);
    unit->setPixelColor(2, B);
    unit->setPixelColor(3, W);

    unit->rotateRight(2);
    EXPECT_EQ(u[0], B);
    EXPECT_EQ(u[1], W);
    EXPECT_EQ(u[2], R);
    EXPECT_EQ(u[3], G);

    unit->rotateRight();
    EXPECT_EQ(u[0], G);
    EXPECT_EQ(u[1], B);
    EXPECT_EQ(u[2], W);
    EXPECT_EQ(u[3], R);

    unit->rotateRight(4);
    EXPECT_EQ(u[0], G);
    EXPECT_EQ(u[1], B);
    EXPECT_EQ(u[2], W);
    EXPECT_EQ(u[3], R);

    unit->rotateRight(4 * 100 + 1);
    EXPECT_EQ(u[0], R);
    EXPECT_EQ(u[1], G);
    EXPECT_EQ(u[2], B);
    EXPECT_EQ(u[3], W);
}

void color_test()
{
    // fromHSV: Red (hue=0)
    Color r = Color::fromHSV(0, 255, 255);
    EXPECT_EQ(r.r, 255);
    EXPECT_EQ(r.g, 0);
    EXPECT_EQ(r.b, 0);

    // fromHSV: Green (hue=85 ~ 120deg)
    Color g = Color::fromHSV(85, 255, 255);
    EXPECT_GT(g.g, g.r);
    EXPECT_GT(g.g, g.b);

    // fromHSV: Blue (hue=170 ~ 240deg)
    Color b = Color::fromHSV(170, 255, 255);
    EXPECT_GT(b.b, b.r);
    EXPECT_GT(b.b, b.g);

    // fromHSV: saturation=0 => white
    Color w = Color::fromHSV(0, 0, 255);
    EXPECT_EQ(w.r, 255);
    EXPECT_EQ(w.g, 255);
    EXPECT_EQ(w.b, 255);

    // fromHSV: value=0 => black
    Color k = Color::fromHSV(0, 255, 0);
    EXPECT_EQ(k.r, 0);
    EXPECT_EQ(k.g, 0);
    EXPECT_EQ(k.b, 0);
}

void config_test(UnitLED* unit)
{
    // config brightness is applied after begin()
    auto cfg       = unit->config();
    cfg.brightness = 42;
    unit->config(cfg);
    // Note: config is applied on next begin(), verify getter
    EXPECT_EQ(unit->config().brightness, 42);
}

TEST_F(TestSK6812GRB, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get());
    fill_test(unit.get());
    shift_test(unit.get());
    color_test();
    config_test(unit.get());
}

TEST_F(TestWS2812, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get());
    fill_test(unit.get());
    shift_test(unit.get());
    color_test();
    config_test(unit.get());
}
