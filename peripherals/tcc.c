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
#include "tcc.h"

void tcc_sync(uint8_t instance) {
    while (TCC_Peripherals[instance].tcc->SYNCBUSY.reg);
}

bool tcc_setup(uint8_t instance, uint8_t clocksource) {
    if (instance >= Num_TCC_Instances) return false;

#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the TCC
    PM->APBCMASK.reg |= TCC_Peripherals[instance].clock_enable_mask;
    // Configure TCC to use the selected clock source
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TCC_Peripherals[instance].gclk_id) | 
                        GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(clocksource);
#else
    // enable the TCC
    MCLK->APBCMASK.reg |= TCC_Peripherals[instance].clock_enable_mask;
    GCLK->PCHCTRL[TCC_Peripherals[instance].gclk_id].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(clocksource);
#endif

    // disable and reset the TCC
    tcc_disable(instance);
    TCC_Peripherals[instance].tcc->CTRLA.bit.SWRST = 1;
    tcc_sync(instance);

    return true;
}

void tcc_enable(uint8_t instance) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->APBCMASK.reg &= ~(TCC_Peripherals[instance].clock_enable_mask);
#else
    MCLK->APBCMASK.reg &= ~(TCC_Peripherals[instance].clock_enable_mask);
#endif
    TCC_Peripherals[instance].tcc->CTRLA.bit.ENABLE = 1;
    tcc_sync(instance);
}

void tcc_disable(uint8_t instance) {
    TCC_Peripherals[instance].tcc->CTRLA.bit.ENABLE = 0;
    tcc_sync(instance);
#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->APBCMASK.reg &= ~(TCC_Peripherals[instance].clock_enable_mask);
#else
    MCLK->APBCMASK.reg &= ~(TCC_Peripherals[instance].clock_enable_mask);
#endif
}
