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

typedef enum {
    SPI_DOPO_0_SCK_1 = 0,
    SPI_DOPO_2_SCK_3,
    SPI_DOPO_3_SCK_1,
    SPI_DOPO_0_SCK_3,
} spi_dopo_t;

typedef enum {
    SPI_DIPO_0 = 0,
    SPI_DIPO_1,
    SPI_DIPO_2,
    SPI_DIPO_3,
    SPI_DIPO_NONE = 0xff
} spi_dipo_t;

void spi_init(spi_mode_t mode, uint32_t baud);

void spi_enable(void);

uint8_t spi_transfer(uint8_t data);

void spi_disable(void);

void spi_init_custom(uint8_t sercom, spi_dopo_t dopo, spi_dipo_t dipo, spi_mode_t mode, uint32_t baud);

void spi_enable_custom(uint8_t sercom);

uint8_t spi_transfer_custom(uint8_t sercom, uint8_t data);

void spi_disable_custom(uint8_t sercom);
