/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_Hex.cpp
  @brief Hex,NeoHex unit for M5UnitUnified
*/
#include "unit_Hex.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;

namespace {
/*
    00,01,02,03
   04,05,06,07,08
  09,10,11,12,13,14
15,16,17,18,19,20,21
 22,23,24,25,26,27
   28,29,30,31,32
    33,34,35,36
*/
constexpr uint16_t index_table[6][37] = {
    // 0 deg
    {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16, 17, 18,
     19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36},
    // 60 deg
    {3,  8,  14, 21, 2,  7,  13, 20, 27, 1, 6,  12, 19, 26, 32, 0,  5,  11, 18,
     25, 31, 36, 4,  10, 17, 24, 30, 35, 9, 16, 23, 29, 34, 15, 22, 28, 33},
    // 120 deg
    {21, 27, 32, 36, 14, 20, 26, 31, 35, 8, 13, 19, 25, 30, 34, 3, 7, 12, 18,
     24, 29, 33, 2,  6,  11, 17, 23, 28, 1, 5,  10, 16, 22, 0,  4, 9, 15},
    // 180 deg
    {36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18,
     17, 16, 15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0},
    // 240 deg
    {33, 28, 22, 15, 34, 29, 23, 16, 9, 35, 30, 24, 17, 10, 4,  36, 31, 25, 18,
     11, 5,  0,  32, 26, 19, 12, 6,  1, 27, 20, 13, 7,  2,  21, 14, 8,  3},
    // 300 deg
    {15, 9, 4, 0,  22, 16, 10, 5,  1, 28, 23, 17, 11, 6,  2,  33, 29, 24, 18,
     12, 7, 3, 34, 30, 25, 19, 13, 8, 35, 31, 26, 20, 14, 36, 32, 27, 21}};

}  // namespace

namespace m5 {
namespace unit {

// explicit instantiation
template class UnitHexLED37<UnitWS2812>;
template class UnitHexLED37<UnitSK6812GRB>;

// class UnitHexLED37
template <class BaseClass>
uint32_t UnitHexLED37<BaseClass>::index_of(const uint32_t n) const
{
    return index_table[this->rotation()][n % this->numPixels()];
}

template <class BaseClass>
const char UnitHexLED37<BaseClass>::name[] = "UnitHexLED37";
template <class BaseClass>
const types::uid_t UnitHexLED37<BaseClass>::uid{"UnitHexLED37"_mmh3};
template <class BaseClass>
const types::attr_t UnitHexLED37<BaseClass>::attr{types::attribute::AccessGPIO};

// class UnitHex
const char UnitHex::name[] = "UnitHex";
const types::uid_t UnitHex::uid{"UnitHex"_mmh3};
const types::attr_t UnitHex::attr{attribute::AccessGPIO};

// class UnitNeoHex
const char UnitNeoHex::name[] = "UnitNeoHex";
const types::uid_t UnitNeoHex::uid{"UnitNeoHex"_mmh3};
const types::attr_t UnitNeoHex::attr{attribute::AccessGPIO};

}  // namespace unit
}  // namespace m5
