/** 
 * @file tcc.h
 * @brief Timer/Counter for Control Applications (TCC) Peripheral
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

typedef enum {
    TCC_PRESCALER_DIV1 = 0,
    TCC_PRESCALER_DIV2 = 1,
    TCC_PRESCALER_DIV4 = 2,
    TCC_PRESCALER_DIV8 = 3,
    TCC_PRESCALER_DIV16 = 4,
    TCC_PRESCALER_DIV64 = 5,
    TCC_PRESCALER_DIV256 = 6,
    TCC_PRESCALER_DIV1024 = 7
} tcc_prescaler_value_t;

typedef enum {
    TCC_WAVEGEN_NORMAL_FREQUENCY = 0,
    TCC_WAVEGEN_MATCH_FREQUENCY = 1,
    TCC_WAVEGEN_NORMAL_PWM = 2,
// These modes may be supported in the future, but I don't have a use for them right now.
    // TCC_WAVEGEN_DUAL_SLOPE_TOP = 7,
    // TCC_WAVEGEN_DUAL_SLOPE_BOTTOM = 5,
    // TCC_WAVEGEN_DUAL_SLOPE_BOTH = 6,
    // TCC_WAVEGEN_DUAL_SLOPE_CRITICAL = 4,
} tcc_wavegen_t;

typedef enum {
    TCC_OUTPUT_MATRIX_MODULO_4 = 0,
    TCC_OUTPUT_MATRIX_MODULO_2 = 1,
    TCC_OUTPUT_MATRIX_ALL_CC0 = 2,
    TCC_OUTPUT_MATRIX_WO0_CC0_OTHERS_CC1 = 3,
} tcc_output_matrix_t;

typedef enum {
    TCC_CHANNEL_POLARITY_NORMAL = 0,
    TCC_CHANNEL_POLARITY_INVERTED = 1,
} tcc_channel_polarity_t;

/** @brief Enables the peripheral clock for the TCC and clocks it with the selected
 *         clock source. Also resets the TCC to its starting configuration.
  * @details This just sets up the TCC in its reset state. You are still responsible
  *          for configuring it with the options you need for waveform output.
  * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
  * @param clocksource The clock source to use for the TCC.
  * @param prescaler The prescaler factor, which divides the clock source by a factor:
  *                  * TCC_PRESCALER_DIV1 - clock source unchanged
  *                  * TCC_PRESCALER_DIV2 - clock source divided by 2
  *                  * TCC_PRESCALER_DIV4 - clock source divided by 4
  *                  * TCC_PRESCALER_DIV8 - clock source divided by 8
  *                  * TCC_PRESCALER_DIV16 - clock source divided by 16
  *                  * TCC_PRESCALER_DIV64 - clock source divided by 64
  *                  * TCC_PRESCALER_DIV256 - clock source divided by 256
  *                  * TCC_PRESCALER_DIV102 - clock source divided by 1024
  * @return true if the TCC was set up, false if instance was out of range (i.e. you
  *         asked for TCC2, but the microcontroller only has one TCC).
  * @note if this function returns false, you shouldn't interact with this TCC instance
  *       with any other functions; they don't do the bounds check that this does.
  */
bool tcc_init(uint8_t instance, generic_clock_generator_t clocksource, tcc_prescaler_value_t prescaler);

/**
 * @brief Sets whether the TCC should run in standby mode.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param runStandby true if the TCC should run in standby mode, false otherwise.
 */
void tcc_set_run_in_standby(uint8_t instance, bool runStandby);

/**
 * @brief Sets the waveform generation mode for the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param mode The waveform generation mode to use. One of:
 *             * TCC_WAVEGEN_NORMAL_FREQUENCY - Normal frequency generation. The TOP value is
 *               PER, and waveform output WO[n] toggles on a match with a compare channel (n % 4
 *               by default; @see `tcc_set_output_matrix` for other options). This mode results
 *               in a square wave with a frequency dependent on the PER register, and a duty
 *               cycle that depends on the CCn register.
 *             * TCC_WAVEGEN_MATCH_FREQUENCY - Match frequency generation. The TOP value is CC0.
 *               WO[0] toggles on a match with TOP / CC0 and the counter resets. This results
 *               in a square wave with a 50% duty cycle whose frequency depends on CC0. 
 *             * TCC_WAVEGEN_NORMAL_PWM - Normal pulse-width modulation. The TOP value is PER.
 *               Waveform output WO[n] is set at the start of the period and cleared on a match
 *               with a compare channel (n % 4 by default; @see `tcc_set_output_matrix` for other
 *               options). This mode results in a square wave with a frequency defined by the
 *               PER register, and a duty cycle defined by the CCn register.
*/
void tcc_set_wavegen(uint8_t instance, tcc_wavegen_t mode);

/**
 * @brief Sets the output matrix for the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param mode The output matrix mode to use. One of:
 *            * TCC_OUTPUT_MATRIX_MODULO_4 - Compare channels CC0-CC3 are distributed across
 *              waveform outputs WO[0]-WO[7] in a modulo 4 fashion, such that CC0 controls WO[0]
 *              and WO[4], CC1 controls WO[1] and WO[5], CC2 controls WO[2] and WO[6], and CC3
 *              controls WO[3] and WO[7].
 *            * TCC_OUTPUT_MATRIX_MODULO_2 - Compare channels CC0-CC1 are distributed across
 *              waveform outputs WO[0]-WO[7] in a modulo 2 fashion, such that CC0 controls WO0,
 *              WO2, WO4, and WO6, and CC1 controls WO1, WO3, WO5, and WO7.
 *            * TCC_OUTPUT_MATRIX_ALL_CC0 - Compare channel CC0 controls all waveform outputs.
 *            * TCC_OUTPUT_MATRIX_WO0_CC0_OTHERS_CC1 - Compare channel CC0 controls waveform
 *              output WO0, and compare channel CC1 controls all other waveform outputs.
 */
void tcc_set_output_matrix(uint8_t instance, tcc_output_matrix_t mode);

/**
 * @brief Sets the polarity of a channel.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param channel The waveform output channel from 0 to 7.
 * @param polarity The polarity to set. One of:
 *                * TCC_CHANNEL_POLARITY_NORMAL - The output is high when WO[n] is set.
 *                * TCC_CHANNEL_POLARITY_INVERTED - The output is low  when WO[n] is set.
 * @details Inverting one of the waveform outputs is useful to drive a device with a differential
 *          signal; inverting several outputs can be useful if you are driving something whose
 *          polarity is reversed, like a common anode LED.
*/
void tcc_set_channel_polarity(uint8_t instance, uint8_t channel, tcc_channel_polarity_t polarity);

/** @brief Enables the TCC. Make sure to call tcc_init first to set it up.
  * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
  */
void tcc_enable(uint8_t instance);

/** @brief Checks whether the TCC is enabled.
  * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
  * @return true if the TCC is enabled, false if instance is disabled.
  */
bool tcc_is_enabled(uint8_t instance);

/**
 * @brief Sets the period of the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param period The period to set.
 * @details The period is the value that the counter counts up to before resetting. The
 *          frequency of the timer is determined by the period and the prescaler value.
 */
void tcc_set_period(uint8_t instance, uint32_t period);

/**
 * @brief Gets the period of the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @return The current period of value.
 */
uint32_t tcc_get_period(uint8_t instance);

/**
 * @brief Sets the value of a compare channel.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param channel The compare channel to set, from 0 to 3 (or 0-1 on some TCCs; check
 *                the data sheet)
 * @param value The value to set the compare channel to. Max 24 bits (16,777,215).
*/
void tcc_set_cc(uint8_t instance, uint8_t channel, uint32_t value);

/**
 * @brief Sets the value of the counter.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @param value The value to set the counter to. Max 24 bits (16,777,215).
 */
void tcc_set_count(uint8_t instance, uint32_t value);

/**
 * @brief Gets the current value of the counter.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @return The current value of the counter.
 */
uint32_t tcc_get_count(uint8_t instance);

/**
 * @brief Issues a STOP command to the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 * @details When a stop is detected while the counter is running, the counter will
 *          maintain its current value, and the Stop bit in the Status register
 *          will be set (STATUS.STOP).
 * @note There is a more complex way to set what happens to the waveform outputs
 *       in a stop condition: you can set values for each waveform in the DRVCTRL's
 *       Non-Recoverable State Output Enable (DRVCTRL.NREx) and Non- Recoverable State
 *       Output Value (DRVCTRL.NRVx) bits. This is not currently implemented; see
 *       the data sheet for more information.
*/
void tcc_stop(uint8_t instance);

/**
 * @brief Issues a RETRIGGER command to the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 */
void tcc_retrigger(uint8_t instance);

/**
 * @brief Issues an UPDATE command to the TCC.
 * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
 */
void tcc_update(uint8_t instance);

/** @brief Disables the TCC, but retains all its settings.
  * @param instance The TCC peripheral instance as numbered in the data sheet, or 0.
  */
void tcc_disable(uint8_t instance);
