/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_RGB_LED.cpp
  @brief RGB LED, RGB LED Strip  unit for M5UnitUnified
*/
#include "unit_RGB_LED.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace m5 {
namespace unit {

// class UnitRGBLED15
const char UnitRGBLED15::name[] = "UnitRGBLED15";
const types::uid_t UnitRGBLED15::uid{"UnitRGBLED15"_mmh3};
const types::attr_t UnitRGBLED15::attr{attribute::AccessGPIO};

// class UnitRGBLED29
const char UnitRGBLED29::name[] = "UnitRGBLED29";
const types::uid_t UnitRGBLED29::uid{"UnitRGBLED29"_mmh3};
const types::attr_t UnitRGBLED29::attr{attribute::AccessGPIO};

// class UnitRGBLED72
const char UnitRGBLED72::name[] = "UnitRGBLED72";
const types::uid_t UnitRGBLED72::uid{"UnitRGBLED72"_mmh3};
const types::attr_t UnitRGBLED72::attr{attribute::AccessGPIO};

// class UnitRGBLED144
const char UnitRGBLED144::name[] = "UnitRGBLED144";
const types::uid_t UnitRGBLED144::uid{"UnitRGBLED144"_mmh3};
const types::attr_t UnitRGBLED144::attr{attribute::AccessGPIO};

// class UnitRGBLED288
const char UnitRGBLED288::name[] = "UnitRGBLED288";
const types::uid_t UnitRGBLED288::uid{"UnitRGBLED288"_mmh3};
const types::attr_t UnitRGBLED288::attr{attribute::AccessGPIO};

// class UnitRGBLEDStrip30
const char UnitRGBLEDStrip30::name[] = "UnitRGBLEDStrip30";
const types::uid_t UnitRGBLEDStrip30::uid{"UnitRGBLEDStrip30"_mmh3};
const types::attr_t UnitRGBLEDStrip30::attr{attribute::AccessGPIO};

// class UnitRGBLEDStrip60
const char UnitRGBLEDStrip60::name[] = "UnitRGBLEDStrip60";
const types::uid_t UnitRGBLEDStrip60::uid{"UnitRGBLEDStrip60"_mmh3};
const types::attr_t UnitRGBLEDStrip60::attr{attribute::AccessGPIO};

// class UnitRGBLEDStrip120
const char UnitRGBLEDStrip120::name[] = "UnitRGBLEDStrip120";
const types::uid_t UnitRGBLEDStrip120::uid{"UnitRGBLEDStrip120"_mmh3};
const types::attr_t UnitRGBLEDStrip120::attr{attribute::AccessGPIO};

// class UnitRGBLEDStrip300
const char UnitRGBLEDStrip300::name[] = "UnitRGBLEDStrip300";
const types::uid_t UnitRGBLEDStrip300::uid{"UnitRGBLEDStrip300"_mmh3};
const types::attr_t UnitRGBLEDStrip300::attr{attribute::AccessGPIO};

}  // namespace unit
}  // namespace m5
