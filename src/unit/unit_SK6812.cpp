/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_SK6812.cpp
  @brief SK6812 unit for M5UnitUnified
*/
#include "unit_SK6812.hpp"
#include <M5Utility.hpp>

using namespace m5::utility::mmh3;
using namespace m5::unit::types;
using namespace m5::unit::gpio;

namespace m5 {
namespace unit {

// class UnitSK6812
const char UnitSK6812::name[] = "UnitSK6812";
const types::uid_t UnitSK6812::uid{"UnitSK6812"_mmh3};
const types::attr_t UnitSK6812::attr{attribute::AccessGPIO};

adapter_config_t UnitSK6812::rmt_config()
{
    adapter_config_t cfg{};
    cfg.mode                   = Mode::RmtTX;
    cfg.tx.tick_ns             = 50;
    cfg.tx.mem_blocks          = 4;
    cfg.tx.idle_output_enabled = true;
    return cfg;
}

// class UnitSK6812GRB
const char UnitSK6812GRB::name[] = "UnitSK6812GRB";
const types::uid_t UnitSK6812GRB::uid{"UnitSK6812GRB"_mmh3};
const types::attr_t UnitSK6812GRB::attr{attribute::AccessGPIO};

// class UnitSK6812GRBW
const char UnitSK6812GRBW::name[] = "UnitSK6812GRBW";
const types::uid_t UnitSK6812GRBW::uid{"UnitSK6812GRBW"_mmh3};
const types::attr_t UnitSK6812GRBW::attr{attribute::AccessGPIO};

}  // namespace unit
}  // namespace m5
