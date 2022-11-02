#include "piezo.h"
#include "system.h"
#include "tcc.h"
#include "sam.h"

static Tcc* piezo_tcc = NULL;
static int8_t wo_p = -1;
static int8_t wo_n = -1;

void piezo_tcc_init(uint8_t instance, uint8_t channel) {
    // if we were able to set up the TCC on GCLK0...
    if (tcc_setup(instance, 0)) {
        // ...enable it...
        tcc_enable(instance);
        // ...and store it as well as the channel we're going to use.
        piezo_tcc = TCC_Peripherals[instance].tcc;
        wo_p = channel;
        wo_n = -1;
        // Disable the TCC so we can configure it.
        piezo_tcc->CTRLA.bit.ENABLE = 0;
        // Normal PWM mode.
        piezo_tcc->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_NPWM_Val;
        // 8 MHz / 8 = 1 MHz clock.
        piezo_tcc->CTRLA.bit.PRESCALER = TCC_CTRLA_PRESCALER_DIV8_Val;
        // Set the duty cycle to 0% (logic low, buzzer off)
        piezo_tcc->CC[wo_p % 4].reg = 0;
        // and the period to a reasonable value (we'll set it before playing a sound)
        piezo_tcc->PER.reg = 128;
        // finally enable the TCC
        piezo_tcc->CTRLA.bit.ENABLE = 1;
        // and wait for it to sync
        tcc_sync(instance);
    }
}

void piezo_tcc_init_differential(uint8_t instance, uint8_t channel_p, uint8_t channel_n) {
    // setting up differential drive starts by setting up the positive channel as above.
    piezo_tcc_init(instance, channel_p);
    // if it worked...
    if (piezo_tcc != NULL) {
        // stash our channel
        wo_n = channel_n;
        // and disable it for some more configuration.
        piezo_tcc->CTRLA.bit.ENABLE = 0;
        // First, invert the output of the negative pin relative to the positive one.
        piezo_tcc->DRVCTRL.reg |= TCC_DRVCTRL_INVEN(wo_n);
        // then set its duty cycle to 100% (since it's inverted, this will be logic low)
        piezo_tcc->CC[wo_n % 4].reg = piezo_tcc->PER.reg;
        // finally reenable and sync.
        piezo_tcc->CTRLA.bit.ENABLE = 1;
        tcc_sync(instance);
    }
}

void piezo_tcc_on(uint32_t freq) {
    // check if we have a valid TCC
    if (piezo_tcc != NULL) {
        // if so, calculate our period (1 MHz clock / the frequency)
        uint32_t period = 1000000 / freq;
        // set the period buffer to the period for our desired frequency
        piezo_tcc->PER.reg = period;
        // and the CC buffers to half that (50% duty cycle).
        // note that there are only four CC registers for eight WO channels.
        // in the default configutation, WO4/5/6/7 use CC0/1/2/3 respectively.
        piezo_tcc->CC[wo_p % 4].reg = period / 2;
        if (wo_n >= 0) {
            piezo_tcc->CC[wo_n % 4].reg = period / 2;
        }
    }
}

void piezo_tcc_off(void) {
    if (piezo_tcc != NULL) {
        // set the duty cycle of the positive pin to 0%, logic low
        piezo_tcc->CC[wo_p % 4].reg = 0;
        if (wo_n >= 0) {
            // and of the negative pin to 100% (inverted to logic low)
            piezo_tcc->CC[wo_n % 4].reg = piezo_tcc->PER.reg;
        }
    }
}
