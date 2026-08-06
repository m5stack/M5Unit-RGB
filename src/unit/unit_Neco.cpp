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

    if (adapter()->type() == Adapter::Type::GPIO) {
        auto* ad = asAdapter<AdapterGPIOBase>(Adapter::Type::GPIO);
        if (ad) {
            const auto rx = ad->rx_pin();
            // Button needs an assigned GPIO with internal pull-up.
            // GPIO_IS_VALID_OUTPUT_GPIO happens to exclude ESP32 classic input-only
            // pins (GPIO 34-39) which also lack internal pull-up. On ESP32-S3/C6/H2
            // all valid GPIOs pass, so the filter is effectively "usable for pullup".
            _btn_available = (rx >= 0) && GPIO_IS_VALID_OUTPUT_GPIO(rx);
            if (_btn_available) {
                pinModeRX(gpio::Mode::InputPullup);
            } else {
                M5_LIB_LOGW("RX GPIO %d is not usable for button; button disabled", static_cast<int>(rx));
            }
        }
    }
    return true;
}

void UnitNeco::update(const bool)
{
    if (!_btn_available) {
        return;
    }
    bool rx{};
    if (readDigitalRX(rx)) {
        setRawState(m5::utility::millis(), !rx);
    }
}

}  // namespace unit
}  // namespace m5
