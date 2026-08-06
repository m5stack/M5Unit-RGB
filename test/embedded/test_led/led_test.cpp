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

// Fixture that pre-sets config.brightness=42 before begin() to prove
// begin() applies config (guards against regression where brightness member
// initializer alone would still yield the default 10).
template <class U>
class TestLED_ConfigApplied : public TestLED<U> {
protected:
    U* get_instance() override
    {
        auto ptr       = new U(NUM_OF_PIXELS);
        auto cfg       = ptr->config();
        cfg.brightness = 42;
        ptr->config(cfg);
        return ptr;
    }
};

using TestSK6812GRB_Config = TestLED_ConfigApplied<UnitSK6812GRB>;
using TestWS2812_Config    = TestLED_ConfigApplied<UnitWS2812>;

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

void scale8_test()
{
    using m5::unit::rgb::scale8;
    // Identity: scale8(a, 255) == a (b=255 must not shave off 1 via >>8 truncation)
    EXPECT_EQ(scale8(0, 255), 0);
    EXPECT_EQ(scale8(1, 255), 1);
    EXPECT_EQ(scale8(128, 255), 128);
    EXPECT_EQ(scale8(254, 255), 254);
    EXPECT_EQ(scale8(255, 255), 255);  // regression: (255*255)>>8 == 254

    // Symmetric: scale8(255, b) == b
    EXPECT_EQ(scale8(255, 0), 0);
    EXPECT_EQ(scale8(255, 1), 1);
    EXPECT_EQ(scale8(255, 128), 128);
    EXPECT_EQ(scale8(255, 254), 254);

    // Zero on either side
    EXPECT_EQ(scale8(0, 0), 0);
    EXPECT_EQ(scale8(0, 128), 0);
    EXPECT_EQ(scale8(128, 0), 0);

    // Mid-range rounded values
    EXPECT_EQ(scale8(128, 128), 64);  // (128*128+127)/255 = 16511/255 = 64
    EXPECT_EQ(scale8(100, 100), 39);  // (100*100+127)/255 = 10127/255 = 39
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

    // fromHSV: saturation=0 => white (v=255 must survive as 255, catches >>8 truncation)
    Color w = Color::fromHSV(0, 0, 255);
    EXPECT_EQ(w.r, 255);
    EXPECT_EQ(w.g, 255);
    EXPECT_EQ(w.b, 255);

    // fromHSV: value=0 => black
    Color k = Color::fromHSV(0, 255, 0);
    EXPECT_EQ(k.r, 0);
    EXPECT_EQ(k.g, 0);
    EXPECT_EQ(k.b, 0);

    // fromHSV: sat=0 => grayscale for any hue (v value must be preserved on every channel)
    for (uint16_t hue : {0u, 43u, 85u, 128u, 170u, 213u}) {
        Color gray = Color::fromHSV(static_cast<uint8_t>(hue), 0, 128);
        EXPECT_EQ(gray.r, 128) << "hue=" << hue;
        EXPECT_EQ(gray.g, 128) << "hue=" << hue;
        EXPECT_EQ(gray.b, 128) << "hue=" << hue;
    }

    // fromHSV: sat=0, v extremes => value preserved (regression against >>8 rounding loss)
    Color w1 = Color::fromHSV(0, 0, 1);
    EXPECT_EQ(w1.r, 1);
    EXPECT_EQ(w1.g, 1);
    EXPECT_EQ(w1.b, 1);
    Color w254 = Color::fromHSV(0, 0, 254);
    EXPECT_EQ(w254.r, 254);
    EXPECT_EQ(w254.g, 254);
    EXPECT_EQ(w254.b, 254);
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

// Range validation across fill / fillRainbow / fillGradation / fillRandom (#6).
void range_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    const uint16_t n = unit->numPixels();

    // first >= numPixels must return false for every fill variant.
    EXPECT_FALSE(unit->fill(R, n, 0));
    EXPECT_FALSE(unit->fill(R, n, 1));
    EXPECT_FALSE(unit->fill(R, n + 5, 0));
    EXPECT_FALSE(unit->fillRainbow(n, 0, 0));
    EXPECT_FALSE(unit->fillRainbow(n + 5, 4, 0));
    EXPECT_FALSE(unit->fillGradation(R, R, n, 0));
    EXPECT_FALSE(unit->fillGradation(R, R, n + 1, 3));
    EXPECT_FALSE(unit->fillRandom(n, 0));
    EXPECT_FALSE(unit->fillRandom(n + 1, 3));

    // count overflow is clamped to remaining pixels (fill succeeds).
    unit->clear();
    EXPECT_TRUE(unit->fill(R, 0, 999));
    for (uint16_t i = 0; i < n; ++i) {
        EXPECT_EQ(unit->color(i), R);
    }
}

// End-to-end write test — regression coverage for 0.0.2:
//   #3 RMT error propagation (writePixels must return true on success),
//   #4 InterfaceGPIO destructor / RMT v2 encoder alloc,
//   #5 brightness scaling (255 must map to 255).
// Leaves LEDs lit as a visual "tests complete" indicator.
void hw_write_test(UnitLED* unit)
{
    unit->setBrightness(255);
    EXPECT_TRUE(unit->fill(Color(0, 32, 0)));  // dim green
    EXPECT_TRUE(unit->show());
}

TEST_F(TestSK6812GRB, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get());
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestWS2812, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get());
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestSK6812GRB_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}

TEST_F(TestWS2812_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
