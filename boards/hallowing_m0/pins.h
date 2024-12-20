#pragma once

#include "hal_gpio.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"

// Analog pins
HAL_GPIO_PIN(AREF, A,  3)
HAL_GPIO_PIN(A0,   A,  2)
HAL_GPIO_PIN(A1,   B,  8)
HAL_GPIO_PIN(A2,   B,  9)
HAL_GPIO_PIN(A3,   A,  4)
HAL_GPIO_PIN(A4,   A,  5)
HAL_GPIO_PIN(A5,   A,  6)
HAL_GPIO_PIN(A6,   B,  2) // Battery voltage
HAL_GPIO_PIN(A7,   B,  3)
HAL_GPIO_PIN(A8,   A,  8) // STEMMA 4
HAL_GPIO_PIN(A9,   A,  9)
HAL_GPIO_PIN(A10,  A, 10)
HAL_GPIO_PIN(A11,  A, 11) // STEMMA 3

// Digital pins
HAL_GPIO_PIN(D0,   A,  9) // also RX below
HAL_GPIO_PIN(D1,   A, 10) // also TX below
HAL_GPIO_PIN(D3,   A, 11) // STEMMA 3
HAL_GPIO_PIN(D4,   A,  8) // STEMMA 4
HAL_GPIO_PIN(D5,   A, 15)
HAL_GPIO_PIN(D6,   A, 18)
HAL_GPIO_PIN(D7,   A, 21) // ???
HAL_GPIO_PIN(D8,   A, 12) // Neopixel
HAL_GPIO_PIN(D9,   A, 19)
HAL_GPIO_PIN(D10,  A, 20)
HAL_GPIO_PIN(D11,  A, 21)
HAL_GPIO_PIN(D12,  A, 22)
HAL_GPIO_PIN(D13,  A, 23) // also LED
HAL_GPIO_PIN(LED,  A, 23)

// SPI
HAL_GPIO_PIN(SCK,  B, 23)
HAL_GPIO_PIN(MOSI, B, 22)
HAL_GPIO_PIN(MISO, B,  3)
#define SPI_SERCOM 5
#define SPI_SERCOM_DOPO SPI_DOPO_2_SCK_3
#define SPI_SERCOM_DIPO SPI_DIPO_1

// I2C
HAL_GPIO_PIN(SDA,  A, 16)
HAL_GPIO_PIN(SCL,  A, 17)
#define I2C_SERCOM 1

// UART
HAL_GPIO_PIN(TX,   A, 10)
HAL_GPIO_PIN(RX,   A, 9)
#define UART_SERCOM 0
#define UART_SERCOM_TXPO UART_TXPO_2
#define UART_SERCOM_RXPO UART_RXPO_1

// TFT
HAL_GPIO_PIN(TFT_BACKLIGHT, A, 0)
HAL_GPIO_PIN(TFT_CS,        A, 1)
HAL_GPIO_PIN(TFT_DC,        A, 28)
HAL_GPIO_PIN(TFT_RESET,     A, 27)

// Accelerometer
HAL_GPIO_PIN(ACCEL_INT, A, 14)

// Flash chip
HAL_GPIO_PIN(FLASH_SCK,  B, 11)
HAL_GPIO_PIN(FLASH_MOSI, B, 10)
HAL_GPIO_PIN(FLASH_MISO, A, 13)
HAL_GPIO_PIN(FLASH_CS,   A,  7)

// Neopixel
HAL_GPIO_PIN(NEOPIXEL,   A, 12)
