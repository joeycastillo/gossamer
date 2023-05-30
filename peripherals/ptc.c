/*
 * FreeTouch, a QTouch-compatible library - tested on ATSAMD21 only!
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Limor 'ladyada' Fried for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "ptc.h"

#ifdef HAS_PTC

static void _ptc_sync() {
    while (((Ptc *)PTC)->CTRLB.bit.SYNCFLAG);
}

static bool _ptc_is_conversion_finished() {
    return ((Ptc *)PTC)->CONVCTRL.bit.CONVERT == 0;
}

void ptc_init(void) {
    /* Enable the APB clock for PTC. */
    PM->APBCMASK.reg |= PM_APBCMASK_PTC;
    /* Clock PTC with GCLK2 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_PTC;
    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 0;
    while (((Ptc *)PTC)->CTRLB.bit.SYNCFLAG) {};
    ((Ptc *)PTC)->CTRLA.bit.SWRESET = 1;
    while (((Ptc *)PTC)->CTRLA.bit.SWRESET) {};

    _ptc_sync();
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 0;
    _ptc_sync();

    ((Ptc *)PTC)->UNK4C04.reg &= 0xF7; // MEMORY[0x42004C04] &= 0xF7u;
    ((Ptc *)PTC)->UNK4C04.reg &= 0xFB; // MEMORY[0x42004C04] &= 0xFBu;
    ((Ptc *)PTC)->UNK4C04.reg &= 0xFC; // MEMORY[0x42004C04] &= 0xFCu;
    _ptc_sync();
    ((Ptc *)PTC)->FREQCTRL.reg &= 0x9F; // MEMORY[0x42004C0C] &= 0x9Fu;
    _ptc_sync();
    ((Ptc *)PTC)->FREQCTRL.reg &= 0xEF; // MEMORY[0x42004C0C] &= 0xEFu;
    _ptc_sync();
    ((Ptc *)PTC)->FREQCTRL.bit.SAMPLEDELAY = 0; // MEMORY[0x42004C0C] &= 0xF0u;
    ((Ptc *)PTC)->CTRLC.bit.INIT = 1;           // MEMORY[0x42004C05] |= 1u;
    ((Ptc *)PTC)->CTRLA.bit.RUNINSTANDBY = 1;   // MEMORY[0x42004C00] |= 4u;
    _ptc_sync();
    ((Ptc *)PTC)->INTDISABLE.bit.WCO = 1;
    _ptc_sync();
    ((Ptc *)PTC)->INTDISABLE.bit.EOC = 1;
    _ptc_sync();

    // enable the sensor, only done once per line
    _ptc_sync();
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 1;
    _ptc_sync();
}

void ptc_enable(uint8_t channel) {
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 0;
    _ptc_sync();
    if (channel < 8) {
        _ptc_sync();
        ((Ptc *)PTC)->YENABLEL.reg |= 1 << channel;
        _ptc_sync();
    } else if (channel < 16) {
        ((Ptc *)PTC)->YENABLEH.reg |= 1 << (channel - 8);
    }
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 1;
    _ptc_sync();
}

uint16_t ptc_get_value(uint8_t channel) {
    ((Ptc *)PTC)->CTRLA.bit.RUNINSTANDBY = 1;
    _ptc_sync();
    ((Ptc *)PTC)->CTRLA.bit.ENABLE = 1;
    _ptc_sync();
    ((Ptc *)PTC)->INTDISABLE.bit.WCO = 1;
    _ptc_sync();
    ((Ptc *)PTC)->INTFLAGS.bit.WCO = 1;
    _ptc_sync();
    ((Ptc *)PTC)->INTFLAGS.bit.EOC = 1;
    _ptc_sync();

    // set up pin!
    _ptc_sync();
    if (channel < 8) {
        ((Ptc *)PTC)->YSELECTL.reg = 1 << channel;
    } else {
        ((Ptc *)PTC)->YSELECTL.reg = 0;
    }

    if (channel > 7) {
        ((Ptc *)PTC)->YSELECTH.reg = 1 << (channel - 8);
    } else {
        ((Ptc *)PTC)->YSELECTH.reg = 0;
    }

    _ptc_sync();
    // set up sense resistor
    ((Ptc *)PTC)->SERRES.bit.RESISTOR = RESISTOR_0;
    _ptc_sync();
    // set up prescalar
    ((Ptc *)PTC)->CONVCTRL.bit.ADCACCUM = OVERSAMPLE_4;
    _ptc_sync();
    // set up freq hopping
    ((Ptc *)PTC)->FREQCTRL.bit.FREQSPREADEN = 0;
    ((Ptc *)PTC)->FREQCTRL.bit.SAMPLEDELAY = 0;
    // set up compensation cap + int (?) cap
    _ptc_sync();
    ((Ptc *)PTC)->COMPCAPL.bit.VALUE = 0;
    ((Ptc *)PTC)->COMPCAPH.bit.VALUE = 0x20;
    _ptc_sync();
    ((Ptc *)PTC)->INTCAP.bit.VALUE = 0x3F;
    _ptc_sync();

    ((Ptc *)PTC)->BURSTMODE.reg = 0xA4;
    _ptc_sync();

    ((Ptc *)PTC)->CONVCTRL.bit.CONVERT = 1;
    _ptc_sync();

    while(_ptc_is_conversion_finished());

    return ((Ptc *)PTC)->RESULT.reg;
}

#endif
