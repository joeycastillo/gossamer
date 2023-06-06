/*
 * Copyright (c) 2014-2016, Alex Taradov <alex@taradov.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

//-----------------------------------------------------------------------------
#include "saml21.h"
#include "system.h"
#include "pins.h"

// NOTE: all chip variants also have a TC4 peripheral, but it's more complicated
// to set it up since its clock enable is on APBD and not APBC. FOR NOW, just use
// TC0-TC1 for L21E and L21G, and TC0-TC3 for L21J.
TC_Instance_Details TC_Peripherals[] = {
    {TC0, MCLK_APBCMASK_TC0, TC0_GCLK_ID},
    {TC1, MCLK_APBCMASK_TC1, TC1_GCLK_ID},
#if defined(__SAML21J15B__) || defined(__ATSAML21J15B__) || \
    defined(__SAML21J16B__) || defined(__ATSAML21J16B__) || \
    defined(__SAML21J17B__) || defined(__ATSAML21J17B__) || \
    defined(__SAML21J18B__) || defined(__ATSAML21J18B__)
    {TC2, MCLK_APBCMASK_TC2, TC2_GCLK_ID},
    {TC3, MCLK_APBCMASK_TC3, TC3_GCLK_ID},
#endif
};
#if defined(__SAML21J15B__) || defined(__ATSAML21J15B__) || \
    defined(__SAML21J16B__) || defined(__ATSAML21J16B__) || \
    defined(__SAML21J17B__) || defined(__ATSAML21J17B__) || \
    defined(__SAML21J18B__) || defined(__ATSAML21J18B__)
uint8_t Num_TC_Instances = 2;
#else
uint8_t Num_TC_Instances = 4;
#endif
uint8_t TC_First_Index = 0;

TCC_Instance_Details TCC_Peripherals[] = {
    {TCC0, MCLK_APBCMASK_TCC0, TCC0_GCLK_ID},
    {TCC1, MCLK_APBCMASK_TCC1, TCC1_GCLK_ID},
    {TCC2, MCLK_APBCMASK_TCC2, TCC2_GCLK_ID},
};
uint8_t Num_TCC_Instances = 3;

//-----------------------------------------------------------------------------
void sys_init(void) {
    // Switch to 8MHz clock
    OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_FSEL_8;

    // Switch to the highest performance level
    PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
    PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2_Val;
    while (!PM->INTFLAG.reg);

    // set up low power 32k oscillator on GCLK2
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(1) |
                           GCLK_GENCTRL_SRC_OSCULP32K |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;

#ifdef CRYSTALLESS
    uint32_t *calibrationArea = (uint32_t*) NVMCTRL_OTP5;
    uint32_t calib = ((*calibrationArea) & 0x1FC0) >> 6;
    OSC32KCTRL->OSC32K.reg = OSC32KCTRL_OSC32K_CALIB(calib) |
                             OSC32KCTRL_OSC32K_EN1K |
                             OSC32KCTRL_OSC32K_EN32K |
                             OSC32KCTRL_OSC32K_RUNSTDBY |
                             OSC32KCTRL_OSC32K_ONDEMAND |
                             OSC32KCTRL_OSC32K_ENABLE;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_OSC1K;
    // set up high accuracy 1k oscillator on GCLK3
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(32) |
                           GCLK_GENCTRL_SRC_OSC32K |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;
#else
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_STARTUP(0x6) |
                              OSC32KCTRL_XOSC32K_XTALEN |
                              OSC32KCTRL_XOSC32K_EN1K |
                              OSC32KCTRL_XOSC32K_EN32K |
                              OSC32KCTRL_XOSC32K_RUNSTDBY |
                              OSC32KCTRL_XOSC32K_ONDEMAND |
                              OSC32KCTRL_XOSC32K_ENABLE;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_XOSC1K;
    // set up high accuracy 1k oscillator on GCLK3
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(32) |
                           GCLK_GENCTRL_SRC_XOSC32K |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;
#endif
}

static void enable_48mhz_clock(void) {
    NVMCTRL->CTRLB.bit.RWS = 1;

#if defined(CRYSTALLESS)
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

#else
    /* Setup GCLK1 using the external 32.768 kHz oscillator */
    GCLK->GENCTRL[1].reg =
        GCLK_GENCTRL_SRC_XOSC32K |
        GCLK_GENCTRL_DIV(1) |
        /* Improve the duty cycle. */
        GCLK_GENCTRL_IDC |
        GCLK_GENCTRL_GENEN;

    /* Wait for the write to complete */
    while(GCLK->SYNCBUSY.bit.GENCTRL1);

    // Configure CLK_DFLL48M_REF to use GCLK1 as its source
    GCLK->PCHCTRL[0].reg = GCLK_PCHCTRL_GEN_GCLK1 | GCLK_PCHCTRL_CHEN;

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
    OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_ONDEMAND | OSCCTRL_DFLLCTRL_RUNSTDBY | OSCCTRL_DFLLCTRL_USBCRM | OSCCTRL_DFLLCTRL_ENABLE;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY));
#endif

    // assign DFLL to GCLK0
    GCLK->GENCTRL[0].reg = GCLK_GENCTRL_SRC(GCLK_GENCTRL_SRC_DFLL48M) | GCLK_GENCTRL_DIV(1) | GCLK_GENCTRL_GENEN;
    while (GCLK->SYNCBUSY.bit.GENCTRL0); // wait for generator control 1 to sync
}

uint32_t get_cpu_frequency(void) {
    // if GCLK0's source is the DFLL, we're running at 48 MHz
    if (GCLK->GENCTRL[0].bit.SRC == GCLK_GENCTRL_SRC_DFLL48M_Val) {
        return 48000000;
    }

    // otherwise, we're running at 16 MHz divided by the prescaler
    switch (OSCCTRL->OSC16MCTRL.bit.FSEL) {
    case 0:
        return 4000000;
    case 1:
        return 8000000;
    case 2:
        return 12000000;
    case 3:
        return 16000000;
    default:
        return 0;
    }
}

bool set_cpu_frequency(uint32_t freq) {
    if (freq == 48000000) {
        enable_48mhz_clock();
        return true;
    }

    if (GCLK->GENCTRL[0].bit.SRC == GCLK_GENCTRL_SRC_DFLL48M_Val) {
        GCLK->GENCTRL[0].bit.SRC = GCLK_GENCTRL_SRC_OSC16M_Val;
        while(GCLK->SYNCBUSY.bit.GENCTRL0);
    }

    switch (freq) {
    case 4000000:
        OSCCTRL->OSC16MCTRL.bit.FSEL = 0;
        break;
    case 8000000:
        OSCCTRL->OSC16MCTRL.bit.FSEL = 1;
        break;
    case 12000000:
        OSCCTRL->OSC16MCTRL.bit.FSEL = 2;
        break;
    case 16000000:
        OSCCTRL->OSC16MCTRL.bit.FSEL = 3;
        break;
    default:
        return false;
    }
    return true;
}

void _enter_standby_mode() {
    PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_STANDBY_Val;
    __WFI();
}
