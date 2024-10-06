/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
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

#include <stddef.h>
#include "rtc.h"

rtc_cb_t _rtc_callback = NULL;

#if defined(_SAMD21_) || defined(_SAMD11_)
#define CTRLREG (RTC->MODE2.CTRL)
#else
#define CTRLREG (RTC->MODE2.CTRLA)
#endif

bool rtc_is_enabled(void) {
    return true;
}

static void _rtc_sync(void) {
}

void rtc_init(void) {
}

void rtc_enable(void) {
}

void rtc_set_date_time(rtc_date_time_t date_time) {
}

rtc_date_time_t rtc_get_date_time(void) {
    rtc_date_time_t date_time = {0};
    return date_time;
}

void rtc_enable_alarm_interrupt(rtc_date_time_t alarm_time, rtc_alarm_match_t mask) {
}

void rtc_configure_callback(rtc_cb_t callback) {
}

void rtc_disable_alarm_interrupt(void) {
}

void irq_handler_rtc(void);

void irq_handler_rtc(void) {
}
