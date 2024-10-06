/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "pins.h"
#include "system.h"
#include "sercom.h"
#include "spi.h"

#if defined(SPI_SERCOM)

void spi_init(spi_mode_t mode, uint32_t baud) {
}

void spi_enable(void) {
}

uint8_t spi_transfer(uint8_t data) {
    return 0;
}

void spi_disable(void) {
}

#endif

void spi_init_instance(uint8_t sercom, spi_dopo_t dopo, spi_dipo_t dipo, spi_mode_t mode, uint32_t baud) {
}

void spi_enable_instance(uint8_t sercom) {
}

uint8_t spi_transfer_instance(uint8_t sercom, uint8_t data) {
    return 0;
}

void spi_disable_instance(uint8_t sercom) {
}
