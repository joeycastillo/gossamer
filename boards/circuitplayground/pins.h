#pragma once

#include "hal_gpio.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"

// Analog pins
HAL_GPIO_PIN(A0,            A,  2)
HAL_GPIO_PIN(A1,            A,  5)
HAL_GPIO_PIN(A2,            A,  6)
HAL_GPIO_PIN(A3,            A,  7)
HAL_GPIO_PIN(A4,            B,  3)
HAL_GPIO_PIN(A5,            B,  2)
HAL_GPIO_PIN(A6,            B,  9)
HAL_GPIO_PIN(A7,            B,  8)
HAL_GPIO_PIN(TEMP_SENSE,    A,  9)
HAL_GPIO_PIN(LIGHT_SENSE,   A, 11)

// Digital pins
HAL_GPIO_PIN(LED,           A, 17)
HAL_GPIO_PIN(BUTTON_A,      A, 28)
HAL_GPIO_PIN(BUTTON_B,      A, 14)
HAL_GPIO_PIN(SWITCH,        A, 15)
HAL_GPIO_PIN(IR_REMOTE_RX,  A, 12)
HAL_GPIO_PIN(IR_REMOTE_TX,  A, 23)
HAL_GPIO_PIN(IR_ANALOG,     A,  4)
HAL_GPIO_PIN(ACCEL_INT,     A, 27)
HAL_GPIO_PIN(SPEAKER_SHDN,  A, 30)

// Neopixel
HAL_GPIO_PIN(NEOPIXEL,      B,  23)

// Flash chip
HAL_GPIO_PIN(FLASH_SCK,     A, 21)
HAL_GPIO_PIN(FLASH_MOSI,    A, 20)
HAL_GPIO_PIN(FLASH_MISO,    A, 16)
HAL_GPIO_PIN(FLASH_CS,      B, 22)

// I2C
HAL_GPIO_PIN(SDA,           A,  0)
HAL_GPIO_PIN(SCL,           A,  1)
#define I2C_SERCOM 1

// UART
HAL_GPIO_PIN(RX,   B,  9)
HAL_GPIO_PIN(TX,   B,  8)
#define UART_SERCOM 4
#define UART_SERCOM_TXPO UART_TXPO_0
#define UART_SERCOM_RXPO UART_RXPO_1

// TFT Gizmo
HAL_GPIO_PIN(TFT_SCK,       B,  3)
HAL_GPIO_PIN(TFT_MOSI,      B,  2)
HAL_GPIO_PIN(TFT_BACKLIGHT, A,  7)
HAL_GPIO_PIN(TFT_CS,        B,  9)
HAL_GPIO_PIN(TFT_DC,        B,  8)
