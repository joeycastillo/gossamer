////< @file uart.h
/* 
 * MIT License
 *
 * Copyright (c) 2024 Joey Castillo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * @addtogroup uart UART Peripheral
 * @brief The UART peripheral is used for serial communication over TX and RX pins.
 * @details Note that Gossaermer's UART implementation is interrupt-driven, which is
 *          different from the SPI and I2C peripherals. If you want to use the UART,
 *          you MUST implement the following function, replacing 'N' with the SERCOM
 *          number you are using:
 *              ```void irq_handler_sercomN(void) { uart_irq_handler(N); }```
 *          This interrupt handler will allow the UART to buffer incoming data to a
 *          FIFO, which is what you're actually reading from when you call `uart_read`.
 * @warning If you don't implement the interrupt handler, your application will hang,
 *          and you'll be confused until you read this line! (or in my case, write it)
 * @{
 */ 

/// @brief UART transmit pinout options
typedef enum {
    UART_TXPO_0 = 0,
    UART_TXPO_2,
    UART_TXPO_0_FLOW_CONTROL,
    UART_TXPO_NONE = 0xff
} uart_txpo_t;

/// @brief UART receive pinout options
typedef enum {
    UART_RXPO_0 = 0,
    UART_RXPO_1,
    UART_RXPO_2,
    UART_RXPO_3,
    UART_RXPO_NONE = 0xff
} uart_rxpo_t;

/**
 * @brief Initializes the UART peripheral for a board with a defined UART_SERCOM.
 * @param baud The desired baud rate.
 */
void uart_init(uint32_t baud);

/**
 * @brief Enables the UART peripheral for a board with a defined UART_SERCOM.
 */
void uart_enable(void);

/**
 * @brief Sets whether the UART_SERCOM should run in standby mode.
 * @param run_in_standby true if the UART should run in standby mode.
 */
void uart_set_run_in_standby(bool run_in_standby);

/**
 * @brief Sets IRDA mode for the UART_SERCOM.
 * @param irda true if the UART should run in IRDA mode.
 */
void uart_set_irda_mode(bool irda);

/**
 * @brief Writes data to the UART peripheral for a board with a defined UART_SERCOM.
 * @param data The data to write.
 * @param length The number of bytes to write.
 */
void uart_write(char *data, size_t length);

/**
 * @brief Reads data from the UART peripheral for a board with a defined UART_SERCOM.
 * @param data The buffer to read into.
 * @param max_length The maximum number of bytes to read.
 * @return The number of bytes read.
 */
size_t uart_read(char *data, size_t max_length);

/**
 * @brief Reads a single byte from the UART peripheral for a board with a defined UART_SERCOM.
 * @param byte The byte read.
 * @return true if a byte was read.
 */
bool uart_read_byte(char *byte);

/**
 * @brief Disables the UART peripheral for a board with a defined UART_SERCOM.
 */
void uart_disable(void);

/**
 * @brief initializes a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to initialize.
 * @param txpo The TX pinout. @see uart_txpo_t
 * @param rxpo The RX pinout. @see uart_rxpo_t
 * @param baud The desired baud rate
 */
void uart_init_instance(uint8_t sercom, uart_txpo_t txpo, uart_rxpo_t rxpo, uint32_t baud);

/**
 * @brief Enables a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to enable.
 */
void uart_set_run_in_standby_instance(uint8_t sercom, bool run_in_standby);

/**
 * @brief Sets IRDA mode for a specific SERCOM instance.
 * @param sercom The SERCOM instance to configure.
 * @param irda true if the UART should run in IRDA mode.
 */
void uart_set_irda_mode_instance(uint8_t sercom, bool irda);

/**
 * @brief Enables a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to enable.
 */
void uart_enable_instance(uint8_t sercom);

/**
 * @brief Writes data to a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to use.
 * @param data The data to write.
 * @param length The number of bytes to write.
 */
void uart_write_instance(uint8_t sercom, char *data, size_t length);

/**
 * @brief Reads data from a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to use.
 * @param data The buffer to read into.
 * @param max_length The maximum number of bytes to read.
 * @return The number of bytes read.
 */
size_t uart_read_instance(uint8_t sercom, char *data, size_t max_length);

/**
 * @brief Reads a single byte from a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to use.
 * @param byte The byte read.
 * @return true if a byte was read.
 */
bool uart_read_byte_instance(uint8_t sercom, char *byte);

/**
 * @brief Disables a specific SERCOM UART instance.
 * @param sercom The SERCOM instance to disable.
 */
void uart_disable_instance(uint8_t sercom);

/**
 * @brief UART IRQ handler
 * @details Call this function from the IRQ handler for the UART SERCOM.
 *          For example, if you are using SERCOM5, you would do this:
 *          `void irq_handler_sercom5(void) { uart_irq_handler(5); }`
 *          UART transmssion and reception are interrupt driven, and
 *          nothing will work if this function is not called.
 * @param sercom SERCOM number
*/
void uart_irq_handler(uint8_t sercom);

/** @} */
