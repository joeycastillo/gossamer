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
#include "pins.h"

#ifdef HAS_PTC

static void sync_config(Ptc const *module_inst) {
    while (module_inst->CTRLB.bit.SYNCFLAG);
}

void ptc_init(Ptc *module_inst) {
    /* Enable the APB clock for PTC. */
    PM->APBCMASK.reg |= PM_APBCMASK_PTC;
    /* Clock PTC with GCLK2 */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK2 | GCLK_CLKCTRL_ID_PTC;
    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};
    module_inst->CTRLA.bit.ENABLE = 0;
    while (module_inst->CTRLB.bit.SYNCFLAG) {};
    module_inst->CTRLA.bit.SWRESET = 1;
    while (module_inst->CTRLA.bit.SWRESET) {};

    sync_config(module_inst);
    module_inst->CTRLA.bit.ENABLE = 0;
    sync_config(module_inst);

    module_inst->UNK4C04.reg &= 0xF7; // MEMORY[0x42004C04] &= 0xF7u;
    module_inst->UNK4C04.reg &= 0xFB; // MEMORY[0x42004C04] &= 0xFBu;
    module_inst->UNK4C04.reg &= 0xFC; // MEMORY[0x42004C04] &= 0xFCu;
    sync_config(module_inst);
    module_inst->FREQCTRL.reg &= 0x9F; // MEMORY[0x42004C0C] &= 0x9Fu;
    sync_config(module_inst);
    module_inst->FREQCTRL.reg &= 0xEF; // MEMORY[0x42004C0C] &= 0xEFu;
    sync_config(module_inst);
    module_inst->FREQCTRL.bit.SAMPLEDELAY = 0; // MEMORY[0x42004C0C] &= 0xF0u;
    module_inst->CTRLC.bit.INIT = 1;           // MEMORY[0x42004C05] |= 1u;
    module_inst->CTRLA.bit.RUNINSTANDBY = 1;   // MEMORY[0x42004C00] |= 4u;
    sync_config(module_inst);
    module_inst->INTDISABLE.bit.WCO = 1;
    sync_config(module_inst);
    module_inst->INTDISABLE.bit.EOC = 1;
    sync_config(module_inst);

    // enable the sensor, only done once per line
    sync_config(module_inst);
    module_inst->CTRLA.bit.ENABLE = 1;
    sync_config(module_inst);
}

void ptc_enable_channel(Ptc *module_inst, uint8_t channel) {
    module_inst->CTRLA.bit.ENABLE = 0;
    sync_config(module_inst);
    if (channel < 8) {
        sync_config(module_inst);
        module_inst->YENABLEL.reg |= 1 << channel;
        sync_config(module_inst);
    } else if (channel < 16) {
        module_inst->YENABLEH.reg |= 1 << (channel - 8);
    }
    module_inst->CTRLA.bit.ENABLE = 1;
    sync_config(module_inst);
}

void ptc_start_conversion(Ptc *module_inst, uint8_t channel) {
    module_inst->CTRLA.bit.RUNINSTANDBY = 1;
    sync_config(module_inst);
    module_inst->CTRLA.bit.ENABLE = 1;
    sync_config(module_inst);
    module_inst->INTDISABLE.bit.WCO = 1;
    sync_config(module_inst);
    module_inst->INTFLAGS.bit.WCO = 1;
    sync_config(module_inst);
    module_inst->INTFLAGS.bit.EOC = 1;
    sync_config(module_inst);

    // set up pin!
    sync_config(module_inst);
    if (channel < 8) {
        module_inst->YSELECTL.reg = 1 << channel;
    } else {
        module_inst->YSELECTL.reg = 0;
    }

    if (channel > 7) {
        module_inst->YSELECTH.reg = 1 << (channel - 8);
    } else {
        module_inst->YSELECTH.reg = 0;
    }

    sync_config(module_inst);
    // set up sense resistor
    module_inst->SERRES.bit.RESISTOR = RESISTOR_0;
    sync_config(module_inst);
    // set up prescalar
    module_inst->CONVCTRL.bit.ADCACCUM = OVERSAMPLE_4;
    sync_config(module_inst);
    // set up freq hopping
    module_inst->FREQCTRL.bit.FREQSPREADEN = 0;
    module_inst->FREQCTRL.bit.SAMPLEDELAY = 0;
    // set up compensation cap + int (?) cap
    sync_config(module_inst);
    module_inst->COMPCAPL.bit.VALUE = 0;
    module_inst->COMPCAPH.bit.VALUE = 0x20;
    sync_config(module_inst);
    module_inst->INTCAP.bit.VALUE = 0x3F;
    sync_config(module_inst);

    module_inst->BURSTMODE.reg = 0xA4;
    sync_config(module_inst);

    module_inst->CONVCTRL.bit.CONVERT = 1;
    sync_config(module_inst);
}

bool ptc_is_conversion_finished(Ptc *module_inst) {
    return module_inst->CONVCTRL.bit.CONVERT == 0;
}

uint16_t ptc_get_conversion_result(Ptc *module_inst) {
    sync_config(module_inst);
    return module_inst->RESULT.reg;
}

#endif