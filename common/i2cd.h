////< @file i2cd.h
/*
 * MIT License
 *
 * Copyright (c) 2026 Joey Castillo
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

/**
 * @addtogroup i2cd I2C Device Mode Peripheral
 * @brief The I2C device mode peripheral allows the chip to act as an I2C device,
 *        responding to a controller driving the bus.
 * @details I2C device mode is interrupt-driven. You MUST implement the following
 *          function, replacing 'N' with the SERCOM number you are using:
 *              ```void irq_handler_sercomN(void) { i2c_device_irq_handler(N); }```
 * @warning If you don't implement the interrupt handler, your device will not
 *          respond to any I2C transactions.
 * @{
 */

/// @brief Called when a controller addresses this device.
/// @param sercom The SERCOM instance that received the address match.
/// @param direction false = controller will write (device receives data),
///                  true = controller will read (device must provide data).
typedef void (*i2c_device_address_cb_t)(uint8_t sercom, bool direction);

/// @brief Called when the controller has written a byte to this device.
/// @param sercom The SERCOM instance that received the data.
/// @param data The byte received from the controller.
typedef void (*i2c_device_data_received_cb_t)(uint8_t sercom, uint8_t data);

/// @brief Called when the controller wants to read a byte from this device.
/// @param sercom The SERCOM instance requesting data.
/// @return The byte to send to the controller.
typedef uint8_t (*i2c_device_data_requested_cb_t)(uint8_t sercom);

/// @brief Called when the controller sends a stop condition, ending the transaction.
/// @param sercom The SERCOM instance that received the stop.
typedef void (*i2c_device_stop_cb_t)(uint8_t sercom);

/// @brief Called when a bus error occurs.
/// @param sercom The SERCOM instance that encountered the error.
typedef void (*i2c_device_error_cb_t)(uint8_t sercom);

#if defined(I2C_SERCOM)

/**
 * @brief Initializes I2C device mode for a board with a defined I2C_SERCOM.
 * @param address The 7-bit I2C address this device should respond to.
 * @note You are responsible for setting the appropriate pin mux for the SDA and SCL pins.
 */
void i2c_device_init(uint8_t address);

/**
 * @brief Enables I2C device mode for a board with a defined I2C_SERCOM.
 */
void i2c_device_enable(void);

/**
 * @brief Checks if I2C device mode is enabled for a board with a defined I2C_SERCOM.
 * @returns true if I2C device mode is enabled, false if it is not.
 */
bool i2c_device_is_enabled(void);

/**
 * @brief Sets callbacks for I2C device mode events on a board with a defined I2C_SERCOM.
 * @param on_address_match Called when the controller addresses this device. May be NULL.
 * @param on_data_received Called when the controller writes a byte. May be NULL.
 * @param on_data_requested Called when the controller reads a byte. May be NULL (sends 0x00).
 * @param on_stop Called when the controller sends a stop condition. May be NULL.
 * @param on_error Called on bus error. May be NULL.
 */
void i2c_device_set_callbacks(
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error);

/**
 * @brief Disables I2C device mode for a board with a defined I2C_SERCOM.
 */
void i2c_device_disable(void);

#endif

/**
 * @brief Initializes I2C device mode on the given SERCOM.
 * @param sercom The SERCOM instance, as numbered in the data sheet.
 * @param address The 7-bit I2C address this device should respond to.
 * @note You are responsible for setting the appropriate pin mux for the SDA and SCL pins.
 */
void i2c_device_init_instance(uint8_t sercom, uint8_t address);

/**
 * @brief Enables I2C device mode on the given SERCOM.
 * @param sercom The SERCOM instance, as numbered in the data sheet.
 */
void i2c_device_enable_instance(uint8_t sercom);

/**
 * @brief Checks if I2C device mode is enabled on the given SERCOM.
 * @param sercom The SERCOM instance, as numbered in the data sheet.
 * @returns true if I2C device mode is enabled, false if it is not.
 */
bool i2c_device_is_enabled_instance(uint8_t sercom);

/**
 * @brief Sets callbacks for I2C device mode events on the given SERCOM.
 * @param sercom The SERCOM instance, as numbered in the data sheet.
 * @param on_address_match Called when the controller addresses this device. May be NULL.
 * @param on_data_received Called when the controller writes a byte. May be NULL.
 * @param on_data_requested Called when the controller reads a byte. May be NULL (sends 0x00).
 * @param on_stop Called when the controller sends a stop condition. May be NULL.
 * @param on_error Called on bus error. May be NULL.
 */
void i2c_device_set_callbacks_instance(
    uint8_t sercom,
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error);

/**
 * @brief Disables I2C device mode on the given SERCOM.
 * @param sercom The SERCOM instance, as numbered in the data sheet.
 */
void i2c_device_disable_instance(uint8_t sercom);

/**
 * @brief I2C device mode IRQ handler.
 * @details Call this from the SERCOM IRQ handler for your I2C device SERCOM:
 *          `void irq_handler_sercom0(void) { i2c_device_irq_handler(0); }`
 * @param sercom The SERCOM instance number.
 */
void i2c_device_irq_handler(uint8_t sercom);

/** @} */
