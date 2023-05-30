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
#include "uart.h"

#if defined(UART_GCLK_CLKCTRL_ID)

void uart_init(uint32_t baud) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= UART_SERCOM_APBCMASK;
    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | UART_GCLK_CLKCTRL_ID;
    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};
#else
    /* Enable the APB clock for SERCOM. */
#if defined(UART_SERCOM_APBCMASK)
    MCLK->APBCMASK.reg |= UART_SERCOM_APBCMASK;
#elif defined(UART_SERCOM_APBDMASK)
    MCLK->APBDMASK.reg |= UART_SERCOM_APBDMASK;
#endif

    /* Enable GCLK1 for the SERCOM */
    GCLK->PCHCTRL[UART_GCLK_CLKCTRL_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0_Val;
    /* Wait for bus synchronization. */
    while (GCLK->SYNCBUSY.bit.GENCTRL0) {};
#endif

    /* Reset the SERCOM. */
    UART_SERCOM->USART.CTRLA.bit.ENABLE = 0;
    while (UART_SERCOM->USART.SYNCBUSY.bit.ENABLE) {};
    UART_SERCOM->USART.CTRLA.bit.SWRST = 1;
    while (UART_SERCOM->USART.SYNCBUSY.bit.SWRST || UART_SERCOM->USART.SYNCBUSY.bit.ENABLE) {};

    /* Setup UART controller. */
    UART_SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(1) |
                                   SERCOM_USART_CTRLA_SAMPR(0) |
                                   SERCOM_USART_CTRLA_DORD |
                                   UART_SERCOM_TXPO |
                                   UART_SERCOM_RXPO;
    UART_SERCOM->USART.CTRLB.bit.TXEN = 1;
    UART_SERCOM->USART.CTRLB.bit.RXEN = 1;

    /* Set baud */
    uint32_t cpu_speed = get_cpu_frequency();
    uint64_t br = (uint64_t)65536 * (cpu_speed - 16 * baud) / cpu_speed;
    UART_SERCOM->USART.BAUD.bit.BAUD = br + 1;
}

void uart_enable(void) {
    /* Enable the SERCOM. */
    UART_SERCOM->USART.CTRLA.bit.ENABLE = 1;
    while (UART_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}
}

void uart_write(uint8_t *data, size_t length) {
    while (!UART_SERCOM->USART.INTFLAG.bit.DRE) {}

    for (size_t i = 0; i < length; i++) {
        UART_SERCOM->USART.DATA.bit.DATA = data[i];
        while (!UART_SERCOM->USART.INTFLAG.bit.TXC) {}
    }
}

uint8_t uart_read(void) {
    while (!UART_SERCOM->USART.INTFLAG.bit.DRE) {}

    while (!UART_SERCOM->USART.INTFLAG.bit.RXC) {}
    return UART_SERCOM->USART.DATA.bit.DATA;
}

void uart_disable(void) {
    UART_SERCOM->USART.CTRLA.bit.ENABLE = 0;
    while (UART_SERCOM->USART.SYNCBUSY.bit.ENABLE) {}
}

#endif
