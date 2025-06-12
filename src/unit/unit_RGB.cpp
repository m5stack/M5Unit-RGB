/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_RGB.cpp
  @brief RGB unit for M5UnitUnified
*/
#include "unit_RGB.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace m5 {
namespace unit {

// class UnitRGB
const char UnitRGB::name[] = "UnitRGB";
const types::uid_t UnitRGB::uid{"UnitRGB"_mmh3};
const types::attr_t UnitRGB::attr{attribute::AccessGPIO};

}  // namespace unit
}  // namespace m5
