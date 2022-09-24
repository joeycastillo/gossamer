#pragma once

#include "hal_gpio.h"


// Analog pins
HAL_GPIO_PIN(AREF, A, 3)
HAL_GPIO_PIN(A0,   A, 2)
HAL_GPIO_PIN(A1,   A, 4)
HAL_GPIO_PIN(A2,   A, 6)
HAL_GPIO_PIN(A3,   A, 7)

// Digital pins
HAL_GPIO_PIN(LED, A,  5)

// SPI
HAL_GPIO_PIN(SCK,  A, 16)
HAL_GPIO_PIN(MOSI, A, 22)
HAL_GPIO_PIN(MISO, A, 23)

// I2C
HAL_GPIO_PIN(SDA,  A, 14)
HAL_GPIO_PIN(SCL,  A, 15)

// UART
HAL_GPIO_PIN(TX, A, 8)
HAL_GPIO_PIN(RX, A, 9)
