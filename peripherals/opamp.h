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

void opamp_init(void);

void opamp_enable(uint16_t channel);

void opamp_set_muxpos(uint16_t channel, uint8_t muxpos);

void opamp_set_muxneg(uint16_t channel, uint8_t muxneg);

void opamp_set_potmux(uint16_t channel, uint8_t potmux);

void opamp_set_res1mux(uint16_t channel, uint8_t res1mux);

void opamp_set_res2mux(uint16_t channel, uint8_t res2mux);

void opamp_disable(uint16_t channel);

#endif