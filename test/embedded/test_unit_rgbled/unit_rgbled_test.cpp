/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitRGBLED15 (SKU A035, 15 x SK6812GRB strip)
*/
#include "../led_test_common.hpp"

using TestUnitRGBLED15        = TestSKU<UnitRGBLED15>;
using TestUnitRGBLED15_Config = TestSKU_ConfigApplied<UnitRGBLED15>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitRGBLED15>()
{
    return "UnitRGBLED15";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitRGBLED15, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_RGBLED15);
    fill_test(unit.get());
    shift_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitRGBLED15_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
