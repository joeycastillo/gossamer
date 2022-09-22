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

void i2c_init(void);

I2CResult i2c_write(uint8_t address, uint8_t* data, size_t len);