/*
 * SPDX-FileCopyrightText: 2026 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  UnitTest for UnitNeoHex (SKU A045-B, 37 x SK6812GRB hexagonal)
*/
#include "../led_test_common.hpp"

using TestUnitNeoHex = TestSKU<UnitNeoHex>;

namespace testing {
namespace internal {
template <>
std::string GetTypeName<UnitNeoHex>()
{
    return "UnitNeoHex";
}
}  // namespace internal
}  // namespace testing

TEST_F(TestUnitNeoHex, All)
{
    SCOPED_TRACE(ustr);
    basic_test(unit.get(), expected::UNIT_NEOHEX);
    fill_test(unit.get());
    shift_test(unit.get());
    scale8_test();
    color_test();
    config_test(unit.get());
    range_test(unit.get());
    hw_write_test(unit.get());
}

TEST_F(TestUnitNeoHex, HexRotation)
{
    SCOPED_TRACE(ustr);
    hex_rotation_test(unit.get());
}
