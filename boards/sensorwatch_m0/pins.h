#pragma once

#include "hal_gpio.h"


// Analog pins
HAL_GPIO_PIN(A0,   B, 4)
HAL_GPIO_PIN(A1,   B, 1)
HAL_GPIO_PIN(A2,   B, 2)
HAL_GPIO_PIN(A3,   B, 3)
HAL_GPIO_PIN(A4,   B, 0)

// Light
HAL_GPIO_PIN(LED1,  A, 20)
HAL_GPIO_PIN(LED2,  A, 21)
HAL_GPIO_PIN(LED,   A, 20)

// Sound
HAL_GPIO_PIN(BUZZER,  A, 27)

// Buttons
HAL_GPIO_PIN(BTN_LIGHT, A, 22)
HAL_GPIO_PIN(BTN_MODE,  A, 22)
HAL_GPIO_PIN(BTN_ALARM, A,  2)

// SPI
HAL_GPIO_PIN(SCK,  B, 1)
HAL_GPIO_PIN(MOSI, B, 2)
HAL_GPIO_PIN(CS,   B, 3)
HAL_GPIO_PIN(MISO, B, 0)

// I2C
HAL_GPIO_PIN(SDA,  B, 30)
HAL_GPIO_PIN(SCL,  B, 31)