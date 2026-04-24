/*
 * MIT License
 *
 * Copyright (c) 2026 Joey Castillo
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
#include "sercom.h"
#include "i2cd.h"

static i2c_device_address_cb_t _on_address_match = NULL;
static i2c_device_data_received_cb_t _on_data_received = NULL;
static i2c_device_data_requested_cb_t _on_data_requested = NULL;
static i2c_device_stop_cb_t _on_stop = NULL;
static i2c_device_error_cb_t _on_error = NULL;

#if defined(I2C_SERCOM)

void i2c_device_init(uint8_t address) {
    i2c_device_init_instance(I2C_SERCOM, address);
}

void i2c_device_enable(void) {
    i2c_device_enable_instance(I2C_SERCOM);
}

bool i2c_device_is_enabled(void) {
    return i2c_device_is_enabled_instance(I2C_SERCOM);
}

void i2c_device_set_callbacks(
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error) {
    i2c_device_set_callbacks_instance(I2C_SERCOM,
        on_address_match, on_data_received, on_data_requested,
        on_stop, on_error);
}

void i2c_device_disable(void) {
    i2c_device_disable_instance(I2C_SERCOM);
}

#endif

void i2c_device_init_instance(uint8_t sercom, uint8_t address) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;

    _sercom_clock_setup(sercom);

    /* Reset the SERCOM. */
    SERCOM->I2CS.CTRLA.bit.ENABLE = 0;
    while (SERCOM->I2CS.SYNCBUSY.bit.ENABLE) {};
    SERCOM->I2CS.CTRLA.bit.SWRST = 1;
    while (SERCOM->I2CS.SYNCBUSY.bit.SWRST || SERCOM->I2CS.CTRLA.bit.SWRST) {};

    /* Configure SERCOM for I2C device. */
#if defined(_SAMD21_) || defined(_SAMD11_)
    SERCOM->I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_SDAHOLD(0) |
                             SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;
#else
    SERCOM->I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_SDAHOLD(0) |
                             SERCOM_I2CS_CTRLA_MODE(4);
#endif

    /* Enable smart mode: reading/writing DATA auto-ACKs and clears DRDY. */
    SERCOM->I2CS.CTRLB.reg = SERCOM_I2CS_CTRLB_SMEN;

    /* Set our device address (7-bit). */
    SERCOM->I2CS.ADDR.reg = SERCOM_I2CS_ADDR_ADDR(address);

    /* Enable interrupts for address match, data ready, stop, and error. */
    SERCOM->I2CS.INTENSET.reg = SERCOM_I2CS_INTENSET_AMATCH |
                                SERCOM_I2CS_INTENSET_DRDY |
                                SERCOM_I2CS_INTENSET_PREC |
                                SERCOM_I2CS_INTENSET_ERROR;

    NVIC_ClearPendingIRQ(SERCOM_Peripherals[sercom].interrupt_line);
    NVIC_EnableIRQ(SERCOM_Peripherals[sercom].interrupt_line);
}

void i2c_device_enable_instance(uint8_t sercom) {
    _sercom_enable(sercom);
}

bool i2c_device_is_enabled_instance(uint8_t sercom) {
    return _sercom_is_enabled(sercom);
}

void i2c_device_set_callbacks_instance(
    uint8_t sercom,
    i2c_device_address_cb_t on_address_match,
    i2c_device_data_received_cb_t on_data_received,
    i2c_device_data_requested_cb_t on_data_requested,
    i2c_device_stop_cb_t on_stop,
    i2c_device_error_cb_t on_error) {
    (void)sercom;
    _on_address_match = on_address_match;
    _on_data_received = on_data_received;
    _on_data_requested = on_data_requested;
    _on_stop = on_stop;
    _on_error = on_error;
}

void i2c_device_disable_instance(uint8_t sercom) {
    NVIC_DisableIRQ(SERCOM_Peripherals[sercom].interrupt_line);
    _sercom_disable(sercom);
}

void i2c_device_irq_handler(uint8_t sercom) {
    Sercom* SERCOM = SERCOM_Peripherals[sercom].sercom;
    uint8_t intflag = SERCOM->I2CS.INTFLAG.reg;

    if (intflag & SERCOM_I2CS_INTFLAG_ERROR) {
        if (_on_error) _on_error(sercom);
        /* Clear error status bits (write-1-to-clear). */
        SERCOM->I2CS.STATUS.reg = SERCOM_I2CS_STATUS_BUSERR |
                                  SERCOM_I2CS_STATUS_COLL;
        SERCOM->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR;
        return;
    }

    if (intflag & SERCOM_I2CS_INTFLAG_AMATCH) {
        /* Clear any stale stop flag from a previous transaction. */
        if (intflag & SERCOM_I2CS_INTFLAG_PREC) {
            SERCOM->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
        }
        bool direction = SERCOM->I2CS.STATUS.bit.DIR;
        if (_on_address_match) _on_address_match(sercom, direction);
        /* ACK the address and release SCL. */
        SERCOM->I2CS.CTRLB.bit.ACKACT = 0;
        SERCOM->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
        return;
    }

    if (intflag & SERCOM_I2CS_INTFLAG_DRDY) {
        if (SERCOM->I2CS.STATUS.bit.DIR) {
            /* Controller read: provide data.
               Writing DATA clears DRDY and sends the byte. */
            uint8_t byte = _on_data_requested ? _on_data_requested(sercom) : 0x00;
            SERCOM->I2CS.DATA.reg = byte;
        } else {
            /* Controller write: receive data.
               Reading DATA in smart mode auto-ACKs and clears DRDY. */
            uint8_t data = SERCOM->I2CS.DATA.reg;
            if (_on_data_received) _on_data_received(sercom, data);
        }
        return;
    }

    if (intflag & SERCOM_I2CS_INTFLAG_PREC) {
        if (_on_stop) _on_stop(sercom);
        SERCOM->I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
        return;
    }
}
