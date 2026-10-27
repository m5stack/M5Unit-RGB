/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  MultiChannel example — two independent LED units on two GPIO ports

  Each unit is the root of its own chain on its own GPIO, so each one allocates its own RMT TX
  channel and can be shown independently (unlike DaisyChain, where every unit shares one GPIO).

  **** NOTICE *********************************************************************
  This example uses the first two of the following GROVE GPIO ports the board has:
    Port B (out) -> Port C (TXD) -> Port A (only when it is not the internal I2C bus)
  The unit types default to UnitRGB (Unit1) and UnitNeco (Unit2); change UNIT_1_TYPE / UNIT_2_TYPE
  to suit your units (e.g. -D UNIT_2_TYPE=m5::unit::UnitHex).

  Each TX channel takes 4 RMT memory blocks by default (RMT v1: borrowed from the following
  channels, RMT v2: mem_block_symbols = 4 blocks). On chips with 4 TX blocks (ESP32-S3 / ESP32-P4)
  the first unit would take them all and the second one fails to get a channel, so this example
  lowers it to 2 blocks per unit. Long chains may need more blocks.
  *********************************************************************************

  BtnA: cycle the color pattern
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <wiring/m5_unit_unified_wiring.hpp>
#include <M5UnitUnifiedRGB.h>
#include <M5Utility.h>

#if !defined(UNIT_1_TYPE)
#define UNIT_1_TYPE m5::unit::UnitRGB
#endif
#if !defined(UNIT_2_TYPE)
#define UNIT_2_TYPE m5::unit::UnitNeco
#endif

namespace {

auto& lcd = M5.Display;
m5::unit::UnitUnified Units;
UNIT_1_TYPE unit1;
UNIT_2_TYPE unit2;

constexpr uint32_t PATTERN_COUNT{3};
uint32_t pattern{};

// Collect up to max_count distinct GROVE output pins in the order Port B, Port C, Port A
uint32_t collect_pins(int8_t* pins, const uint32_t max_count)
{
    const int8_t in_scl = static_cast<int8_t>(M5.getPin(m5::pin_name_t::in_i2c_scl));
    const int8_t in_sda = static_cast<int8_t>(M5.getPin(m5::pin_name_t::in_i2c_sda));
    const int8_t a_scl  = static_cast<int8_t>(M5.getPin(m5::pin_name_t::port_a_scl));
    const int8_t a_sda  = static_cast<int8_t>(M5.getPin(m5::pin_name_t::port_a_sda));
    // Port A is usable as GPIO only when it is not the internal I2C bus (e.g. Core / Core2 / Fire share it)
    const bool a_is_internal = (a_scl == in_scl && a_sda == in_sda);

    const int8_t candidates[] = {
        static_cast<int8_t>(M5.getPin(m5::pin_name_t::port_b_out)),
        static_cast<int8_t>(M5.getPin(m5::pin_name_t::port_c_txd)),
        a_is_internal ? static_cast<int8_t>(-1) : a_sda,
    };

    uint32_t count{};
    for (const auto pin : candidates) {
        if (pin < 0 || count >= max_count) {
            continue;
        }
        bool dup{};
        for (uint32_t i = 0; i < count; ++i) {
            dup |= (pins[i] == pin);
        }
        if (!dup) {
            pins[count++] = pin;
        }
    }
    return count;
}

void fill(m5::unit::UnitLED& u, const uint8_t r, const uint8_t g, const uint8_t b)
{
    for (uint32_t i = 0; i < u.numPixels(); ++i) {
        u.setPixelColor(i, r, g, b);
    }
}

void apply_pattern(const uint32_t p)
{
    switch (p) {
        case 0:  // 1:red, 2:blue
            fill(unit1, 64, 0, 0);
            fill(unit2, 0, 0, 64);
            break;
        case 1:  // 1:green, 2:red
            fill(unit1, 0, 64, 0);
            fill(unit2, 64, 0, 0);
            break;
        default:  // 1:blue, 2:green
            fill(unit1, 0, 0, 64);
            fill(unit2, 0, 64, 0);
            break;
    }
    const bool ok1 = unit1.show();
    const bool ok2 = unit2.show();
    M5.Log.printf("pattern:%u show 1:%d 2:%d\n", static_cast<unsigned>(p), ok1, ok2);

    lcd.fillScreen(TFT_BLACK);
    lcd.setCursor(0, 0);
    lcd.printf("pattern:%u\nUnit1:%s\nUnit2:%s\n", static_cast<unsigned>(p), ok1 ? "OK" : "NG", ok2 ? "OK" : "NG");
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
    lcd.setTextSize(2);

    int8_t pins[2]{};
    if (collect_pins(pins, 2) < 2) {
        M5_LOGE("This board does not have two usable GROVE GPIO ports");
        m5::unit::wiring::failStop();
    }
    M5_LOGI("Unit1 GPIO:%d Unit2 GPIO:%d", pins[0], pins[1]);

    // 2 memory blocks per unit so that two units fit (see NOTICE above)
    {
        auto cfg1       = unit1.config();
        cfg1.mem_blocks = 2;
        unit1.config(cfg1);
        auto cfg2       = unit2.config();
        cfg2.mem_blocks = 2;
        unit2.config(cfg2);
    }

    const bool unit_ready = Units.add(unit1, static_cast<int8_t>(-1), pins[0]) &&
                            Units.add(unit2, static_cast<int8_t>(-1), pins[1]) && Units.begin();
    if (!unit_ready) {
        M5_LOGE("Failed to begin");
        m5::unit::wiring::failStop();
    }
    M5_LOGI("M5UnitUnified initialized");
    M5_LOGI("%s", Units.debugInfo().c_str());

    unit1.setBrightness(32);
    unit2.setBrightness(32);
    apply_pattern(pattern);
}

void loop()
{
    M5.update();
    Units.update();

    if (M5.BtnA.wasClicked()) {
        pattern = (pattern + 1) % PATTERN_COUNT;
        apply_pattern(pattern);
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
