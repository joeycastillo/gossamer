/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "pins.h"
#include "sam.h"
#include "system.h"
#include "i2c.h"

#if defined(I2C_SERCOM_APBCMASK) && defined(I2C_GCLK_CLKCTRL_ID)

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

void i2c_init(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    /* Enable the APB clock for SERCOM. */
    PM->APBCMASK.reg |= I2C_SERCOM_APBCMASK;
    /* Enable GCLK1 for the SERCOM */
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | I2C_GCLK_CLKCTRL_ID;
#else
    /* Enable the APB clock for SERCOM. */
    MCLK->APBCMASK.reg |= I2C_SERCOM_APBCMASK;
    /* Enable GCLK1 for the SERCOM */
    GCLK->PCHCTRL[I2C_GCLK_CLKCTRL_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN_GCLK0_Val;
#endif

    /* Wait for bus synchronization. */
    while (GCLK->STATUS.bit.SYNCBUSY) {};

    /* Reset the SERCOM. */
    I2C_SERCOM->I2CM.CTRLA.bit.ENABLE = 0;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};
    I2C_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SWRST || I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Configure SERCOM for i2c master. */
    I2C_SERCOM->I2CM.CTRLA.bit.SWRST = 1;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SWRST || I2C_SERCOM->I2CM.CTRLA.bit.SWRST) {};

    I2C_SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0) |        // 0 is standard/fast mode 100 & 400kHz
                                     SERCOM_I2CM_CTRLA_SDAHOLD(0) |      // Hold SDA low for 300-600ns
                                     SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;  // work as master

    /* Enable smart mode */
    I2C_SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN | SERCOM_I2CM_CTRLB_QCEN;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Set baudrate.
        this is from Arduino:
            https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/SERCOM.cpp#L461
    */
    uint32_t baudrate = 400000;                 // Hz
    uint32_t clock_speed = get_cpu_frequency(); // Hz
    uint32_t rise_time = 300;                   // ns
    I2C_SERCOM->I2CM.BAUD.bit.BAUD =
        clock_speed / (2 * baudrate) - 5 - (((clock_speed / 1000000) * rise_time) / (2 * 1000));
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Enable the SERCOM. */
    I2C_SERCOM->I2CM.CTRLA.bit.ENABLE = 1;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Put the bus into the idle state. */
    I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE = BUSSTATE_IDLE;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};
}

I2CResult i2c_write(uint8_t address, uint8_t* data, size_t len) {
    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return I2C_RESULT_ERR_BUSSTATE;
    }

    /* Address + write flag. */
    I2C_SERCOM->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0;

    /* This can hang forever, so put a timeout on it. */
    size_t w = 0;
    for (; w < 100000; w++) {
        if (I2C_SERCOM->I2CM.INTFLAG.bit.MB) {
            break;
        }
    }

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (I2C_SERCOM->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return I2C_RESULT_ERR_BUSSTATE;
    }
    if (I2C_SERCOM->I2CM.STATUS.bit.RXNACK) {
        return I2C_RESULT_ERR_ADDR_NACK;
    }

    /* Send data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Send data and wait for TX complete. */
        I2C_SERCOM->I2CM.DATA.bit.DATA = data[i];

        while (!I2C_SERCOM->I2CM.INTFLAG.bit.MB) {
            /* Check for loss of arbitration or a bus error. We can't continue if those happen. */
            /* BUSERR is set in addition to ARBLOST if arbitration is lost, so just check that one. */
            if (I2C_SERCOM->I2CM.STATUS.bit.BUSERR) {
                return I2C_RESULT_ERR_BUSERR;
            }
        }

        /* If a nack is received we can not continue sending data. */
        if (I2C_SERCOM->I2CM.STATUS.bit.RXNACK) {
            return I2C_RESULT_ERR_DATA_NACK;
        }
    }

    /* Send STOP command. */
    I2C_SERCOM->I2CM.CTRLB.bit.CMD = 3;
    while (I2C_SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    return I2C_RESULT_SUCCESS;
}

#endif
