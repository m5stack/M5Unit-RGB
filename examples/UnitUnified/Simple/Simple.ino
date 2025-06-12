/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*
  Simple example
*/
// *********************************************************************
// Choose using unit
// *********************************************************************
#if !defined(USING_UNIT_RGB) && !defined(USING_UNIT_PUZZLE) && !defined(USING_UNIT_HEX) &&        \
    !defined(USING_UNIT_NEO_HEX) && !defined(USING_UNIT_NECO) && !defined(USING_UNIT_RGBLED15) && \
    !defined(USING_UNIT_RGBLED_STRIP30) && !defined(USING_ATOM_MATRIX_RGB)
// #define USING_UNIT_RGB
// #define USING_UNIT_PUZZLE
// #define USING_UNIT_HEX
// #define USING_UNIT_NEO_HEX
// #define USING_UNIT_NECO
// #define USING_UNIT_RGBLED15
// #define USING_UNIT_RGBLED_STRIP30
// #define USING_ATOM_MATRIX_RGB
#endif
// *********************************************************************
#include "main/Simple.cpp"
