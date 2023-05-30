/**
 * @file uart.h
 * @brief UART Peripheral
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void uart_init(uint32_t baud);

void uart_enable(void);

void uart_write(uint8_t *data, size_t length);

uint8_t uart_read(void);

void uart_disable(void);
