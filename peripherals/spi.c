/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "pins.h"
#include "sam.h"
#include "system.h"
#include "spi.h"

#if defined(SPI_SERCOM_APBCMASK) && defined(SPI_GCLK_CLKCTRL_ID)

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

void spi_init(spi_mode_t mode, uint32_t baud) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= SPI_SERCOM_APBCMASK;
    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | SPI_GCLK_CLKCTRL_ID;
    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};
#else
    /* Enable the APB clock for SERCOM. */
    MCLK->APBCMASK.reg |= SPI_SERCOM_APBCMASK;
    /* Enable GCLK1 for the SERCOM */
    GCLK->PCHCTRL[SPI_GCLK_CLKCTRL_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0_Val;
    /* Wait for bus synchronization. */
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {};
#endif

    /* Reset the SERCOM. */
    SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 0;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {};
    SPI_SERCOM->SPI.CTRLA.bit.SWRST = 1;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.SWRST || SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {};

    /* Setup SPI controller. */
#if defined(_SAMD21_) || defined(_SAMD11_)
    SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE_SPI_MASTER | SPI_SERCOM_DOPO | SPI_SERCOM_DIPO;
#else
    SPI_SERCOM->SPI.CTRLA.reg = SERCOM_SPI_CTRLA_MODE(3) | SPI_SERCOM_DOPO | SPI_SERCOM_DIPO;
#endif

    switch (mode) {
        case SPI_MODE_0:
            SPI_SERCOM->SPI.CTRLA.bit.CPOL = 0;
            SPI_SERCOM->SPI.CTRLA.bit.CPHA = 0;
            break;
        case SPI_MODE_1:
            SPI_SERCOM->SPI.CTRLA.bit.CPOL = 0;
            SPI_SERCOM->SPI.CTRLA.bit.CPHA = 1;
            break;
        case SPI_MODE_2:
            SPI_SERCOM->SPI.CTRLA.bit.CPOL = 1;
            SPI_SERCOM->SPI.CTRLA.bit.CPHA = 0;
            break;
        case SPI_MODE_3:
            SPI_SERCOM->SPI.CTRLA.bit.CPOL = 1;
            SPI_SERCOM->SPI.CTRLA.bit.CPHA = 1;
            break;
    }

    SPI_SERCOM->SPI.CTRLB.bit.RXEN = 1;

    /* Set baud */
    uint32_t baudrate = baud;
    if (baudrate == get_cpu_frequency()) {
        SPI_SERCOM->SPI.BAUD.reg = 0x1;
    } else {
        SPI_SERCOM->SPI.BAUD.reg = get_cpu_frequency() / (2 * baudrate) - 1;
    }
}

void spi_enable(void) {
    /* Enable the SERCOM. */
    SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 1;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {}
}

uint8_t spi_transfer(uint8_t data) {
    while (!SPI_SERCOM->SPI.INTFLAG.bit.DRE) {}

    SPI_SERCOM->SPI.DATA.bit.DATA = data;

    while (!SPI_SERCOM->SPI.INTFLAG.bit.RXC) {}

    return SPI_SERCOM->SPI.DATA.bit.DATA;
}

void spi_disable(void) {
    SPI_SERCOM->SPI.CTRLA.bit.ENABLE = 0;
    while (SPI_SERCOM->SPI.SYNCBUSY.bit.ENABLE) {}
}

#endif
