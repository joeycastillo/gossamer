#include "sercom.h"
#include "system.h"


void _sercom_clock_setup(uint8_t sercom) {
    const uint8_t clock_id = SERCOM_Peripherals[sercom].gclk_id;
    const uint8_t clock_mask = SERCOM_Peripherals[sercom].clock_enable_mask;

#if defined(_SAMD21_) || defined(_SAMD11_)
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= clock_mask;
#elif defined(_SAML21_)
    /* SERCOM5 is on AHB-APB Bridge D (not C like everyone else) */
    if (sercom == 5) MCLK->APBDMASK.reg |= clock_mask;
    else MCLK->APBCMASK.reg |= clock_mask;
#elif defined(_SAMD51_)
    switch (sercom) {
        case 0:
        case 1:
            MCLK->APBAMASK.reg |= clock_mask;
            break;
        case 2:
        case 3:
            MCLK->APBBMASK.reg |= clock_mask;
            break;
        case 4:
        case 5:
        case 6:
        case 7:
            MCLK->APBDMASK.reg |= clock_mask;
            break;
    }
#else
    /* Enable the APB clock for SERCOM. */
    MCLK->APBCMASK.reg |= clock_mask;
#endif

#if defined(_SAMD21_) || defined(_SAMD11_)
    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
                        GCLK_CLKCTRL_GEN_GCLK0 |
                        GCLK_CLKCTRL_ID(clock_id);
    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};
#else
    /* Enable GCLK0 for the SERCOM */
    GCLK->PCHCTRL[clock_id].reg = GCLK_PCHCTRL_CHEN |
                                  GCLK_PCHCTRL_GEN_GCLK0_Val;
    /* Wait for bus synchronization. */
    while (GCLK->SYNCBUSY.reg) {};
#endif
}

void _sercom_enable(uint8_t sercom) {
    // All modes have CTRLA.ENABLE at the same bit position.
    SERCOM_Peripherals[sercom].sercom->SPI.CTRLA.bit.ENABLE = 1;
    while (SERCOM_Peripherals[sercom].sercom->SPI.SYNCBUSY.bit.ENABLE) {}
}

void _sercom_disable(uint8_t sercom) {
    // All modes have CTRLA.ENABLE at the same bit position.
    SERCOM_Peripherals[sercom].sercom->SPI.CTRLA.bit.ENABLE = 0;
    while (SERCOM_Peripherals[sercom].sercom->SPI.SYNCBUSY.bit.ENABLE) {}
}
