/** 
 * @file adc.h
 * @brief Analog to Digital Converter
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


/**
 * @brief Enable the ADC peripheral
 * @details This function enables some sensible defaults for basic use cases:
 *          * Clocks the ADC with the main 8 MHz oscillator and divider of 16
 *            for a 500 kHz ADC clock.
 *          * Sets sampling length to one clock cycle, which is appropriate
 *            for inputs with impedance up to 28 kOhm.
 *          * Sets oversampling of 16 samples, resulting in a conversion time
 *            of 32 microseconds.
 *          * Sets the ADC up for single-ended measurement (you are measuring
 *            the voltage between the analog input pin and ground, not between
 *            two analog input pins)
 *          * On SAM L21 and L22, sets the reference voltage to VDDANA, which
 *            allows use of the full 0-3.3V range.
 *          * On SAM D11 and D21, sets the reference voltage to VDDANA / 2 and
 *            the gain to 1/2, which also allows use of the full 0-3.3V range.
 */
void adc_enable(void);

/**
 * @brief Gets the analog value of the given ADC channel.
 * @param channel The ADC channel to read from (NOT the pin number).
 * @return The analog value of the given ADC channel, from 0-65535 by default.
 */
uint16_t adc_get_analog_value(uint16_t channel);

/**
 * @brief Disables the ADC peripheral
 */
void adc_disable(void);
