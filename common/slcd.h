/**
 * @file slcd.h
 * @brief Segment Liquid Crystal Display (SLCD) peripheral driver (SAM L22 only)
 */
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
#include <stddef.h>
#include "system.h"

#ifdef SLCD

typedef enum {
    SLCD_BIAS_STATIC = 0,
    SLCD_BIAS_HALF = 1,
    SLCD_BIAS_THIRD = 2,
    SLCD_BIAS_FOURTH = 3,
} slcd_bias_value_t;

typedef enum {
    SLCD_DUTY_1_COMMON = 0,
    SLCD_DUTY_2_COMMON = 1,
    SLCD_DUTY_3_COMMON = 2,
    SLCD_DUTY_4_COMMON = 3,
    SLCD_DUTY_6_COMMON = 4,
    SLCD_DUTY_8_COMMON = 5,
} slcd_duty_value_t;

typedef enum {
    SLCD_PRESCALER_DIV16 = 0,
    SLCD_PRESCALER_DIV32 = 1,
    SLCD_PRESCALER_DIV64 = 2,
    SLCD_PRESCALER_DIV128 = 3,
} slcd_prescaler_value_t;

typedef enum {
    SLCD_CLOCKSOURCE_ULP = 0,
    SLCD_CLOCKSOURCE_XOSC = 1,
} slcd_clocksource_value_t;

typedef enum {
    SLCD_CLOCKDIV_1 = 0,
    SLCD_CLOCKDIV_2 = 1,
    SLCD_CLOCKDIV_3 = 2,
    SLCD_CLOCKDIV_4 = 3,
    SLCD_CLOCKDIV_5 = 4,
    SLCD_CLOCKDIV_6 = 5,
    SLCD_CLOCKDIV_7 = 6,
    SLCD_CLOCKDIV_8 = 7,
} slcd_clockdiv_value_t;

typedef enum {
    SLCD_CSRSHIFT_LEFT = 0,
    SLCD_CSRSHIFT_RIGHT = 1,
} slcd_csrshift_value_t;

/**
 * @brief Initializes the SLCD peripheral, but does not enable it.
 * @details This function sets up the SLCD peripheral with some defaults:
 *          * LCD runs in standby
 *          * Lowest possible contrast level (2.45 volts)
 *          * Uses the low power waveform
 *          * Charge pump refresh at 250Hz (slowest option)
 *          * Reference Refresh at 62.5Hz (slowest option)
 *          * Bias buffer enabled (side note / TODO: does turning this off save power?)
 * 
 *          You will need to specify the remaining configuration options.
 *          Duty and bias are easy, but prescaler, clock divider and number of COM lines
 *          will combine to determine the frame rate of the display according to
 *          this formula: 32768/(prescaler * clock_divider * number_of_common_lines)
 * @param lcd_pins A 64-bit mask of the pins to use for the SLCD, as numbered in the pin
 *                 mux table in the data sheet (SLCD/LP[n]). For example, PB08 is SLCD/LP[2]
 *                 and PA05 is SLCD/LP[5], so the mask for using both of these pins would be
 *                 (1 << 2) | (1 << 5). The lowest pins are always used as the common lines,
 *                 so for a 1/2 duty LCD using pins 2, 5, 6, 7, 30 and 35, pins 2 and 5 would
 *                 be the COM0 and COM1, and the rest would be segment lines SEG0-SEG3.
 * @param bias The bias configuration for the SLCD. Valid options are:
 *             * SLCD_BIAS_STATIC for a static display (0V, VLCD)
 *             * SLCD_BIAS_HALF for 1/2 bias (0V, 1/2 VLCD, VLCD)
 *             * SLCD_BIAS_THIRD for 1/3 bias (0V, 1/3 VLCD, 2/3 VLCD, VLCD)
 *             * SLCD_BIAS_FOURTH for 1/4 bias (0V, 1/4 VLCD, 1/2 VLCD, 3/4 VLCD, VLCD)
 * @param duty The duty cycle for the SLCD, aka the number of common lines.
 *             Valid options are: 
 *             * SLCD_DUTY_1_COMMON for a static display
 *             * SLCD_DUTY_2_COMMON for two common lines and a 1/2 duty cycle
 *             * SLCD_DUTY_3_COMMON for three common lines and a 1/3 duty cycle
 *             * SLCD_DUTY_4_COMMON for four common lines and a 1/4 duty cycle
 *             * SLCD_DUTY_6_COMMON for six common lines and a 1/6 duty cycle
 *             * SLCD_DUTY_8_COMMON for eight common lines and a 1/8 duty cycle
 * @param clocksource The clock source for the SLCD.
 *                  Valid options are:
 *                  * SLCD_CLOCKSOURCE_ULP for the low-accuracy, low-power internal 32kHz oscillator
 *                  * SLCD_CLOCKSOURCE_XOSC for an external 32kHz crystal oscillator
 * @param prescaler The prescaler factor, which initially divides the 32kHz clock.
 *                  Valid options are: 
 *                  * SLCD_CTRLA_PRESC_PRESC16_Val - Divide by 16
 *                  * SLCD_CTRLA_PRESC_PRESC32_Val - Divide by 32
 *                  * SLCD_CTRLA_PRESC_PRESC64_Val - Divide by 64
 *                  * SLCD_CTRLA_PRESC_PRESC128_Val - Divide by 128
 * @param clock_divider The clock divider, which divides the prescaled clock.
 *                      Valid options areL
 *                      * SLCD_CLOCKDIV_1 for no division
 *                      * SLCD_CLOCKDIV_2 to divide by 2
 *                      * SLCD_CLOCKDIV_3 to divide by 3
 *                      * SLCD_CLOCKDIV_4 to divide by 4
 *                      * SLCD_CLOCKDIV_5 to divide by 5
 *                      * SLCD_CLOCKDIV_6 to divide by 6
 *                      * SLCD_CLOCKDIV_7 to divide by 7
 *                      * SLCD_CLOCKDIV_8 to divide by 8
 */
void slcd_init(uint64_t lcd_pins, slcd_bias_value_t bias, slcd_duty_value_t duty, slcd_clocksource_value_t clocksource, slcd_prescaler_value_t prescaler, slcd_clockdiv_value_t clkdiv);

/**
 * @brief Sets the contrast level for the display. Valid values are from 0-15.
 * @param contrast The contrast configuration for the SLCD
 * @details In internal supply mode, VLCD is in the range of 2.45V (contrast 0)
 *          to 3.45V (contrast 15).
 * @note You can set the contrast level at any time, without disabling the SLCD.
 */
void slcd_set_contrast(uint8_t contrast);

/**
 * @brief Enables the SLCD peripheral
 */
void slcd_enable(void);

/**
 * @brief Clears all display memory
 */
void slcd_clear(void);

/**
 * @brief Sets a segment in the display memory
 * @param com The common line to set. Valid options are 0-7.
 * @param seg The segment to set. Valid options are 0-39.
 */
void slcd_set_segment(uint8_t com, uint8_t seg);

/**
 * @brief Clears a segment in the display memory
 * @param com The common line to clear. Valid options are 0-7.
 * @param seg The segment to clear. Valid options are 0-39.
 */
void slcd_clear_segment(uint8_t com, uint8_t seg);

/**
 * @brief Configures one of the three frame counters.
 * @note SLCD must be disabled to configure frame counters.
 * @param fc The frame counter to enable. Valid options are 0, 1, or 2.
 * @param overflow_count The number of frames to count before the overflow fires.
 *                       Maximum value is 32 frames.
 * @param prescale If true, the frame counter will count every 8th frame instead of every frame.
 *                 This will let you count up to 256 frames in increments of 8.
 */
void slcd_configure_frame_counter(uint8_t fc, uint8_t overflow_count, bool prescale);

/**
 * @brief Enables or disables one of the three frame counters.
 * @note You can enable a frame counter at any time; there is no need to disable the SLCD first.
 * @param fc The frame counter to enable. Valid options are 0, 1, or 2.
 * @param enabled If true, the frame counter is enabled. If false, the frame counter is disabled.
 */
void slcd_set_frame_counter_enabled(uint8_t fc, bool enabled);

/**
 * @brief Configures the blink mode, but does not start blinking.
 * @note SLCD must be disabled to configure the blinking mode.
 * @param blink_all If true, all segments will blink. If false, only the segments
 *                  specified in the next two masks will blink.
 * @param bss0 Blink segment select 0. A 1 in any bit position (n) will cause the (COMn, SEG0)
 *             segment to blink. Ignored if blink_all is true.
 * @param bss1 Blink segment select 1. A 1 in any bit position (n) will cause the (COMn, SEG1)
 *             segment to blink. Ignored if blink_all is true.
 * @param fc The frame counter to use for blinking. Valid options are 0, 1, or 2.
 */
void slcd_configure_blink(bool blink_all, uint8_t bss0, uint8_t bss1, uint8_t fc);

/**
 * @brief Enables or disables blinking according to the configuration set by slcd_configure_blink.
 * @note You can enable blinking at any time; there is no need to disable the SLCD first.
 * @param enabled If true, blinking is enabled. If false, blinking is disabled.
 */
void slcd_set_blink_enabled(bool enabled);

/**
 * @brief Configures the Circular Shift Register animation, but does not start it.
 *        This is pretty obscure and you should consult the datasheet for more information.
 * @note SLCD must be disabled to configure the circular shift register.
 * @param initial_value 16 bits. The initial value of the circular shift register.
 * @param size The size of the circular shift register. Valid options are 2 through 16.
 * @param shift_dir The direction of the shift. Valid options are SLCD_CSRSHIFT_LEFT or SLCD_CSRSHIFT_RIGHT.
 * @param fc The frame counter to use for shifting. Valid options are 0, 1, or 2.
 */
void slcd_configure_circular_shift_animation(uint16_t initial_value, uint8_t size, slcd_csrshift_value_t shift_dir, uint8_t fc);

/**
 * @brief Enables or disables circular shift register animation according to the configuration set
 *        by slcd_configure_circular_shift_animation.
 * @note You can enable the CSR animation at any time; there is no need to disable the SLCD first.
 * @param enabled If true, the CSR animation is enabled. If false, the CSR animation is disabled.
 */
void slcd_set_circular_shift_animation_enabled(bool enabled);

/**
 * @brief Disables the SLCD peripheral
 */
void slcd_disable(void);


#endif // SLCD