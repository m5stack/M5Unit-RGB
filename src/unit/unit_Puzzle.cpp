/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Puzzle.cpp
  @brief Puzzle unit for M5UnitUnified
*/
#include "unit_Puzzle.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace m5 {
namespace unit {

// class UnitPuzzle
const char UnitPuzzle::name[] = "UnitPuzzle";
const types::uid_t UnitPuzzle::uid{"UnitPuzzle"_mmh3};
const types::attr_t UnitPuzzle::attr{attribute::AccessGPIO};

}  // namespace unit
}  // namespace m5
