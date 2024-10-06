/** 
 * @file app.h
 * @brief Gossamer application framework
 */
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

#include "pins.h"

/** @brief A function you will implement to initialize your application's low-
  *        level setup. The app_init function is called after system clocks are
  *        initialized, and before anything else.
  * @details You may be wondering why there is an `app_init` function as well as
  *          an `app_setup` function. The main difference is that app_init is
  *          only meant to be callec once, at boot, whereas you might want to
  *          call app_setup multiple times, for example if you want to disable
  *          some peripherals before entering STANDBY mode, and re-enable them
  *          after waking up. In essence, you should set up anything that will
  *          run forever in app_init, whereas anything that could get turned off
  *          in the course of execution should be set up in app_setup.
  * @note If your application plans to use the real-time clock perhiperal, you
  *       probably want to call the `rtc_init` function here.
  */
void app_init(void);

/** @brief OPTIONAL: A function you may implement to restore state after waking
  *        up from BACKUP mode on the SAM L21 or L22. This function is called
  *        after system_init, but before app_setup. You can stash a bit of your
  *        application state in the RTC's BKUP and GPn registers, and use this
  *        function as an opportinity to restore it before app_setup is called.
  * @note You do not need to implement this function unless you are using the
  *       SAM L21 or L22 and you want to restore state from BACKUP mode.
  */
void app_wake_from_backup(void);

/** @brief A function you will implement to set up your app. The app_setup
  *         function is like setup() in Arduino. It is called once when the
  *         program begins, after app_init and app_wake_from_backup, if it
  *         exists. You should use this function to set up your application
  *         state and configure any peripherals you need to use.
  */
void app_setup(void);

/** @brief A function you will implement to serve as the app's main run loop.
 *         This method will be called repeatedly, or if you enter STANDBY mode,
 *         as soon as the device wakes from that mode.
  * @note In order to wake from STANDBY mode, you must set up an interrupt or
  *       wake source. Otherwise your app will remain in STANDBY indefinitely.
  * @return true if your app is prepared to enter STANDBY mode. If you return
  *         false, your app's app_loop method will be called again immediately.
  */
bool app_loop(void);

/** @brief OPTIONAL: A function to yield control of the CPU to other tasks.
 *         This function is called in the delay function to allow other tasks
 *         to run while the CPU is busy waiting. By default, this function does
 *         nothing, but if you are using the USB peripheral, you should
 *         implement this function and call `tud_task` at minumum, along with
 *         any other USB-related tasks you need to run.
*/
void yield(void);
