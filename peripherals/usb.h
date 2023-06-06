/* 
 * MIT License
 *
 * Copyright (c) 2023 Joey Castillo
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

// WORK IN PROGRESS, not yet functional.
#ifdef APP_USES_TINYUSB

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "tusb.h"

/** @brief Enables the peripheral clock for USB and clocks it with GCLK0. Also
  *         sets up the USB pins. For USB to work, the CPU speed must be 48 MHz.
  * @warning You MUST call `cpu_set_frequency(48000000)` before calling this
  *          function. The USB peripheral depends on a 48 MHz clock on GCLK0.
  */
void usb_init(void);

/**
 * @brief Initializes the TinyUSB stack and enables the USB peripheral.
 * @details In order to make use of the tinyUSB stack, there are two additional
 *          setup steps you must take:
 *          1. In your Makefile, set the type and number of TinyUSB device
 *             classes you intend to use. For example, if you want to use a USB
 *             mass storage class, you would add `TINYUSB_MSC=1` to your app's
 *             Makefile. You can also add more than one class, e.g. for two CDC
 *             interfaces, you would add `TINYUSB_CDC=2`. Available TinyUSB
 *             classes are:
 *              * TINYUSB_CDC
 *              * TINYUSB_MSC
 *              * TINYUSB_HID
 *              * TINYUSB_MIDI
 *              * TINYUSB_VENDOR
 *          2. You must also set up your USB device descriptor. This is
 *             typically done in a `usb_descriptors.c` file. See the usb app
 *             for an example of how to do this.
 *          3. Depending on which classes you enable, you may need to do some
 *             additional setup. Check out tinyusb's examples for more info.
 */
void usb_enable(void);

/**
 * @brief Disables the USB peripheral.
 * @note Has not been extensively tested, TinyUSB may not like this.
 */
void usb_disable(void);

#endif
