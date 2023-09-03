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

void uart_set_run_in_standby(bool run_in_standby);

void uart_write(uint8_t *data, size_t length);

size_t uart_read(uint8_t *data, size_t max_length);

bool uart_read_byte(uint8_t *byte);

void uart_disable(void);

void uart_init_instance(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud);

void uart_set_run_in_standby_instance(uint8_t sercom, bool run_in_standby);

void uart_enable_instance(uint8_t sercom);

void uart_write_instance(uint8_t sercom, uint8_t *data, size_t length);

size_t uart_read_instance(uint8_t sercom, uint8_t *data, size_t max_length);

bool uart_read_byte_instance(uint8_t sercom, uint8_t *byte);

void uart_disable_instance(uint8_t sercom);

/**
 * @brief UART IRQ handler
 * @details Call this function from the IRQ handler for the UART SERCOM.
 *          For example, if you are using SERCOM5, you would do this:
 *          `void irq_handler_sercom5(void) { uart_irq_handler(5); }`
 *          UART transmssion and reception are interrupt driven, and
 *          nothing will work if this function is not called.
 * @param sercom SERCOM number
 * @todo Really need to refactor this so it's called from the HAL, not the app.
*/
void uart_irq_handler(uint8_t sercom);
