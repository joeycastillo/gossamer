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
#include "sercom.h"
#include "i2c.h"

#define BUSSTATE_UNKNOWN 0
#define BUSSTATE_IDLE 1
#define BUSSTATE_OWNER 2
#define BUSSTATE_BUSY 3

#if defined(I2C_SERCOM)

void i2c_init(void) {
    i2c_init_instance(I2C_SERCOM, 100000);
}

void i2c_enable(void) {
    i2c_enable_instance(I2C_SERCOM);
}

i2c_result_t i2c_write(uint8_t address, uint8_t* data, size_t len) {
    return i2c_write_instance(I2C_SERCOM, address, data, len);
}

i2c_result_t i2c_read(uint8_t address, uint8_t* data, size_t len) {
    return i2c_read_instance(I2C_SERCOM, address, data, len);
}

void i2c_disable(void) {
    i2c_disable_instance(I2C_SERCOM);
}

#endif

void i2c_init_instance(uint8_t sercom, uint32_t baud) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;

    _sercom_clock_setup(sercom);

    /* Reset the SERCOM. */
    SERCOM->I2CM.CTRLA.bit.ENABLE = 0;
    while (SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};
    SERCOM->I2CM.CTRLA.bit.SWRST = 1;
    while (SERCOM->I2CM.SYNCBUSY.bit.SWRST || SERCOM->I2CM.SYNCBUSY.bit.ENABLE) {};

    /* Configure SERCOM for i2c master. */
    SERCOM->I2CM.CTRLA.bit.SWRST = 1;
    while (SERCOM->I2CM.SYNCBUSY.bit.SWRST || SERCOM->I2CM.CTRLA.bit.SWRST) {};

#if defined(_SAMD21_) || defined(_SAMD11_)
    SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0) |        // 0 is standard/fast mode 100 & 400kHz
                                     SERCOM_I2CM_CTRLA_SDAHOLD(0) |      // Hold SDA low for 300-600ns
                                     SERCOM_I2CM_CTRLA_MODE_I2C_MASTER;  // work as master
#else
    SERCOM->I2CM.CTRLA.reg = SERCOM_I2CM_CTRLA_SPEED(0) |        // 0 is standard/fast mode 100 & 400kHz
                                     SERCOM_I2CM_CTRLA_SDAHOLD(0) |      // Hold SDA low for 300-600ns
                                     SERCOM_I2CM_CTRLA_MODE(5);  // work as master
#endif
    /* Enable smart mode */
    SERCOM->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_SMEN;
    while (SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    /* Set baudrate.
        this is from Arduino:
            https://github.com/arduino/ArduinoCore-samd/blob/master/cores/arduino/SERCOM.cpp#L461
    */
    uint32_t clock_speed = get_cpu_frequency(); // Hz
    uint32_t rise_time = 300;                   // ns
    SERCOM->I2CM.BAUD.bit.BAUD =
        clock_speed / (2 * baud) - 5 - (((clock_speed / 1000000) * rise_time) / (2 * 1000));
    while (SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};
}

void i2c_enable_instance(uint8_t sercom) {
    /* Enable the SERCOM. */
    _sercom_enable(sercom);
    /* Put the bus into the idle state. */
    SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.BUSSTATE = BUSSTATE_IDLE;
    while (SERCOM_Peripherals[sercom].sercom->I2CM.SYNCBUSY.bit.SYSOP) {};
}

i2c_result_t i2c_write_instance(uint8_t sercom, uint8_t address, uint8_t* data, size_t len) {
    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return I2C_RESULT_ERR_BUSSTATE;
    }

    /* Address + write flag. */
    SERCOM_Peripherals[sercom].sercom->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0;

    /* This can hang forever, so put a timeout on it. */
    size_t w = 0;
    for (; w < 100000; w++) {
        if (SERCOM_Peripherals[sercom].sercom->I2CM.INTFLAG.bit.MB) {
            break;
        }
    }

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return I2C_RESULT_ERR_BUSSTATE;
    }
    if (SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.RXNACK) {
        return I2C_RESULT_ERR_ADDR_NACK;
    }

    /* Send data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Send data and wait for TX complete. */
        SERCOM_Peripherals[sercom].sercom->I2CM.DATA.bit.DATA = data[i];

        while (!SERCOM_Peripherals[sercom].sercom->I2CM.INTFLAG.bit.MB) {
            /* Check for loss of arbitration or a bus error. We can't continue if those happen. */
            /* BUSERR is set in addition to ARBLOST if arbitration is lost, so just check that one. */
            if (SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.BUSERR) {
                return I2C_RESULT_ERR_BUSERR;
            }
        }

        /* If a nack is received we can not continue sending data. */
        if (SERCOM_Peripherals[sercom].sercom->I2CM.STATUS.bit.RXNACK) {
            return I2C_RESULT_ERR_DATA_NACK;
        }
    }

    /* Send STOP command. */
    SERCOM_Peripherals[sercom].sercom->I2CM.CTRLB.bit.CMD = 3;
    while (SERCOM_Peripherals[sercom].sercom->I2CM.SYNCBUSY.bit.SYSOP) {};

    return I2C_RESULT_SUCCESS;
}

i2c_result_t i2c_read_instance(uint8_t sercom, uint8_t address, uint8_t* data, size_t len) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;

    /* Before trying to write, check to see if the bus is busy, if it is,
       bail.
    */
    if (SERCOM->I2CM.STATUS.bit.BUSSTATE == BUSSTATE_BUSY) {
        return I2C_RESULT_ERR_BUSSTATE;
    }

    /* Address + read flag. */
    SERCOM->I2CM.ADDR.bit.ADDR = (address << 0x1ul) | 0x1ul;
    
    // Prepare ACK
    SERCOM->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;

    /* This can hang forever, so put a timeout on it. */
    size_t w = 0;
    for (; w < 100000; w++) {
        if (SERCOM->I2CM.INTFLAG.bit.SB) {
            break;
        }
    }

    /* Check for loss of bus or NACK - in either case we can't continue. */
    if (SERCOM->I2CM.STATUS.bit.BUSSTATE != BUSSTATE_OWNER) {
        return I2C_RESULT_ERR_BUSSTATE;
    }
    if (SERCOM->I2CM.STATUS.bit.RXNACK) {
        return I2C_RESULT_ERR_ADDR_NACK;
    }

    /* Receive data bytes. */
    for (size_t i = 0; i < len; i++) {
        /* Receive data and wait for RX complete. */
        data[i] = SERCOM->I2CM.DATA.bit.DATA;
        while (!SERCOM->I2CM.INTFLAG.bit.SB);
    }

    /* Send STOP command, NACK */
    SERCOM->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT | SERCOM_I2CM_CTRLB_CMD(3);
    while (SERCOM->I2CM.SYNCBUSY.bit.SYSOP) {};

    return I2C_RESULT_SUCCESS;
}

void i2c_disable_instance(uint8_t sercom) {
    _sercom_disable(sercom);
}
