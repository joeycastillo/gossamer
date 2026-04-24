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

#include "i2cd.h"

#if defined(I2C_SERCOM)

void i2c_device_init(uint8_t address) {
    (void)address;
}

void i2c_device_enable(void) {
}

bool i2c_device_is_enabled(void) {
    return false;
}

void i2c_device_set_callbacks(
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error) {
    (void)on_address_match;
    (void)on_data_received;
    (void)on_data_requested;
    (void)on_stop;
    (void)on_error;
}

void i2c_device_disable(void) {
}

#endif

void i2c_device_init_instance(uint8_t sercom, uint8_t address) {
    (void)sercom;
    (void)address;
}

void i2c_device_enable_instance(uint8_t sercom) {
    (void)sercom;
}

bool i2c_device_is_enabled_instance(uint8_t sercom) {
    (void)sercom;
    return false;
}

void i2c_device_set_callbacks_instance(
    uint8_t sercom,
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error) {
    (void)sercom;
    (void)on_address_match;
    (void)on_data_received;
    (void)on_data_requested;
    (void)on_stop;
    (void)on_error;
}

void i2c_device_disable_instance(uint8_t sercom) {
    (void)sercom;
}

void i2c_device_irq_handler(uint8_t sercom) {
    (void)sercom;
}
