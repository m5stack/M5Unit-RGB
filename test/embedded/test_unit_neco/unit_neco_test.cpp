/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitNeco (SKU U163, 35 x WS2812 + touch button)
*/
#include "../led_test_common.hpp"

using TestUnitNeco = TestSKU<UnitNeco>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitNeco>()
{
    return "UnitNeco";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitNeco, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_NECO);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitNeco, Button)
{
    SCOPED_TRACE(ustr);
    neco_button_test(unit.get());
}
