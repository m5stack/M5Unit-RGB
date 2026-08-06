/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitRGB (SKU U003, 3 x SK6812GRB)
*/
#include "../led_test_common.hpp"

using TestUnitRGB        = TestSKU<UnitRGB>;
using TestUnitRGB_Config = TestSKU_ConfigApplied<UnitRGB>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitRGB>()
{
    return "UnitRGB";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitRGB, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_RGB);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitRGB_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
