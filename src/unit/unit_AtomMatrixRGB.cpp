/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_AtomMatrixRGB.cpp
  @brief Atom Matrix RGB for M5UnitUnified
*/
#include "unit_AtomMatrixRGB.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace {
constexpr int8_t ATOM_MATRIX_LED_PIN{27};
}

namespace m5 {
namespace unit {

// class UnitAtomMatrixRGB
const char UnitAtomMatrixRGB::name[] = "UnitAtomMatrixRGB";
const types::uid_t UnitAtomMatrixRGB::uid{"UnitAtomMatrixRGB"_mmh3};
const types::attr_t UnitAtomMatrixRGB::attr{attribute::AccessGPIO};

bool UnitAtomMatrixRGB::begin()
{
    if (adapter()->type() != Adapter::Type::GPIO) {
        M5_LIB_LOGE("This unit is not available via PbHub");
        return false;
    }
    auto ad = asAdapter<AdapterGPIO>(Adapter::Type::GPIO);
    if (!ad || ad->tx_pin() != ATOM_MATRIX_LED_PIN) {
        M5_LIB_LOGE("Illegal adapter %u", ad ? ad->tx_pin() : -1);
        return false;
    }
    return UnitLED::begin();
}

}  // namespace unit
}  // namespace m5
