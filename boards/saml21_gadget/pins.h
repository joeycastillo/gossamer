#pragma once

#include "hal_gpio.h"


// Analog pins
HAL_GPIO_PIN(A0,   A, 2)
HAL_GPIO_PIN(A1,   A, 5)
HAL_GPIO_PIN(A2,   A, 6)
HAL_GPIO_PIN(A3,   A, 7)

// LED (for testing, should be PA27)
HAL_GPIO_PIN(LED,  A, 14)

// SPI
HAL_GPIO_PIN(SCK,  A, 11)
HAL_GPIO_PIN(MOSI, A,  4)
HAL_GPIO_PIN(MISO, A, 10)

// I2C
HAL_GPIO_PIN(SDA,  A, 8)
HAL_GPIO_PIN(SCL,  A, 9)

// UART
HAL_GPIO_PIN(TX,   A, 14)
HAL_GPIO_PIN(RX,   A, 15)
