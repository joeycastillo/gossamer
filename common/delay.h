////< @file delay.h
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma once

/**
 * @brief delay Delay routines
 * @brief Functions for delaying execution using the SYSTICK peripheral.
 * @{
 */ 

/**
 * @brief Initializes the system tick peripheral. This is required to use the delay functions.
 * @details The system tick only runs while the CPU is running, and does not fire in STANDBY mode.
 */
void delay_init(void);

/**
 * @brief Delays for the given number of milliseconds.
 * @param ms The number of milliseconds to delay.
 */
void delay_ms(const uint16_t ms);

/**
 * @brief Delays for the given number of microseconds.
 * @param us The number of microseconds to delay.
 */
void delay_us(const uint16_t us);
