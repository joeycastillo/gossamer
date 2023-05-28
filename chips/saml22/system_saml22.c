/*
 * Copyright (c) 2014-2017, Alex Taradov <alex@taradov.com>
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
#include "saml22.h"
#include "system.h"

TC_Instance_Details TC_Peripherals[] = {
    {TC0, MCLK_APBCMASK_TC0, TC0_GCLK_ID},
    {TC1, MCLK_APBCMASK_TC1, TC1_GCLK_ID},
    {TC2, MCLK_APBCMASK_TC2, TC2_GCLK_ID},
    {TC3, MCLK_APBCMASK_TC3, TC3_GCLK_ID},
};
uint8_t Num_TC_Instances = 4;
uint8_t TC_First_Index = 0;

TCC_Instance_Details TCC_Peripherals[] = {
    {TCC0, MCLK_APBCMASK_TCC0, TCC0_GCLK_ID},
};
uint8_t Num_TCC_Instances = 1;

//-----------------------------------------------------------------------------
void sys_init(void) {
    // Switch to 8MHz clock
    OSCCTRL->OSC16MCTRL.reg = OSCCTRL_OSC16MCTRL_ENABLE | OSCCTRL_OSC16MCTRL_FSEL_8;

    // Switch to the highest performance level
    PM->INTFLAG.reg = PM_INTFLAG_PLRDY;
    PM->PLCFG.reg = PM_PLCFG_PLSEL_PL2_Val;
    while (!PM->INTFLAG.reg);
}

uint32_t get_cpu_frequency(void) {
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
