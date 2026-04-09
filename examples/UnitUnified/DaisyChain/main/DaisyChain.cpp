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
  -> UnitRGBLEDStrip30

  Modify the source code to suit your own connection situation.

  NOTE: Depending on the core device, the power supply from the port may not be
  sufficient for all LEDs.
  *********************************************************************************
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <M5UnitUnifiedRGB.h>
#include <M5Utility.h>
#include <esp_random.h>

using m5::unit::rgb::Color;

namespace {
auto& lcd = M5.Display;
m5::unit::UnitUnified Units;

m5::unit::UnitRGB rgb;
m5::unit::UnitPuzzle puzzle[2];
m5::unit::UnitHex hex;
m5::unit::UnitRGBLED15 tape;
m5::unit::UnitNeco neco;
m5::unit::UnitRGBLEDStrip30 strip;
LGFX_Sprite sprite;

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

    //    neco.show();  // If the last tail is applied, it is applied to all

    //
    strip.fillRainbow();
    strip.show();
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
    static auto rgb_ms   = m5::utility::millis();
    static auto puz0_ms  = rgb_ms;
    static auto puz1_ms  = rgb_ms;
    static auto hex_ms   = rgb_ms;
    static auto tape_ms  = rgb_ms;
    static auto neco_ms  = rgb_ms;
    static auto strip_ms = rgb_ms;

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
        neco_ms = now;
        neco.rotateRight(1);
        neco.show();
    }
    /*
      Button status in UnitNeco cannot be acquired by daisy chain or connection via PbHub
      if(neco.wasClicked()){
           // Not reached!
      }
     */
    if (now >= strip_ms + 334) {
        //        strip.show();
    }
}

}  // namespace

#include <esp_log.h>

void setup()
{
    M5.begin();
    M5.setTouchButtonHeightByRatio(100);

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

    // Debug: Show power status
    M5_LOGI("=== Power Status ===");
    M5_LOGI("ExtOutput: %d", M5.Power.getExtOutput());
    M5_LOGI("BatVoltage: %d mV", M5.Power.getBatteryVoltage());
    M5_LOGI("BatCurrent: %d mA", M5.Power.getBatteryCurrent());
    M5_LOGI("BatLevel: %d%%", M5.Power.getBatteryLevel());
    M5_LOGI("isCharging: %d", M5.Power.isCharging());
    M5_LOGI("PMIC type: %d", M5.Power.getType());
    M5_LOGI("====================");

    // Enable DMA for RMT if supported (improves stability with long LED chains)
#if SOC_RMT_SUPPORT_DMA
    {
        auto ucfg     = rgb.config();
        ucfg.with_dma = true;
        rgb.config(ucfg);
    }
#endif

    if (!neco.add(strip, 0) ||                                 //
        !tape.add(neco, 0) ||                                  // RGBLED -> Neco
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

    M5_LOGI("M5UnitUnified initialized");
    M5_LOGI("%s", Units.debugInfo().c_str());

    sprite.setPsram(false);
    sprite.setColorDepth(1);
    sprite.createSprite(8, 8);

    rgb.setBrightness(10);
    puzzle[0].setBrightness(10);
    puzzle[1].setBrightness(10);
    hex.setBrightness(10);
    tape.setBrightness(10);
    neco.setBrightness(10);
    init_each();

    lcd.fillScreen(TFT_DARKGREEN);
}
void loop()
{
    static bool show{true};

    M5.update();
    Units.update();

    if (M5.BtnA.wasClicked()) {
        show = !show;
        if (show) {
            init_each();
        } else {
            clear_each();
        }
    }

    if (show) {
        loop_each();
    }
    m5::utility::delay(1);
}
