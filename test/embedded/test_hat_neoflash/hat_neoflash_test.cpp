/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for HatNeoFlash (SKU U071, 18x7 = 126 x WS2812)
*/
#include "../led_test_common.hpp"

// HatNeoFlash uses Hat header rx pin (not GROVE).
class TestHatNeoFlash : public GPIOComponentTestBase<HatNeoFlash> {
protected:
    HatNeoFlash* get_instance() override
    {
        return new HatNeoFlash();
    }
    bool begin() override
    {
        return this->Units.add(*this->unit, -1, m5::unit::wiring::hatGPIOPins().rx) && this->Units.begin();
    }
};

// BeginAppliesConfig fixture: pre-set config.brightness=42 before begin().
class TestHatNeoFlash_Config : public TestHatNeoFlash {
protected:
    HatNeoFlash* get_instance() override
    {
        auto* ptr      = new HatNeoFlash();
        auto cfg       = ptr->config();
        cfg.brightness = 42;
        ptr->config(cfg);
        return ptr;
    }
};

namespace testing {
namespace internal {
template <>
std::string GetTypeName<HatNeoFlash>()
{
    return "HatNeoFlash";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestHatNeoFlash, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::HAT_NEOFLASH);
    fill_test(unit.get());
    shift_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestHatNeoFlash, RectRotation)
{
    SCOPED_TRACE(ustr);
    rect_rotation_test(unit.get(), 18, 7);
}

TEST_F(TestHatNeoFlash_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
