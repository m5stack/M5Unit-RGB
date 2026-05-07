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
    pinModeRX(gpio::Mode::InputPullup);  // For Button

    if (adapter()->type() == Adapter::Type::GPIO) {
        auto* ad = asAdapter<AdapterGPIOBase>(Adapter::Type::GPIO);
        if (ad) {
            // Input-only GPIOs (e.g. ESP32 GPIO 34-39) lack internal
            // pull-up circuitry.  Without pull-up the button line
            // floats, so reads return random noise.
            // GPIO_IS_VALID_OUTPUT_GPIO is false for these pins.
            _btn_available = GPIO_IS_VALID_OUTPUT_GPIO(ad->rx_pin());
            if (!_btn_available) {
                M5_LIB_LOGW(
                    "RX GPIO %d is input-only (no pull-up); button disabled. Use a port with output-capable GPIO",
                    static_cast<int>(ad->rx_pin()));
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
