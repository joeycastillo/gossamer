////< @file i2s.h
/*
 * MIT License
 *
 * Copyright (c) 2023 Joey Castillo
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
#include "sam.h"

/**
 * @addtogroup i2s Inter-IC Sound interface
 * @brief Functions for configuring and using the I2S peripheral (SAM D21 only).
 * @{
 */ 

#ifdef I2S

typedef enum {
    I2S_MODE_NORMAL,
    I2S_MODE_RIGHT_JUSTIFIED,
    I2S_MODE_LEFT_JUSTIFIED,
} i2s_mode_t;

/**
 * @brief Initializes the I2S peripheral.
 * @param instance The I2S peripheral instance to initialize.
 * @param sampleRate The sample rate to use.
 * @param bitsPerSample The number of bits per sample.
 * @param mode The I2S mode to use. @see i2s_mode_t
 * @note This function only initializes the I2S peripheral. You must configure the pins yourself.
 */
void i2s_init(uint8_t instance, uint16_t sampleRate, uint8_t bitsPerSample, i2s_mode_t mode);

void i2s_enable_receiver(uint8_t instance);

#endif

/** @} */