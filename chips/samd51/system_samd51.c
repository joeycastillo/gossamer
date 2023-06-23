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
#include "samd51.h"
#include "system.h"

const TC_Instance_Details TC_Peripherals[] = {
    {TC0, MCLK_APBAMASK_TC0, TC0_GCLK_ID},
    {TC1, MCLK_APBAMASK_TC1, TC1_GCLK_ID},
    {TC2, MCLK_APBBMASK_TC2, TC2_GCLK_ID},
    {TC3, MCLK_APBBMASK_TC3, TC3_GCLK_ID},
#if defined(__SAMD51J18A__) || defined(__ATSAMD51J18A__) || \
    defined(__SAMD51J19A__) || defined(__ATSAMD51J19A__) || \
    defined(__SAMD51J19B__) || defined(__ATSAMD51J19B__) || \
    defined(__SAMD51J20A__) || defined(__ATSAMD51J20A__) || \
    defined(__SAMD51J20C__) || defined(__ATSAMD51J20C__) || \
    defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__) || \
    defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__) || \
    defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__) || \
    defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
    {TC4, MCLK_APBCMASK_TC4, TC4_GCLK_ID},
    {TC5, MCLK_APBCMASK_TC5, TC5_GCLK_ID},
#endif
#if defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__) || \
    defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__) || \
    defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__) || \
    defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
    {TC6, MCLK_APBDMASK_TC6, TC6_GCLK_ID},
    {TC7, MCLK_APBDMASK_TC7, TC7_GCLK_ID},
#endif

};

const uint8_t Num_TC_Instances = TC_INST_NUM;
const uint8_t TC_First_Index = 0;

const TCC_Instance_Details TCC_Peripherals[] = {
#if defined(__SAMD51J18A__) || defined(__ATSAMD51J18A__) || \
    defined(__SAMD51J19A__) || defined(__ATSAMD51J19A__) || \
    defined(__SAMD51J19B__) || defined(__ATSAMD51J19B__) || \
    defined(__SAMD51J20A__) || defined(__ATSAMD51J20A__) || \
    defined(__SAMD51J20C__) || defined(__ATSAMD51J20C__) || \
    defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__) || \
    defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__) || \
    defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__) || \
    defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
#endif
    {TCC0, MCLK_APBBMASK_TCC0, TCC0_GCLK_ID},
    {TCC1, MCLK_APBBMASK_TCC1, TCC1_GCLK_ID},
    {TCC2, MCLK_APBCMASK_TCC2, TCC2_GCLK_ID},
};
const uint8_t Num_TCC_Instances = TCC_INST_NUM;

const SERCOM_Instance_Details SERCOM_Peripherals[] = {
    {SERCOM0, MCLK_APBAMASK_SERCOM0, SERCOM0_GCLK_ID_CORE},
    {SERCOM1, MCLK_APBAMASK_SERCOM1, SERCOM1_GCLK_ID_CORE},
    {SERCOM2, MCLK_APBBMASK_SERCOM2, SERCOM2_GCLK_ID_CORE},
    {SERCOM3, MCLK_APBBMASK_SERCOM3, SERCOM3_GCLK_ID_CORE},
    {SERCOM4, MCLK_APBDMASK_SERCOM4, SERCOM4_GCLK_ID_CORE},
    {SERCOM5, MCLK_APBDMASK_SERCOM5, SERCOM5_GCLK_ID_CORE},
#if defined(__SAMD51N19A__) || defined(__ATSAMD51N19A__) || \
    defined(__SAMD51N20A__) || defined(__ATSAMD51N20A__) || \
    defined(__SAMD51P19A__) || defined(__ATSAMD51P19A__) || \
    defined(__SAMD51P20A__) || defined(__ATSAMD51P20A__)
    {SERCOM6, MCLK_APBDMASK_SERCOM6, SERCOM6_GCLK_ID_CORE},
    {SERCOM7, MCLK_APBDMASK_SERCOM7, SERCOM7_GCLK_ID_CORE},
#endif
};
const uint8_t Num_SERCOM_Instances = SERCOM_INST_NUM;

//-----------------------------------------------------------------------------
void sys_init(void) {
    // No oscillator setup; SAM D51 defaults to 48 MHz DFLL

    // set up low power 32k oscillator on GCLK2
    GCLK->GENCTRL[2].reg = GCLK_GENCTRL_DIV(1) |
                           GCLK_GENCTRL_SRC_OSCULP32K |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;

#ifdef CRYSTALLESS
    OSC32KCTRL->OSCULP32K.reg = OSC32KCTRL_OSCULP32K_EN1K |
                                OSC32KCTRL_OSCULP32K_EN32K;
    OSC32KCTRL->RTCCTRL.reg = OSC32KCTRL_RTCCTRL_RTCSEL_ULP1K;
    // set up 1k oscillator on GCLK3
    GCLK->GENCTRL[3].reg = GCLK_GENCTRL_DIV(32) |
                           GCLK_GENCTRL_SRC_OSCULP32K |
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

uint32_t get_cpu_frequency(void) {
    // TODO: implement faster clock speeds
    return 48000000;
}

bool set_cpu_frequency(uint32_t freq) {
    // TODO: implement faster clock speeds
    return freq == 48000000;
}

void _enter_standby_mode() {
    PM->SLEEPCFG.bit.SLEEPMODE = PM_SLEEPCFG_SLEEPMODE_STANDBY_Val;
    __WFI();
}
