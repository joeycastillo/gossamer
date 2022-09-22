#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma once

void systick_init(void);

void delay_ms(const uint16_t ms);
