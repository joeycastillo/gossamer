/*
 * MIT License
 *
 * Copyright (c) 2023 Joey Castillo
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
#include "system.h"
#include "i2s.h"

/// TODO: I2S on SAM D51
#ifdef _SAMD21_

void i2s_init(uint8_t instance, uint16_t sampleRate, uint8_t bitsPerSample, i2s_mode_t mode) {
    if (instance > 1) return;
    PM->APBCMASK.reg |= PM_APBCMASK_I2S;

    uint32_t clockDivider = sampleRate * 2 * bitsPerSample;
    while (GCLK->STATUS.bit.SYNCBUSY);
    GCLK->GENDIV.reg = GCLK_GENDIV_ID(8) |
                       GCLK_GENDIV_DIV(clockDivider);
    while (GCLK->STATUS.bit.SYNCBUSY);
    GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(8) |
                        GCLK_GENCTRL_SRC_OSC8M |
                        GCLK_GENCTRL_IDC |
                        GCLK_GENCTRL_GENEN;

    GCLK->CLKCTRL.reg = (I2S_GCLK_ID_0 + instance) |
                        GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN(8);
    I2S->CTRLA.reg |= I2S_CTRLA_SWRST;
    I2S->CLKCTRL[instance].bit.SCKSEL = I2S_CLKCTRL_SCKSEL_MCKDIV_Val;
    I2S->CLKCTRL[instance].bit.FSSEL = I2S_CLKCTRL_FSSEL_SCKDIV_Val;

    I2S->CTRLA.bit.ENABLE = 0;
    while(I2S->SYNCBUSY.bit.ENABLE);

    if (mode == I2S_MODE_NORMAL) {
        I2S->CLKCTRL[instance].bit.BITDELAY = I2S_CLKCTRL_BITDELAY_I2S_Val;
    } else {
        I2S->CLKCTRL[instance].bit.BITDELAY = I2S_CLKCTRL_BITDELAY_LJ_Val;
    }

    I2S->CLKCTRL[instance].bit.NBSLOTS = 1;

    switch (bitsPerSample) {
        case 32:
            I2S->CLKCTRL[instance].bit.SLOTSIZE = I2S_CLKCTRL_SLOTSIZE_32_Val;
            I2S->SERCTRL[instance].bit.DATASIZE = I2S_SERCTRL_DATASIZE_32_Val;
            break;

        case 24:
            I2S->CLKCTRL[instance].bit.SLOTSIZE = I2S_CLKCTRL_SLOTSIZE_24_Val;
            I2S->SERCTRL[instance].bit.DATASIZE = I2S_SERCTRL_DATASIZE_24_Val;
            break;

        case 16:
            I2S->CLKCTRL[instance].bit.SLOTSIZE = I2S_CLKCTRL_SLOTSIZE_16_Val;
            I2S->SERCTRL[instance].bit.DATASIZE = I2S_SERCTRL_DATASIZE_16_Val;
            break;

        case 8:
            I2S->CLKCTRL[instance].bit.SLOTSIZE = I2S_CLKCTRL_SLOTSIZE_8_Val;
            I2S->SERCTRL[instance].bit.DATASIZE = I2S_SERCTRL_DATASIZE_8_Val;
            break;
    }

    if (mode == I2S_MODE_RIGHT_JUSTIFIED) {
        I2S->SERCTRL[instance].bit.SLOTADJ = I2S_SERCTRL_SLOTADJ_RIGHT_Val;
    } else {
        I2S->SERCTRL[instance].bit.SLOTADJ = I2S_SERCTRL_SLOTADJ_LEFT_Val;
    }

    I2S->SERCTRL[instance].bit.CLKSEL = (instance == 0) ? I2S_SERCTRL_CLKSEL_CLK0_Val : I2S_SERCTRL_CLKSEL_CLK1_Val;

    I2S->CTRLA.bit.ENABLE = 1;
    while(I2S->SYNCBUSY.bit.ENABLE);
}

void i2s_enable_receiver(uint8_t instance) {
    I2S->SERCTRL[instance].bit.SERMODE = I2S_SERCTRL_SERMODE_RX_Val;
    if (instance == 0) {
        I2S->CTRLA.reg |= I2S_CTRLA_SEREN0 | I2S_CTRLA_CKEN0;
    } else {
        I2S->CTRLA.reg |= I2S_CTRLA_SEREN1 | I2S_CTRLA_CKEN1;
    }
}

#endif
