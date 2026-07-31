/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Daisy chain example — BtnA cycles two demo modes

  A: Independent Worlds  Each unit runs its own animation in local coordinates
  B: Comet               Pixel comet with fading tail sweeps the full chain

  **** NOTICE *********************************************************************
  This example assumes the following connections
  Core -> UnitRGB -> UnitPuzzle -> UnitPuzzle -> UnitHex -> UnitRGBLed15 -> UnitNeco
  -> UnitRGBLEDStrip30

  Modify the source code to suit your own connection situation.

  Hardware limits (see README "Note (large cascades / PbHub)"):
    - USB-powered boards (NanoC6 / NanoH2, ~500 mA) cannot supply enough current
      for the full ~248-LED chain; the 5V rail sags and downstream LEDs dim or go
      dark. Reduce chain length, lower brightness, or use an external 5V power source.
  *********************************************************************************
*/
#include <M5Unified.h>
#include <M5UnitUnified.h>
#include <wiring/m5_unit_unified_wiring.hpp>
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

// Chain in root→leaf order (set in setup)
constexpr uint8_t CHAIN_LEN{7};
m5::unit::UnitLED* s_chain[CHAIN_LEN];

// Per-unit LED offset and count within the flat chain (used by Demo C)
constexpr uint16_t OFFSETS[CHAIN_LEN]{0, 3, 67, 131, 168, 183, 219};
constexpr uint16_t COUNTS[CHAIN_LEN]{3, 64, 64, 37, 15, 36, 30};
constexpr uint16_t TOTAL_LEDS{249};

// Show entire chain via leaf
void show_all()
{
    strip.show();
}

// Shared HSV→RGB (S=1, V=v, H=0..359)
Color hue_color(uint16_t h, uint8_t v)
{
    h %= 360;
    const uint8_t hi{static_cast<uint8_t>(h / 60)};
    const uint8_t f{static_cast<uint8_t>(static_cast<uint16_t>(v) * (h % 60) / 60)};
    const uint8_t q{static_cast<uint8_t>(v - f)};
    switch (hi) {
        case 0:
            return Color(v, f, 0);
        case 1:
            return Color(q, v, 0);
        case 2:
            return Color(0, v, f);
        case 3:
            return Color(0, q, v);
        case 4:
            return Color(f, 0, v);
        default:
            return Color(v, 0, q);
    }
}

// Triangle wave 0..255
uint8_t tri(uint16_t p)
{
    p &= 511;
    return p < 256 ? static_cast<uint8_t>(p) : static_cast<uint8_t>(511u - p);
}

// =====================================================
// Shared: rotation step for puzzle[0], puzzle[1], hex
// step() called once per full loop completion by Demo C.
// =====================================================
namespace unit_rot {

uint8_t s_rot[3]{};

void reset()
{
    s_rot[0] = s_rot[1] = s_rot[2] = 0;
    puzzle[0].setRotation(0);
    puzzle[1].setRotation(0);
    hex.setRotation(0);
}

void step()
{
    puzzle[0].setRotation(++s_rot[0]);
    puzzle[1].setRotation(++s_rot[1]);
    hex.setRotation(++s_rot[2]);
}

}  // namespace unit_rot

// =====================================================
// Demo A  Independent Worlds
//
// Each unit runs a completely independent animation using
// only its own local coordinate system — no chain offsets
// or inter-unit position calculations needed.
//
// In FastLED this would require:
//   leds[OFFSET + j] with manual per-segment offset tracking,
//   manual brightness scaling, and manual pixel rotation.
// Here each unit object encapsulates all of that.
// =====================================================
namespace demo_a {

// Per-unit timers
uint32_t s_ms_rgb{0};
uint32_t s_ms_puz0{0};
uint32_t s_ms_puz1{0};
uint32_t s_ms_hex{0};
uint32_t s_ms_hex_rot{0};
uint32_t s_ms_tape{0};
uint32_t s_ms_neco{0};
uint32_t s_ms_strip{0};
uint32_t s_ms_show{0};

// rgb: 3 pixels cycle R→G→B one at a time
uint8_t s_rgb_phase{0};

// puzzle[0]: rainbow row, view rotates 90° per step
uint8_t s_puz0_rot{0};

// puzzle[1]: sparkle — each pixel fades independently
uint8_t s_spark[64]{};

// hex: hue cycles, brightness breathes, view rotates
uint16_t s_hex_hue{0};
uint16_t s_hex_bri_phase{0};
uint8_t s_hex_rot{0};

// tape: Larson scanner
uint8_t s_tape_pos{0};
int8_t s_tape_dir{1};

// strip: moving hue gradient
uint16_t s_strip_hue{0};

void start()
{
    // Each unit initialized in its own coordinate system — no offset knowledge
    rgb.setBrightness(10);
    rgb.clear();
    rgb.setPixelColor(0, Color(255, 0, 0));

    puzzle[0].setBrightness(10);
    puzzle[0].setRotation(0);
    puzzle[0].fillRainbow(0, 8);  // one row of rainbow, rest dark

    puzzle[1].setBrightness(10);
    puzzle[1].clear();
    for (auto& v : s_spark) v = 0;

    hex.setBrightness(10);
    hex.fill(hue_color(0, 255), 0, 7);  // 7 of 37 LEDs lit

    tape.setBrightness(10);
    tape.clear();
    tape.setPixelColor(0, Color(0, 0, 255));

    neco.setBrightness(10);
    neco.clear();
    neco.fillRainbow(0, 6);  // 6 of 36 LEDs lit

    strip.setBrightness(10);
    strip.clear();
    for (uint16_t j = 0; j < 8; ++j) {
        strip.setPixelColor(j, hue_color(j * 45, 255));
    }

    s_rgb_phase     = 0;
    s_puz0_rot      = 0;
    s_hex_hue       = 0;
    s_hex_bri_phase = 0;
    s_hex_rot       = 0;
    s_tape_pos      = 0;
    s_tape_dir      = 1;
    s_strip_hue     = 0;

    const uint32_t now{m5::utility::millis()};
    s_ms_rgb = s_ms_puz0 = s_ms_puz1 = s_ms_hex = s_ms_hex_rot = s_ms_tape = s_ms_neco = s_ms_strip = s_ms_show = now;

    show_all();
}

void update()
{
    const auto now{m5::utility::millis()};

    // rgb — one pixel lit at a time, cycles R→G→B (600ms/step)
    if (now - s_ms_rgb >= 600) {
        s_ms_rgb    = now;
        s_rgb_phase = (s_rgb_phase + 1) % 3;
        rgb.clear();
        const Color rgb_colors[]{Color(255, 0, 0), Color(0, 255, 0), Color(0, 0, 255)};
        rgb.setPixelColor(s_rgb_phase, rgb_colors[s_rgb_phase]);
    }

    // puzzle[0] — rainbow row, setRotation() reorients the local grid (700ms/step)
    if (now - s_ms_puz0 >= 700) {
        s_ms_puz0 = now;
        puzzle[0].setRotation(++s_puz0_rot);
    }

    // puzzle[1] — sparkle: pixels light up randomly and fade independently (40ms)
    if (now - s_ms_puz1 >= 40) {
        s_ms_puz1 = now;
        for (uint8_t j = 0; j < 64; ++j) {
            if (s_spark[j] > 25)
                s_spark[j] -= 25;
            else
                s_spark[j] = 0;
        }
        for (uint8_t i = 0; i < 3; ++i) {
            s_spark[esp_random() % 64] = 255;
        }
        // setPixelColor(j, ...) — j is local to puzzle[1], no chain offset
        for (uint8_t j = 0; j < 64; ++j) {
            puzzle[1].setPixelColor(j, Color(s_spark[j], s_spark[j] / 2, 0));
        }
    }

    // hex — 7 LEDs, hue cycles, brightness breathes 0..10 (30ms)
    if (now - s_ms_hex >= 30) {
        s_ms_hex = now;
        s_hex_hue += 2;
        s_hex_bri_phase += 6;
        hex.fill(hue_color(s_hex_hue, 255), 0, 7);
        hex.setBrightness(static_cast<uint8_t>((tri(s_hex_bri_phase) * 10u) >> 8));
    }

    // hex — view rotates 60° per step (800ms)
    if (now - s_ms_hex_rot >= 800) {
        s_ms_hex_rot = now;
        hex.setRotation(++s_hex_rot);
    }

    // tape — Larson scanner, pixel bounces 0..14 (60ms/step)
    if (now - s_ms_tape >= 60) {
        s_ms_tape = now;
        tape.clear();
        if (s_tape_pos > 0) tape.setPixelColor(s_tape_pos - 1, Color(0, 0, 80));
        tape.setPixelColor(s_tape_pos, Color(0, 0, 255));
        if (s_tape_pos < 14) tape.setPixelColor(s_tape_pos + 1, Color(0, 0, 80));
        s_tape_pos = static_cast<uint8_t>(s_tape_pos + s_tape_dir);
        if (s_tape_pos >= 14 || s_tape_pos == 0) s_tape_dir = -s_tape_dir;
    }

    // neco — rainbow scrolls via rotateRight, no position math needed (100ms)
    if (now - s_ms_neco >= 100) {
        s_ms_neco = now;
        neco.rotateRight(1);
    }

    // strip — 8-pixel scrolling hue window, rest dark (60ms)
    if (now - s_ms_strip >= 60) {
        s_ms_strip = now;
        s_strip_hue += 4;
        strip.clear();
        const uint16_t base{static_cast<uint16_t>(s_strip_hue % 30)};
        for (uint16_t i = 0; i < 8; ++i) {
            strip.setPixelColor((base + i) % 30, hue_color((s_strip_hue + i * 45) % 360, 255));
        }
    }

    // Show once per ~30fps
    if (now - s_ms_show >= 33) {
        s_ms_show = now;
        show_all();
    }
}

}  // namespace demo_a

// =====================================================
// Demo B  Comet
// Pixel comet + fading tail sweeps through all 249 LEDs.
// Max simultaneous LEDs: TAIL_LEN (12)
// =====================================================
namespace demo_b {

constexpr float SPEED{1.0f};
constexpr uint8_t TAIL_LEN{12};
constexpr uint8_t MAX_V{30};

float s_pos{0.0f};
uint16_t s_hue{0};
uint32_t s_ms{0};

void start()
{
    for (uint8_t i = 0; i < CHAIN_LEN; ++i) {
        s_chain[i]->setBrightness(255);
        s_chain[i]->clear();
    }
    s_pos = 0.0f;
    s_hue = 0;
    s_ms  = 0;
    unit_rot::reset();
    show_all();
}

void update()
{
    const auto now{m5::utility::millis()};
    if (now - s_ms < 16) return;
    s_ms = now;

    s_pos += SPEED;
    if (s_pos >= TOTAL_LEDS + TAIL_LEN) {
        s_pos = 0.0f;
        s_hue = (s_hue + 40) % 360;
        unit_rot::step();
    }

    for (uint8_t u = 0; u < CHAIN_LEN; ++u) {
        const uint16_t off{OFFSETS[u]};
        const uint16_t cnt{COUNTS[u]};
        for (uint16_t j = 0; j < cnt; ++j) {
            const float dist{s_pos - static_cast<float>(off + j)};
            if (dist >= 0.0f && dist < static_cast<float>(TAIL_LEN)) {
                const uint8_t v{static_cast<uint8_t>((1.0f - dist / TAIL_LEN) * MAX_V)};
                s_chain[u]->setPixelColor(j, hue_color(s_hue, v));
            } else {
                s_chain[u]->setPixelColor(j, Color());
            }
        }
    }
    show_all();
}

}  // namespace demo_b

// =====================================================

enum class Mode : uint8_t { A = 0, B = 1 };
Mode s_mode{Mode::A};

void start_demo(Mode m)
{
    s_mode = m;
    switch (m) {
        case Mode::A:
            demo_a::start();
            break;
        case Mode::B:
            demo_b::start();
            break;
    }
    static const char* MODE_NAMES[]{"A: Worlds", "B: Comet"};
    M5.Log.printf("==== %s\n", MODE_NAMES[static_cast<uint8_t>(m)]);
    if (lcd.width() > 0) {
        lcd.fillScreen(TFT_DARKGREEN);
        lcd.setCursor(0, 0);
        lcd.setTextSize(2);
        lcd.print(MODE_NAMES[static_cast<uint8_t>(m)]);
    }
}

}  // namespace

void setup()
{
    M5.begin();
    M5.setTouchButtonHeightByRatio(100);

    if (lcd.height() > lcd.width()) {
        lcd.setRotation(1);
    }

#if SOC_RMT_SUPPORT_DMA
    {
        auto ucfg     = rgb.config();
        ucfg.with_dma = true;
        rgb.config(ucfg);
    }
#endif

    // Use default Both role — the chain contains a unit that needs rx (UnitNeco button).
    if (!neco.add(strip, 0) ||                     //
        !tape.add(neco, 0) ||                      //
        !hex.add(tape, 0) ||                       //
        !puzzle[1].add(hex, 0) ||                  //
        !puzzle[0].add(puzzle[1], 0) ||            //
        !rgb.add(puzzle[0], 0) ||                  //
        !m5::unit::wiring::addGPIO(Units, rgb) ||  //
        !Units.begin()) {
        M5_LOGE("Failed to begin");
        lcd.fillScreen(TFT_RED);
        while (true) {
            m5::utility::delay(10000);
        }
    }
    M5_LOGI("M5UnitUnified initialized");
    M5_LOGI("%s", Units.debugInfo().c_str());

    s_chain[0] = &rgb;
    s_chain[1] = &puzzle[0];
    s_chain[2] = &puzzle[1];
    s_chain[3] = &hex;
    s_chain[4] = &tape;
    s_chain[5] = &neco;
    s_chain[6] = &strip;

    start_demo(Mode::A);
}

void loop()
{
    M5.update();
    Units.update();

    if (M5.BtnA.wasClicked()) {
        start_demo(static_cast<Mode>((static_cast<uint8_t>(s_mode) + 1) % 2));
    }

    switch (s_mode) {
        case Mode::A:
            demo_a::update();
            break;
        case Mode::B:
            demo_b::update();
            break;
    }
    m5::utility::delay(1);
}
