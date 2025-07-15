/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_WS2812.cpp
  @brief WS2812 unit for M5UnitUnified
*/
#include "unit_WS2812.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace m5 {
namespace unit {

// class UnitWS2812
const char UnitWS2812::name[] = "UnitWS2812";
const types::uid_t UnitWS2812::uid{"UnitWS2812"_mmh3};
const types::attr_t UnitWS2812::attr{attribute::AccessGPIO};

adapter_config_t UnitWS2812::rmt_config()
{
    adapter_config_t cfg{};
    cfg.mode                   = Mode::RmtTX;
    cfg.tx.tick_ns             = 50;
    cfg.tx.mem_blocks          = 4;
    cfg.tx.idle_output_enabled = true;
    cfg.tx.idle_level_high     = true;
    return cfg;
}

}  // namespace unit
}  // namespace m5
