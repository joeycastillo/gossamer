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
#include "tc.h"

static void tc_sync(uint8_t instance) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT16.STATUS.bit.SYNCBUSY);
#else
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT16.SYNCBUSY.reg);
#endif
}

bool tc_init(uint8_t instance, generic_clock_generator_t clocksource, tc_prescaler_value_t prescaler) {
    if ((instance - TC_First_Index) >= Num_TC_Instances ||
        TC_Peripherals[instance - TC_First_Index].tc == NULL) {
        return false;
    }

#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the TC
    PM->APBCMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    // Configure TC to use the selected clock souece
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(TC_Peripherals[instance - TC_First_Index].gclk_id) | 
                        GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(clocksource);
#elif defined(_SAML21_)
    // On L21, TC4 is on the APBD bus, not the APBC bus.
    if (instance == 4) {
        MCLK->APBDMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    } else {
        MCLK->APBCMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    }
    GCLK->PCHCTRL[TC_Peripherals[instance - TC_First_Index].gclk_id].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(clocksource);
#else
    // enable the TC
    MCLK->APBCMASK.reg |= TC_Peripherals[instance - TC_First_Index].clock_enable_mask;
    GCLK->PCHCTRL[TC_Peripherals[instance - TC_First_Index].gclk_id].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(clocksource);
#endif

    // reset the TC
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.SWRST = 1;
    tc_sync(instance);
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.PRESCALER = prescaler;

    return true;
}

void tc_set_counter_mode(uint8_t instance, tc_counter_mode_t mode) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.MODE = mode;
}

void tc_set_run_in_standby(uint8_t instance, bool runStandby) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.RUNSTDBY = runStandby;
}

void tc_set_run_on_demand(uint8_t instance, bool onDemand) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.ONDEMAND = onDemand;
}

void tc_set_wavegen(uint8_t instance, tc_wavegen_t mode) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.WAVEGEN = mode;
#else
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.WAVE.bit.WAVEGEN = mode;
#endif
}

void tc_set_channel_polarity(uint8_t instance, uint8_t channel, tc_channel_polarity_t polarity) {
    uint8_t value = 
#if defined(_SAMD21_) || defined(_SAMD11_)
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLC.reg;
#else
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.DRVCTRL.reg;
#endif
    if (polarity == TC_CHANNEL_POLARITY_INVERTED) {
        value |= (1 << channel);
    } else {
        value &= ~(1 << channel);
    }
#if defined(_SAMD21_) || defined(_SAMD11_)
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLC.reg = value;
#else
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.DRVCTRL.reg = value;
#endif
}

void tc_enable(uint8_t instance) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.ENABLE = 1;
    tc_sync(instance);
}

bool tc_is_enabled(uint8_t instance) {
    return TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.ENABLE;
}

void tc_count8_set_period(uint8_t instance, uint8_t period) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.PER.reg = period;
}

uint8_t tc_count8_get_period(uint8_t instance) {
    return TC_Peripherals[instance - TC_First_Index].tc->COUNT8.PER.reg;
}

void tc_count8_set_cc(uint8_t instance, uint8_t channel, uint8_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.CC[channel].reg = value;
}

void tc_count16_set_cc(uint8_t instance, uint8_t channel, uint16_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CC[channel].reg = value;
}

void tc_count32_set_cc(uint8_t instance, uint8_t channel, uint32_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT32.CC[channel].reg = value;
}

void tc_count8_set_count(uint8_t instance, uint8_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT8.COUNT.reg = value;
}

void tc_count16_set_count(uint8_t instance, uint16_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.COUNT.reg = value;
}

void tc_count32_set_count(uint8_t instance, uint32_t value) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT32.COUNT.reg = value;
}

static void _tc_request_read_count(uint8_t instance) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.READREQ.reg = TC_READREQ_RREQ | TC_READREQ_ADDR(0x10);
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT16.STATUS.bit.SYNCBUSY);
#else
    TC_Peripherals[instance - TC_First_Index].tc->COUNT32.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_READSYNC_Val;
    while (TC_Peripherals[instance - TC_First_Index].tc->COUNT16.SYNCBUSY.reg);
#endif
}

uint8_t tc_count8_get_count(uint8_t instance) {
    _tc_request_read_count(instance);
    return TC_Peripherals[instance - TC_First_Index].tc->COUNT8.COUNT.reg;    
}

uint16_t tc_count16_get_count(uint8_t instance) {
    _tc_request_read_count(instance);
    return TC_Peripherals[instance - TC_First_Index].tc->COUNT16.COUNT.reg;
}

uint32_t tc_count32_get_count(uint8_t instance) {
    _tc_request_read_count(instance);
    return TC_Peripherals[instance - TC_First_Index].tc->COUNT32.COUNT.reg;
}

void tc_set_event_action(uint8_t instance, tc_event_action_t action) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.EVCTRL.bit.EVACT = action;
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.EVCTRL.bit.TCEI = !!action;
}

void tc_stop(uint8_t instance) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_STOP_Val;
}

void tc_retrigger(uint8_t instance) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLBSET.bit.CMD = TC_CTRLBSET_CMD_RETRIGGER_Val;
}

void tc_disable(uint8_t instance) {
    TC_Peripherals[instance - TC_First_Index].tc->COUNT16.CTRLA.bit.ENABLE = 0;
    tc_sync(instance);
}

// TODO: every peripheral should have a deinit function, make public when that's done
/*
void tc_deinit(uint8_t instance) {
    #if defined(_SAMD21_) || defined(_SAMD11_)
        PM->APBAMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
    #else
        MCLK->APBCMASK.reg &= ~(TC_Peripherals[instance - TC_First_Index].clock_enable_mask);
    #endif
}
*/
