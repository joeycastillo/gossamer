////< @file spi.h
/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @addtogroup spi SPI Peripheral
 * @brief The SPI peripheral is used for synchronous serial communication with other devices.
 * @details Gossamer's SPI implementation only supports SPI controller mode, not peripheral mode.
 *          Your DOPO pin in this case is COPI/MOSI, and your DIPO pin is CIPO/MISO.
 * @{
 */ 

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

/**
 * @brief Initializes the SPI SERCOM as a controller for a board with a defined SPI_SERCOM.
 * @param baud The desired baud rate
 */
void spi_init(uint32_t baud);

/**
 * @brief Enables the SPI SERCOM for a board with a defined SPI_SERCOM.
 */
void spi_enable(void);

/**
 * @brief Transfers a byte over SPI for a board with a defined SPI_SERCOM.
 * @param data The byte to send.
 * @return The byte received.
 */
uint8_t spi_transfer(uint8_t data);

/**
 * @brief Disables the SPI SERCOM for a board with a defined SPI_SERCOM.
 */
void spi_disable(void);

/**
 * @brief Initializes a specific SERCOM instance as an SPI controller.
 * @param sercom The SERCOM instance to initialize.
 * @param dopo The data out pinout. @see spi_dopo_t
 * @param dipo The data in pinout. @see spi_dipo_t
 * @param baud The desired baud rate
 */
void spi_init_instance(uint8_t sercom, spi_dopo_t dopo, spi_dipo_t dipo, uint32_t baud);

/**
 * @brief Enables a specific SERCOM instance for SPI.
 * @param sercom The SERCOM instance to enable.
 */
void spi_enable_instance(uint8_t sercom);

/**
 * @brief Transfers a byte over SPI.
 * @param sercom The SERCOM instance to use.
 * @param data The byte to send.
 * @return The byte received.
 */
uint8_t spi_transfer_instance(uint8_t sercom, uint8_t data);

/**
 * @brief Disables a specific SERCOM instance for SPI.
 * @param sercom The SERCOM instance to disable.
 */
void spi_disable_instance(uint8_t sercom);

/** @} */