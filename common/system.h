/*
 * MIT License
 *
 * Copyright (c) 2020 Joey Castillo
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sam.h"

#pragma once

typedef struct {
    Tc* tc;
    uint32_t clock_enable_mask;
    uint8_t gclk_id;
} TC_Instance_Details;

typedef struct {
    Tcc* tcc;
    uint32_t clock_enable_mask;
    uint8_t gclk_id;
} TCC_Instance_Details;

extern TC_Instance_Details TC_Peripherals[];
extern uint8_t Num_TC_Instances;

extern TCC_Instance_Details TCC_Peripherals[];
extern uint8_t Num_TCC_Instances;

////< @file system.h

/// @{
/** @brief Initializes the system clocks and performs any required system-wide setup.
  * @details Gossamer aims for consistency at system startup, no matter which
  *          chip you are working with. To that end:
  *           * GCLK0, the main system clock, is set to 8 MHz.
  *           * GCLK2 is the low-power 32768 Hz clock, OSCULP32K
  *           * GCLK3 is a 1024 Hz clock, derived from the most accurate source available.
  *          GCLK0 is set to ONDEMAND with RUNSTDBY off, so the main clock remains
  *          off when in standby mode. GCLK2 and GCLK3 are also ONDEMAND, but RUNSTDBY
  *          is on. This ensures that if a peripheral requests it, the clock will remain
  *          on in standby, but if no peripheral requests it, it will not run at all.
  */
void sys_init(void);

/** @brief Gets the CPU frequency.
  * @return 8000000 by default, or another value if set by `set_cpu_frequency`
  */
uint32_t get_cpu_frequency(void);

/** @brief Sets the CPU frequency.
  * @param freq one of 1000000, 2000000, 4000000, 8000000, 12000000 or 16000000.
  * @details Supported CPU frequencies vary from chip to chip:
  *           * On the SAM D11 and D21, you can choose 8, 4, 2 or 1 MHz.
  *           * On the SAM L21 and L22, you can choose 16, 12, 8 or 4 MHz.
  *          While you can run these chips at higher frequencies using the DFLL,
  *          Gossamer aims for simplicity: this means that it only supports the
  *          internal oscillator at one of its standard frequencies.
  * @note Some peripherals like SERCOMs and TC/TCCs read the CPU frequency when
  *       they are configured, to set critical timing-related parameters like baud
  *       rate or period. If you change the CPU frequency after setting up these
  *       peripherals and then attempt to use them, they will very likely behave in
  *       unexpected ways. Thus you should disable any peripherals that depend on
  *       GCLK0 before calling set_cpu_frequency, and then re-enable them after.
  */
bool set_cpu_frequency(uint32_t freq);

/** @brief Enters the low-power STANDBY mode. Does not return until an interrupt fires.
  * @details You should not generally need to call this function; at the end of every
  *          app_loop invocation, the main run loop enters standby mode if your app
  *          indicated that it was appropriate.
  */
void _enter_standby_mode(void);

/// @}
