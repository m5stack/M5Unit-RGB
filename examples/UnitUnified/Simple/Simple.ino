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
// *************************************************************
// Choose one define symbol to match the unit you are using
// *************************************************************
#if !defined(USING_UNIT_RGB) && !defined(USING_UNIT_PUZZLE) && !defined(USING_UNIT_HEX) &&        \
    !defined(USING_UNIT_NEO_HEX) && !defined(USING_UNIT_NECO) && !defined(USING_UNIT_RGBLED15) && \
    !defined(USING_UNIT_RGBLED_STRIP30) && !defined(USING_ATOM_MATRIX_RGB) && !defined(USING_HAT_NEOFLASH)
// For Unit RGB (U003)
// #define USING_UNIT_RGB
// For Unit Puzzle (U193)
// #define USING_UNIT_PUZZLE
// For Unit Hex (A045)
// #define USING_UNIT_HEX
// For Unit NeoHex (A045-B)
// #define USING_UNIT_NEO_HEX
// For Unit Neco (U163)
// #define USING_UNIT_NECO
// For Unit RGB LED (A035)
// #define USING_UNIT_RGBLED15
// For Unit RGB LED Strip (A093)
// #define USING_UNIT_RGBLED_STRIP30
// For ATOM Matrix built-in 5x5 RGB LED (C008-B)
// #define USING_ATOM_MATRIX_RGB
// For Hat NeoFlash (U071)
// #define USING_HAT_NEOFLASH
#endif
// *************************************************************
#include "main/Simple.cpp"
