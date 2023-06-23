#pragma once

#include "hal_gpio.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"

// Analog pins
HAL_GPIO_PIN(A0,   A,  2)
HAL_GPIO_PIN(A1,   A,  5)
HAL_GPIO_PIN(A2,   B,  8)
HAL_GPIO_PIN(A3,   A,  9)
HAL_GPIO_PIN(A4,   A,  4)
HAL_GPIO_PIN(A5,   A,  6)
HAL_GPIO_PIN(A6,   B,  1) // Battery voltage
HAL_GPIO_PIN(A7,   B,  4) // Light sensor
HAL_GPIO_PIN(A8,   B,  3) // STEMMA 2
HAL_GPIO_PIN(A9,   B,  2) // STEMMA 3

// Digital pins
HAL_GPIO_PIN(D0,   B, 17) // also RX below
HAL_GPIO_PIN(D1,   B, 16) // also TX below
HAL_GPIO_PIN(D2,   B,  3) // STEMMA 2
HAL_GPIO_PIN(D3,   B,  2) // STEMMA 3
HAL_GPIO_PIN(D4,   A, 14)
HAL_GPIO_PIN(D5,   A, 16)
HAL_GPIO_PIN(D6,   A, 18)
HAL_GPIO_PIN(D7,   B, 14) // Accelerometer interrupt
HAL_GPIO_PIN(D8,   A, 15) // Neopixel
HAL_GPIO_PIN(D9,   A, 19)
HAL_GPIO_PIN(D10,  A, 20)
HAL_GPIO_PIN(D11,  A, 21)
HAL_GPIO_PIN(D12,  A, 22)
HAL_GPIO_PIN(D13,  A, 23) // also LED
HAL_GPIO_PIN(LED,  A, 23)

// SPI
HAL_GPIO_PIN(SCK,  A, 17)
HAL_GPIO_PIN(MOSI, B, 23)
HAL_GPIO_PIN(MISO, B, 22)

// I2C
HAL_GPIO_PIN(SDA,  A, 12)
HAL_GPIO_PIN(SCL,  A, 13)
#define I2C_SERCOM 2

// UART
HAL_GPIO_PIN(TX,   B, 16)
HAL_GPIO_PIN(RX,   B, 17)
#define UART_SERCOM 5
#define UART_SERCOM_TXPO UART_TXPO_0
#define UART_SERCOM_RXPO UART_RXPO_1

// TFT
HAL_GPIO_PIN(TFT_BACKLIGHT, A, 0)
HAL_GPIO_PIN(TFT_CS,        A, 1)
HAL_GPIO_PIN(TFT_DC,        A, 28)
HAL_GPIO_PIN(TFT_RESET,     A, 27)

// Accelerometer
HAL_GPIO_PIN(ACCEL_INT, A, 14)

// Neopixel
HAL_GPIO_PIN(NEOPIXEL, A, 15)
