/** 
 * @file opamp.h
 * @brief Operational Amplifier Peripheral
 */
/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef OPAMP

#define OPAMP_MUXNEG_NEG (0)
#define OPAMP_MUXNEG_LADDER (1)
#define OPAMP_MUXNEG_OUT (2)
#define OPAMP0_MUXNEG_DAC (3)
#define OPAMP1_MUXNEG_DAC (3)
#define OPAMP2_MUXNEG_DAC (5)
#define OPAMP2_MUXNEG_NEG_0 (3)
#define OPAMP2_MUXNEG_NEG_1 (4)

#define OPAMP_MUXPOS_POS (0)
#define OPAMP_MUXPOS_LADDER (1)
#define OPAMP0_MUXPOS_DAC (2)
#define OPAMP1_MUXPOS_OUT_0 (2)
#define OPAMP2_MUXPOS_OUT_1 (2)
#define OPAMP_MUXPOS_GND (3)
#define OPAMP2_MUXPOS_POS_0 (4)
#define OPAMP2_MUXPOS_POS_1 (5)
#define OPAMP2_MUXPOS_LADDER_0 (6)

#define OPAMP_POTMUX_RATIO_14_2 (0)
#define OPAMP_POTMUX_RATIO_12_4 (1)
#define OPAMP_POTMUX_RATIO_8_8  (2)
#define OPAMP_POTMUX_RATIO_6_10 (3)
#define OPAMP_POTMUX_RATIO_4_12 (4)
#define OPAMP_POTMUX_RATIO_3_13 (5)
#define OPAMP_POTMUX_RATIO_2_14 (6)
#define OPAMP_POTMUX_RATIO_1_15 (7)

#define OPAMP_RES1MUX_POS (0)
#define OPAMP_RES1MUX_NEG (1)
#define OPAMP0_RES1MUX_DAC (2)
#define OPAMP1_RES1MUX_OUT_0 (2)
#define OPAMP2_RES1MUX_OUT_1 (2)
#define OPAMP_RES1MUX_GND (3)
#define OPAMP_RES1MUX_NC (4)

#define OPAMP_RES2MUX_VCC (0)
#define OPAMP_RES2MUX_OUT (1)
#define OPAMP_RES2MUX_NC (2)

/**
 * @brief Initializes the OPAMP peripheral, but does not enable any opamps.
 */
void opamp_init(void);

/**
 * @brief Enables the given opamp.
 * @param instance The opamp instance to enable.
 */
void opamp_enable(uint16_t instance);

/**
 * @brief Sets the positive input mux for the given opamp.
 * @param instance The opamp instance to configure.
 * @param muxpos The desired connection, one of:
 *               * OPAMP_MUXPOS_POS - the positive input pin for this instance
 *               * OPAMP_MUXPOS_GND - ground
 *               * OPAMP_MUXPOS_LADDER - the resistor ladder tap for this instance
 *               * OPAMP0_MUXPOS_DAC - the DAC output (only valid for OPAMP0)
 *               * OPAMP1_MUXPOS_OUT_0 - OPAMP0's output (only valid for OPAMP1)
 *               * OPAMP2_MUXPOS_OUT_1 - OPAMP1's output (only valid for OPAMP2)
 *               * OPAMP2_MUXPOS_POS_0 - OPAMP0's positive input pin (only valid for OPAMP2)
 *               * OPAMP2_MUXPOS_POS_1 - OPAMP1's positive input pin (only valid for OPAMP2)
 *               * OPAMP2_MUXPOS_LADDER_0 - OPAMP0's resistor ladder tap (only valid for OPAMP2)
 */
void opamp_set_muxpos(uint16_t instance, uint8_t muxpos);

/**
 * @brief Sets the negative input mux for the given opamp.
 * @param instance The opamp instance to configure.
 * @param muxneg The desired connection, one of:
 *              * OPAMP_MUXNEG_NEG - the negative input pin for this instance
 *              * OPAMP_MUXNEG_LADDER - the resistor ladder tap for this instance
 *              * OPAMP_MUXNEG_OUT - the output of this instance
 *              * OPAMP0_MUXNEG_DAC - the DAC output (only valid for OPAMP0)
 *              * OPAMP1_MUXNEG_DAC - the DAC output (only valid for OPAMP1)
 *              * OPAMP2_MUXNEG_DAC - the DAC output (only valid for OPAMP2)
 *              * OPAMP2_MUXNEG_NEG_0 - OPAMP0's negative input pin (only valid for OPAMP2)
 *              * OPAMP2_MUXNEG_NEG_1 - OPAMP1's negative input pin (only valid for OPAMP2)
 */
void opamp_set_muxneg(uint16_t instance, uint8_t muxneg);

/**
 * @brief Sets the potentiometer (resistor ladder) mux for the given opamp.
 * @param instance The opamp instance to configure.
 * @param potmux The desired resistor ladder ratio, one of:
 *              * OPAMP_POTMUX_RATIO_14_2 - 14:2 ratio
 *              * OPAMP_POTMUX_RATIO_12_4 - 12:4 ratio
 *              * OPAMP_POTMUX_RATIO_8_8 - 8:8 ratio
 *              * OPAMP_POTMUX_RATIO_6_10 - 6:10 ratio
 *              * OPAMP_POTMUX_RATIO_4_12 - 4:12 ratio
 *              * OPAMP_POTMUX_RATIO_3_13 - 3:13 ratio
 *              * OPAMP_POTMUX_RATIO_2_14 - 2:14 ratio
 *              * OPAMP_POTMUX_RATIO_1_15 - 1:15 ratio
 */
void opamp_set_potmux(uint16_t instance, uint8_t potmux);

/**
 * @brief Sets the connection for the bottom of the resistor ladder for the given instance.
 * @param instance The opamp instance to configure.
 * @param res1mux The desired connection, one of:
 *              * OPAMP_RES1MUX_POS - the positive input pin for this instance
 *              * OPAMP_RES1MUX_NEG - the negative input pin for this instance
 *              * OPAMP_RES1MUX_GND - ground
 *              * OPAMP0_RES1MUX_DAC - the DAC output (only valid for OPAMP0)
 *              * OPAMP1_RES1MUX_OUT_0 - OPAMP0's output (only valid for OPAMP1)
 *              * OPAMP2_RES1MUX_OUT_1 - OPAMP1's output (only valid for OPAMP2)
 *              * OPAMP_RES1MUX_NC - not connected
 */
void opamp_set_res1mux(uint16_t instance, uint8_t res1mux);

/**
 * @brief Sets the connection for the top of the resistor ladder for the given instance.
 * @param instance The opamp instance to configure.
 * @param res2mux The desired connection, one of:
 *                * OPAMP_RES2MUX_VCC - VDDANA
 *                * OPAMP_RES2MUX_OUT - the output of this instance
 *                * OPAMP_RES2MUX_NC - not connected
 */
void opamp_set_res2mux(uint16_t instance, uint8_t res2mux);

/**
 * @brief Disables the given opamp.
 * @param instance The opamp instance to disable.
 * @note You should disable the opamp before reconfiguring it, and re-enable it afterwards.
 */
void opamp_disable(uint16_t instance);

#endif