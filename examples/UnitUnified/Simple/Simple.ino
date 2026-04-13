/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Simple example

  NOTE: Depending on the core device, the power supply from the port may not be
  sufficient for all LEDs.
  If the lights turn on unexpectedly, try lowering the brightness or reducing the
  number of lights turned on at once to see if that resolves the issue.
*/
// *********************************************************************
// Choose using unit
// *********************************************************************
#if !defined(USING_UNIT_RGB) && !defined(USING_UNIT_PUZZLE) && !defined(USING_UNIT_HEX) &&        \
    !defined(USING_UNIT_NEO_HEX) && !defined(USING_UNIT_NECO) && !defined(USING_UNIT_RGBLED15) && \
    !defined(USING_UNIT_RGBLED_STRIP30) && !defined(USING_ATOM_MATRIX_RGB) && !defined(USING_HAT_NEOFLASH)
// #define USING_UNIT_RGB
// #define USING_UNIT_PUZZLE
// #define USING_UNIT_HEX
// #define USING_UNIT_NEO_HEX
// #define USING_UNIT_NECO
// #define USING_UNIT_RGBLED15
// #define USING_UNIT_RGBLED_STRIP30
// #define USING_ATOM_MATRIX_RGB
// #define USING_HAT_NEOFLASH
#endif
// *********************************************************************
#include "main/Simple.cpp"
