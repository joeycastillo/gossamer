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

#include "pins.h"
#include "system.h"
#include "tc.h"

void tc_sync(uint8_t instance) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT8.STATUS.bit.SYNCBUSY);
#else
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT8.SYNCBUSY.reg);
#endif
}

bool tc_setup(uint8_t instance, uint8_t clocksource) {
    if ((instance - TC_First_Index) >= Num_TC_Instances) return false;

#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the TC
    PM->APBCMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    // Configure TC to use the selected clock souece
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC_Peripherals[instance - TC_First_Index].gclk_id) | 
                        GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(clocksource);
#else
    // enable the TC
    MCLK->APBAMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    GCLK->PCHCTRL[TC_Peripherals[instance - TC_First_Index].gclk_id].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(clocksource);
#endif

    // disable and reset the TC
    tc_disable(instance);
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.CTRLA.bit.SWRST = 1;
    tc_sync(instance);

    return true;
}

void tc_enable(uint8_t instance) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->APBAMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
#else
    MCLK->APBCMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
#endif
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.CTRLA.bit.ENABLE = 1;
    tc_sync(instance);
}

void tc_disable(uint8_t instance) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.CTRLA.bit.ENABLE = 0;
    tc_sync(instance);
#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->APBAMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
#else
    MCLK->APBCMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
#endif
}
