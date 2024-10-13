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

const tc_instance_details_t TC_Peripherals[] = {
    {TC0, MCLK_APBCMASK_TC0, TC0_GCLK_ID},
    {TC1, MCLK_APBCMASK_TC1, TC1_GCLK_ID},
#if defined(__SAML21J15B__) || defined(__ATSAML21J15B__) || \
    defined(__SAML21J16B__) || defined(__ATSAML21J16B__) || \
    defined(__SAML21J17B__) || defined(__ATSAML21J17B__) || \
    defined(__SAML21J18B__) || defined(__ATSAML21J18B__)
    {TC2, MCLK_APBCMASK_TC2, TC2_GCLK_ID},
    {TC3, MCLK_APBCMASK_TC3, TC3_GCLK_ID},
#else
// NOTE: TC2 and TC3 are not available on the SAML21G or SAML21E.
// tc_init will recognize this if you try to use them, and return false.
    {NULL, 0, 0},
    {NULL, 0, 0},
#endif
    {TC4, MCLK_APBDMASK_TC4, TC4_GCLK_ID},
};
// we can't use TCC_INST_NUM here because the numbering is wacky on the L21.
const uint8_t Num_TC_Instances = 5;
const uint8_t TC_First_Index = 0;

const tcc_instance_details_t TCC_Peripherals[] = {
    {TCC0, MCLK_APBCMASK_TCC0, TCC0_GCLK_ID},
    {TCC1, MCLK_APBCMASK_TCC1, TCC1_GCLK_ID},
    {TCC2, MCLK_APBCMASK_TCC2, TCC2_GCLK_ID},
};
const uint8_t Num_TCC_Instances = TCC_INST_NUM;

const sercom_instance_details_t SERCOM_Peripherals[] = {
    {SERCOM0, MCLK_APBCMASK_SERCOM0, SERCOM0_GCLK_ID_CORE, SERCOM0_IRQn},
    {SERCOM1, MCLK_APBCMASK_SERCOM1, SERCOM1_GCLK_ID_CORE, SERCOM1_IRQn},
    {SERCOM2, MCLK_APBCMASK_SERCOM2, SERCOM2_GCLK_ID_CORE, SERCOM2_IRQn},
    {SERCOM3, MCLK_APBCMASK_SERCOM3, SERCOM3_GCLK_ID_CORE, SERCOM3_IRQn},
    {SERCOM4, MCLK_APBCMASK_SERCOM4, SERCOM4_GCLK_ID_CORE, SERCOM4_IRQn},
    {SERCOM5, MCLK_APBDMASK_SERCOM5, SERCOM5_GCLK_ID_CORE, SERCOM5_IRQn},
};
const uint8_t Num_SERCOM_Instances = SERCOM_INST_NUM;

static uint32_t _cpu_frequency;

//-----------------------------------------------------------------------------
void sys_init(void) {
    // Switch to 8MHz clock
    OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_FSEL_8;
    // stash the current CPU frequency
    _cpu_frequency = 8000000;

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
    OSC32KCTRL->XOSC32K.reg = OSC32KCTRL_XOSC32K_STARTUP(0x3) |
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

void _enable_48mhz_gclk1(void) {
    // reset flags and disable DFLL
    OSCCTRL->INTFLAG.reg = OSCCTRL_INTFLAG_DFLLRDY;
    OSCCTRL->DFLLCTRL.reg = 0;
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY));

    // set the coarse and fine values to speed up frequency lock.
    uint32_t coarse =(*((uint32_t *)NVMCTRL_OTP5)) >> 26;
    OSCCTRL->DFLLVAL.reg = OSCCTRL_DFLLVAL_COARSE(coarse) |
                           OSCCTRL_DFLLVAL_FINE(0x200);

    // set USB clock recovery mode, closed loop mode and chill cycle disable.
    // USB clock recovery mode recovers a 1 kHz clock from the USB start of frame packets.
    OSCCTRL->DFLLCTRL.reg = OSCCTRL_DFLLCTRL_USBCRM |
                            OSCCTRL_DFLLCTRL_MODE |
                            OSCCTRL_DFLLCTRL_CCDIS;

    // set coarse and fine steps, and multiplier (48 MHz = 1000 Hz * 48000)
    OSCCTRL->DFLLMUL.reg = OSCCTRL_DFLLMUL_CSTEP(1) |
                           OSCCTRL_DFLLMUL_FSTEP(1) |
                           OSCCTRL_DFLLMUL_MUL(48000);

    // Wait for the write to complete
    while (!(OSCCTRL->STATUS.reg & OSCCTRL_STATUS_DFLLRDY));

    // enable the DFLL
    OSCCTRL->DFLLCTRL.bit.ENABLE = 1;

    // assign DFLL to GCLK1
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_SRC_DFLL48M |
                           GCLK_GENCTRL_DIV(1) |
                           GCLK_GENCTRL_IDC |
                           GCLK_GENCTRL_GENEN;

    // wait for generator control 1 to sync
    while (GCLK->SYNCBUSY.bit.GENCTRL1);
}

uint32_t get_cpu_frequency(void) {
    return _cpu_frequency;
}

bool set_cpu_frequency(uint32_t freq) {
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

    _cpu_frequency = freq;

    return true;
}

void _enter_standby_mode() {
    PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_STANDBY_Val;
    while (PM->SLEEPCFG.bit.SLEEPMODE != PM_SLEEPCFG_SLEEPMODE_STANDBY_Val);

    __DSB();
    __WFI();
}
