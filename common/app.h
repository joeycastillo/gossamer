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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "sam.h"
#include "pins.h"

////< @file app.h

/// @{
/** @brief A function you will implement to initialize your application state. The app_init function is called after
  *        system clocks are initialized, and before anything else. Use it to set up any internal data structures or
  *        state required by your app, and set up any low-level parameters like CPU speed or microcontroller register
  *        configuration. If your application plans to use the real-time clock, you should call `rtc_init` here.
  */
void app_init(void);

/** @brief A function you will implement to set up your application. The app_setup function is like setup() in Arduino.
  *        It is called once when the program begins. You should set pin modes and enable any peripherals you want to
  *        set up (real-time clock, I2C, etc.) Depending on your application, you may or may not want to configure
  *        sensors here. For example, a low-power accelerometer that will run at all times should be configured here,
  *        whereas you may want to enable a more power-hungry sensor only when you need it.
  */
void app_setup(void);

/** @brief A function you will implement to serve as the app's main run loop. This method will be called repeatedly,
           or if you enter STANDBY mode, as soon as the device wakes from sleep.
  * @return You should return true if your app is prepared to enter STANDBY mode. If you return false, your app's
  *         app_loop method will be called again immediately.
  */
bool app_loop(void);

/// @}
