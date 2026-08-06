/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitRGBLEDStrip30 (SKU A093, 30 x SK6812GRB strip)
*/
#include "../led_test_common.hpp"

using TestUnitRGBLEDStrip30 = TestSKU<UnitRGBLEDStrip30>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitRGBLEDStrip30>()
{
    return "UnitRGBLEDStrip30";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitRGBLEDStrip30, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_RGBLEDSTRIP30);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}
