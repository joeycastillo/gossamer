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
#include "hal_gpio.h"

#if defined(_SAMD21_) || defined(_SAMD11_)
#define CTRLREG (EIC->CTRL)
#else
#define CTRLREG (EIC->CTRLA)
#endif

#if defined(_SAMD11_)
static const int8_t _eic_pin_to_channel[1][32] = {
    {-1, -1, 2, 3, 4, 5, 6, 7, 6, 7, 2, 3, -1, -1, -1, 1, 0, 1, -1, -1, -1, -1, 6, 7, 4, 5, -1, 7, -1, -1, 2, 3},
};
#else
#ifdef _SAMD51_
/// TODO: Do mapping for SAM D51
#warning "External Interrupt Controller not yet implemented for SAM D51"
#endif
static const int8_t _eic_pin_to_channel[3][32] = {
    {0, 1, 2, 3, 4, 5, 6, 7, -1, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7, 12, 13, -1, 15, 8, -1, 10, 11},
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, 14, 15},
    {8, 9, 10, 11, -1, 13, 14, 15, -1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1, -1, 1, 2, 3, 4, -1, -1, -1}
};
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
    // enable the EIC's peripheral clock
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    // Configure EIC to use GCLK2 (the 32.768 kHz low power oscillator)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(EIC_GCLK_ID) |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN(2);
#else
    // enable the EIC's peripheral clock
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
}

void eic_enable(void) {
    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

#if defined(_SAMD51_)
    /// TODO: SAM D51 has interrupt lines for each channel
#else
    NVIC_ClearPendingIRQ(EIC_IRQn);
    NVIC_EnableIRQ(EIC_IRQn);
#endif
}

int8_t eic_configure_pin(const uint8_t pin, eic_interrupt_trigger_t trigger, bool filten) {
    uint16_t port = pin >> 5;
    int8_t channel = _eic_pin_to_channel[port][pin & 0x1F];
    if (channel < 0) {
        return -1;
    }
    // Channels 0-7 occupy CONFIG[0] and channels 8-15 occupy CONFIG[1].
    uint8_t config_index = (channel > 7) ? 1 : 0;
    // SENSEx occupies the lower three bits of each four-bit configuration slot, and FILTENx the fourth bit.
    uint8_t channel_pos = 4 * (channel % 8);

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    /// FIXME: Should we have macros for this that accept a port / pin?
    PORT->Group[port].DIRCLR.reg = (1 << (pin & 0x1F));
    PORT->Group[port].PINCFG[pin & 0x1F].reg |= PORT_PINCFG_INEN;
    PORT->Group[port].PINCFG[pin & 0x1F].reg |= PORT_PINCFG_PMUXEN;
    if (pin & 1) PORT->Group[port].PMUX[(pin & 0x1F) >> 1].bit.PMUXO = HAL_GPIO_PMUX_EIC;
    else PORT->Group[port].PMUX[(pin & 0x1F) >> 1].bit.PMUXE = HAL_GPIO_PMUX_EIC;

    // configure the interrupt, which consists of setting SENSEx with the trigger, and FILTENx to enable or disable filtering
    uint32_t config = EIC->CONFIG[config_index].reg;
    // clear all four bits of the selected configuration slot.
    config &= ~(0xF << channel_pos);
    // set the trigger
    config |= trigger << channel_pos;
    // enable the filter if needed
    if (filten) config |= 0x8 << channel_pos;
    EIC->CONFIG[config_index].reg = config;

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    return channel;
}

bool eic_enable_interrupt(const uint8_t pin) {
    int8_t channel = _eic_pin_to_channel[pin >> 5][pin & 0x1F];
    if (channel < 0) {
        return false;
    }

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    EIC->INTENSET.reg = 1 << channel;
#if defined(_SAMD21_) || defined(_SAMD11_)
    uint32_t wakeup = EIC->WAKEUP.reg;
    EIC->WAKEUP.reg = wakeup | (1 << channel);
#endif

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    return true;
}

bool eic_disable_interrupt(const uint8_t pin) {
    int8_t channel = _eic_pin_to_channel[pin >> 5][pin & 0x1F];
    if (channel < 0) {
        return false;
    }

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    EIC->INTENCLR.reg = 1 << channel;
#if defined(_SAMD21_) || defined(_SAMD11_)
    uint32_t wakeup = EIC->WAKEUP.reg;
    EIC->WAKEUP.reg = wakeup & ~(1 << channel);
#endif

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    return true;
}

bool eic_enable_event(const uint8_t pin) {
    int8_t channel = _eic_pin_to_channel[pin >> 5][pin & 0x1F];
    if (channel < 0) {
        return false;
    }

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    EIC_EVCTRL_Type evctrl;
    evctrl.reg = EIC->EVCTRL.reg | EIC_EVCTRL_EXTINTEO(1 << channel);
    EIC->EVCTRL.reg = evctrl.reg;

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    return true;
}

bool eic_disable_event(const uint8_t pin) {
    int8_t channel = _eic_pin_to_channel[pin >> 5][pin & 0x1F];
    if (channel < 0) {
        return false;
    }

    CTRLREG.bit.ENABLE = 0;
    _eic_sync();

    EIC_EVCTRL_Type evctrl;
    evctrl.reg = EIC->EVCTRL.reg & ~EIC_EVCTRL_EXTINTEO(1 << channel);
    EIC->EVCTRL.reg = evctrl.reg;

    CTRLREG.bit.ENABLE = 1;
    _eic_sync();

    return true;
}

void eic_disable(void) {
    CTRLREG.bit.ENABLE = 0;
#if defined(_SAMD51_)
    /// TODO: SAM D51 has interrupt lines for each channel
#else
    NVIC_DisableIRQ(EIC_IRQn);
    NVIC_ClearPendingIRQ(EIC_IRQn);
#endif
}

void eic_configure_callback(eic_cb_t callback) {
    _eic_callback = callback;
}

void irq_handler_eic(void);
void irq_handler_eic(void) {
    uint8_t channel = __builtin_ctz(EIC->INTFLAG.reg);
    if (_eic_callback != NULL) {
        _eic_callback(channel);
    }
    EIC->INTFLAG.reg = EIC_INTFLAG_MASK;
}
