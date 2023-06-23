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
#include "adc.h"

#if defined(_SAMD51_)
/// TODO: Deal with dual ADC situation on SAM D51
#define ADC ADC0
#endif
#if !defined(ADC_INPUTCTRL_MUXNEG_GND_Val) && defined(_SAML22_)
#define ADC_INPUTCTRL_MUXNEG_GND_Val (0x18)
#endif

static void _adc_sync(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    while (ADC->STATUS.bit.SYNCBUSY);
#else
    while (ADC->SYNCBUSY.reg);
#endif
}

uint16_t adc_get_analog_value(uint16_t channel) {
    if (ADC->INPUTCTRL.bit.MUXPOS != channel) {
        ADC->INPUTCTRL.bit.MUXPOS = channel;
        _adc_sync();
    }

    ADC->SWTRIG.bit.START = 1;
    while (!ADC->INTFLAG.bit.RESRDY); // wait for "result ready" flag

    return ADC->RESULT.reg;
}

void adc_init(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    // enable the ADC
    PM->APBAMASK.reg |= PM_APBCMASK_ADC;
    // Configure ADC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(ADC_GCLK_ID) | GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN(0);
#elif defined(_SAML21_)
    // enable the ADC
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC;
    // Configure ADC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
#elif defined(_SAML22_)
    // enable the ADC
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_ADC;
    // Configure ADC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->PCHCTRL[ADC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
#else // SAM D51
    // enable the ADC
    MCLK->APBDMASK.reg |= MCLK_APBDMASK_ADC0;
    // Configure ADC to use GCLK0 (the main 8 MHz oscillator)
    GCLK->PCHCTRL[ADC0_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK0 | GCLK_PCHCTRL_CHEN;
#endif

    if (ADC->CTRLA.bit.ENABLE) {
        ADC->CTRLA.bit.ENABLE = 0;
        _adc_sync();
    }
    ADC->CTRLA.bit.SWRST = 1;
    _adc_sync();

#if defined(_SAMD51_)
    ADC->CTRLA.bit.PRESCALER = ADC_CTRLA_PRESCALER_DIV16_Val;
#else
    ADC->CTRLB.bit.PRESCALER = ADC_CTRLB_PRESCALER_DIV16_Val;
#endif

#if defined(_SAMD21_) || defined(_SAMD11_)
    // set calibration data
    uint32_t bias = (*((uint32_t *) ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk) >> ADC_FUSES_BIASCAL_Pos;
    uint32_t linearity = (*((uint32_t *) ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk) >> ADC_FUSES_LINEARITY_0_Pos;
    linearity |= ((*((uint32_t *) ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk) >> ADC_FUSES_LINEARITY_1_Pos) << 5;
    _adc_sync();
    // set calibration data
    ADC->CALIB.reg = ADC_CALIB_BIAS_CAL(bias) | ADC_CALIB_LINEARITY_CAL(linearity);
    // set reference voltage to VDDANA / 2
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
    // oversample to 16 bits
    ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
    // compare input to GND
    ADC->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
    // ...and divide by 2 since our reference is VDDANA / 2
    ADC->INPUTCTRL.bit.GAIN = ADC_INPUTCTRL_GAIN_DIV2_Val;
#elif defined(_SAML22_) || defined(_SAML21_)
    // set calibration data
    ADC->CALIB.reg = ADC_CALIB_BIASREFBUF((*(uint32_t *)ADC_FUSES_BIASREFBUF_ADDR >> ADC_FUSES_BIASREFBUF_Pos)) |
                ADC_CALIB_BIASCOMP((*(uint32_t *)ADC_FUSES_BIASCOMP_ADDR >> ADC_FUSES_BIASCOMP_Pos));
    _adc_sync();
    // set reference voltage to VDDANA
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC2_Val;
    // oversample to 16 bits
    ADC->CTRLC.bit.RESSEL = ADC_CTRLC_RESSEL_16BIT_Val;
    // compare input to GND
    ADC->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
#else
    // set calibration data
    ADC->CALIB.reg = ADC_CALIB_BIASREFBUF((*(uint32_t *)ADC0_FUSES_BIASREFBUF_ADDR >> ADC0_FUSES_BIASREFBUF_Pos)) |
                     ADC_CALIB_BIASCOMP((*(uint32_t *)ADC0_FUSES_BIASCOMP_ADDR >> ADC0_FUSES_BIASCOMP_Pos));
    _adc_sync();
    // set reference voltage to VDDANA
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTVCC1_Val;
    // oversample to 16 bits
    ADC->CTRLB.bit.RESSEL = ADC_CTRLB_RESSEL_16BIT_Val;
    // compare input to GND
    ADC->INPUTCTRL.bit.MUXNEG = ADC_INPUTCTRL_MUXNEG_GND_Val;
#endif
    ADC->AVGCTRL.bit.SAMPLENUM = ADC_AVGCTRL_SAMPLENUM_16_Val;
    ADC->SAMPCTRL.bit.SAMPLEN = 0;
    ADC->INTENSET.reg = ADC_INTENSET_RESRDY;
}

void adc_enable(void) {
    ADC->CTRLA.bit.ENABLE = 1;
    _adc_sync();
    // throw away one measurement after reference change (the channel doesn't matter).
    adc_get_analog_value(ADC_INPUTCTRL_MUXPOS_SCALEDCOREVCC);
}

void adc_disable(void) {
    ADC->CTRLA.bit.ENABLE = 0;
    _adc_sync();

#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->APBAMASK.reg &= ~PM_APBCMASK_ADC;
#elif defined(_SAML21_)
    MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_ADC;
#elif defined(_SAML22_)
    MCLK->APBCMASK.reg &= ~MCLK_APBCMASK_ADC;
#else // SAM D51
    MCLK->APBDMASK.reg &= ~MCLK_APBDMASK_ADC0;
#endif
}
