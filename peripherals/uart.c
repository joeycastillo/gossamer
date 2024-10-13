/*
    Bits cribbed from Alex Taradov's BSD-3 licensed UART peripheral,
    Copyright (c) 2017-2023, Alex Taradov <alex@taradov.com>. All rights reserved.
    Full text available at: https://opensource.org/licenses/BSD-3
    Other stuff is MIT-licensed by me, Joey Castillo.
    Copyright 2023 Joey Castillo for Oddly Specific Objects.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "pins.h"
#include "sam.h"
#include "system.h"
#include "sercom.h"
#include "uart.h"

#ifndef SERCOM_USART_CTRLA_MODE_USART_INT_CLK
#define SERCOM_USART_CTRLA_MODE_USART_INT_CLK SERCOM_USART_CTRLA_MODE(1)
#endif

#define UART_BUF_SIZE 256

typedef struct {
    int     wr;
    int     rd;
    uint8_t data[UART_BUF_SIZE];
} fifo_buffer_t;

static volatile fifo_buffer_t uart_rx_fifo;
static volatile fifo_buffer_t uart_tx_fifo;
static volatile bool uart_fifo_overflow = false;

static bool fifo_push(volatile fifo_buffer_t *fifo, uint8_t value) {
    int next_wr = (fifo->wr + 1) % UART_BUF_SIZE;

    if (next_wr == fifo->rd) return false;

    fifo->data[fifo->wr] = value;
    fifo->wr = next_wr;

    return true;
}

static bool fifo_pop(volatile fifo_buffer_t *fifo, uint8_t *value) {
    if (fifo->rd == fifo->wr) return false;

    *value = fifo->data[fifo->rd];
    fifo->rd = (fifo->rd + 1) % UART_BUF_SIZE;

    return true;
}

static bool uart_write_byte(uint8_t sercom, uint8_t byte) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;
    bool res = false;

    NVIC_DisableIRQ(SERCOM_Peripherals[sercom].interrupt_line);

    if (fifo_push(&uart_tx_fifo, byte)) {
        SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_DRE;
        res = true;
    }

    NVIC_EnableIRQ(SERCOM_Peripherals[sercom].interrupt_line);

    return res;
}

#if defined(UART_SERCOM)

bool uart_read_byte(uint8_t *byte) {
    return uart_read_byte_instance(UART_SERCOM, byte);
}

void uart_init(uint32_t baud) {
    uart_init_instance(UART_SERCOM, UART_SERCOM_TXPO, UART_SERCOM_RXPO, baud);
}

void uart_set_run_in_standby(bool run_in_standby) {
    uart_set_run_in_standby_instance(UART_SERCOM, run_in_standby);
}

void uart_set_irda_mode(bool irda) {
    uart_set_irda_mode_instance(UART_SERCOM, irda);
}

void uart_enable(void) {
    uart_enable_instance(UART_SERCOM);
}

void uart_write(uint8_t *data, size_t length) {
    uart_write_instance(UART_SERCOM, data, length);
}

size_t uart_read(uint8_t *data, size_t max_length) {
    return uart_read_instance(UART_SERCOM, data, max_length);
}

void uart_disable(void) {
    uart_disable_instance(UART_SERCOM);
}

#endif

void uart_init_instance(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud) {
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

    SERCOM->USART.CTRLA.reg = SERCOM_USART_CTRLA_FORM(0) |
                              SERCOM_USART_CTRLA_MODE_USART_INT_CLK |
                              SERCOM_USART_CTRLA_DORD |
                              SERCOM_USART_CTRLA_SAMPR(0) |
                              SERCOM_USART_CTRLA_TXPO(txpo) |
                              SERCOM_USART_CTRLA_RXPO(rxpo);

    uint32_t cpu_speed = get_cpu_frequency();
    uint64_t br = (uint64_t)65536 * (cpu_speed - 16 * baud) / cpu_speed;
    SERCOM->USART.BAUD.bit.BAUD = br + 1;

    uart_tx_fifo.wr = 0;
    uart_tx_fifo.rd = 0;

    uart_rx_fifo.wr = 0;
    uart_rx_fifo.rd = 0;

    uart_fifo_overflow = false;

    SERCOM->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;

    NVIC_ClearPendingIRQ(SERCOM_Peripherals[sercom].interrupt_line);
    NVIC_EnableIRQ(SERCOM_Peripherals[sercom].interrupt_line);
}

void uart_set_run_in_standby_instance(uint8_t sercom, bool run_in_standby) {
    SERCOM_Peripherals[sercom].sercom->USART.CTRLA.bit.RUNSTDBY = run_in_standby;
}

void uart_set_irda_mode_instance(uint8_t sercom, bool irda) {
    SERCOM_Peripherals[sercom].sercom->USART.CTRLB.bit.ENC = irda;
    while (SERCOM_Peripherals[sercom].sercom->USART.SYNCBUSY.bit.CTRLB);
}

void uart_enable_instance(uint8_t sercom) {
    _sercom_enable(sercom);
}

void uart_write_instance(uint8_t sercom, uint8_t *data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        uart_write_byte(sercom, data[i]);
    }
}

size_t uart_read_instance(uint8_t sercom, uint8_t *data, size_t max_length) {
    size_t bytes_read;
    for (bytes_read = 0; bytes_read < max_length; bytes_read++) {
        if (!uart_read_byte_instance(sercom, &data[bytes_read])) break;
    }

    return bytes_read;
}

bool uart_read_byte_instance(uint8_t sercom, uint8_t *byte) {
    bool res = false;

    NVIC_DisableIRQ(SERCOM_Peripherals[sercom].interrupt_line);

    if (uart_fifo_overflow) {
        uart_fifo_overflow = false;
        res = true;
    } else if (fifo_pop(&uart_rx_fifo, byte)) {
        res = true;
    }

    NVIC_EnableIRQ(SERCOM_Peripherals[sercom].interrupt_line);

    return res;
}

void uart_disable_instance(uint8_t sercom) {
    _sercom_disable(sercom);
}

void uart_irq_handler(uint8_t sercom) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;

    int flags = SERCOM->USART.INTFLAG.reg;

    if (flags & SERCOM_USART_INTFLAG_RXC) {
        int status = SERCOM->USART.STATUS.reg;
        uint8_t byte = SERCOM->USART.DATA.reg;

        SERCOM->USART.STATUS.reg = status;

        if (!fifo_push(&uart_rx_fifo, byte)) uart_fifo_overflow = true;
    }

    if (flags & SERCOM_USART_INTFLAG_DRE) {
        uint8_t byte;

        if (fifo_pop(&uart_tx_fifo, &byte)) SERCOM->USART.DATA.reg = byte;
        else SERCOM->USART.INTENCLR.reg = SERCOM_USART_INTENCLR_DRE;
    }
}
