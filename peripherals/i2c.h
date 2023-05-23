/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with I2C devices. */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    I2C_RESULT_SUCCESS = 0,
    I2C_RESULT_ERR_ADDR_NACK = -1,
    I2C_RESULT_ERR_BUSSTATE = -2,
    I2C_RESULT_ERR_BUSERR = -3,
    I2C_RESULT_ERR_DATA_NACK = -4,
} I2CResult;

/**
 * @brief Initializes the I2C peripheral.
 * @details Initializes the peripheral with a bus speed of 400kHz. You are still responsible
 *          for setting the SDA and SCL pins to the correct peripheral pin mux.
 * @note Requires that your board have I2C_SERCOM_APBCMASK and I2C_GCLK_CLKCTRL_ID defined.
 */
void i2c_init(void);

/**
 * @brief Writes data to an I2C device at the provided address.
 * @param address The I2C address of the device.
 * @param data The data to write.
 * @param len The length of the data to write.
 */
I2CResult i2c_write(uint8_t address, uint8_t* data, size_t len);