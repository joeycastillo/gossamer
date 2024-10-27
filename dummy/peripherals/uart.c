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
#include "system.h"
#include "sercom.h"
#include "uart.h"

#if defined(UART_SERCOM)

bool uart_read_byte(char *byte) {
    return false;
}

void uart_init(uint32_t baud) {
}

void uart_set_run_in_standby(bool run_in_standby) {
}

void uart_enable(void) {
}

void uart_write(char *data, size_t length) {
}

size_t uart_read(char *data, size_t max_length) {
    return 0;
}

void uart_disable(void) {
}

#endif

void uart_init_instance(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud) {
}

void uart_set_run_in_standby_instance(uint8_t sercom, bool run_in_standby) {
}

void uart_enable_instance(uint8_t sercom) {
}

void uart_write_instance(uint8_t sercom, char *data, size_t length) {
}

size_t uart_read_instance(uint8_t sercom, char *data, size_t max_length) {
    return 0;
}

bool uart_read_byte_instance(uint8_t sercom, char *byte) {
    return false;
}

void uart_disable_instance(uint8_t sercom) {
}

void uart_irq_handler(uint8_t sercom) {
}
