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
  * @return true if the TC was set up, false if instance was out of range (i.e. you
  *         asked for TCC2, but the microcontroller only has one TC).
  * @note if this function returns false, you shouldn't interact with this TC instance
  *       with any other functions; they don't do the bounds check that this does.
  */
bool tc_setup(uint8_t instance, uint8_t clocksource);

/** @brief Enables the TC. Make sure to call tcc_setup first to set it up.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_enable(uint8_t instance);

/** @brief Disables the TC, but retains all its settings.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_disable(uint8_t instance);
