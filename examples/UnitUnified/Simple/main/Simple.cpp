/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Simple example

  NOTE: Depending on the core device, the power supply from the port may not be
  sufficient for all LEDs.
  If the lights turn on unexpectedly, try lowering the brightness or reducing the
  number of lights turned on at once to see if that resolves the issue.
  If the lights turn on unexpectedly, try lowering the brightness or reducing the number of lights turned on at once to
  see if that resolves the issue.
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedRGB.h>
#include <M5Utility.h>
#include <esp_random.h>
#include <driver/gpio.h>

// *********************************************************************
// Choose using unit
// *************************************************************
// Choose one define symbol to match the unit you are using
// *************************************************************
#if !defined(USING_UNIT_RGB) && !defined(USING_UNIT_PUZZLE) && !defined(USING_UNIT_HEX) &&        \
    !defined(USING_UNIT_NEO_HEX) && !defined(USING_UNIT_NECO) && !defined(USING_UNIT_RGBLED15) && \
    !defined(USING_UNIT_RGBLED_STRIP30) && !defined(USING_ATOM_MATRIX_RGB) && !defined(USING_HAT_NEOFLASH)
// For Unit RGB (U003)
// #define USING_UNIT_RGB
// For Unit Puzzle (U193)
// #define USING_UNIT_PUZZLE
// For Unit Hex (A045)
// #define USING_UNIT_HEX
// For Unit NeoHex (A045-B)
// #define USING_UNIT_NEO_HEX
// For Unit Neco (U163)
// #define USING_UNIT_NECO
// For Unit RGB LED (A035)
// #define USING_UNIT_RGBLED15
// For Unit RGB LED Strip (A093)
// #define USING_UNIT_RGBLED_STRIP30
// For ATOM Matrix built-in 5x5 RGB LED
// #define USING_ATOM_MATRIX_RGB
// For Hat NeoFlash
// #define USING_HAT_NEOFLASH
#endif

using m5::unit::rgb::Color;

namespace {

int get_hat_gpio_pin(const m5::board_t board)
{
    switch (board) {
        case m5::board_t::board_M5StickC:
        case m5::board_t::board_M5StickCPlus:
        case m5::board_t::board_M5StickCPlus2:
            return 26;
        case m5::board_t::board_M5StickS3:
            return 0;
        case m5::board_t::board_M5StackCoreInk:
            return 26;
        case m5::board_t::board_ArduinoNessoN1:
            return 7;
        default:
            return -1;
    }
}

auto& lcd = M5.Display;
m5::unit::UnitUnified Units;
#if defined(USING_UNIT_RGB)
#pragma message "Using: USING_UNIT_RGB"
m5::unit::UnitRGB unit;
#elif defined(USING_UNIT_PUZZLE)
#pragma message "Using: USING_UNIT_PUZZLE"
m5::unit::UnitPuzzle unit;
#elif defined(USING_UNIT_HEX)
#pragma message "Using: USING_UNIT_HEX"
m5::unit::UnitHex unit;
#elif defined(USING_UNIT_NEO_HEX)
#pragma message "Using: USING_UNIT_NEO_HEX"
m5::unit::UnitNeoHex unit;
#elif defined(USING_UNIT_NECO)
#pragma message "Using: USING_UNIT_NECO"
m5::unit::UnitNeco unit;
#elif defined(USING_UNIT_RGBLED15)
#pragma message "Using: USING_UNIT_RGBLED15"
m5::unit::UnitRGBLED15 unit;
#elif defined(USING_UNIT_RGBLED_STRIP30)
#pragma message "Using: USING_UNIT_RGBLED_STRIP30"
m5::unit::UnitRGBLEDStrip30 unit;
#elif defined(USING_ATOM_MATRIX_RGB)
#pragma message "Using: USING_ATOM_MATRIX_RGB"
m5::unit::UnitAtomMatrixRGB unit;
#elif defined(USING_HAT_NEOFLASH)
#pragma message "Using: USING_HAT_NEOFLASH"
m5::unit::HatNeoFlash unit;
#else
#error Choose unit!
#endif

void log_pixel_color()
{
    std::string s{};
    for (uint32_t i = 0; i < unit.numPixels(); ++i) {
        Color c = unit.absoluteColor(i);
        s += m5::utility::formatString("[%02u]:(%3u,%3u,%3u,%3u)\n", i, c.w, c.r, c.g, c.b);
    }
    M5.Log.printf(s.c_str());
}

void draw_pixel_color()
{
    uint32_t ww = lcd.width() / 16;
    uint32_t hh = lcd.height() / 16;
    uint32_t left{}, top{};

    lcd.startWrite();
    for (uint32_t i = 0; i < unit.numPixels(); ++i) {
        Color c = unit.absoluteColor(i);
        lcd.fillRect(left, top, ww, hh, c.wrgb);
        lcd.drawRect(left, top, ww, hh, TFT_WHITE);
        left += ww;
        if (left >= lcd.width()) {
            top += hh;
            left = 0;
        }
    }
    lcd.endWrite();
}

}  // namespace

void setup()
{
    delay(2000);

    M5.begin();
    M5.setTouchButtonHeightByRatio(100);

    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

#if defined(USING_ATOM_MATRIX_RGB)
    // ATOM Matrix built-in 5x5 RGB LED (not an external unit)
    M5_LOGI("USING_ATOM_MATRIX_RGB");
    auto pin_num_gpio_in  = -1;
    auto pin_num_gpio_out = 27;
#elif defined(USING_HAT_NEOFLASH)
    M5_LOGI("USING_HAT_NEOFLASH");
    auto pin_num_gpio_in  = -1;
    auto pin_num_gpio_out = get_hat_gpio_pin(M5.getBoard());
    // StickS3 GPIO0 is a strapping pin shared with PMIC open-drain output.
    // Max drive strength is required for stable WS2812 signal with USB connected.
    if (M5.getBoard() == m5::board_t::board_M5StickS3 && pin_num_gpio_out == 0) {
        gpio_set_drive_capability((gpio_num_t)pin_num_gpio_out, GPIO_DRIVE_CAP_3);
    }
#else
    // Neco button requires a GPIO with internal pull-up support.
    // Some ports use input-only GPIOs (e.g. GPIO 36) that lack
    // internal pull-up, so the button line floats and reads noise.
    // Use a port with output-capable GPIO instead.
    auto pin_num_gpio_in  = M5.getPin(m5::pin_name_t::port_b_in);
    auto pin_num_gpio_out = M5.getPin(m5::pin_name_t::port_b_out);
    if (pin_num_gpio_in < 0 || pin_num_gpio_out < 0) {
        M5_LOGW("PortB is not available");
        Wire.end();
        pin_num_gpio_in  = M5.getPin(m5::pin_name_t::port_a_pin1);
        pin_num_gpio_out = M5.getPin(m5::pin_name_t::port_a_pin2);
    }
#endif
    M5_LOGI("getPin :%d,%d", pin_num_gpio_in, pin_num_gpio_out);

    if (!Units.add(unit, pin_num_gpio_in, pin_num_gpio_out) || !Units.begin()) {
        M5_LOGE("Failed to begin");
        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }
    M5_LOGI("M5UnitUnified initialized");
    M5_LOGI("%s", Units.debugInfo().c_str());

    unit.setBrightness(10);
    unit.show();

    lcd.fillScreen(0);
    draw_pixel_color();
    log_pixel_color();
}

void loop()
{
    M5.update();
    Units.update();

#if defined(USING_UNIT_NECO)
    if (unit.wasPressed()) {
        M5.Speaker.tone(4000, 20);
        M5.Log.printf("WasPressed Neko\n");
    }
    if (unit.wasReleased()) {
        M5.Log.printf("WasReleased Neko\n");
    }
#endif

    if (M5.BtnA.wasClicked()) {
        static uint32_t func{};

        switch (func) {
            case 0:  // set first 3 LED
                M5.Log.printf("==== set\n");
                unit.clear();
                unit.setPixelColor(0, 100, 0, 0);
                unit.setPixelColor(1, 0, 100, 0);
                unit.setPixelColor(2, 0, 0, 100);
                unit.show();
                draw_pixel_color();
                break;

            case 1:  // brightness 0
                M5.Log.printf("==== brightness 0\n");
                unit.setPixelColor(0, 100, 0, 0);
                unit.setPixelColor(1, 0, 100, 0);
                unit.setPixelColor(2, 0, 0, 100);
                unit.setBrightness(0);
                unit.show();
                draw_pixel_color();
                break;

            case 2:  // brightness 16
                // Pixel values are retained despite changes in brightness
                M5.Log.printf("==== brightness on\n");
                unit.setBrightness(10);
                unit.show();
                draw_pixel_color();
                break;

            case 3:  // setPixelColor
                M5.Log.printf("==== setPixelColor\n");
                unit.setPixelColor(0, 0, 0, 100);         // RGB
                unit.setPixelColor(1, 0x0000FF00);        // WRGB32
                unit.setPixelColor(2, Color(100, 0, 0));  // Color
                unit.show();
                draw_pixel_color();
                break;

            case 4:  // Swap [0] <-> [1]
            {
                M5.Log.printf("==== swap 0-1\n");
                Color tmp = unit.color(0);
                unit.setPixelColor(0, unit.color(1));
                unit.setPixelColor(1, tmp);
                unit.show();
                draw_pixel_color();
            } break;

            case 5:  // Swap [1] <-> [2]
            {
                M5.Log.printf("==== swap 1-2\n");
                // Accessible by Color in [n]
                Color tmp = unit[1];
                unit[1]   = unit[2];
                unit[2]   = tmp;
                unit.show();
                draw_pixel_color();
            } break;

            case 6:  // Partial fill (power-safe: width() LEDs at a time, height() steps)
            {
                M5.Log.printf("==== fill\n");
                constexpr Color c_table[] = {
                    {100, 0, 0}, {0, 100, 0}, {0, 0, 100}, {100, 100, 0}, {0, 100, 100}, {100, 0, 100},
                };
                uint16_t w    = std::max<uint16_t>(1, unit.width());
                uint16_t cidx = 0;
                // Forward: one row at a time
                for (uint16_t row = 0; row < unit.height(); ++row) {
                    unit.clear();
                    uint16_t off = row * w;
                    uint16_t cnt = std::min<uint16_t>(w, unit.numPixels() - off);
                    unit.fill(c_table[cidx % 6], off, cnt);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(300);
                    ++cidx;
                }
                // Backward
                for (int32_t row = unit.height() - 1; row >= 0; --row) {
                    unit.clear();
                    uint16_t off = row * w;
                    uint16_t cnt = std::min<uint16_t>(w, unit.numPixels() - off);
                    unit.fill(c_table[cidx % 6], off, cnt);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(300);
                    ++cidx;
                }
                unit.clear();
                unit.show();
            } break;

            case 7:  // Fill random (width() LEDs per row, height() steps)
            {
                M5.Log.printf("==== fill random\n");
                uint16_t w = std::max<uint16_t>(1, unit.width());
                // Forward
                for (uint16_t row = 0; row < unit.height(); ++row) {
                    unit.clear();
                    uint16_t off = row * w;
                    uint16_t cnt = std::min<uint16_t>(w, unit.numPixels() - off);
                    unit.fillRandom(off, cnt);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(300);
                }
                // Backward
                for (int32_t row = unit.height() - 1; row >= 0; --row) {
                    unit.clear();
                    uint16_t off = row * w;
                    uint16_t cnt = std::min<uint16_t>(w, unit.numPixels() - off);
                    unit.fillRandom(off, cnt);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(300);
                }
                unit.clear();
                unit.show();
            } break;

            case 8:  // shift pixel array (rainbow width() LEDs)
            {
                M5.Log.printf("==== shift\n");
                uint16_t w = std::max<uint16_t>(1, unit.width());
                unit.clear();
                unit.fillRainbow(0, w);
                unit.show();
                draw_pixel_color();

                uint32_t count = std::min<uint16_t>(10U, unit.numPixels());
                while (count--) {
                    unit.shiftRight(2);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(20);
                }
                unit.clear();
                unit.fillRainbow(0, w);
                unit.show();
                m5::utility::delay(500);

                count = std::min<uint16_t>(10U, unit.numPixels());
                while (count--) {
                    unit.shiftLeft(2);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(20);
                }
            } break;

            case 9:  // rotate pixel array (rainbow width() LEDs)
            {
                M5.Log.printf("==== rotate\n");
                uint16_t w = std::max<uint16_t>(1, unit.width());
                unit.clear();
                unit.fillRainbow(0, w);
                unit.show();
                draw_pixel_color();
                m5::utility::delay(200);
                for (uint16_t i = 0; i < unit.numPixels(); ++i) {
                    unit.rotateRight(1);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(20);
                }

                unit.clear();
                unit.fillRainbow(0, w);
                unit.show();
                draw_pixel_color();
                m5::utility::delay(200);
                for (uint16_t i = 0; i < unit.numPixels(); ++i) {
                    unit.rotateLeft(1);
                    unit.show();
                    draw_pixel_color();
                    m5::utility::delay(20);
                }
            } break;
        }
        //        M5.Speaker.tone(2000, 20);
        if (++func > 9) {
            static int8_t rot{};
            --rot;                  // Rotate counter clock wise
            unit.setRotation(rot);  // Rotate all pixels
            unit.show();
            func = 0;
        }
    }
}
