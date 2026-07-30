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
#if defined(M5_UNIT_UNIFIED_USING_RMT_V2)
#include <driver/rmt_tx.h>
#include <driver/rmt_encoder.h>
#else
#include <driver/rmt.h>
#endif

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit;
using namespace m5::unit::rgb;

namespace {
// Color order
int8_t offset_table[][4] = {
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

int8_t offset_of(const Order f, uint8_t off)
{
    auto i = m5::stl::to_underlying(f);
    if (i >= 0 && i < m5::stl::size(offset_table) && off < 4) {
        return offset_table[i][off];
    }
    return -1;
}

inline bool is_unit_led(const Component* u)
{
    return (u->category() == category_t::UnitLED);
}

inline bool is_unit_pbhub(Component* u)
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
    rmt_translator_get_context(item_num, (void**)&ctx);

    const uint8_t* psrc   = (const uint8_t*)src;
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

Color Color::fromHSV(const uint8_t h, const uint8_t s, const uint8_t v)
{
    return Color(hsv2rgb(h, s, v));
}

}  // namespace rgb

///@cond
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

    virtual UnitLED::item_container_type items() override
    {
        const int bits_per_pixel  = 24 + 8 * _unit.hasWhite();
        const uint32_t total_bits = _unit.numPixels() * bits_per_pixel;

        auto align = 3 + _unit.hasWhite();
        auto roff  = _unit.offsetR();
        auto goff  = _unit.offsetG();
        auto boff  = _unit.offsetB();
        auto woff  = _unit.offsetW();

        UnitLED::item_container_type v;
        v.reserve(total_bits + 1 /*reset*/);

        const auto one  = _unit.rmt_item_one();
        const auto zero = _unit.rmt_item_zero();

        for (uint32_t i = 0; i < _unit.numPixels(); ++i) {
            const uint8_t* p = _unit.pixels().data() + i * align;
            float scale      = _unit.brightness() / 255.f;
            uint8_t colors[4]{};
            colors[roff] = p[roff] * scale;
            colors[goff] = p[goff] * scale;
            colors[boff] = p[boff] * scale;
            if (woff >= 0) {
                colors[woff] = p[woff] * scale;
            }
            for (uint_fast8_t ci = 0; ci < align; ++ci) {
                uint8_t bit{0x80};
                while (bit) {
                    v.push_back((colors[ci] & bit) ? one : zero);
                    bit >>= 1;
                }
            }
        }
        // M5_LIB_LOGV("Make %u/%zu->%zu items", _unit.numPixels(), _unit.pixels().size(), v.size());
        return v;
    }

    // Append RMT items for a single LED unit directly into buf (no temporary vector)
    static void append_items(UnitLED& led, UnitLED::item_container_type& buf)
    {
        auto align = 3 + led.hasWhite();
        auto roff  = led.offsetR();
        auto goff  = led.offsetG();
        auto boff  = led.offsetB();
        auto woff  = led.offsetW();

        const auto one    = led.rmt_item_one();
        const auto zero   = led.rmt_item_zero();
        const float scale = led.brightness() / 255.f;

        for (uint32_t i = 0; i < led.numPixels(); ++i) {
            const uint8_t* p = led.pixels().data() + i * align;
            uint8_t colors[4]{};
            colors[roff] = p[roff] * scale;
            colors[goff] = p[goff] * scale;
            colors[boff] = p[boff] * scale;
            if (woff >= 0) {
                colors[woff] = p[woff] * scale;
            }
            for (uint_fast8_t ci = 0; ci < align; ++ci) {
                uint8_t bit{0x80};
                while (bit) {
                    buf.push_back((colors[ci] & bit) ? one : zero);
                    bit >>= 1;
                }
            }
        }
    }

    virtual UnitLED::item_container_type items_from_root() override
    {
        // 1. Walk to root and count total pixels
        uint32_t cnt = _unit.numPixels();
        auto p       = _unit.parent();
        while (p && is_unit_led(p)) {
            cnt += static_cast<UnitLED*>(p)->numPixels();
            p = p->parent();
        }

        // 2. Single buffer allocation
        auto align = 3 + _unit.hasWhite();
        UnitLED::item_container_type v;
        v.reserve(cnt * align * 8 + 1 /*reset*/);

        // 3. Walk from root, append items directly
        p = &_unit;
        while (p && p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
        while (p && p != &_unit) {
            append_items(*static_cast<UnitLED*>(p), v);
            p = p->child(0);
        }
        append_items(_unit, v);

        // 4. Reset item
        v.push_back(_unit.rmt_item_reset());
        return v;
    }

    virtual bool write(const uint8_t* data, const uint32_t size)
    {
        return _unit.writeWithTransaction(data, size, 50 /* alldone wait ms */) == m5::hal::error::error_t::OK;
    }

    // Build tree: root → leaf
    void build_tree(std::vector<UnitLED*>& tree)
    {
        Component* p = &_unit;
        while (p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
        while (p) {
            tree.push_back(static_cast<UnitLED*>(p));
            p = p->child(0);
        }
    }

    // Build brightness-applied byte buffer from tree
    void build_pixel_bytes(const std::vector<UnitLED*>& tree, std::vector<uint8_t>& buf)
    {
        uint32_t total = 0;
        for (auto* led : tree) {
            total += led->numPixels() * (3 + led->hasWhite());
        }
        buf.reserve(total);
        for (auto* led : tree) {
            auto align         = 3 + led->hasWhite();
            uint8_t bri        = led->brightness();
            const uint8_t* src = led->pixels().data();
            for (uint32_t i = 0; i < led->numPixels() * align; ++i) {
                buf.push_back((src[i] * bri) >> 8);
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
        auto channel = (rmt_channel_t)ad->impl()->rmtTxChannel();
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
        err = rmt_tx_wait_all_done(tx_handle, pdMS_TO_TICKS(100));
        if (err != ESP_OK) {
            M5_LIB_LOGE("rmt_tx_wait_all_done failed: %d", err);
            return false;
        }

        // 4. Send reset pulse
        auto reset = _unit.rmt_item_reset();
        rmt_transmit(tx_handle, _copy_encoder_for_reset(), &reset, sizeof(reset), &tx_cfg);
        rmt_tx_wait_all_done(tx_handle, pdMS_TO_TICKS(10));
        return true;
    }

    rmt_encoder_handle_t _copy_encoder_for_reset()
    {
        if (!_reset_encoder) {
            rmt_copy_encoder_config_t cfg{};
            rmt_new_copy_encoder(&cfg, &_reset_encoder);
        }
        return _reset_encoder;
    }

    rmt_encoder_handle_t _bytes_encoder{};
    rmt_encoder_handle_t _reset_encoder{};
#endif
};

class InterfacePbHub : public UnitLED::Interface {
public:
    explicit InterfacePbHub(UnitLED& u) : UnitLED::Interface(u)
    {
    }

    virtual UnitLED::container_type rgb888_items()
    {
        UnitLED::container_type v;
        auto align        = 3 + _unit.hasWhite();
        auto roff         = _unit.offsetR();
        auto goff         = _unit.offsetG();
        auto boff         = _unit.offsetB();
        const float scale = _unit.brightness() / 255.f;

        v.reserve(_unit.numPixels() * 3);

        for (uint32_t i = 0; i < _unit.numPixels(); ++i) {
            const uint8_t* p = _unit.pixels().data() + i * align;
            v.emplace_back(p[roff] * scale);
            v.emplace_back(p[goff] * scale);
            v.emplace_back(p[boff] * scale);
        }
        return v;
    }

    virtual UnitLED::container_type rgb888_items_from_root()
    {
        UnitLED::container_type v;
        // auto align = 3 + _unit.hasWhite();

        // Get the number of items from the root LED
        uint32_t cnt = _unit.numPixels();
        auto p       = _unit.parent();
        while (p && is_unit_led(p)) {
            cnt += static_cast<UnitLED*>(p)->numPixels();
            p = p->parent();
        }
        v.reserve(cnt * 3);
        // M5_LIB_LOGE("  reserve:%u %u", cnt, cnt * (3 + hasWhite()) * 8);

        // Get the items from the root LED
        p = &_unit;
        while (p && p->parent() && is_unit_led(p->parent())) {
            p = p->parent();
        }
        while (p && p != &_unit) {
            auto i = static_cast<UnitLED*>(p)->rgb888_items();
            v.insert(v.end(), i.begin(), i.end());
            p = p->child(0);
        }
        auto i = rgb888_items();
        v.insert(v.end(), i.begin(), i.end());
        return v;
    }

    virtual bool write(const uint8_t* data, const uint32_t size)
    {
        return _unit.writeWithTransaction(data, size, true) == m5::hal::error::error_t::OK;
    }
};
///@endcond

// class UnitLED
const char UnitLED::name[] = "UnitLED";
const types::uid_t UnitLED::uid{"UnitLED"_mmh3};
const types::attr_t UnitLED::attr{attribute::AccessGPIO};

bool UnitLED::begin()
{
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
    } else {
        _interface.reset(new InterfacePbHub(*this));
    }

    _brightness = _cfg.brightness;

    return true;
}

bool UnitLED::writePixels(void)
{
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
    if (n < _numPixels) {
        Color c = color(n);
        c.r     = r;
        c.g     = g;
        c.b     = b;
        setPixelColor(n, c);
        return true;
    }
    return false;
}

bool UnitLED::fill(const Color& c, const uint16_t first, const uint16_t count)
{
    if (first + count > _numPixels) {
        return false;
    }
    auto last = count ? (first + count) : (first + _numPixels - first);
    for (uint16_t n = first; n < last; ++n) {
        setPixelColor(n, c);
    }
    return true;
}

bool UnitLED::fillRainbow(const uint16_t first, const uint16_t count, const uint8_t firstHue)
{
    auto num = count ? count : (_numPixels - first);
    if (first + num > _numPixels || !num) {
        return false;
    }

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
    auto num = count ? count : (_numPixels - first);
    if (first + num > _numPixels || num < 2) {
        return false;
    }

    for (uint16_t i = 0; i < num - 1; ++i) {
        float ratio = static_cast<float>(i) / (num - 1);
        uint8_t r   = static_cast<uint8_t>(firstClr.r + ratio * (lastClr.r - firstClr.r));
        uint8_t g   = static_cast<uint8_t>(firstClr.g + ratio * (lastClr.g - firstClr.g));
        uint8_t b   = static_cast<uint8_t>(firstClr.b + ratio * (lastClr.b - firstClr.b));
        uint8_t w   = static_cast<uint8_t>(firstClr.w + ratio * (lastClr.w - firstClr.w));
        setPixelColor(first + i, Color(r, g, b, w));
    }
    setPixelColor(first + num - 1, lastClr);
    return true;
}

bool UnitLED::fillRandom(const uint16_t first, const uint16_t count)
{
    auto num = count ? count : (_numPixels - first);
    if (first + num > _numPixels || !num) {
        return false;
    }

    for (uint16_t i = first; i < first + num; ++i) {
        setPixelColor(i, esp_random(), esp_random(), esp_random());
    }
    return true;
}

void UnitLED::setRotation(const int8_t rot)
{
    auto prev    = _rot;
    auto new_rot = rotation_of(rot);
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
    if (n < _numPixels) {
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
    if (n < _numPixels) {
        //        M5_LIB_LOGV("[%u]:%08X", n, c.wrgb);
        uint8_t* p  = _pixels.data() + n * (3 + hasWhite());
        p[_offsetR] = c.r;
        p[_offsetG] = c.g;
        p[_offsetB] = c.b;
        if (_offsetW >= 0) {
            p[_offsetW] = c.w;
        }
        return true;
    }
    return false;
}

UnitLED::item_container_type UnitLED::items()
{
    return _interface->items();
}

UnitLED::container_type UnitLED::rgb888_items()
{
    return _interface->rgb888_items();
}

void UnitLED::shift_left(const uint16_t count, const bool wrap)
{
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

void UnitLED::shift_right(uint16_t count, bool wrap)
{
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
