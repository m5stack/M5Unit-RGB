/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file unit_RGB_LED.hpp
  @brief RGB LED, RGB LED Strip unit for M5UnitUnified
*/
#ifndef M5_UNIT_RGB_UNIT_RGB_LED_HPP
#define M5_UNIT_RGB_UNIT_RGB_LED_HPP

#include "unit_SK6812.hpp"

namespace m5 {
namespace unit {

/*!
  @class m5::unit::UnitRGBLED15
  @brief RGB LED 15 pixels Unit
  @details 10cm in length
*/
class UnitRGBLED15 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLED15, 0x00);

public:
    //! @brief Constructor (15 LEDs)
    UnitRGBLED15() : UnitSK6812GRB(15 /* 15 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLED29
  @brief RGB LED 29 pixels Unit
  @details 20cm in length
*/
class UnitRGBLED29 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLED29, 0x00);

public:
    //! @brief Constructor (29 LEDs)
    UnitRGBLED29() : UnitSK6812GRB(29 /* 29 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLED72
  @brief RGB LED 72 pixels Unit
  @details 50cm in length
*/
class UnitRGBLED72 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLED72, 0x00);

public:
    //! @brief Constructor (72 LEDs)
    UnitRGBLED72() : UnitSK6812GRB(72 /* 72 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLED144
  @brief RGB LED 144 pixels Unit
  @details 1m in length
*/
class UnitRGBLED144 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLED144, 0x00);

public:
    //! @brief Constructor (144 LEDs)
    UnitRGBLED144() : UnitSK6812GRB(144 /* 144 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLED288
  @brief RGB LED 288 pixels Unit
  @details 2m in length
*/
class UnitRGBLED288 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLED288, 0x00);

public:
    //! @brief Constructor (288 LEDs)
    UnitRGBLED288() : UnitSK6812GRB(288 /* 288 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLEDStrip30
  @brief RGB LED Strip 30 pixels Unit
  @details 50cm in length
*/
class UnitRGBLEDStrip30 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLEDStrip30, 0x00);

public:
    //! @brief Constructor (30 LEDs)
    UnitRGBLEDStrip30() : UnitSK6812GRB(30 /* 30 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLEDStrip60
  @brief RGB LED Strip 60 pixels Unit
  @details 1m in length
*/
class UnitRGBLEDStrip60 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLEDStrip60, 0x00);

public:
    //! @brief Constructor (60 LEDs)
    UnitRGBLEDStrip60() : UnitSK6812GRB(60 /* 60 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLEDStrip120
  @brief RGB LED Strip 120 pixels Unit
  @details 2m in length
*/
class UnitRGBLEDStrip120 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLEDStrip120, 0x00);

public:
    //! @brief Constructor (120 LEDs)
    UnitRGBLEDStrip120() : UnitSK6812GRB(120 /* 120 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

/*!
  @class m5::unit::UnitRGBLEDStrip300
  @brief RGB LED Strip 300 pixels Unit
  @details 5m in length
*/
class UnitRGBLEDStrip300 : public UnitSK6812GRB {
    M5_UNIT_COMPONENT_HPP_BUILDER(UnitRGBLEDStrip300, 0x00);

public:
    //! @brief Constructor (300 LEDs)
    UnitRGBLEDStrip300() : UnitSK6812GRB(300 /* 300 pixels */)
    {
        auto ccfg         = component_config();
        ccfg.max_children = 1;  // Can connect 1 Unit
        component_config(ccfg);
    }
};

}  // namespace unit
}  // namespace m5
#endif
