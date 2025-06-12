/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Simple example
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedRGB.h>
#include <M5Utility.h>
#include <esp_random.h>

// *********************************************************************
// Choose using unit
// *********************************************************************
#if !defined(USING_UNIT_RGB) && !defined(USING_UNIT_PUZZLE) && !defined(USING_UNIT_HEX) &&        \
    !defined(USING_UNIT_NEO_HEX) && !defined(USING_UNIT_NECO) && !defined(USING_UNIT_RGBLED15) && \
    !defined(USING_UNIT_RGBLED_STRIP30) && !defined(USING_ATOM_MATRIX_RGB)
// #define USING_UNIT_RGB
// #define USING_UNIT_PUZZLE
// #define USING_UNIT_HEX
// #define USING_UNIT_NEO_HEX
// #define USING_UNIT_NECO
// #define USING_UNIT_RGBLED15
// #define USING_UNIT_RGBLED_STRIP30
// #define USING_ATOM_MATRIX_RGB
#endif
// *********************************************************************

using m5::unit::rgb::Color;

namespace {
auto& lcd = M5.Display;
m5::unit::UnitUnified Units;
#if defined(USING_UNIT_RGB)
m5::unit::UnitRGB unit;
#elif defined(USING_UNIT_PUZZLE)
m5::unit::UnitPuzzle unit;
#elif defined(USING_UNIT_HEX)
m5::unit::UnitHex unit;
#elif defined(USING_UNIT_NEO_HEX)
m5::unit::UnitNeoHex unit;
#elif defined(USING_UNIT_NECO)
m5::unit::UnitNeco unit;
#elif defined(USING_UNIT_RGBLED15)
m5::unit::UnitRGBLED15 unit;
#elif defined(USING_UNIT_RGBLED_STRIP30)
m5::unit::UnitRGBLEDStrip30 unit;
#elif defined(USING_ATOM_MATRIX_RGB)
m5::unit::UnitAtomMatrixRGB unit;
#else
#error Choose unit!
#endif

void log_pixel_color()
{
    std::string s{};
    for (uint32_t i = 0; i < unit.numPixels(); ++i) {
        Color c = unit.absoluteColor(i);
        s += m5::utility::formatString("[%02u]:(%3u,%3u,%3u)\n", i, c.r, c.g, c.b, c.w);
    }
    M5.Log.printf(s.c_str());
}

void draw_pxiel_color()
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

    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

    // AtomMatrix RGB
#if defined(USING_ATOM_MATRIX_RGB)
    auto brd = M5.getBoard();
    if (brd != m5::board_t::board_M5AtomMatrix) {
        M5_LOGE("This core is not M5AtomMatrix %u", brd);
        while (true) {
            m5::utility::delay(10000);
        }
    }
    auto pin_num_gpio_in  = -1;
    auto pin_num_gpio_out = 27;
#else
    // PortB if available, PortA if not
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
    M5_LOGI("M5UnitUnified has been begun");
    M5_LOGI("%s", Units.debugInfo().c_str());

    unit.setBrightness(10);
    unit.show();

    lcd.fillScreen(0);
    draw_pxiel_color();
    log_pixel_color();
}

void loop()
{
    M5.update();
    auto touch = M5.Touch.getDetail();
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

    if (M5.BtnA.wasClicked() || touch.wasClicked()) {
        static uint32_t func{};

        switch (func) {
            case 0:  // set first 3 LED
                M5.Log.printf("==== set\n");
                unit.clear();
                unit.setPixelColor(0, 100, 0, 0);
                unit.setPixelColor(1, 0, 100, 0);
                unit.setPixelColor(2, 0, 0, 100);
                unit.show();
                draw_pxiel_color();
                break;

            case 1:  // brightness 0
                M5.Log.printf("==== brightness 0\n");
                unit.setPixelColor(0, 100, 0, 0);
                unit.setPixelColor(1, 0, 100, 0);
                unit.setPixelColor(2, 0, 0, 100);
                unit.setBrightness(0);
                unit.show();
                draw_pxiel_color();
                break;

            case 2:  // brightness 16
                // Pixel values are retained despite changes in brightness
                M5.Log.printf("==== brightness 16\n");
                unit.setBrightness(10);
                unit.show();
                draw_pxiel_color();
                break;

            case 3:  // setPixelColor
                M5.Log.printf("==== setPixelColor\n");
                unit.setPixelColor(0, 0, 0, 100);         // RGB
                unit.setPixelColor(1, 0x0000FF00);        // WRGB32
                unit.setPixelColor(2, Color(100, 0, 0));  // Color
                unit.show();
                draw_pxiel_color();
                break;

            case 4:  // Swap [0] <-> [1]
            {
                M5.Log.printf("==== swap 0-1\n");
                Color tmp = unit.color(0);
                unit.setPixelColor(0, unit.color(1));
                unit.setPixelColor(1, tmp);
                unit.show();
                draw_pxiel_color();
            } break;

            case 5:  // Swap [1] <-> [2]
            {
                M5.Log.printf("==== swap 1-2\n");
                // Accessible by Color in [n]
                Color tmp = unit[1];
                unit[1]   = unit[2];
                unit[2]   = tmp;
                unit.show();
                draw_pxiel_color();
            } break;

            case 6:  // flll
                M5.Log.printf("==== fill\n");
                unit.fill(Color(100, 0, 0), 0, unit.numPixels() / 3);
                unit.fill(Color(0, 100, 0), unit.numPixels() / 3);
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);

                unit.fill(0x00FFFFFF);
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);

                unit.clear();
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);
                break;

            case 7:  // Fill random
                M5.Log.printf("==== fill random\n");

                unit.fillRandom(unit.numPixels() / 2);
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);

                unit.fillRandom(0, unit.numPixels() / 2);
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);

                unit.fillRandom();
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(1000);
                break;

            case 8:  // shift pixel array
            {
                M5.Log.printf("==== shift\n");

                unit.fillRainbow();
                unit.show();
                draw_pxiel_color();

                uint32_t count = std::min((uint16_t)10U, unit.numPixels());
                while (count--) {
                    unit.shiftRight(2);
                    unit.show();
                    draw_pxiel_color();
                    m5::utility::delay(20);
                }
                unit.fillRainbow();
                unit.show();
                m5::utility::delay(500);

                count = std::min((uint16_t)10U, unit.numPixels());
                while (count--) {
                    unit.shiftLeft(2);
                    unit.show();
                    draw_pxiel_color();
                    m5::utility::delay(20);
                }
            } break;

            case 9:  // rotate pixel array
                M5.Log.printf("==== rotate\n");
                unit.fillRainbow();
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(200);
                for (uint16_t i = 0; i < unit.numPixels(); ++i) {
                    unit.rotateRight(1);
                    unit.show();
                    draw_pxiel_color();
                    m5::utility::delay(20);
                }

                unit.fillRainbow();
                unit.show();
                draw_pxiel_color();
                m5::utility::delay(200);
                for (uint16_t i = 0; i < unit.numPixels(); ++i) {
                    unit.rotateLeft(1);
                    unit.show();
                    draw_pxiel_color();
                    m5::utility::delay(20);
                }
                break;
        }
        M5.Speaker.tone(2000, 20);
        if (++func > 9) {
            static int8_t rot{};
            --rot;                  // Rotate counter clock wise
            unit.setRotation(rot);  // Rotate all pixels
            unit.show();
            func = 0;
        }
    }
}
