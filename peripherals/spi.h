/**
 * @file spi.h
 * @brief SPI Peripheral
 */
/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

/* Routines for interacting with SPI devices. */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    SPI_MODE_0 = 0,
    SPI_MODE_1,
    SPI_MODE_2,
    SPI_MODE_3,
} spi_mode_t;

void spi_init(spi_mode_t mode, uint32_t baud);

void spi_enable(void);

uint8_t spi_transfer(uint8_t data);

void spi_disable(void);
