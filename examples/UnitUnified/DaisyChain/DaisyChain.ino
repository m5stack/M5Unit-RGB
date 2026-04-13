/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Daisy chain example

  **** NOTICE *********************************************************************
  This example assumes the following connections
  Core -> UnitRGB -> UnitPuzzle -> UnitPuzzle -> UnitHex -> UnitRGBLed15 -> UnitNeco

  Modify the source code to suit your own connection situation.

  NOTE: Depending on the core device, the power supply from the port may not be
  sufficient for all LEDs.
  If the lights turn on unexpectedly, try lowering the brightness or reducing the
  number of lights turned on at once to see if that resolves the issue.
  *********************************************************************************
*/
#include "main/DaisyChain.cpp"
