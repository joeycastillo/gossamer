/** 
 * @file tc.h
 * @brief Timer/Counter Peripheral
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
#include "sam.h"
#include "system.h"

typedef enum {
    TC_PRESCALER_DIV1 = 0,
    TC_PRESCALER_DIV2 = 1,
    TC_PRESCALER_DIV4 = 2,
    TC_PRESCALER_DIV8 = 3,
    TC_PRESCALER_DIV16 = 4,
    TC_PRESCALER_DIV64 = 5,
    TC_PRESCALER_DIV256 = 6,
    TC_PRESCALER_DIV1024 = 7
} tc_prescaler_value_t;

/** @brief Waits for the TC to synchronize.
  * @details 
  * @param instance The TC peripheral instance as numbered in the data sheet.
  */
void tc_sync(uint8_t instance);

/** @brief Enables the peripheral clock for the TC and clocks it with the selected
 *         clock source. Also resets the TC to its starting configuration.
  * @details This just sets up the TC in its reset state. You are still responsible
  *          for configuring it with the options you need for things like clock
  *          prescaling and waveform output.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  * @param clocksource The generic clock source to use as the TC clock.
  * @param prescaler The prescaler factor, which divides the clock source by a factor:
  *                  * TC_PRESCALER_DIV1 - clock source unchanged
  *                  * TC_PRESCALER_DIV2 - clock source divided by 2
  *                  * TC_PRESCALER_DIV4 - clock source divided by 4
  *                  * TC_PRESCALER_DIV8 - clock source divided by 8
  *                  * TC_PRESCALER_DIV16 - clock source divided by 16
  *                  * TC_PRESCALER_DIV64 - clock source divided by 64
  *                  * TC_PRESCALER_DIV256 - clock source divided by 256
  *                  * TC_PRESCALER_DIV102 - clock source divided by 1024
  * @return true if the TC was set up, false if instance was out of range (i.e. you
  *         asked for TC3, but the microcontroller only has two TCs).
  * @note if this function returns false, you shouldn't interact with this TC instance
  *       with any other functions; they don't do the bounds check that this does.
  */
bool tc_setup(uint8_t instance, generic_clock_generator_t clocksource, tc_prescaler_value_t prescaler);

/** @brief Enables the TC. Make sure to call tcc_setup first to set it up.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_enable(uint8_t instance);

/** @brief Disables the TC, but retains all its settings.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_disable(uint8_t instance);
