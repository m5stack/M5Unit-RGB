/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitPuzzle (SKU U193, 8x8 = 64 x WS2812)
*/
#include "../led_test_common.hpp"

using TestUnitPuzzle        = TestSKU<UnitPuzzle>;
using TestUnitPuzzle_Config = TestSKU_ConfigApplied<UnitPuzzle>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitPuzzle>()
{
    return "UnitPuzzle";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitPuzzle, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_PUZZLE);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitPuzzle, RectRotation)
{
    SCOPED_TRACE(ustr);
    rect_rotation_test(unit.get(), 8, 8);
}

TEST_F(TestUnitPuzzle_Config, BeginAppliesConfig)
{
    SCOPED_TRACE(ustr);
    EXPECT_EQ(unit->brightness(), 42);
}
