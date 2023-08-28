/*
 * Copyright (c) 2016, Alex Taradov <alex@taradov.com>
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
#include "samd11.h"
#include "system.h"

const TC_Instance_Details TC_Peripherals[] = {
    {TC1, PM_APBCMASK_TC1, GCLK_CLKCTRL_ID_TC1_TC2},
    {TC2, PM_APBCMASK_TC2, GCLK_CLKCTRL_ID_TC1_TC2},
};
const uint8_t Num_TC_Instances = TC_INST_NUM;
const uint8_t TC_First_Index = 1;

const TCC_Instance_Details TCC_Peripherals[] = {
    {TCC0, PM_APBCMASK_TCC0, GCLK_CLKCTRL_ID_TCC0},
};
const uint8_t Num_TCC_Instances = TCC_INST_NUM;

const SERCOM_Instance_Details SERCOM_Peripherals[] = {
    {SERCOM0, PM_APBCMASK_SERCOM0, SERCOM0_GCLK_ID_CORE, SERCOM0_IRQn},
    {SERCOM1, PM_APBCMASK_SERCOM1, SERCOM1_GCLK_ID_CORE, SERCOM1_IRQn},
#if defined(__SAMD11D14AM__) || defined(__ATSAMD11D14AM__) || \
    defined(__SAMD11D14AS__) || defined(__ATSAMD11D14AS__) || \
    defined(__SAMD11D14AU__) || defined(__ATSAMD11D14AU__)
    {SERCOM2, PM_APBCMASK_SERCOM2, SERCOM2_GCLK_ID_CORE, SERCOM2_IRQn},
#endif
};
const uint8_t Num_SERCOM_Instances = SERCOM_INST_NUM;

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
    uint32_t calib = (*((uint32_t *) FUSES_OSC32K_ADDR) & FUSES_OSC32K_Msk);
    SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_CALIB(calib) |
                          SYSCTRL_OSC32K_STARTUP(0x6) |
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

static void enable_48mhz_clock(void) {
    NVMCTRL->CTRLB.bit.RWS = 1;

#if defined(CRYSTALLESS)
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    /* Write the coarse and fine calibration from NVM. */
    uint32_t coarse =
        ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;
    uint32_t fine =
        ((*(uint32_t*)FUSES_DFLL48M_FINE_CAL_ADDR) & FUSES_DFLL48M_FINE_CAL_Msk) >> FUSES_DFLL48M_FINE_CAL_Pos;

    SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(fine);

    /* Wait for the write to finish. */
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};
    SYSCTRL->DFLLCTRL.reg |=
        /* Enable USB clock recovery mode */
        SYSCTRL_DFLLCTRL_USBCRM |
        /* Disable chill cycle as per datasheet to speed up locking.
        This is specified in section 17.6.7.2.2, and chill cycles
        are described in section 17.6.7.2.1. */
        SYSCTRL_DFLLCTRL_CCDIS;

    /* Configure the DFLL to multiply the 1 kHz clock to 48 MHz */
    SYSCTRL->DFLLMUL.reg =
        /* This value is output frequency / reference clock frequency,
        so 48 MHz / 1 kHz */
        SYSCTRL_DFLLMUL_MUL(48000) |
        /* The coarse and fine values can be set to their minimum
        since coarse is fixed in USB clock recovery mode and
        fine should lock on quickly. */
        SYSCTRL_DFLLMUL_FSTEP(1) |
        SYSCTRL_DFLLMUL_CSTEP(1);
    /* Closed loop mode */
    SYSCTRL->DFLLCTRL.bit.MODE = 1;

    /* Enable the DFLL */
    SYSCTRL->DFLLCTRL.bit.ENABLE = 1;

    /* Wait for the write to complete */
    while (!SYSCTRL->PCLKSR.bit.DFLLRDY) {};
#else
    /* Configure GCLK1's divider - in this case, no division - so just divide by one */
    GCLK->GENDIV.reg =
        GCLK_GENDIV_ID(1) |
        GCLK_GENDIV_DIV(1);

    /* Setup GCLK1 using the external 32.768 kHz oscillator */
    GCLK->GENCTRL.reg =
        GCLK_GENCTRL_ID(1) |
        GCLK_GENCTRL_SRC_XOSC32K |
        /* Improve the duty cycle. */
        GCLK_GENCTRL_IDC |
        GCLK_GENCTRL_GENEN;

    /* Wait for the write to complete */
    while(GCLK->STATUS.bit.SYNCBUSY);

    GCLK->CLKCTRL.reg =
        GCLK_CLKCTRL_ID_DFLL48 |
        GCLK_CLKCTRL_GEN_GCLK1 |
        GCLK_CLKCTRL_CLKEN;

    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
    SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    /* Set up the multiplier. This tells the DFLL to multiply the 32.768 kHz
    reference clock to 48 MHz */
    SYSCTRL->DFLLMUL.reg =
        /* This value is output frequency / reference clock frequency,
        so 48 MHz / 32.768 kHz */
        SYSCTRL_DFLLMUL_MUL(1465) |
        /* The coarse and fine step are used by the DFLL to lock
        on to the target frequency. These are set to half
        of the maximum value. Lower values mean less overshoot,
        whereas higher values typically result in some overshoot but
        faster locking. */
        SYSCTRL_DFLLMUL_FSTEP(511) | // max value: 1023
        SYSCTRL_DFLLMUL_CSTEP(31);  // max value: 63

    /* Wait for the write to finish */
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    uint32_t coarse = (*((uint32_t *)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk) >> FUSES_DFLL48M_COARSE_CAL_Pos;

    SYSCTRL->DFLLVAL.bit.COARSE = coarse;

    /* Wait for the write to finish */
    while(!SYSCTRL->PCLKSR.bit.DFLLRDY);

    SYSCTRL->DFLLCTRL.reg |=
        /* Closed loop mode */
        SYSCTRL_DFLLCTRL_MODE |
        /* Wait for the frequency to be locked before outputting the clock */
        SYSCTRL_DFLLCTRL_WAITLOCK |
        /* Enable it */
        SYSCTRL_DFLLCTRL_ENABLE;

    /* Wait for the frequency to lock */
    while (!SYSCTRL->PCLKSR.bit.DFLLLCKC || !SYSCTRL->PCLKSR.bit.DFLLLCKF) {}

#endif

    /* Setup GCLK0 using the DFLL @ 48 MHz */
    GCLK->GENCTRL.reg =
        GCLK_GENCTRL_ID(0) |
        GCLK_GENCTRL_SRC_DFLL48M |
        /* Improve the duty cycle. */
        GCLK_GENCTRL_IDC |
        GCLK_GENCTRL_GENEN;

    /* Wait for the write to complete */
    while(GCLK->STATUS.bit.SYNCBUSY);
}

uint32_t get_cpu_frequency(void) {
    // if GCLK0's source is the DFLL, we're running at 48 MHz
    if (GCLK->GENCTRL.bit.SRC == GCLK_GENCTRL_SRC_DFLL48M_Val) {
        return 48000000;
    }

    // otherwise, we're running at 8 MHz divided by the prescaler
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
    if (freq == 48000000) {
        enable_48mhz_clock();
        return true;
    }

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
    return true;
}

void _enter_standby_mode() {
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
    __WFI();
}
