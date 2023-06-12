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
#include "sercom.h"
#include "uart.h"

#if defined(UART_SERCOM)

void uart_init(uint32_t baud) {
    uart_init_custom(UART_SERCOM, UART_SERCOM_TXPO, UART_SERCOM_RXPO, baud);
}

void uart_enable(void) {
    uart_enable_custom(UART_SERCOM);
}

void uart_write(uint8_t *data, size_t length) {
    uart_write_custom(UART_SERCOM, data, length);
}

uint8_t uart_read(void) {
    return uart_read_custom(UART_SERCOM);
}

void uart_disable(void) {
    uart_disable_custom(UART_SERCOM);
}

#endif

void uart_init_custom(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;

    _sercom_clock_setup(sercom);

    /* Reset the SERCOM. */
    SERCOM->USART.CTRLA.bit.ENABLE = 0;
    while (SERCOM->USART.SYNCBUSY.bit.ENABLE) {};
    SERCOM->USART.CTRLA.bit.SWRST = 1;
    while (SERCOM->USART.SYNCBUSY.bit.SWRST || SERCOM->USART.SYNCBUSY.bit.ENABLE) {};

    if (txpo == UART_TXPO_NONE) {
        SERCOM->USART.CTRLB.bit.TXEN = 0;
        txpo = 0;
    } else {
        SERCOM->USART.CTRLB.bit.TXEN = 1;
    }

    if (rxpo == UART_RXPO_NONE) {
        SERCOM->USART.CTRLB.bit.RXEN = 0;
        rxpo = 0;
    } else {
        SERCOM->USART.CTRLB.bit.RXEN = 1;
    }

    /* Setup UART controller. */
    SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE(1) |
                                   SERCOM_USART_CTRLA_SAMPR(0) |
                                   SERCOM_USART_CTRLA_DORD |
                                   SERCOM_USART_CTRLA_TXPO(txpo) |
                                   SERCOM_USART_CTRLA_RXPO(rxpo);

    /* Set baud */
    uint32_t cpu_speed = get_cpu_frequency();
    uint64_t br = (uint64_t)65536 * (cpu_speed - 16 * baud) / cpu_speed;
    SERCOM->USART.BAUD.bit.BAUD = br + 1;
}

void uart_enable_custom(uint8_t sercom) {
    _sercom_enable(sercom);
}

void uart_write_custom(uint8_t sercom, uint8_t *data, size_t length) {
    while (!SERCOM_Peripherals[sercom].sercom->USART.INTFLAG.bit.DRE) {}

    for (size_t i = 0; i < length; i++) {
        SERCOM_Peripherals[sercom].sercom->USART.DATA.bit.DATA = data[i];
        while (!SERCOM_Peripherals[sercom].sercom->USART.INTFLAG.bit.TXC) {}
    }
}

uint8_t uart_read_custom(uint8_t sercom) {
    while (!SERCOM_Peripherals[sercom].sercom->USART.INTFLAG.bit.DRE) {}

    while (!SERCOM_Peripherals[sercom].sercom->USART.INTFLAG.bit.RXC) {}
    return SERCOM_Peripherals[sercom].sercom->USART.DATA.bit.DATA;
}

void uart_disable_custom(uint8_t sercom) {
    _sercom_disable(sercom);
}
