/*
 * FreeTouch, a QTouch-compatible library - tested on ATSAMD21 only!
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Limor 'ladyada' Fried for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sam.h"

#ifdef _SAMD21_
#include "component/samd21_ptc_component.h"
#define HAS_PTC 1
#endif

#ifdef _SAMD11_
#include "component/samd11_ptc_component.h"
#define HAS_PTC 1
#endif

#ifdef HAS_PTC

/* Touch library oversampling (filter) setting */
typedef enum tag_oversample_level_t {
  OVERSAMPLE_1,
  OVERSAMPLE_2,
  OVERSAMPLE_4,
  OVERSAMPLE_8,
  OVERSAMPLE_16,
  OVERSAMPLE_32,
  OVERSAMPLE_64
} ptc_oversample_t;

/* Touch library series resistor setting */
typedef enum tag_series_resistor_t {
  RESISTOR_0,
  RESISTOR_20K,
  RESISTOR_50K,
  RESISTOR_100K,
} ptc_series_resistor_t;

typedef enum tag_freq_mode_t {
  FREQ_MODE_NONE,
  FREQ_MODE_HOP,
  FREQ_MODE_SPREAD,
  FREQ_MODE_SPREAD_MEDIAN
} ptc_freq_mode_t;

typedef enum tag_freq_hop_t {
  FREQ_HOP_1,
  FREQ_HOP_2,
  FREQ_HOP_3,
  FREQ_HOP_4,
  FREQ_HOP_5,
  FREQ_HOP_6,
  FREQ_HOP_7,
  FREQ_HOP_8,
  FREQ_HOP_9,
  FREQ_HOP_10,
  FREQ_HOP_11,
  FREQ_HOP_12,
  FREQ_HOP_13,
  FREQ_HOP_14,
  FREQ_HOP_15,
  FREQ_HOP_16
} ptc_freq_hop_t;

/**
 * @brief Initialize the peripheral touch controller.
 */
void ptc_init(void);

/**
 * @brief Enable the given PTC channel.
 * @param channel The PTC channel (NOT pin number) to enable.
 * @note You are still responsible for configuring the pin mux.
 */
void ptc_enable_channel(uint8_t channel);

/**
 * @brief Get a value from the given channel.
 * @param channel The PTC channel (NOT pin number) to read.
 * @return The touch value read from the channel.
 */
uint16_t ptc_get_value(uint8_t channel);

#endif
