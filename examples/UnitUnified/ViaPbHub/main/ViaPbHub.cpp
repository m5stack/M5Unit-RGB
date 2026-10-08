/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Example of using via UnitPbHub

  Core ---> PbHub -+--> ch:0 UnitRGB
                   |
                   +--> ch:1 UnitPuzzle -> UnitPuzzle (NOTICE: 64+64 = 128)
                   |
                   +--> ch:2 UnitHex -> UnitNeoHex
                   |
                   +--> ch:3 UnitRGBLED15
                   |
                   +--> ch:4 UnitNeco
                   |
                   +--> ch5: UnitRGBLEDStrip30

  Hardware limits (see README "Note (large cascades / PbHub)"):
    1. PbHub caps each channel at 74 LEDs. On ch:1 (64+64=128), the second Puzzle's
       last 54 LEDs receive no data and stay unlit.
    2. USB-powered boards (NanoC6 / NanoH2, ~500 mA) cannot supply enough current
       for the full cascade; the 5V rail sags and downstream LEDs dim or go dark.
       Reduce total LED count, lower brightness, or use an external 5V power source.
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <wiring/m5_unit_unified_wiring.hpp>
#include <M5UnitUnifiedRGB.h>
#include <M5UnitUnifiedHUB.h>  // UnitPbHub
#include <M5Utility.h>

using m5::unit::rgb::Color;

namespace {
auto& lcd = M5.Display;

m5::unit::UnitUnified Units;

m5::unit::UnitPbHub hub;

m5::unit::UnitRGB rgb;
m5::unit::UnitPuzzle puzzle[2];
m5::unit::UnitHex hex;
m5::unit::UnitNeoHex neoHex;
m5::unit::UnitRGBLED15 rgbled;
m5::unit::UnitNeco neco;  // Button API is not available that via PbHub
m5::unit::UnitRGBLEDStrip30 strip;

void loop_rgb(const m5::unit::types::elapsed_time_t now)
{
    constexpr uint8_t table[3][4 /* index, r,g,b */] = {{0, 100, 0, 0}, {2, 0, 0, 100}, {1, 100, 100, 0}};

    static auto ms = m5::utility::millis();
    static uint8_t idx{};

    if (now - ms > 1000) {
        ms  = now;
        idx = (idx + 1) % 3;
        rgb.clear();
        rgb.setPixelColor(table[idx][0], table[idx][1], table[idx][2], table[idx][3]);
        rgb.show();
    }
}

void loop_puzzle(const m5::unit::types::elapsed_time_t now)
{
    /*
      NOTICE!: The PbHUB can manage up to 74 LEDs per channel
      So, note that the second LED of the puzzle only lights up 10
     */
    static auto ms = m5::utility::millis();
    if (now - ms > 50) {
        ms = now;
        puzzle[0].rotateRight();
        puzzle[1].rotateLeft();
        puzzle[1].show();
    }
}

void loop_hex(const m5::unit::types::elapsed_time_t now)
{
    static auto ms = m5::utility::millis();
    if (now - ms > 250) {
        ms = now;
        hex.setRotation(hex.rotation() + 1);        // Clock wise
        neoHex.setRotation(neoHex.rotation() - 1);  // Counter clock wise
        neoHex.show();
    }
}

uint32_t roundTrip(uint32_t count, uint32_t max)
{
    uint32_t period   = 2 * max;
    uint32_t index    = count % period;
    int32_t diff      = static_cast<int32_t>(index) - static_cast<int32_t>(max);
    uint32_t abs_diff = (diff ^ (diff >> 31)) - (diff >> 31);
    return max - abs_diff;
}

void loop_rgbled(const m5::unit::types::elapsed_time_t now)
{
    static auto ms = m5::utility::millis();
    static uint32_t counter{};

    if (now - ms > 30) {
        ms      = now;
        int idx = roundTrip(++counter, 14);  // 0-14-0...
        rgbled.clear();
        if (idx > 0) {
            rgbled.setPixelColor(idx - 1, 32, 0, 0);
        }
        rgbled.setPixelColor(idx, 128, 0, 0);
        if (idx + 1 < static_cast<int>(rgbled.numPixels())) {
            rgbled.setPixelColor(idx + 1, 32, 0, 0);
        }
        rgbled.show();
    }
}

void loop_neco(const m5::unit::types::elapsed_time_t now)
{
    static auto ms = m5::utility::millis();
    static uint32_t counter{};

    if (now - ms > 40) {
        int idx = roundTrip(++counter, 17);  // 0-17-0...

        neco.clear();
        if (idx == 17) {
            neco.setPixelColor(idx, 0, 100, 0);

        } else {
            neco.setPixelColor(idx, 0, 100, 0);
            neco.setPixelColor(35 - idx - 1, 0, 100, 0);
        }
        neco.show();
    }
}

void loop_strip(const m5::unit::types::elapsed_time_t now)
{
    static auto ms = m5::utility::millis();
    static uint32_t counter{};

    if (now - ms > 2000) {
        ms = now;
        strip.fillRandom();
        strip.show();
    }
}

}  // namespace

void setup()
{
    M5.begin();
    M5.setTouchButtonHeightByRatio(100);

    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

    // Connect units to hub
    if (!hub.add(rgb, 0) ||              // PbHub ch:0 -> UnitRGB
        !puzzle[0].add(puzzle[1], 0) ||  // UnitPuzzle[0] -> UnitPuzzle[1]
        !hub.add(puzzle[0], 1) ||        // PbHub ch:1 -> UnitPuzzle[0]
        !hex.add(neoHex, 0) ||           // UnitHex -> UnitNeoHex
        !hub.add(hex, 2) ||              // PbHub ch:2 -> UnitHex
        !hub.add(rgbled, 3) ||           // PbHub ch:3 -> UnitRGBLED15
        !hub.add(neco, 4) ||             // PbHub ch:4 -> UnitNeco
        !hub.add(strip, 5)) {            // PbHub ch:5 -> UnitRGBLEDStrip
        M5_LOGE("Failed to connect units");
        m5::unit::wiring::failStop();
    }

    // Board-aware I2C: NessoN1 PortB -> SoftwareI2C, NanoC6/NanoH2 -> Ex_I2C, others -> Wire
    bool unit_ready = m5::unit::wiring::addI2C(Units, hub) && Units.begin();

    if (!unit_ready) {
        M5_LOGE("Failed to begin");
        m5::unit::wiring::failStop();
    }
    M5_LOGI("M5UnitUnified initialized");
    M5_LOGI("%s", Units.debugInfo().c_str());

    lcd.fillScreen(TFT_DARKGREEN);

    //
    rgb.setPixelColor(0, 100, 0, 0);
    rgb.show();

    puzzle[0].setRotation(1);
    puzzle[1].setRotation(-1);
    puzzle[0].fillRainbow();
    puzzle[1].fillRainbow();
    puzzle[1].show();

    hex.fillRainbow();
    neoHex.fillRainbow();
    neoHex.show();
}

void loop()
{
    M5.update();
    Units.update();

    static bool pause{};
    if (M5.BtnA.wasClicked()) {
        pause = !pause;
    }

    if (!pause) {
        auto now = m5::utility::millis();
        loop_rgb(now);
        loop_puzzle(now);
        loop_hex(now);
        loop_rgbled(now);
        loop_neco(now);
        loop_strip(now);
    }
}

#if !defined(ARDUINO)
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#if CONFIG_FREERTOS_UNICORE
static inline void feedIdleTaskPeriodically(void)
{
    constexpr uint32_t FEED_INTERVAL_MS   = 2000;
    constexpr TickType_t FEED_SLEEP_TICKS = pdMS_TO_TICKS(5);
    static uint32_t s_last_feed_ms{};
    const uint32_t now_ms{static_cast<uint32_t>(esp_timer_get_time() / 1000)};
    if (now_ms - s_last_feed_ms >= FEED_INTERVAL_MS) {
        s_last_feed_ms = now_ms;
        vTaskDelay(FEED_SLEEP_TICKS);
    }
}
#endif

extern "C" void app_main(void)
{
    setup();
    for (;;) {
#if CONFIG_FREERTOS_UNICORE
        feedIdleTaskPeriodically();
#endif
        loop();
    }
}
#endif
