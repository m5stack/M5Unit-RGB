/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Daisy chain example

  **** NOTICE *********************************************************************
  This example assumes the following connections
  Core -> UnitRGB -> UnitPuzzle -> UnitPuzzle -> UnitHex -> UnitRGBLed15 -> UnitNeco

  Modify the source code to suit your own connection situation.
  *********************************************************************************
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedRGB.h>
#include <M5Utility.h>
#include <esp_random.h>

// When this is defined, treat each individual unit separately
#define USING_EACH_UNIT

using m5::unit::rgb::Color;

namespace {
auto& lcd = M5.Display;
m5::unit::UnitUnified Units;

#if defined(USING_EACH_UNIT)
m5::unit::UnitRGB rgb;
m5::unit::UnitPuzzle puzzle[2];
m5::unit::UnitHex hex;
m5::unit::UnitRGBLED15 tape;
m5::unit::UnitNeco neco;
#else
// Number of the all pixles
constexpr uint16_t NUMS{
    3 +         // RGB
    (64 * 2) +  // Puzzle x2
    37 +        // Hex
    15 +        // RGBLed15 (10cm)
    35          // Neco
};
m5::unit::UnitSK6812GRB unit(NUMS);  // Handle LEDs in bulk like FastLED and Adafruit_NeoPixel
#endif
LGFX_Sprite sprite;

#if defined(USING_EACH_UNIT)
void set_char_pixel(const char ch)
{
    sprite.setCursor(0, 0);
    sprite.printf("%c", ch);
    for (uint_fast8_t y = 0; y < 8; ++y) {
        for (uint_fast8_t x = 0; x < 8; ++x) {
            auto c = sprite.readPixelValue(x, y);
            // M5_LOGI("(%u,%u) %08X", x, y, c);
            puzzle[1].setPixelColor(y * 8 + x, c ? Color(0, 100, 0) : Color());
        }
    }
}

void init_each()
{
    rgb.setPixelColor(0, 0, 0, 100);

    puzzle[0].setRotation(1);
    puzzle[0].fillRainbow();

    puzzle[1].setRotation(3);
    set_char_pixel('!' + (esp_random() % (127 - 33)));

    hex.fillRainbow();

    tape.fillGradation(Color(0, 200, 0), Color(200, 0, 200));

    neco.fillRainbow();

    neco.show();  // If the last tail is applied, it is applied to all
}

void clear_each()
{
    rgb.clear();
    puzzle[0].clear();
    puzzle[1].clear();
    hex.clear();
    tape.clear();
    neco.clear();
    neco.show();  // If the last tail is applied, it is applied to all
}

void loop_each()
{
    static auto rgb_ms  = m5::utility::millis();
    static auto puz0_ms = rgb_ms;
    static auto puz1_ms = rgb_ms;
    static auto hex_ms  = rgb_ms;
    static auto tape_ms = rgb_ms;
    static auto neco_ms = rgb_ms;

    auto now = m5::utility::millis();

    static uint8_t phase{};
    if (now >= rgb_ms + 1000) {
        phase  = (phase + 1) % 3;
        rgb_ms = now;
        switch (phase) {
            case 0:
                rgb.setPixelColor(0, 0, 0, 100);
                rgb.fill(Color(), 1);
                break;
            case 1:
                rgb.setPixelColor(0, 0, 0, 0);
                rgb.setPixelColor(1, 100, 100, 0);
                rgb.setPixelColor(2, 0, 0, 0);
                break;
            case 2:
                rgb.fill(Color(), 0, 2);
                rgb.setPixelColor(2, 100, 0, 0);
                break;
        }
        rgb.show();
    }

    if (now >= puz0_ms + 100) {
        puz0_ms = now;
        puzzle[0].rotateRight(1);
        puzzle[0].show();
    }
    if (now >= puz1_ms + 50) {
        static int16_t bri{10};
        static int16_t add{2};
        puz1_ms = now;

        bri += add;
        if (bri > 32) {
            bri = 32;
            add = -add;
        }
        if (bri < 0) {
            bri = 0;
            add = -add;
            set_char_pixel('!' + (esp_random() % (127 - 33)));
        }
        puzzle[1].setBrightness(bri);
        puzzle[1].show();
    }

    if (now >= hex_ms + 250) {
        static uint8_t rot{};
        ++rot;
        hex_ms = now;
        hex.setRotation(rot);
        hex.show();
    }

    if (now >= tape_ms + 2000) {
        tape_ms = now;
        Color c(esp_random(), esp_random(), 0);
        tape.fillGradation(c, Color(255 - c.r, 255 - c.g, 255 - c.b));
        tape.show();
    }

    if (now >= neco_ms + 150) {
        neco.rotateRight(1);
        neco.show();
    }
    /*
      Button status in UnitNeco cannot be acquired by daisy chain or connection via PbHub
      if(neco.wasClicked()){
           // Not reached!
      }
     */
}
#else
void init_all()
{
    unit.fillRainbow();
    unit.show();
}

void clear_all()
{
    unit.clear();
    unit.show();
}

void loop_all()
{
    static auto ms = m5::utility::millis();
    auto now       = m5::utility::millis();

    if (now > ms + 50) {
        ms = now;
        unit.rotateRight(1);
        unit.show();
    }
}
#endif

}  // namespace

#include <esp_log.h>

void setup()
{
    //    esp_log_level_set("*", ESP_LOG_DEBUG);
    M5.begin();

    // The screen shall be in landscape mode
    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

    // PortB if available, PortA if not
    auto pin_num_gpio_in  = M5.getPin(m5::pin_name_t::port_b_in);
    auto pin_num_gpio_out = M5.getPin(m5::pin_name_t::port_b_out);
    if (pin_num_gpio_in < 0 || pin_num_gpio_out < 0) {
        M5_LOGW("PortB is not available");
        Wire.end();
        pin_num_gpio_in  = M5.getPin(m5::pin_name_t::port_a_pin1);
        pin_num_gpio_out = M5.getPin(m5::pin_name_t::port_a_pin2);
    }
    M5_LOGI("getPin: OUT:%d", pin_num_gpio_out);

#if defined(USING_EACH_UNIT)
    if (!tape.add(neco, 0) ||                                  // RGBLED -> Neco
        !hex.add(tape, 0) ||                                   // hex -> RGBLED
        !puzzle[1].add(hex, 0) ||                              // puzzle[1] -> hex
        !puzzle[0].add(puzzle[1], 0) ||                        // puzzle[0] -> puzzle[1]
        !rgb.add(puzzle[0], 0) ||                              // rgb -> puzzle[0]
        !Units.add(rgb, pin_num_gpio_in, pin_num_gpio_out) ||  // core -> rgb
        !Units.begin()) {
        M5_LOGE("Failed to begin");
        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }
#else
    if (!Units.add(unit, pin_num_gpio_in, pin_num_gpio_out) || !Units.begin()) {
        M5_LOGE("Failed to begin");
        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }
#endif

    M5_LOGI("M5UnitUnified has been begun");
    M5_LOGI("%s", Units.debugInfo().c_str());

    sprite.setPsram(false);
    sprite.setColorDepth(1);
    sprite.createSprite(8, 8);

#if defined(USING_EACH_UNIT)
    rgb.setBrightness(10);
    puzzle[0].setBrightness(10);
    puzzle[1].setBrightness(10);
    hex.setBrightness(10);
    tape.setBrightness(10);
    neco.setBrightness(10);
    init_each();
#else
    unit.setBrightness(10);
    init_all();
#endif

    lcd.fillScreen(TFT_DARKGREEN);
}
void loop()
{
    static bool show{true};

    M5.update();
    auto touch = M5.Touch.getDetail();
    Units.update();

    if (M5.BtnA.wasClicked() || touch.wasClicked()) {
        show = !show;
#if defined(USING_EACH_UNIT)
        if (show) {
            init_each();
        } else {
            clear_each();
        }
#else
        if (show) {
            init_all();
        } else {
            clear_all();
        }
#endif
    }

    if (show) {
#if defined(USING_EACH_UNIT)
        loop_each();
#else
        loop_all();
#endif
    }
    m5::utility::delay(1);
}
