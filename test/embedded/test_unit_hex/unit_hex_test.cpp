/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitHex (SKU A045, 37 x WS2812 hexagonal)
*/
#include "../led_test_common.hpp"

using TestUnitHex = TestSKU<UnitHex>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitHex>()
{
    return "UnitHex";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitHex, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_HEX);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitHex, HexRotation)
{
    SCOPED_TRACE(ustr);
    hex_rotation_test(unit.get());
}
