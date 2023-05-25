/*
 * Copyright (c) 2015, Alex Taradov <alex@taradov.com>
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
#include "samd21.h"
#include "system.h"

TCC_Instance_Details TCC_Peripherals[3] = {
    {TCC0, PM_APBCMASK_TCC0, GCLK_CLKCTRL_ID_TCC0_TCC1},
    {TCC1, PM_APBCMASK_TCC1, GCLK_CLKCTRL_ID_TCC0_TCC1},
    {TCC2, PM_APBCMASK_TCC2, GCLK_CLKCTRL_ID_TCC2_TC3},
};
uint8_t Num_TCC_Instances = 3;

TC_Instance_Details TC_Peripherals[4] = {
    {TC3, PM_APBCMASK_TC3, GCLK_CLKCTRL_ID_TCC2_TC3},
    {TC4, PM_APBCMASK_TC4, GCLK_CLKCTRL_ID_TC4_TC5},
    {TC5, PM_APBCMASK_TC5, GCLK_CLKCTRL_ID_TC4_TC5},
#if defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__) || \
    defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__) || \
    defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__) || \
    defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
    {TC6, PM_APBCMASK_TC6, GCLK_CLKCTRL_ID_TC6_TC7},
    {TC7, PM_APBCMASK_TC7, GCLK_CLKCTRL_ID_TC6_TC7},
#endif
};

uint8_t TC_First_Index = 3;

#if defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__) || \
    defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__) || \
    defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__) || \
    defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
uint8_t Num_TC_Instances = 5;
#else
uint8_t Num_TC_Instances = 3;
#endif

//-----------------------------------------------------------------------------
void sys_init(void) {
    // Switch GCLK0 to 8MHz clock (disable prescaler)
    SYSCTRL->OSC8M.bit.PRESC = 0;
    // make sure main oscillator stays off in standby
    SYSCTRL->OSC8M.bit.RUNSTDBY = 0;

    // GCLK1 is reserved fot the DFLL, for USB type stuff.

    // GCLK2 is the low-accuracy 32kHz oscillator
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(2) | GCLK_GENDIV_DIV(1);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(2) | GCLK_GENCTRL_SRC_OSCULP32K |
                        GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    while(GCLK->STATUS.bit.SYNCBUSY);

    // GCLK3 is a more accurate 1024 Hz clock
#ifdef CRYSTALLESS
    // if no crystal, set up internal 32k oscillator
    if (!SYSCTRL->OSC32K.bit.ENABLE) {
        uint32_t calib = (*((uint32_t *) FUSES_OSC32K_CAL_ADDR) & FUSES_OSC32K_CAL_Msk);
        SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_STARTUP(0x6) | SYSCTRL_OSC32K_CALIB(calib) | 
                            SYSCTRL_XOSC32K_RUNSTDBY | SYSCTRL_OSC32K_ONDEMAND |
                            SYSCTRL_OSC32K_EN32K;
        SYSCTRL->OSC32K.bit.ENABLE = 1;
        while(!SYSCTRL->PCLKSR.bit.OSC32KRDY);

        // connect external crystal to GCLK3, divide by 32 for a 1.024 kHz clock
        GCLK->GENDIV.reg = GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(32);
        GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(3) | GCLK_GENCTRL_SRC_OSC32K |
                            GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
        while(GCLK->STATUS.bit.SYNCBUSY);
    }
#else
    // otherwise, set up external 32k crystal oscillator
    if (!SYSCTRL->XOSC32K.bit.ENABLE) {
        SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP(0x7) | SYSCTRL_XOSC32K_EN32K |
                               SYSCTRL_XOSC32K_XTALEN | SYSCTRL_XOSC32K_RUNSTDBY;
        SYSCTRL->XOSC32K.bit.ENABLE = 1;
        while(!SYSCTRL->PCLKSR.bit.XOSC32KRDY);

        // connect external crystal to GCLK3, divide by 32 for a 1.024 kHz clock
        GCLK->GENDIV.reg = GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(32);
        GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(3) | GCLK_GENCTRL_SRC_XOSC32K |
                            GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
        while(GCLK->STATUS.bit.SYNCBUSY);
    }
#endif
}

uint32_t get_cpu_frequency(void) {
    switch (SYSCTRL->OSC8M.bit.PRESC) {
    case 3:
        return 1000000;
    case 2:
        return 2000000;
    case 1:
        return 4000000;
    case 0:
        return 8000000;
    default:
        return 0;
    }
}

bool set_cpu_frequency(uint32_t freq) {
    switch (freq) {
    case 1000000:
        SYSCTRL->OSC8M.bit.PRESC = 3;
        break;
    case 2000000:
        SYSCTRL->OSC8M.bit.PRESC = 2;
        break;
    case 4000000:
        SYSCTRL->OSC8M.bit.PRESC = 1;
        break;
    case 8000000:
        SYSCTRL->OSC8M.bit.PRESC = 0;
        break;
    default:
        return false;
    }
    return true;
}

void _enter_standby_mode() {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}
