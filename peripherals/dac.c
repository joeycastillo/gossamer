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
#include "sam.h"
#include "system.h"
#include "dac.h"

#ifdef DAC

static void _dac_sync() {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (DAC->STATUS.bit.SYNCBUSY);
#else // SAM L21
    while (DAC->SYNCBUSY.reg);
#endif
}

void dac_enable(uint16_t channel) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    (void) channel; // these chips have only one DAC

    // enable the DAC
    PM->APBCMASK.reg |= PM_APBCMASK_DAC;
    // Configure DAC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(DAC_GCLK_ID) | GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN(0);
#else // SAM L21
    // enable the DAC
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_DAC;
    // Configure DAC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->PCHCTRL[DAC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
#endif
    if (DAC->CTRLA.bit.ENABLE) {
        DAC->CTRLA.bit.ENABLE = 0;
        _dac_sync();
    }
    
    // TODO: move this to a dac_init function, this will break the use of dual DAC on SAM L21
    DAC->CTRLA.bit.SWRST = 1;
    _dac_sync();

#if defined(_SAMD21_) || defined(_SAMD11_)
    DAC->CTRLB.bit.REFSEL = DAC_CTRLB_REFSEL_AVCC_Val;
    DAC->CTRLB.bit.EOEN = 1;
#else
    DAC->CTRLB.bit.REFSEL = DAC_CTRLB_REFSEL_VDDANA_Val;
    DAC->DACCTRL[channel].bit.ENABLE = 1;
    DAC->DACCTRL[channel].bit.CCTRL = DAC_DACCTRL_CCTRL_CC1M_Val;
#endif
    DAC->CTRLA.bit.ENABLE = 1;
    _dac_sync();
}

void dac_set_analog_value(uint16_t channel, uint16_t value) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    (void) channel; // these chips have only one DAC

    DAC->DATA.reg = value;
#else // SAM L21
    DAC->DATA[channel].reg = value;
#endif
    _dac_sync();
}

void dac_disable(uint16_t channel) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    (void) channel; // these chips have only one DAC

    DAC->CTRLA.bit.ENABLE = 0;
    _dac_sync();

    PM->APBAMASK.reg &= ~PM_APBCMASK_DAC;
#else // SAM L21
    // TODO: deal with dual DAC case on L21
    DAC->CTRLA.bit.ENABLE = 0;
    _dac_sync();
    DAC->DACCTRL[channel].bit.ENABLE = 0;
    _dac_sync();

    MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_DAC;
#endif
}

#endif
