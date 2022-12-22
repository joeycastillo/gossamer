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
#include "eic.h"
#include "sam.h"

#if defined(_SAMD21_) || defined(_SAMD11_)
#define CTRLREG (EIC->CTRL)
#else
#define CTRLREG (EIC->CTRLA)
#endif

eic_cb_t _eic_callback = NULL;

static void _eic_sync(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (EIC->STATUS.bit.SYNCBUSY);
#else
    while (EIC->SYNCBUSY.reg);
#endif
}

void eic_init(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the EIC
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    // Configure EIC to use GCLK2 (the 32.768 kHz low power oscillator)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(EIC_GCLK_ID) | GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN(2);
#else
    // enable the EIC
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
#endif

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    CTRLREG.bit.SWRST = 1;
    _eic_sync();

#if defined(_SAML21_) || defined(_SAML22_)
    // Configure EIC to use the 32.768 kHz low power oscillator directly.
    EIC->CTRLA.bit.CKSEL = 1;
#endif
    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
}

void eic_configure_channel(const uint8_t channel, eic_interrupt_trigger trigger) {
    uint8_t config_index = (channel > 7) ? 1 : 0;
    uint8_t sense_pos = 4 * (channel % 8);

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    uint32_t config = EIC->CONFIG[config_index].reg;
    config &= ~(7 << sense_pos);
    config |= trigger << (sense_pos);
    EIC->CONFIG[config_index].reg = config;
    EIC->INTENSET.reg = 1 << channel;
#if defined(_SAMD21_) || defined(_SAMD11_)
    uint32_t wakeup = EIC->WAKEUP.reg;
    EIC->WAKEUP.reg = wakeup | (1 << channel);
#endif

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();
}

void eic_configure_callback(eic_cb_t callback) {
    _eic_callback = callback;
}

void irq_handler_eic(void);
void irq_handler_eic(void) {
    if (_eic_callback != NULL) {
        _eic_callback();
    }
    EIC->INTFLAG.reg = EIC_INTFLAG_MASK;
}
