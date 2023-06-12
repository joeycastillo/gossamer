/**
 * @file uart.h
 * @brief UART Peripheral
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef enum {
    UART_TXPO_0 = 0,
    UART_TXPO_2,
    UART_TXPO_0_FLOW_CONTROL,
    UART_TXPO_NONE = 0xff
} uart_txpo_t;

typedef enum {
    UART_RXPO_0 = 0,
    UART_RXPO_1,
    UART_RXPO_2,
    UART_RXPO_3,
    UART_RXPO_NONE = 0xff
} uart_rxpo_t;

void uart_init(uint32_t baud);

void uart_enable(void);

void uart_write(uint8_t *data, size_t length);

uint8_t uart_read(void);

void uart_disable(void);

void uart_init_custom(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud);

void uart_enable_custom(uint8_t sercom);

void uart_write_custom(uint8_t sercom, uint8_t *data, size_t length);

uint8_t uart_read_custom(uint8_t sercom);

void uart_disable_custom(uint8_t sercom);
