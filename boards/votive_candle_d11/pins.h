#pragma once

#include "hal_gpio.h"
#include "sam.h"

// DAC
HAL_GPIO_PIN(BUZZER, A, 2)

// Buttons
HAL_GPIO_PIN(BUTTON2, A, 3)
HAL_GPIO_PIN(BUTTON1, A, 16)

// Default LED
HAL_GPIO_PIN(LED, A, 4)

// Red/Green LED
HAL_GPIO_PIN(GREEN1, A, 4)
HAL_GPIO_PIN(RED1,   A, 5)
HAL_GPIO_PIN(GREEN2, A, 30)
HAL_GPIO_PIN(RED2,   A, 31)
HAL_GPIO_PIN(GREEN3, A, 22)
HAL_GPIO_PIN(RED3,   A, 23)

// White LED
HAL_GPIO_PIN(WARM_WHITE, A, 14)
HAL_GPIO_PIN(COOL_WHITE, A, 15)

// Sense pins
HAL_GPIO_PIN(SENSE1, A, 6)
HAL_GPIO_PIN(SENSE2, A, 7)
