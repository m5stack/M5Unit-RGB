/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Shared test primitives for M5Unit-RGB per-SKU tests.
*/
#ifndef M5_UNIT_RGB_LED_TEST_COMMON_HPP
#define M5_UNIT_RGB_LED_TEST_COMMON_HPP

#include <gtest/gtest.h>
#include <Wire.h>
#include <M5Unified.h>
#include <M5UnitUnified.hpp>
#include <googletest/test_template.hpp>
#include <M5UnitUnifiedRGB.h>
#include <wiring/m5_unit_unified_wiring.hpp>
#include <chrono>
#include <cmath>
#include <iostream>
#include <vector>
#include <set>

// Every test fixture drives the LEDs over GPIO (RMT); ESP32-C2 / C61 have no RMT
#if defined(M5_UNIT_UNIFIED_HAS_RMT) && !M5_UNIT_UNIFIED_HAS_RMT
#error "Embedded LED tests require RMT"
#endif

using namespace m5::unit::googletest;
using namespace m5::unit;
using namespace m5::unit::rgb;

inline constexpr Color COLOR_BLACK{};

// Expected static properties for a given SKU (used by basic_test).
struct BasicExpect {
    uint16_t pixels;
    rgb::Order order;
    bool has_white;
    int8_t off_r;
    int8_t off_g;
    int8_t off_b;
    int8_t off_w;
};

namespace expected {
constexpr BasicExpect UNIT_RGB{3, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_PUZZLE{64, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_HEX{37, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_NEOHEX{37, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_RGBLED15{15, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_RGBLEDSTRIP30{30, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_NECO{35, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect HAT_NEOFLASH{126, Order::grb24, false, 1, 0, 2, -1};
constexpr BasicExpect UNIT_ATOMMATRIXRGB{25, Order::grb24, false, 1, 0, 2, -1};
}  // namespace expected

// Fixture template for Grove-based units (default 0-arg ctor + wiring::addGPIO).
template <class U>
class TestSKU : public GPIOComponentTestBase<U> {
protected:
    virtual U* get_instance() override
    {
        return new U();
    }
};

// BeginAppliesConfig fixture: pre-set config.brightness=42 before begin().
template <class U>
class TestSKU_ConfigApplied : public TestSKU<U> {
protected:
    U* get_instance() override
    {
        auto* ptr      = new U();
        auto cfg       = ptr->config();
        cfg.brightness = 42;
        ptr->config(cfg);
        return ptr;
    }
};

inline __attribute__((unused)) void print_pixels(const UnitLED* u)
{
    for (uint16_t i = 0; i < u->numPixels(); ++i) {
        auto c  = u->color(i);
        auto ac = u->absoluteColor(i);
        M5_LOGI("I[%02u]:%3u,%3u,%3u A[%02u]:%3u,%3u,%3u", i, c.r, c.g, c.b, i, ac.r, ac.g, ac.b);
    }
}

// Verify static properties and basic pixel access on the first min(4, numPixels()) pixels.
inline void basic_test(UnitLED* unit, const BasicExpect& e)
{
    EXPECT_EQ(unit->numPixels(), e.pixels);
    EXPECT_EQ(unit->hasWhite(), e.has_white);
    EXPECT_EQ(unit->order(), e.order);
    EXPECT_EQ(unit->offsetR(), e.off_r);
    EXPECT_EQ(unit->offsetG(), e.off_g);
    EXPECT_EQ(unit->offsetB(), e.off_b);
    EXPECT_EQ(unit->offsetW(), e.off_w);

    EXPECT_EQ(unit->brightness(), 10);
    unit->setBrightness(12);
    EXPECT_EQ(unit->brightness(), 12);
    unit->setBrightness(10);

    const uint16_t N = std::min<uint16_t>(4, unit->numPixels());
    auto& u          = *unit;

    if (N >= 1) {
        EXPECT_TRUE(unit->setPixelColor(0, Color(1, 2, 3)));
    }
    if (N >= 2) {
        EXPECT_TRUE(unit->setPixelColor(1, 4, 5, 6));
    }
    if (N >= 3) {
        EXPECT_TRUE(unit->setPixelColor(2, 7, 8, 9, 255));
    }
    if (N >= 4) {
        EXPECT_TRUE(unit->setPixelColor(3, (10U << 16) | (11U << 8) | 12U));
    }

    EXPECT_FALSE(unit->setPixelColor(unit->numPixels(), COLOR_BLACK));
    EXPECT_FALSE(unit->setPixelColor(65535, COLOR_BLACK));

    if (N >= 1) {
        EXPECT_EQ(u[0], unit->color(0));
        EXPECT_EQ(u[0], unit->absoluteColor(0));
        EXPECT_EQ(u[0], Color(1, 2, 3));
    }
    if (N >= 2) {
        EXPECT_EQ(u[1], Color(4, 5, 6));
    }
    if (N >= 3) {
        EXPECT_EQ(u[2], Color(7, 8, 9));
    }
    if (N >= 4) {
        EXPECT_EQ(u[3], Color(10, 11, 12));
    }

    EXPECT_EQ(unit->color(unit->numPixels()), COLOR_BLACK);
    EXPECT_EQ(unit->color(65535), COLOR_BLACK);
    EXPECT_EQ(unit->absoluteColor(unit->numPixels()), COLOR_BLACK);
    EXPECT_EQ(unit->absoluteColor(65535), COLOR_BLACK);

    if (N >= 2) {
        u[0] = u[1];
        u[1] = Color(13, 14, 15);
        EXPECT_EQ(u[0], Color(4, 5, 6));
        EXPECT_EQ(u[1], Color(13, 14, 15));
    }

    EXPECT_EQ(unit->rotation(), 0);
    if (unit->maxRotation() >= 2) {
        unit->setRotation(1);
        EXPECT_EQ(unit->rotation(), 1);
        unit->setRotation(0);
        EXPECT_EQ(unit->rotation(), 0);
    }

    unit->clear();
}

inline void fill_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    constexpr Color G(0, 255, 0);
    constexpr Color B(0, 0, 255);
    auto& u          = *unit;
    const uint16_t N = unit->numPixels();

    unit->clear();
    EXPECT_TRUE(unit->fill(R, 0, 1));
    if (N >= 2) {
        EXPECT_TRUE(unit->fill(G, 1, 1));
    }
    if (N >= 3) {
        EXPECT_TRUE(unit->fill(B, 2));
    }
    EXPECT_EQ(u[0], R);
    if (N >= 2) {
        EXPECT_EQ(u[1], G);
    }
    for (uint16_t i = 2; i < N; ++i) {
        EXPECT_EQ(u[i], B) << "pos=" << i;
    }

    unit->clear();
    EXPECT_TRUE(unit->fillRainbow());
    bool any_nonblack_rainbow = false;
    for (uint16_t i = 0; i < N; ++i) {
        if (unit->color(i).wrgb != 0) {
            any_nonblack_rainbow = true;
            break;
        }
    }
    EXPECT_TRUE(any_nonblack_rainbow);

    unit->clear();
    for (uint16_t i = 0; i < N; ++i) {
        EXPECT_EQ(u[i], COLOR_BLACK) << "pos=" << i;
    }

    EXPECT_TRUE(unit->fillGradation(R, B));
    if (N >= 2) {
        EXPECT_EQ(unit->color(0), R);
        EXPECT_EQ(unit->color(N - 1), B);
    }

    unit->clear();
    EXPECT_TRUE(unit->fill(R, 0, 0));
    for (uint16_t i = 0; i < N; ++i) {
        EXPECT_EQ(u[i], R) << "pos=" << i;
    }

    unit->clear();
    const uint16_t mid = N / 2;
    EXPECT_TRUE(unit->fill(G, mid, 0));
    for (uint16_t i = 0; i < mid; ++i) {
        EXPECT_EQ(u[i], COLOR_BLACK) << "pos=" << i;
    }
    for (uint16_t i = mid; i < N; ++i) {
        EXPECT_EQ(u[i], G) << "pos=" << i;
    }

    unit->clear();
    EXPECT_FALSE(unit->fill(R, N, 1));
    EXPECT_FALSE(unit->fill(R, N + 1, 1));

    unit->clear();
    EXPECT_TRUE(unit->fillRandom());
    bool any_nonblack_random = false;
    for (uint16_t i = 0; i < N; ++i) {
        if (unit->color(i).wrgb != 0) {
            any_nonblack_random = true;
            break;
        }
    }
    EXPECT_TRUE(any_nonblack_random);

    unit->clear();
}

inline void shift_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    constexpr Color G(0, 255, 0);
    constexpr Color B(0, 0, 255);
    constexpr Color W(255, 255, 255);
    auto& u                = *unit;
    const uint16_t N       = unit->numPixels();
    const uint16_t P       = std::min<uint16_t>(4, N);
    const Color pattern[4] = {R, G, B, W};

    auto set_pattern = [&]() {
        unit->clear();
        for (uint16_t i = 0; i < P; ++i) {
            unit->setPixelColor(i, pattern[i]);
        }
    };

    for (uint16_t s : {uint16_t(1), uint16_t(2), N}) {
        set_pattern();
        unit->shiftLeft(s);
        for (uint16_t i = 0; i < N; ++i) {
            const uint32_t src = static_cast<uint32_t>(i) + s;
            const Color exp    = (src < P) ? pattern[src] : COLOR_BLACK;
            EXPECT_EQ(u[i], exp) << "shiftLeft s=" << s << " pos=" << i;
        }
    }

    set_pattern();
    unit->shiftLeft(65535);
    for (uint16_t i = 0; i < N; ++i) {
        EXPECT_EQ(u[i], COLOR_BLACK) << "shiftLeft(65535) pos=" << i;
    }

    for (uint16_t s : {uint16_t(1), uint16_t(2), N}) {
        set_pattern();
        unit->shiftRight(s);
        for (uint16_t i = 0; i < N; ++i) {
            const bool in_bounds = (i >= s) && ((i - s) < P);
            const Color exp      = in_bounds ? pattern[i - s] : COLOR_BLACK;
            EXPECT_EQ(u[i], exp) << "shiftRight s=" << s << " pos=" << i;
        }
    }

    set_pattern();
    unit->shiftRight(65535);
    for (uint16_t i = 0; i < N; ++i) {
        EXPECT_EQ(u[i], COLOR_BLACK) << "shiftRight(65535) pos=" << i;
    }

    for (uint16_t s : {uint16_t(1), uint16_t(2), N, uint16_t(N * 3 + 1)}) {
        set_pattern();
        unit->rotateLeft(s);
        for (uint16_t i = 0; i < N; ++i) {
            const uint16_t src = static_cast<uint16_t>((static_cast<uint32_t>(i) + s) % N);
            const Color exp    = (src < P) ? pattern[src] : COLOR_BLACK;
            EXPECT_EQ(u[i], exp) << "rotateLeft s=" << s << " pos=" << i;
        }
    }

    for (uint16_t s : {uint16_t(1), uint16_t(2), N, uint16_t(N * 3 + 1)}) {
        set_pattern();
        unit->rotateRight(s);
        for (uint16_t i = 0; i < N; ++i) {
            const uint16_t src = static_cast<uint16_t>((static_cast<uint32_t>(i) + N - (s % N)) % N);
            const Color exp    = (src < P) ? pattern[src] : COLOR_BLACK;
            EXPECT_EQ(u[i], exp) << "rotateRight s=" << s << " pos=" << i;
        }
    }

    unit->clear();
}

inline void scale8_test()
{
    using m5::unit::rgb::scale8;
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

inline void color_test()
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

inline void config_test(UnitLED* unit)
{
    auto cfg       = unit->config();
    cfg.brightness = 42;
    unit->config(cfg);
    EXPECT_EQ(unit->config().brightness, 42);
}

inline void range_test(UnitLED* unit)
{
    constexpr Color R(255, 0, 0);
    const uint16_t n = unit->numPixels();

    EXPECT_FALSE(unit->fill(R, n, 0));
    EXPECT_FALSE(unit->fill(R, n, 1));
    EXPECT_FALSE(unit->fill(R, n + 5, 0));
    EXPECT_FALSE(unit->fillRainbow(n, 0, 0));
    EXPECT_FALSE(unit->fillRainbow(n + 5, 4, 0));
    EXPECT_FALSE(unit->fillGradation(R, R, n, 0));
    EXPECT_FALSE(unit->fillGradation(R, R, n + 1, 3));
    EXPECT_FALSE(unit->fillRandom(n, 0));
    EXPECT_FALSE(unit->fillRandom(n + 1, 3));

    unit->clear();
    EXPECT_TRUE(unit->fill(R, 0, 999));
    for (uint16_t i = 0; i < n; ++i) {
        EXPECT_EQ(unit->color(i), R);
    }

    unit->clear();
}

// End-to-end write test — regression coverage:
//   #3 RMT error propagation (writePixels must return true on success),
//   #4 InterfaceGPIO destructor / RMT v2 encoder alloc,
//   #5 brightness scaling (255 must map to 255).
// Leaves LEDs lit as a visual "tests complete" indicator.
inline void hw_write_test(UnitLED* unit)
{
    unit->setBrightness(255);
    EXPECT_TRUE(unit->fill(Color(0, 32, 0)));
    EXPECT_TRUE(unit->show());
}

// Rotation test for RectLED (WIDTH x HEIGHT grid, 4 rotation steps).
inline void rect_rotation_test(UnitLED* unit, uint16_t W, uint16_t H)
{
    ASSERT_EQ(unit->maxRotation(), 4u);

    constexpr Color R(255, 0, 0);
    constexpr Color B(0, 0, 255);

    unit->setRotation(0);
    unit->clear();
    unit->setPixelColor(0, R);
    unit->setPixelColor(unit->numPixels() - 1, B);
    EXPECT_EQ(unit->absoluteColor(0), R);
    EXPECT_EQ(unit->absoluteColor(unit->numPixels() - 1), B);

    unit->setRotation(2);
    unit->clear();
    unit->setPixelColor(0, R);
    unit->setPixelColor(unit->numPixels() - 1, B);
    EXPECT_EQ(unit->absoluteColor(unit->numPixels() - 1), R) << "rot180: logical[0] -> physical last";
    EXPECT_EQ(unit->absoluteColor(0), B) << "rot180: logical[last] -> physical 0";

    for (uint8_t rot : {uint8_t(1), uint8_t(3)}) {
        unit->setRotation(rot);
        unit->clear();
        unit->setPixelColor(0, R);
        if (W > 1 && H > 1) {
            EXPECT_NE(unit->absoluteColor(0), R) << "rot=" << int(rot);
            bool found = false;
            for (uint16_t i = 0; i < unit->numPixels(); ++i) {
                if (unit->absoluteColor(i) == R) {
                    found = true;
                    break;
                }
            }
            EXPECT_TRUE(found) << "rot=" << int(rot);
        }
    }

    unit->setRotation(0);
    unit->clear();
}

// Rotation test for HexLED37 (6 rotation steps: 0/60/120/180/240/300 deg).
inline void hex_rotation_test(UnitLED* unit)
{
    ASSERT_EQ(unit->maxRotation(), 6u);

    constexpr Color R(255, 0, 0);

    unit->setRotation(0);
    unit->clear();
    unit->setPixelColor(0, R);
    EXPECT_EQ(unit->absoluteColor(0), R);

    for (uint8_t rot = 1; rot < 6; ++rot) {
        unit->setRotation(rot);
        unit->clear();
        unit->setPixelColor(0, R);
        EXPECT_NE(unit->absoluteColor(0), R) << "rot=" << int(rot);
    }

    unit->setRotation(0);
    unit->clear();
}

// UnitNeco: button verification. If wiring cannot support InputPullup
// (e.g. ESP32 classic GPIO 34-39), skip the read check with a warning log.
inline void neco_button_test(UnitNeco* unit)
{
    if (!unit->buttonAvailable()) {
        M5_LIB_LOGW("Skipping Neco button read: RX pin cannot support InputPullup");
        return;
    }
    unit->update();
    EXPECT_FALSE(unit->isPressed()) << "Button appears pressed at rest — check wiring";
}

#endif
