////< @file tc.h
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

/**
 * @addtogroup tc Timer/Counter
 * @brief Functions for configuring and using the Timer/Counter peripherals.
 * @{
 */ 

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

typedef enum {
    TC_COUNTER_MODE_16BIT = 0,
    TC_COUNTER_MODE_8BIT = 1,
    TC_COUNTER_MODE_32BIT = 2,
} tc_counter_mode_t;

typedef enum {
    TC_WAVEGEN_NORMAL_FREQUENCY = 0,
    TC_WAVEGEN_MATCH_FREQUENCY = 1,
    TC_WAVEGEN_NORMAL_PWM = 2,
    TC_WAVEGEN_MATCH_PWM = 3,
} tc_wavegen_t;

typedef enum {
    TC_CHANNEL_POLARITY_NORMAL = 0,
    TC_CHANNEL_POLARITY_INVERTED = 1,
} tc_channel_polarity_t;

typedef enum {
  TC_EVENT_ACTION_DISABLED = 0x0ul,
  TC_EVENT_ACTION_RETRIGGER = 0x1ul,
  TC_EVENT_ACTION_COUNT = 0x2ul,
  TC_EVENT_ACTION_START = 0x3ul,
  TC_EVENT_ACTION_STAMP = 0x4ul,  /// NOTE: Not available on SAM D11 or D21
  TC_EVENT_ACTION_PPW = 0x5ul,
  TC_EVENT_ACTION_PWP = 0x6ul,
  TC_EVENT_ACTION_PW = 0x7ul,     /// NOTE: Not available on SAM D11 or D21
} tc_event_action_t;

/** @brief Enables the peripheral clock for the TC and clocks it with the selected
  *        clock source. Also resets the TC to its starting configuration, which
  *        is COUNT16 mode.
  * @details This just sets up the TC in its reset state. You are still responsible
  *          for configuring it with the options you need for waveform output.
  * @param instance The TC peripheral instance as numbered in the data sheet.
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
bool tc_init(uint8_t instance, generic_clock_generator_t clocksource, tc_prescaler_value_t prescaler);

/**
 * @brief Sets the TC's counting mode (an 8, 16 or 32-bit counter),
 * @details In addition to setting the maximum value the counter can count to (MAX),
 *          this mode also affects the the available registers and the register layout.
 *          As a result, you must use the correctly prefixed function calls to set CC
 *          and PER values, depending on the mode set here.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param mode The counting mode to set:
 *             * TC_MODE_16 is the default and has a MAX value of 65,535. The CC0 and
 *               CC1 registers can accept values from 0-65535, and can be set using the
 *               tc_count16_set_cc function.
 *             * TC_COUNTER_MODE_8BIT has a MAX value of 255. The CC0 and CC1 registers can
 *               accept values from 0-255, and can be set using the tc_count8_set_cc
 *               function. This mode also gives you access to a PER register that can set
 *               the period. The PER register can be set using tc_count8_set_period.
 *             * TC_COUNTER_MODE_32BIT has a MAX value of 4294967295, and is more complex as it
 *               involves ganging the TC with an adjacent TC. This mode is not fully
 *               supported or tested, and may require additional setup.
 */
void tc_set_counter_mode(uint8_t instance, tc_counter_mode_t mode);

/**
 * @brief Sets the TC's run-in-standby mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param runStandby true if the TC should keep running in standby mode, false if not.
 */
void tc_set_run_in_standby(uint8_t instance, bool runStandby);

/**
 * @brief Sets the TC's waveform generation mode.
 * @details This function influences how the waveform is generated by determining what
 *          happens when the counter matches and overflows.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param mode The waveform generation mode to set:
 *            * TC_WAVEGEN_NORMAL_FREQUENCY - Normal frequency generation. The TOP value is
 *              either PER (in 8-bit mode) or MAX (in 16-bit mode). Waveform output WO[0] /
 *              WO[1] toggles on a match with CC0 / CC1, and the counter resets at TOP.
 *            * TC_WAVEGEN_MATCH_FREQUENCY - Match frequency generation. The TOP value is CC0.
 *              WO[0] toggles on a match with TOP / CC0 and the counter resets. This results
 *              in a square wave with a 50% duty cycle whose frequency depends on CC0. 
 *              It is also useful for creating a timer that overflows at a specific frequency
 *              for generating interrupts or triggering DMA requests.
 *            * TC_WAVEGEN_NORMAL_PWM - Normal pulse-width modulation. The TOP value is either
 *              PER (in 8-bit mode) or MAX (in 16-bit mode). Waveform output WO[0] / WO[1] is
 *              set on a match with CC0 / CC1, and cleared on a match with TOP. This results
 *              in a pair of square waves whose duty cycles are determined by CC0 / CC1.
 *            * TC_WAVEGEN_MATCH_PWM - Match pulse-width modulation. The TOP value is CC0.
 *              WO[1] toggles on a match with CC1, and WO[0] emits a one-clock-wide pulse on
 *              every overflow (i.e. when the counter matches CC0 and loops). This results in
 *              a square wave on WO[1] whose period depends on CC0 and whose duty cycle depends
 *              on CC1, and a series of pulses on WO[0] that occurs at the same period.
 */
void tc_set_wavegen(uint8_t instance, tc_wavegen_t mode);

/**
 * @brief Sets the polarity of one of the waveform output channels.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param channel The waveform output channel. 0 or 1
 * @param polarity The polarity to set. One of:
 *                * TCC_CHANNEL_POLARITY_NORMAL - The output is high when WO[n] is set.
 *                * TCC_CHANNEL_POLARITY_INVERTED - The output is low  when WO[n] is set.
 * @details This function can be used to invert one or both of the waveform outputs. Inverting
 *          one of the waveform outputs is useful to drive a device with a differential signal;
 *          inverting both outputs can be useful if you are driving something whose polarity is
 *          reversed, like a common anode LED.
 */
void tc_set_channel_polarity(uint8_t instance, uint8_t channel, tc_channel_polarity_t polarity);

/** @brief Enables the TC. Make sure to call tc_init first to set it up.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_enable(uint8_t instance);

/** @brief Checks whether the TC is enabled.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  * @return true if the TC is enabled, false if instance is disabled.
  */
bool tc_is_enabled(uint8_t instance);

/**
 * @brief Sets the TC's period or PER register in 8-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param period The period to set. This value must be between 0 and 255, inclusive.
 *               Used above as described in `tc_set_wavegen`.
 */
void tc_count8_set_period(uint8_t instance, uint8_t period);

/**
 * @brief Gets the TC's period or PER register in 8-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @return The current period value.
 */
uint8_t tc_count8_get_period(uint8_t instance);

/**
 * @brief Sets the TC's compare channel CC0 or CC1 register in 8-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param channel The compare channel whose value set. This value must be 0 or 1.
 * @param value The value to set. This value must be between 0 and 255, inclusive.
 *              Used above as described in `tc_set_wavegen`.
 */
void tc_count8_set_cc(uint8_t instance, uint8_t channel, uint8_t value);

/**
 * @brief Sets the TC's compare channel CC0 or CC1 register in 16-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param channel The compare channel whose value set. This value must be 0 or 1.
 * @param value The value to set. This value must be between 0 and 65535, inclusive.
 *              Used above as described in `tc_set_wavegen`.
 */
void tc_count16_set_cc(uint8_t instance, uint8_t channel, uint16_t value);

/**
 * @brief Sets the TC's compare channel CC0 or CC1 register in 32-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param channel The compare channel whose value set. This value must be 0 or 1.
 * @param value The value to set.
 */
void tc_count32_set_cc(uint8_t instance, uint8_t channel, uint32_t value);

/**
 * @brief Sets the TC's counter value in 8-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
void tc_count8_set_count(uint8_t instance, uint8_t value);

/**
 * @brief Sets the TC's counter value in 16-bit mode.
 * @details It is possible to change the counter value when the counter is stopped
 *          or when it is running. 
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
void tc_count16_set_count(uint8_t instance, uint16_t value);

/**
 * @brief Sets the TC's counter value in 32-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
void tc_count32_set_count(uint8_t instance, uint32_t value);

/**
 * @brief Gets the TC's current counter value in 8-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
uint8_t tc_count8_get_count(uint8_t instance);

/**
 * @brief Gets the TC's current counter value in 16-bit mode.
 * @details It is possible to change the counter value when the counter is stopped
 *          or when it is running. 
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
uint16_t tc_count16_get_count(uint8_t instance);

/**
 * @brief Gets the TC's current counter value in 32-bit mode.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param value The value to set.
 */
uint32_t tc_count32_get_count(uint8_t instance);

/**
 * @brief Sets the action to take when the TC receives an event.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 * @param action The action to take. @see tc_event_action_t
 */
void tc_set_event_action(uint8_t instance, tc_event_action_t action);

/**
 * @brief Issues a STOP command to the TC.
 * @details When a Stop is detected while the counter is running, the counter will
 *          retain its current value. All waveforms are cleared and the Stop bit in
 *          the Status register is set (STATUS.STOP).
 * @param instance The TC peripheral instance as numbered in the data sheet.
 */
void tc_stop(uint8_t instance);

/**
 * @brief Issues a RETRIGGER command to the TC.
 * @details When the command is detected during counting operation, the counter will
 *          be reset. When the re-trigger command is detected while the counter is
 *          stopped, the counter will resume counting from its current value.
 * @param instance The TC peripheral instance as numbered in the data sheet.
 */
void tc_retrigger(uint8_t instance);

/** @brief Disables the TC, but retains all its settings.
  * @param instance The TC peripheral instance as numbered in the data sheet, or 0.
  */
void tc_disable(uint8_t instance);

/** @} */
