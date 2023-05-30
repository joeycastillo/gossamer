/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
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
#include "sam.h"
#include "system.h"
#include "opamp.h"

#ifdef OPAMP

void opamp_init(void) {
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_OPAMP;
    OPAMP->CTRLA.reg |= OPAMP_CTRLA_SWRST;
    OPAMP->CTRLA.reg |= OPAMP_CTRLA_ENABLE;
}

void opamp_enable(uint16_t instance) {
    if (OPAMP->CTRLA.bit.ENABLE == 0) {
        OPAMP->CTRLA.reg |= OPAMP_CTRLA_ENABLE;
    }

    OPAMP->OPAMPCTRL[instance].reg |= OPAMP_OPAMPCTRL_ENABLE;
}

void opamp_set_muxpos(uint16_t instance, uint8_t muxpos) {
    OPAMP->OPAMPCTRL[instance].bit.MUXPOS = muxpos;
}

void opamp_set_muxneg(uint16_t instance, uint8_t muxneg) {
    OPAMP->OPAMPCTRL[instance].bit.MUXNEG = muxneg;
}

void opamp_set_potmux(uint16_t instance, uint8_t potmux) {
    OPAMP->OPAMPCTRL[instance].bit.POTMUX = potmux;
}


void opamp_set_res1mux(uint16_t instance, uint8_t res1mux) {
    if (res1mux != OPAMP_RES1MUX_NC) {
        OPAMP->OPAMPCTRL[instance].bit.RES1MUX = res1mux;
        OPAMP->OPAMPCTRL[instance].bit.RES1EN = 1;
    } else {
        OPAMP->OPAMPCTRL[instance].bit.RES1EN = 0;
    }
}

void opamp_set_res2mux(uint16_t instance, uint8_t res2mux) {
    switch (res2mux) {
        case OPAMP_RES2MUX_VCC:
            OPAMP->OPAMPCTRL[instance].bit.RES2OUT = 0;
            OPAMP->OPAMPCTRL[instance].bit.RES2VCC = 1;
            break;
        case OPAMP_RES2MUX_OUT:
            OPAMP->OPAMPCTRL[instance].bit.RES2VCC = 0;
            OPAMP->OPAMPCTRL[instance].bit.RES2OUT = 1;
            break;
        case OPAMP_RES2MUX_NC:
        default:
            OPAMP->OPAMPCTRL[instance].bit.RES2VCC = 0;
            OPAMP->OPAMPCTRL[instance].bit.RES2OUT = 0;
            break;
    }
}

void opamp_set_analog_connection(uint16_t instance, bool connected) {
    OPAMP->OPAMPCTRL[instance].bit.ANAOUT = connected;
}

void opamp_disable(uint16_t instance) {
    OPAMP->OPAMPCTRL[instance].reg &= ~OPAMP_OPAMPCTRL_ENABLE;

    if (OPAMP->OPAMPCTRL[0].bit.ENABLE == 0 &&
        OPAMP->OPAMPCTRL[1].bit.ENABLE == 0 &&
        OPAMP->OPAMPCTRL[2].bit.ENABLE == 0) {
        OPAMP->CTRLA.reg &= ~OPAMP_CTRLA_ENABLE;
    }
}

#endif
