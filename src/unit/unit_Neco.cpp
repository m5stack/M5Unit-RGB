/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Neco.cpp
  @brief Neco LED unit for M5UnitUnified
*/
#include "unit_Neco.hpp"

using namespace m5::utility::mmh3;
using namespace m5::unit::types;

namespace m5 {
namespace unit {

// class UnitNeco
const char UnitNeco::name[] = "UnitNeco";
const types::uid_t UnitNeco::uid{"UnitNeco"_mmh3};
const types::attr_t UnitNeco::attr{attribute::AccessGPIO};

bool UnitNeco::begin()
{
    if (!UnitLED::begin()) {
        return false;
    }
    return pinModeRX(gpio::Mode::InputPullup);  // For Button
}

void UnitNeco::update(const bool)
{
    /*
      Connection via PbHub does not get button status accurately
     */
    if (adapter()->type() == Adapter::Type::GPIO) {
        bool rx{};
        if (readDigitalRX(rx)) {
            setRawState(m5::utility::millis(), !rx);
        }
    }
}

}  // namespace unit
}  // namespace m5
