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
#include "usb.h"
#include "tc.h"

#ifdef APP_USES_TINYUSB

void usb_init(void) {
    _enable_48mhz_gclk1();
#if defined(_SAMD21_) || defined(_SAMD11_)
    // Enable USB clocks...
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;

    // ...and assign GCLK1 to USB
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(USB_GCLK_ID) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(1);
#else
    // Enable USB clocks...
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;

    // ...and assign GCLK1 to USB
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;
    while (GCLK->PCHCTRL[USB_GCLK_ID].bit.CHEN == 0);
#endif

    // USB Pin Init
    HAL_GPIO_USB_N_out();
    HAL_GPIO_USB_P_out();
    HAL_GPIO_USB_N_clr();
    HAL_GPIO_USB_P_clr();
    HAL_GPIO_USB_N_pmuxen(HAL_GPIO_PMUX_COM);
    HAL_GPIO_USB_P_pmuxen(HAL_GPIO_PMUX_COM);
}

void usb_enable(void) {
    tusb_init();
}

bool usb_is_enabled(void) {
    return USB->DEVICE.CTRLA.bit.ENABLE;
}

void usb_disable(void) {
    USB->DEVICE.CTRLA.bit.ENABLE = 0;
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE);
}

void irq_handler_usb(void);
void irq_handler_usb(void) {
    tud_int_handler(0);
}

#endif
