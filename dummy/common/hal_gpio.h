/** 
 * @file hal_gpio.h
 * @brief GPIO Macros
 */
/*
 * Copyright (c) 2014-2016, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdint.h>

#pragma once

/*- Definitions -------------------------------------------------------------*/
#define HAL_GPIO_PORTA       0
#define HAL_GPIO_PORTB       1
#define HAL_GPIO_PORTC       2

#define HAL_GPIO_PMUX_A      0
#define HAL_GPIO_PMUX_B      1
#define HAL_GPIO_PMUX_C      2
#define HAL_GPIO_PMUX_D      3
#define HAL_GPIO_PMUX_E      4
#define HAL_GPIO_PMUX_F      5
#define HAL_GPIO_PMUX_G      6
#define HAL_GPIO_PMUX_H      7
#define HAL_GPIO_PMUX_I      8
#define HAL_GPIO_PMUX_J      9
#define HAL_GPIO_PMUX_K      10
#define HAL_GPIO_PMUX_L      11
#define HAL_GPIO_PMUX_M      12
#define HAL_GPIO_PMUX_N      13

#define HAL_GPIO_PIN(name, port, pin)            \
  static inline void HAL_GPIO_##name##_set(void)        \
  {                    \
    (void)HAL_GPIO_##name##_set;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_clr(void)        \
  {                    \
    (void)HAL_GPIO_##name##_clr;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_toggle(void)        \
  {                    \
    (void)HAL_GPIO_##name##_toggle;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_write(int value)        \
  {                    \
    (void)HAL_GPIO_##name##_write;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_drvstr(int value)        \
  {                    \
    (void)HAL_GPIO_##name##_write;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_in(void)          \
  {                    \
    (void)HAL_GPIO_##name##_in;              \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_out(void)        \
  {                    \
    (void)HAL_GPIO_##name##_out;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_off(void)        \
  {                    \
    (void)HAL_GPIO_##name##_off;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_pullup(void)        \
  {                    \
    (void)HAL_GPIO_##name##_pullup;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_pulldown(void)        \
  {                    \
    (void)HAL_GPIO_##name##_pulldown;            \
  }                    \
                    \
  static inline int HAL_GPIO_##name##_read(void)        \
  {                    \
    return 0; \
    (void)HAL_GPIO_##name##_read;            \
  }                    \
                    \
  static inline int HAL_GPIO_##name##_state(void)        \
  {                    \
    return 0; \
    (void)HAL_GPIO_##name##_state;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_pmuxen(int mux)        \
  {                    \
    (void)HAL_GPIO_##name##_pmuxen;            \
  }                    \
                    \
  static inline void HAL_GPIO_##name##_pmuxdis(void)        \
  {                    \
    (void)HAL_GPIO_##name##_pmuxdis;            \
  }                    \
                    \
  static inline uint8_t HAL_GPIO_##name##_pin(void)        \
  {                    \
    return 0; \
    (void)HAL_GPIO_##name##_pin;            \
  }                    \
                    \

HAL_GPIO_PIN(SWCLK, A, 30)
HAL_GPIO_PIN(SWDIO, A, 31)

#ifdef APP_USES_TINYUSB
HAL_GPIO_PIN(USB_N, A, 24)
HAL_GPIO_PIN(USB_P, A, 25)
#endif
