/*
 * Copyright (c) 2015, Alex Taradov <alex@taradov.com>
 * Copyright (c) 2023, Joey Castillo
 * with an assist from Thea Flowers:
 *      https://blog.thea.codes/understanding-the-sam-d21-clocks/
 *
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

const tc_instance_details_t TC_Peripherals[] = {
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

const uint8_t Num_TC_Instances = TC_INST_NUM;
const uint8_t TC_First_Index = 3;

const tcc_instance_details_t TCC_Peripherals[] = {
    {TCC0, PM_APBCMASK_TCC0, GCLK_CLKCTRL_ID_TCC0_TCC1},
    {TCC1, PM_APBCMASK_TCC1, GCLK_CLKCTRL_ID_TCC0_TCC1},
    {TCC2, PM_APBCMASK_TCC2, GCLK_CLKCTRL_ID_TCC2_TC3},
};
const uint8_t Num_TCC_Instances = TCC_INST_NUM;

const sercom_instance_details_t SERCOM_Peripherals[] = {
    {SERCOM0, PM_APBCMASK_SERCOM0, SERCOM0_GCLK_ID_CORE, SERCOM0_IRQn},
    {SERCOM1, PM_APBCMASK_SERCOM1, SERCOM1_GCLK_ID_CORE, SERCOM1_IRQn},
    {SERCOM2, PM_APBCMASK_SERCOM2, SERCOM2_GCLK_ID_CORE, SERCOM2_IRQn},
    {SERCOM3, PM_APBCMASK_SERCOM3, SERCOM3_GCLK_ID_CORE, SERCOM3_IRQn},
#if defined(__SAMD21G15A__) || defined(__ATSAMD21G15A__) || \
    defined(__SAMD21G16A__) || defined(__ATSAMD21G16A__) || \
    defined(__SAMD21G17A__) || defined(__ATSAMD21G17A__) || \
    defined(__SAMD21G17AU__) || defined(__ATSAMD21G17AU__) || \
    defined(__SAMD21G18A__) || defined(__ATSAMD21G18A__) || \
    defined(__SAMD21G18AU__) || defined(__ATSAMD21G18AU__) || \
    defined(__SAMD21J15A__) || defined(__ATSAMD21J15A__) || \
    defined(__SAMD21J16A__) || defined(__ATSAMD21J16A__) || \
    defined(__SAMD21J17A__) || defined(__ATSAMD21J17A__) || \
    defined(__SAMD21J18A__) || defined(__ATSAMD21J18A__)
    {SERCOM4, PM_APBCMASK_SERCOM4, SERCOM4_GCLK_ID_CORE, SERCOM4_IRQn},
    {SERCOM5, PM_APBCMASK_SERCOM5, SERCOM5_GCLK_ID_CORE, SERCOM5_IRQn},
#endif
};
const uint8_t Num_SERCOM_Instances = SERCOM_INST_NUM;

static uint32_t _cpu_frequency;

//-----------------------------------------------------------------------------
void sys_init(void) {
    // Switch GCLK0 to 8MHz clock (disable prescaler)
    SYSCTRL->OSC8M.bit.PRESC = 0;
    // stash the current CPU frequency
    _cpu_frequency = 8000000;
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
    uint32_t calib = (*((uint32_t *) FUSES_OSC32K_CAL_ADDR) & FUSES_OSC32K_CAL_Msk);
    SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_CALIB(calib) |
                          SYSCTRL_OSC32K_STARTUP(0x3) |
                          SYSCTRL_OSC32K_EN32K |
                          SYSCTRL_XOSC32K_RUNSTDBY;
    SYSCTRL->OSC32K.bit.ENABLE = 1;
    while(!SYSCTRL->PCLKSR.bit.OSC32KRDY);

    // connect internal OSC32K to GCLK3, divide by 32 for a 1.024 kHz clock
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(3) | GCLK_GENDIV_DIV(32);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(3) | GCLK_GENCTRL_SRC_OSC32K |
                        GCLK_GENCTRL_IDC | GCLK_GENCTRL_GENEN;
    while(GCLK->STATUS.bit.SYNCBUSY);
#else
    // otherwise, set up external 32k crystal oscillator
    SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP(0x7)|
                           SYSCTRL_XOSC32K_EN32K |
                           SYSCTRL_XOSC32K_XTALEN |
                           SYSCTRL_XOSC32K_RUNSTDBY;
    SYSCTRL->XOSC32K.bit.ENABLE = 1;
    while(!SYSCTRL->PCLKSR.bit.XOSC32KRDY);

    // connect external crystal to GCLK3, divide by 32 for a 1.024 kHz clock
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(3) |
                       GCLK_GENDIV_DIV(32);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(3) |
                        GCLK_GENCTRL_SRC_XOSC32K |
                        GCLK_GENCTRL_IDC |
                        GCLK_GENCTRL_GENEN;
    while(GCLK->STATUS.bit.SYNCBUSY);
#endif
}

void _enable_48mhz_gclk1(void) {
    // reset flags and disable DFLL
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    // set the coarse and fine values to speed up frequency lock.
    uint32_t coarse = ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
    uint32_t fine = ((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;
    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);

    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};

    // set USB clock recovery mode, closed loop mode and chill cycle disable.
    // USB clock recovery mode recovers a 1 kHz clock from the USB start of frame packets.
    SYSCTRL->DFLLCTRL.reg |= SYSCTRL_DFLLCTRL_USBCRM |
                             SYSCTRL_DFLLCTRL_MODE |
                             SYSCTRL_DFLLCTRL_CCDIS;

    // set multiplier (48 MHz = 1000 Hz * 48000)
    SYSCTRL->DFLLMUL.reg =
        SYSCTRL_DFLLMUL_FSTEP(1) |
        SYSCTRL_DFLLMUL_CSTEP(1) |
        SYSCTRL_DFLLMUL_MUL(48000);

    // Wait for the write to complete
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY);

    // enable the DFLL
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;

    // assign DFLL to GCLK1
    GCLK->GENCTRL.reg =
        GCLK_GENCTRL_ID(1) |
        GCLK_GENCTRL_SRC_DFLL48M |
        GCLK_GENCTRL_IDC |
        GCLK_GENCTRL_GENEN;

    // wait for GCLK to sync
    while(GCLK->STATUS.bit.SYNCBUSY);
}

uint32_t get_cpu_frequency(void) {
    return _cpu_frequency;
}

bool set_cpu_frequency(uint32_t freq) {
    GCLK->GENCTRL.bit.ID = 0;
    while(GCLK->STATUS.bit.SYNCBUSY);

    if (GCLK->GENCTRL.bit.SRC == GCLK_GENCTRL_SRC_DFLL48M_Val) {
        GCLK->GENCTRL.bit.SRC = GCLK_GENCTRL_SRC_OSC8M_Val;
        while(GCLK->STATUS.bit.SYNCBUSY);
    }

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

    _cpu_frequency = freq;

    return true;
}

void _enter_standby_mode() {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}
