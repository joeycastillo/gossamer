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

/**
 * @brief Sets up the clocks required to run a SERCOM peripheral.
 * @param sercom The SERCOM instance to set up.
 * @note This function is not intended to be called directly. Use the functions
 *       defined in `spi.h`, `i2c.h`, or `uart.h`.
 */
void _sercom_clock_setup(uint8_t sercom);

/**
 * @brief Enables a SERCOM peripheral.
 * @param sercom The SERCOM instance to enable.
 * @note This function is not intended to be called directly. Use the functions
 *       defined in `spi.h`, `i2c.h`, or `uart.h`.
 */
void _sercom_enable(uint8_t sercom);

/**
 * @brief Checks if a SERCOM peripheral is currently enabled.
 * @param sercom The SERCOM instance to check.
 * @note This function is not intended to be called directly. Use the functions
 *       defined in `spi.h`, `i2c.h`, or `uart.h`.
 * @returns true if the SERCOM peripheral is enabled, false if it is not.
 */
bool _sercom_is_enabled(uint8_t sercom);

/**
 * @brief Disables a SERCOM peripheral.
 * @param sercom The SERCOM instance to disable.
 * @note This function is not intended to be called directly. Use the functions
 *       defined in `spi.h`, `i2c.h`, or `uart.h`.
 */
void _sercom_disable(uint8_t sercom);
