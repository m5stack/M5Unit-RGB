/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitAtomMatrixRGB (SKU C008-B, 5x5 = 25 x WS2812 built-in)
*/
#include "../led_test_common.hpp"

// AtomMatrixRGB uses fixed GPIO 27 (not routed through GROVE).
class TestUnitAtomMatrixRGB : public GPIOComponentTestBase<UnitAtomMatrixRGB> {
protected:
    UnitAtomMatrixRGB* get_instance() override
    {
        return new UnitAtomMatrixRGB();
    }
    bool begin() override
    {
        return this->Units.add(*this->unit, -1, 27) && this->Units.begin();
    }
};

// BeginAppliesConfig fixture: pre-set config.brightness=42 before begin().
class TestUnitAtomMatrixRGB_Config : public TestUnitAtomMatrixRGB {
protected:
    UnitAtomMatrixRGB* get_instance() override
    {
        auto* ptr      = new UnitAtomMatrixRGB();
        auto cfg       = ptr->config();
        cfg.brightness = 42;
        ptr->config(cfg);
        return ptr;
    }
};

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitAtomMatrixRGB>()
{
    return "UnitAtomMatrixRGB";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitAtomMatrixRGB, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_ATOMMATRIXRGB);
    fill_test(unit.get());
    shift_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitAtomMatrixRGB, RectRotation)
{
    SCOPED_TRACE(ustr);
    rect_rotation_test(unit.get(), 5, 5);
}

TEST_F(TestUnitAtomMatrixRGB_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
