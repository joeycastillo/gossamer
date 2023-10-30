/** 
 * @file dac.h
 * @brief Digital to Analog Converter
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

// channel mask enum for dac_enable
typedef enum {
    DAC_CHANNEL_NONE = 0,
    DAC_CHANNEL_0 = 1,
    DAC_CHANNEL_1 = 2,
    DAC_CHANNEL_BOTH = 3
} dac_channel_mask_t;

/**
 * @brief Initializes the DAC peripheral, but does not enable it.
 * @param channel The DAC channel to enable.
 * @warning Currently only supports the DAC on VOUT[0]
 */
void dac_init(void);

/**
 * @brief Enables the DAC channels specified in the mask, and disables others.
 * @param channelmask A bitmask of the channels to enable:
 *        1 for channel 0, 2 for channel 1, 3 for both channels.
 *        For MCUs with only one DAC channel, this parameter is ignored.
 */
void dac_enable(dac_channel_mask_t channelmask);

/**
 * @brief Set the analog value of the DAC.
 * @param channel The DAC channel to set.
 * @param value The value to set the DAC to. The range is platform-dependent:
 *              * On SAM L21, the valid range is from 0 to 4095.
 *              * On SAM D21 and D11, the valid range is from 0 to 1023.
 *              In both cases, 0 is 0V and the maximum value is VDDANA.
 */
void dac_set_analog_value(uint16_t channel, uint16_t value);

/**
 * @brief Disables the given DAC channel.
 * @warning Currently only supports the DAC on VOUT[0]
 */
void dac_disable(uint16_t channel);
