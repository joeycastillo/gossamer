#pragma once

#include "hal_gpio.h"

// Detects if we are on USB power.
HAL_GPIO_PIN(VBUS_DET, A, 3)

// Analog pins
HAL_GPIO_PIN(A0,   B, 4)
HAL_GPIO_PIN(A1,   B, 1)
HAL_GPIO_PIN(A2,   B, 2)
HAL_GPIO_PIN(A3,   B, 3)
HAL_GPIO_PIN(A4,   B, 0)

// SPI
HAL_GPIO_PIN(MISO, B, 0)
HAL_GPIO_PIN(SCK,  B, 1)
HAL_GPIO_PIN(MOSI, B, 2)
HAL_GPIO_PIN(CS,   B, 3)
#define SPI_SERCOM 3
#define SPI_SERCOM_DOPO SPI_DOPO_0_SCK_3
#define SPI_SERCOM_DIPO SPI_DIPO_2

// I2C
HAL_GPIO_PIN(SDA,  B, 30)
HAL_GPIO_PIN(SCL,  B, 31)
#define I2C_SERCOM 1

// Temperature sensor
HAL_GPIO_PIN(TS_ENABLE, B, 23)
HAL_GPIO_PIN(TEMPSENSE, A, 3)

// IR Sensor
HAL_GPIO_PIN(IR_ENABLE, B, 22)
HAL_GPIO_PIN(IRSENSE, A, 4)

// Light
HAL_GPIO_PIN(RED,   A, 12)
HAL_GPIO_PIN(GREEN, A, 22)
HAL_GPIO_PIN(BLUE,  A, 13)
HAL_GPIO_PIN(LED,   A, 12)  // Default to red LED for blinky tests
#define WATCH_INVERT_LED_POLARITY
#define WATCH_RED_TCC_CHANNEL 2
#define WATCH_GREEN_TCC_CHANNEL 0
#define WATCH_BLUE_TCC_CHANNEL 3

// Sound
HAL_GPIO_PIN(BUZZER,  A, 27)
#define WATCH_BUZZER_TCC_CHANNEL 1

// Buttons
HAL_GPIO_PIN(BTN_LIGHT, A, 30)
HAL_GPIO_PIN(BTN_MODE,  A, 31)
HAL_GPIO_PIN(BTN_ALARM, A,  2)

// Segment LCD
HAL_GPIO_PIN(SLCD0,  B,  6)
HAL_GPIO_PIN(SLCD1,  B,  7)
HAL_GPIO_PIN(SLCD2,  B,  8)
HAL_GPIO_PIN(SLCD3,  B,  9)
HAL_GPIO_PIN(SLCD4,  A,  5)
HAL_GPIO_PIN(SLCD5,  A,  6)
HAL_GPIO_PIN(SLCD6,  A,  7)
HAL_GPIO_PIN(SLCD7,  A,  8)
HAL_GPIO_PIN(SLCD8,  A,  9)
HAL_GPIO_PIN(SLCD9,  A, 10)
HAL_GPIO_PIN(SLCD10, A, 11)
HAL_GPIO_PIN(SLCD11, B, 11)
HAL_GPIO_PIN(SLCD12, B, 12)
HAL_GPIO_PIN(SLCD13, B, 13)
HAL_GPIO_PIN(SLCD14, B, 14)
HAL_GPIO_PIN(SLCD15, B, 15)
HAL_GPIO_PIN(SLCD16, A, 14)
HAL_GPIO_PIN(SLCD17, A, 15)
HAL_GPIO_PIN(SLCD18, A, 16)
HAL_GPIO_PIN(SLCD19, A, 17)
HAL_GPIO_PIN(SLCD20, A, 18)
HAL_GPIO_PIN(SLCD21, A, 19)
HAL_GPIO_PIN(SLCD22, B, 16)
HAL_GPIO_PIN(SLCD23, B, 17)
HAL_GPIO_PIN(SLCD24, A, 20)
HAL_GPIO_PIN(SLCD25, A, 21)
HAL_GPIO_PIN(SLCD26, A, 23)

#define LCD_PIN_ENABLE (\
    (uint64_t)1 <<  0 | \
    (uint64_t)1 <<  1 | \
    (uint64_t)1 <<  2 | \
    (uint64_t)1 <<  3 | \
    (uint64_t)1 <<  5 | \
    (uint64_t)1 <<  6 | \
    (uint64_t)1 <<  7 | \
    (uint64_t)1 << 11 | \
    (uint64_t)1 << 12 | \
    (uint64_t)1 << 13 | \
    (uint64_t)1 << 14 | \
    (uint64_t)1 << 21 | \
    (uint64_t)1 << 22 | \
    (uint64_t)1 << 23 | \
    (uint64_t)1 << 24 | \
    (uint64_t)1 << 25 | \
    (uint64_t)1 << 30 | \
    (uint64_t)1 << 31 | \
    (uint64_t)1 << 32 | \
    (uint64_t)1 << 33 | \
    (uint64_t)1 << 34 | \
    (uint64_t)1 << 35 | \
    (uint64_t)1 << 42 | \
    (uint64_t)1 << 43 | \
    (uint64_t)1 << 48 | \
    (uint64_t)1 << 49 | \
    (uint64_t)1 << 51 | 0)

