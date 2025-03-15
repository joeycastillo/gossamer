#pragma once

#include "hal_gpio.h"
#include "spi.h"
#include "i2c.h"
#include "uart.h"

// Analog input
HAL_GPIO_PIN(TEMPSENSE,  A, 4)
HAL_GPIO_PIN(LIGHTSENSE, A, 5)

// RGB LED
HAL_GPIO_PIN(GREEN, A, 14)
HAL_GPIO_PIN(RED,   A, 15)
HAL_GPIO_PIN(BLUE,  A, 9)

// Alias for blink sketch
HAL_GPIO_PIN(LED,   A, 15)