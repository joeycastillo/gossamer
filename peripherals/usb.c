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

void usb_setup(void) {
    // disable USB, just in case.
    USB->DEVICE.CTRLA.bit.ENABLE = 0;
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE);

#if defined(_SAMD21_) || defined(_SAMD11_)
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    // set the coarse and fine values to speed up frequency lock.
    uint32_t coarse = ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
    uint32_t fine = ((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;

    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    // Configure the DFLL to multiply the 1 kHz clock to 48 MHz
    SYSCTRL->DFLLMUL.reg = SYSCTRL_DFLLMUL_MUL(48000) | SYSCTRL_DFLLMUL_FSTEP(1) | SYSCTRL_DFLLMUL_CSTEP(1);

    // Set closed loop mode
    SYSCTRL->DFLLCTRL.bit.MODE = 1;

    // Enable USB clock recovery mode and disable chill cycle
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_USBCRM | SYSCTRL_DFLLCTRL_CCDIS;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    // Enable the DFLL
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    // assign DFLL to GCLK1
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(1) | GCLK_GENCTRL_SRC_DFLL48M | GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    while(GCLK->STATUS.bit.SYNCBUSY) {};

    // Enable USB clocks...
    PM->AHBMASK.reg |= PM_AHBMASK_USB;
    PM->APBBMASK.reg |= PM_APBBMASK_USB;

    // ...and assign GCLK1 to USB
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(USB_GCLK_ID) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(1);
#else
    // reset flags and disable DFLL
    OSCCTRL->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRDY;
    OSCCTRL->DFLLCTRL.reg = 0;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY));

    // set the coarse and fine values to speed up frequency lock.
    uint32_t coarse =(*((uint32_t *)NVMCTRL_OTP5)) >> 26;
    OSCCTRL->DFLLVAL.reg = OSCCTRL_DFLLVAL_COARSE(coarse) |
                           OSCCTRL_DFLLVAL_FINE(0x200);
    // set coarse and fine steps, and multiplier (48 MHz = 32768 Hz * 1465)
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP( 1 ) |
                           OSCCTRL_DFLLMUL_FSTEP( 1 ) |
                           OSCCTRL_DFLLMUL_MUL( 1465 );
    // set closed loop mode, chill cycle disable and USB clock recovery mode, and enable the DFLL.
    OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_MODE | OSCCTRL_DFLLCTRL_CCDIS | OSCCTRL_DFLLCTRL_ONDEMAND | OSCCTRL_DFLLCTRL_RUNSTDBY | OSCCTRL_DFLLCTRL_USBCRM | OSCCTRL_DFLLCTRL_ENABLE;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY));

    // assign DFLL to GCLK1
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL48M) | GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_GENEN;// | GCLK_GENCTRL_OE;
    #if defined(_SAML21_)
    while (GCLK->SYNCBUSY.bit.GENCTRL1); // wait for generator control 1 to sync
    #else
    while (GCLK->SYNCBUSY.bit.GENCTRL); // wait for generator control 1 to sync
    #endif

    // assign GCLK1 to USB
    GCLK->PCHCTRL[USB_GCLK_ID].reg = GCLK_PCHCTRL_GEN(1) | GCLK_PCHCTRL_CHEN;
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_USB;
    MCLK->APBBMASK.reg |= MCLK_APBBMASK_USB;
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

void usb_disable(void) {
    USB->DEVICE.CTRLA.bit.ENABLE = 0;
    while (USB->DEVICE.SYNCBUSY.bit.ENABLE);
}
#endif