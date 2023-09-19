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
#include "sam.h"

rtc_cb_t _rtc_callback = NULL;

#if defined(_SAMD21_) || defined(_SAMD11_)
#define CTRLREG (RTC->MODE2.CTRL)
#else
#define CTRLREG (RTC->MODE2.CTRLA)
#endif

bool rtc_is_enabled(void) {
    return CTRLREG.bit.ENABLE;
}

static void _rtc_sync(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (RTC->MODE2.STATUS.bit.SYNCBUSY);
#else
    while (RTC->MODE2.SYNCBUSY.reg & RTC_MODE2_SYNCBUSY_MASK_);
#endif
}

void rtc_init(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the RTC
    PM->APBAMASK.reg |= PM_APBAMASK_RTC;
    // clock RTC with GCLK3 (prescaled 1024 Hz output from the external crystal)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_GEN(3) | GCLK_CLKCTRL_ID(RTC_GCLK_ID) | GCLK_CLKCTRL_CLKEN;
#else
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_RTC;
#endif

    if (rtc_is_enabled()) return; // don't reset the RTC if it's already set up.

    _rtc_sync();
    CTRLREG.bit.SWRST = 1;
    _rtc_sync();

    CTRLREG.bit.MODE = 2; // Mode 2 Clock
    CTRLREG.bit.PRESCALER = RTC_MODE2_CTRLA_PRESCALER_DIV1024_Val; // 1024 Hz input / 1024 = 1 Hz
}

void rtc_enable(void) {
    if (rtc_is_enabled()) return;
    CTRLREG.bit.ENABLE = 1;
    _rtc_sync();
}

void rtc_set_date_time(rtc_date_time date_time) {
    RTC->MODE2.CLOCK.reg = date_time.reg;
    _rtc_sync();
}

rtc_date_time rtc_get_date_time(void) {
    rtc_date_time retval;

    _rtc_sync();
    retval.reg = RTC->MODE2.CLOCK.reg;

    return retval;
}

void rtc_enable_alarm_interrupt(rtc_date_time alarm_time, rtc_alarm_match mask) {
    RTC->MODE2.Mode2Alarm[0].ALARM.reg = alarm_time.reg;
    RTC->MODE2.Mode2Alarm[0].MASK.reg = mask;
    _rtc_sync();
    RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_ALARM0;
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_EnableIRQ(RTC_IRQn);
}

void rtc_configure_callback(rtc_cb_t callback) {
    _rtc_callback = callback;
}

void rtc_disable_alarm_interrupt(void) {
    RTC->MODE2.INTENCLR.reg = RTC_MODE2_INTENCLR_ALARM0;
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_DisableIRQ(RTC_IRQn);
}

void irq_handler_rtc(void);

void irq_handler_rtc(void) {
    if (RTC->MODE2.INTFLAG.bit.ALARM0) {
        RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0;
        if (_rtc_callback != NULL) {
            // TODO: Periodic events and tamper pins on supported devices
            _rtc_callback(0);
        }
    }
}
