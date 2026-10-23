/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_LED.cpp
  @brief LED base unit for M5UnitUnified
*/
#include "unit_LED.hpp"
#include "utility/m5unit_rgb_utility.hpp"
#include <M5Utility.hpp>
#include <esp_random.h>

// Direct GPIO drive needs RMT; ESP32-C2 / C61 have none, so only the PbHub path is available there.
// M5UnitUnified before 0.6.0 does not define M5_UNIT_UNIFIED_HAS_RMT and supports RMT-capable chips only.
#if !defined(M5_UNIT_UNIFIED_HAS_RMT) || M5_UNIT_UNIFIED_HAS_RMT
#define M5_UNIT_RGB_HAS_RMT 1
#else
#define M5_UNIT_RGB_HAS_RMT 0
#endif

#if M5_UNIT_RGB_HAS_RMT
#if defined(M5_UNIT_UNIFIED_USING_RMT_V2)
#include <driver/rmt_tx.h>
#include <driver/rmt_encoder.h>
#else
#include <driver/rmt.h>
#endif
#endif

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit;
using namespace m5::unit::rgb;

namespace {
// Color order
constexpr int8_t offset_table[][4] = {
    // 24bits
    {0, 1, 2, -1},  // rgb24
    {0, 2, 1, -1},  // rbg24
    {1, 0, 2, -1},  // grb24
    {2, 0, 1, -1},  // gbr24
    {1, 2, 0, -1},  // brg24
    {2, 1, 0, -1},  // bgr24
    // 32bits
    {1, 2, 3, 0},  // wrgb32
    {1, 3, 2, 0},  // wrbg32
    {2, 1, 3, 0},  // wgrb32
    {3, 1, 2, 0},  // wgbr32
    {2, 3, 1, 0},  // wbrg32
    {3, 2, 1, 0},  // wbgr32
    //
    {0, 2, 3, 1},  // rwgb32
    {0, 3, 2, 1},  // rwbg32
    {0, 1, 3, 2},  // rgwb32
    {0, 1, 2, 3},  // rgbw32
    {0, 3, 1, 2},  // rbwg32
    {0, 2, 1, 3},  // rbgw32
    //
    {2, 0, 3, 1},  // gwrb32
    {3, 0, 2, 1},  // gwbr32
    {1, 0, 3, 2},  // grwb32
    {1, 0, 2, 3},  // grbw32
    {3, 0, 1, 2},  // gbwr32
    {2, 0, 1, 3},  // gbrw32
    //
    {2, 3, 0, 1},  // bwrg32
    {3, 2, 0, 1},  // bwgr32
    {1, 3, 0, 2},  // brwg32
    {1, 2, 0, 3},  // brgw32
    {3, 1, 0, 2},  // bgwr32
    {2, 1, 0, 3},  // bgrw32
};

int8_t offset_of(const Order f, const uint8_t off)
{
    const auto i = m5::stl::to_underlying(f);
    if (i >= 0 && static_cast<size_t>(i) < m5::stl::size(offset_table) && off < 4) {
        return offset_table[i][off];
    }
    return -1;
}

inline bool is_valid_order(const Order f)
{
    auto i = m5::stl::to_underlying(f);
    return i >= 0 && static_cast<size_t>(i) < m5::stl::size(offset_table);
}

inline bool is_unit_led(const Component* u)
{
    return (u->category() == category_t::UnitLED);
}

inline bool is_unit_pbhub(const Component* u)
{
    static constexpr types::uid_t pbhub_uid{"UnitPbHub"_mmh3};
    return u->identifier() == pbhub_uid;
}

#if !defined(M5_UNIT_UNIFIED_USING_RMT_V2)
// RMT V1: simple translator — converts bytes to RMT items (bit by bit, MSB first)
struct SimpleTranslatorContext {
    rmt_item32_t bit1;
    rmt_item32_t bit0;
};

static void IRAM_ATTR led_rmt_translator(const void* src, rmt_item32_t* dest, size_t src_size, size_t wanted_num,
                                         size_t* translated_size, size_t* item_num)
{
    SimpleTranslatorContext* ctx{};
    rmt_translator_get_context(item_num, reinterpret_cast<void**>(&ctx));

    const uint8_t* psrc   = static_cast<const uint8_t*>(src);
    size_t items_produced = 0;
    size_t bytes_consumed = 0;

    while (items_produced + 8 <= wanted_num && bytes_consumed < src_size) {
        uint8_t byte = psrc[bytes_consumed];
        for (int bit = 7; bit >= 0; --bit) {
            dest[items_produced++] = (byte & (1 << bit)) ? ctx->bit1 : ctx->bit0;
        }
        bytes_consumed++;
    }

    *translated_size = bytes_consumed;
    *item_num        = items_produced;
}
#endif

}  // namespace

namespace m5 {
namespace unit {

namespace rgb {

int8_t offsetR(const Order f)
{
    return offset_of(f, 0);
}

int8_t offsetG(const Order f)
{
    return offset_of(f, 1);
}

int8_t offsetB(const Order f)
{
    return offset_of(f, 2);
}

int8_t offsetW(const Order f)
{
    return offset_of(f, 3);
}

}  // namespace rgb

///@cond
#if M5_UNIT_RGB_HAS_RMT
// For GPIO
class InterfaceGPIO : public UnitLED::Interface {
public:
    explicit InterfaceGPIO(UnitLED& u) : UnitLED::Interface(u)
    {
    }

#if defined(M5_UNIT_UNIFIED_USING_RMT_V2)
    virtual ~InterfaceGPIO()
    {
        if (_bytes_encoder) {
            rmt_del_encoder(_bytes_encoder);
        }
        if (_reset_encoder) {
            rmt_del_encoder(_reset_encoder);
        }
    }
#endif

    virtual bool write(const uint8_t* data, const uint32_t size) override
    {
        return _unit.writeWithTransaction(data, size, 50 /* alldone wait ms */) == m5::hal::error::error_t::OK;
    }

    // Build tree: root → leaf
    void build_tree(std::vector<UnitLED*>& tree) const
    {
        Component* p = &_unit;
        while (p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
        // Only traverse UnitLED children so a non-LED child in the tree cannot be downcast to UnitLED*.
        while (p && is_unit_led(p)) {
            tree.push_back(static_cast<UnitLED*>(p));
            p = p->child(0);
        }
    }

    // Build brightness-applied byte buffer from tree
    static void build_pixel_bytes(const std::vector<UnitLED*>& tree, std::vector<uint8_t>& buf)
    {
        uint32_t total = 0;
        for (const auto* led : tree) {
            total += led->numPixels() * (3 + led->hasWhite());
        }
        buf.reserve(total);
        for (auto* led : tree) {
            auto align         = 3 + led->hasWhite();
            uint8_t bri        = led->brightness();
            const uint8_t* src = led->pixels().data();
            for (uint32_t i = 0; i < led->numPixels() * align; ++i) {
                buf.push_back(rgb::scale8(src[i], bri));
            }
        }
    }

#if !defined(M5_UNIT_UNIFIED_USING_RMT_V2)
    bool writePixelsDirect() override
    {
        auto ad = _unit.asAdapter<AdapterGPIO>(Adapter::Type::GPIO);
        if (!ad) {
            return false;
        }
        auto channel = static_cast<rmt_channel_t>(ad->impl()->rmtTxChannel());
        if (channel >= RMT_CHANNEL_MAX) {
            return false;
        }

        // 1. Build brightness-applied byte buffer
        std::vector<UnitLED*> tree;
        build_tree(tree);
        std::vector<uint8_t> buf;
        build_pixel_bytes(tree, buf);
        if (buf.empty()) {
            return false;
        }

        // 2. Set up simple translator context (just bit patterns)
        SimpleTranslatorContext ctx;
        ctx.bit1 = _unit.rmt_item_one();
        ctx.bit0 = _unit.rmt_item_zero();

        // 3. Send pixel data
        rmt_translator_init(channel, led_rmt_translator);
        rmt_translator_set_context(channel, &ctx);
        auto err = rmt_write_sample(channel, buf.data(), buf.size(), false);
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_write_sample failed: %d", err);
            return false;
        }
        err = rmt_wait_tx_done(channel, pdMS_TO_TICKS(100));
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_wait_tx_done failed: %d", err);
            return false;
        }
        // Latch: hold line low for the chip's reset window (per rmt_item_reset())
        const auto reset_it = _unit.rmt_item_reset();
        const auto cfg_tx   = _unit.rmt_config();
        if (reset_it.duration0 > 0 && cfg_tx.tx.tick_ns > 0) {
            const uint32_t reset_us = (static_cast<uint32_t>(reset_it.duration0) * cfg_tx.tx.tick_ns + 999) / 1000;
            m5::utility::delayMicroseconds(reset_us);
        }
        return true;
    }
#else
    bool writePixelsDirect() override
    {
        auto ad = _unit.asAdapter<AdapterGPIO>(Adapter::Type::GPIO);
        if (!ad) {
            return false;
        }
        auto tx_handle = static_cast<rmt_channel_handle_t>(ad->impl()->rmtTxHandle());
        if (!tx_handle) {
            return false;
        }

        // 1. Build tree and brightness-applied byte buffer
        std::vector<UnitLED*> tree;
        build_tree(tree);
        std::vector<uint8_t> buf;
        build_pixel_bytes(tree, buf);
        if (buf.empty()) {
            return false;
        }

        // 2. Create bytes encoder (lazy init)
        if (!_bytes_encoder) {
            auto bit1 = _unit.rmt_item_one();
            auto bit0 = _unit.rmt_item_zero();
            rmt_bytes_encoder_config_t enc_cfg{};
            enc_cfg.bit0.duration0  = bit0.duration0;
            enc_cfg.bit0.level0     = bit0.level0;
            enc_cfg.bit0.duration1  = bit0.duration1;
            enc_cfg.bit0.level1     = bit0.level1;
            enc_cfg.bit1.duration0  = bit1.duration0;
            enc_cfg.bit1.level0     = bit1.level0;
            enc_cfg.bit1.duration1  = bit1.duration1;
            enc_cfg.bit1.level1     = bit1.level1;
            enc_cfg.flags.msb_first = 1;
            auto err                = rmt_new_bytes_encoder(&enc_cfg, &_bytes_encoder);
            if (err != ESP_OK) {
                M5_LIB_LOGE("rmt_new_bytes_encoder failed: %d", err);
                return false;
            }
        }

        // 3. Transmit
        rmt_transmit_config_t tx_cfg{};
        tx_cfg.loop_count = 0;
        auto err          = rmt_transmit(tx_handle, _bytes_encoder, buf.data(), buf.size(), &tx_cfg);
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_transmit failed: %d", err);
            return false;
        }
        err = rmt_tx_wait_all_done(tx_handle, 100);
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_tx_wait_all_done failed: %d", err);
            return false;
        }

        // 4. Send reset pulse
        auto reset_enc = copy_encoder_for_reset();
        if (!reset_enc) {
            return false;
        }
        auto reset = _unit.rmt_item_reset();
        err        = rmt_transmit(tx_handle, reset_enc, &reset, sizeof(reset), &tx_cfg);
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_transmit(reset) failed: %d", err);
            return false;
        }
        err = rmt_tx_wait_all_done(tx_handle, 100);
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_tx_wait_all_done(reset) failed: %d", err);
            return false;
        }
        return true;
    }

private:
    rmt_encoder_handle_t copy_encoder_for_reset()
    {
        if (!_reset_encoder) {
            rmt_copy_encoder_config_t cfg{};
            auto err = rmt_new_copy_encoder(&cfg, &_reset_encoder);
            if (err != ESP_OK) {
                M5_LIB_LOGE("rmt_new_copy_encoder failed: %d", err);
                _reset_encoder = nullptr;
            }
        }
        return _reset_encoder;
    }

    rmt_encoder_handle_t _bytes_encoder{};
    rmt_encoder_handle_t _reset_encoder{};
#endif
};
#endif  // M5_UNIT_RGB_HAS_RMT

class InterfacePbHub : public UnitLED::Interface {
public:
    explicit InterfacePbHub(UnitLED& u) : UnitLED::Interface(u)
    {
    }

    virtual UnitLED::container_type rgb888_items() override
    {
        UnitLED::container_type v;
        auto align        = 3 + _unit.hasWhite();
        auto roff         = _unit.offsetR();
        auto goff         = _unit.offsetG();
        auto boff         = _unit.offsetB();
        const uint8_t bri = _unit.brightness();

        v.reserve(_unit.numPixels() * 3);

        for (uint32_t i = 0; i < _unit.numPixels(); ++i) {
            const uint8_t* p = _unit.pixels().data() + i * align;
            v.emplace_back(rgb::scale8(p[roff], bri));
            v.emplace_back(rgb::scale8(p[goff], bri));
            v.emplace_back(rgb::scale8(p[boff], bri));
        }
        return v;
    }

    virtual UnitLED::container_type rgb888_items_from_root() override
    {
        UnitLED::container_type v;

        // Get the number of items from the root LED
        uint32_t cnt = _unit.numPixels();
        auto p       = _unit.parent();
        while (p && is_unit_led(p)) {
            cnt += static_cast<UnitLED*>(p)->numPixels();
            p = p->parent();
        }
        v.reserve(cnt * 3);

        // Get the items from the root LED
        p = &_unit;
        while (p && p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
        // Stop at any non-LED child to avoid downcasting an unrelated Component to UnitLED*.
        while (p && p != &_unit && is_unit_led(p)) {
            auto i = static_cast<UnitLED*>(p)->rgb888_items();
            v.insert(v.end(), i.begin(), i.end());
            p = p->child(0);
        }
        auto i = rgb888_items();
        v.insert(v.end(), i.begin(), i.end());
        return v;
    }

    virtual bool write(const uint8_t* data, const uint32_t size) override
    {
        return _unit.writeWithTransaction(data, size, true) == m5::hal::error::error_t::OK;
    }
};
///@endcond

// class UnitLED
const char UnitLED::name[] = "UnitLED";
const types::uid_t UnitLED::uid{"UnitLED"_mmh3};
const types::attr_t UnitLED::attr{attribute::AccessGPIO};

bool UnitLED::add(UnitLED& c, const int16_t channel)
{
    return Component::add(c, channel);
}

bool UnitLED::begin()
{
    if (!is_valid_order(_order)) {
        M5_LIB_LOGE("invalid Order: %d", static_cast<int>(_order));
        return false;
    }
    if (_numPixels == 0) {
        M5_LIB_LOGE("numPixels must be > 0");
        return false;
    }

    // Search root unit
    Component* p = this;
    // 1st: Exists PbHub in tree?
    while (p) {
        p = p->parent();
        if (p && is_unit_pbhub(p)) {
            break;
        }
    }
    // 2nd: Search root UnitLED
    if (!p) {  // No PbHub
        p = this;
        while (p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
    }
    //  p is root (UnitLED or UnitPbHub)

    // begin RMT if Root is not UnitPbHub
    M5_LIB_LOGI("Root is %s", p->deviceName());
    if (!is_unit_pbhub(p)) {
#if M5_UNIT_RGB_HAS_RMT
        gpio::adapter_config_t cfg = rmt_config();
        // .channel and .gpio_num will set in begin(cfg)
        // Override RMT settings from config_t
        if (_cfg.mem_blocks > 0) {
            cfg.tx.mem_blocks = _cfg.mem_blocks;
        }
        if (_cfg.with_dma) {
            cfg.tx.with_dma = true;
        }
        auto ad = asAdapter<AdapterGPIO>(Adapter::Type::GPIO);
        if (!ad || !ad->begin(cfg)) {
            M5_LIB_LOGE("Failed to begin AdapterGPIO");
            return false;
        }
        _interface.reset(new InterfaceGPIO(*this));
#else
        M5_LIB_LOGE("RMT is not supported on this SoC, connect via PbHub");
        return false;
#endif
    } else {
        _interface.reset(new InterfacePbHub(*this));
    }

    _brightness = _cfg.brightness;
    _ready      = true;

    return true;
}

bool UnitLED::writePixels(void)
{
    if (!_ready) {
        return false;
    }
    bool ret{};

    switch (adapter()->type()) {
        // GPIO
        case Adapter::Type::GPIO: {
            ret = _interface->writePixelsDirect();
        } break;
        // PbHub
        case Adapter::Type::I2C: {
            auto v = _interface->rgb888_items_from_root();
            ret    = _interface->write(v.data(), v.size());
        } break;
        default:
            M5_LIB_LOGE("No valid adapter");
            break;
    }
    return ret;
}

bool UnitLED::setPixelColor(const uint16_t n, const uint8_t r, const uint8_t g, const uint8_t b)
{
    if (!_ready || n >= _numPixels) {
        return false;
    }
    Color c = color(n);
    c.r     = r;
    c.g     = g;
    c.b     = b;
    setPixelColor(n, c);
    return true;
}

bool UnitLED::fill(const Color& c, const uint16_t first, const uint16_t count)
{
    if (!_ready || first >= _numPixels) {
        return false;
    }
    const uint16_t remain = _numPixels - first;
    const uint16_t num    = count ? std::min(count, remain) : remain;
    for (uint16_t n = first; n < first + num; ++n) {
        setPixelColor(n, c);
    }
    return true;
}

bool UnitLED::fillRainbow(const uint16_t first, const uint16_t count, const uint8_t firstHue)
{
    if (!_ready || first >= _numPixels) {
        return false;
    }
    const uint16_t remain = _numPixels - first;
    const uint16_t num    = count ? std::min(count, remain) : remain;

    uint16_t hue         = firstHue * 256U;  // 0 - 65535
    const uint16_t delta = 65536 / num;

    for (uint16_t i = first; i < first + num; ++i) {
        setPixelColor(i, hsv2rgb(hue >> 8, 255, 255));  // Using upper 8bits
        hue += delta;
    }
    return true;
}

bool UnitLED::fillGradation(const rgb::Color& firstClr, const rgb::Color& lastClr, const uint16_t first,
                            const uint16_t count)

{
    if (!_ready || first >= _numPixels) {
        return false;
    }
    const uint16_t remain = _numPixels - first;
    const uint16_t num    = count ? std::min(count, remain) : remain;
    if (num < 2) {
        return false;
    }

    for (uint16_t i = 0; i < num - 1; ++i) {
        const float ratio = static_cast<float>(i) / (num - 1);
        const uint8_t r   = static_cast<uint8_t>(firstClr.r + ratio * (lastClr.r - firstClr.r));
        const uint8_t g   = static_cast<uint8_t>(firstClr.g + ratio * (lastClr.g - firstClr.g));
        const uint8_t b   = static_cast<uint8_t>(firstClr.b + ratio * (lastClr.b - firstClr.b));
        const uint8_t w   = static_cast<uint8_t>(firstClr.w + ratio * (lastClr.w - firstClr.w));
        setPixelColor(first + i, Color(r, g, b, w));
    }
    setPixelColor(first + num - 1, lastClr);
    return true;
}

bool UnitLED::fillRandom(const uint16_t first, const uint16_t count)
{
    if (!_ready || first >= _numPixels) {
        return false;
    }
    const uint16_t remain = _numPixels - first;
    const uint16_t num    = count ? std::min(count, remain) : remain;

    for (uint16_t i = first; i < first + num; ++i) {
        setPixelColor(i, esp_random(), esp_random(), esp_random());
    }
    return true;
}

void UnitLED::setRotation(const int8_t rot)
{
    if (!_ready) {
        return;
    }
    const int8_t prev    = _rot;
    const int8_t new_rot = static_cast<int8_t>(rotation_of(rot));
    if (new_rot == prev) {
        return;
    }

    // Save logical pixel colors (read through current rotation)
    std::vector<Color> logical;
    logical.resize(_numPixels);
    for (uint16_t i = 0; i < _numPixels; ++i) {
        logical[i] = get_pixel_color(index_of(i));  // color(i)
    }

    // Apply new rotation and write logical colors back
    _rot = new_rot;
    for (uint16_t i = 0; i < _numPixels; ++i) {
        set_pixel_color(index_of(i), logical[i]);
    }
}

//
Color UnitLED::get_pixel_color(const uint16_t n) const
{
    Color c{};
    if (_ready && n < _numPixels) {
        const uint8_t* p = _pixels.data() + n * (3 + hasWhite());
        c.r              = p[_offsetR];
        c.g              = p[_offsetG];
        c.b              = p[_offsetB];
        c.w              = (_offsetW >= 0) ? p[_offsetW] : 0;
    }
    return c;
}

bool UnitLED::set_pixel_color(const uint16_t n, const Color& c)
{
    if (!_ready || n >= _numPixels) {
        return false;
    }
    uint8_t* p  = _pixels.data() + n * (3 + hasWhite());
    p[_offsetR] = c.r;
    p[_offsetG] = c.g;
    p[_offsetB] = c.b;
    if (_offsetW >= 0) {
        p[_offsetW] = c.w;
    }
    return true;
}

UnitLED::container_type UnitLED::rgb888_items()
{
    return _interface->rgb888_items();
}

void UnitLED::shift_left(const uint16_t count, const bool wrap)
{
    if (!_ready) {
        return;
    }
    uint16_t num = wrap ? (count % _numPixels) : std::min(count, _numPixels);
    if (!num) {
        return;
    }

    std::vector<Color> temp;
    if (wrap) {
        temp.reserve(num);
        for (uint16_t i = 0; i < num; ++i) {
            temp.push_back(color(i));
        }
    }

    for (uint16_t i = 0; i < _numPixels - num; ++i) {
        setPixelColor(i, color(i + num));
    }

    if (wrap) {
        for (uint16_t i = 0; i < num; ++i) {
            setPixelColor(_numPixels - num + i, temp[i]);
        }
    } else {
        for (uint16_t i = _numPixels - num; i < _numPixels; ++i) {
            setPixelColor(i, 0);
        }
    }
}

void UnitLED::shift_right(const uint16_t count, const bool wrap)
{
    if (!_ready) {
        return;
    }
    uint16_t num = wrap ? (count % _numPixels) : std::min(count, _numPixels);
    if (!num) {
        return;
    }

    std::vector<Color> temp;
    if (wrap) {
        temp.reserve(num);
        for (uint16_t i = 0; i < num; ++i) {
            temp.push_back(color(_numPixels - num + i));
        }
    }

    for (int i = _numPixels - 1; i >= static_cast<int>(num); --i) {
        setPixelColor(i, color(i - num));
    }

    if (wrap) {
        for (uint16_t i = 0; i < num; ++i) {
            setPixelColor(i, temp[i]);
        }
    } else {
        for (uint16_t i = 0; i < num; ++i) {
            setPixelColor(i, 0);
        }
    }
}

}  // namespace unit
}  // namespace m5
