/*
 * MIT License
 *
 * Copyright (c) 2021 Igor Levkov
 * Copyright (c) 2023 Joey Castillo
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

#include "sam.h"
#include <string.h>
#include <ctype.h>
#include "ltr390.h"
#include "pins.h"
#include "../peripherals/i2c.h"

#define LTR390_ADDRESS 0x53

static const float gain_factor[5] = {1, 3, 6, 9, 18};
static const float res_factor[6] = {4, 2, 1, 0.5, 0.25, 0.03125};

static uint8_t writeRegister(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    i2c_write(LTR390_ADDRESS, (uint8_t *)&buf, 2);
}

static uint8_t readRegister(uint8_t reg) {
    uint8_t data;

    i2c_write(LTR390_ADDRESS, (uint8_t *)&reg, 1);
    i2c_read(LTR390_ADDRESS, (uint8_t *)&data, 1);

    return data;
}

bool ltr390_init() {
    uint8_t part_id = readRegister(LTR390_PART_ID);

    if ((part_id  >> 4) != 0x0B) {
        return false;
    }

    if (!ltr390_reset()) {
        return false;
    }

    ltr390_enable(true);
    if (!ltr390_enabled()) {
        return false;
    }

    ltr390_setGain(LTR390_GAIN_3);
    ltr390_setResolution(LTR390_RESOLUTION_16BIT);

    return true;
}

bool ltr390_reset(void) {
    uint8_t _r = readRegister(LTR390_MAIN_CTRL);
    _r |= 0b00010000;
    writeRegister(LTR390_MAIN_CTRL, _r);
    delay_ms(10);
    _r = readRegister(LTR390_MAIN_CTRL);
    if (_r != 0) {
        return false;
    }

    return true;
}

void ltr390_enable(bool en) {
    uint8_t _r = readRegister(LTR390_MAIN_CTRL);
    _r |= (1 << 1);
    writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
}

bool ltr390_enabled(void) {
    uint8_t _r = readRegister(LTR390_MAIN_CTRL);
    _r >>= 1;
    _r &= 1;
    return _r;
}

void ltr390_setMode(ltr390_mode_t mode) {
    uint8_t _r = readRegister(LTR390_MAIN_CTRL);
    _r &= 0xF7;
    _r |= ((uint8_t)mode << 3);
    writeRegister(LTR390_MAIN_CTRL, (uint8_t)_r);
}

ltr390_mode_t ltr390_getMode(void) {
    uint8_t _r = readRegister(LTR390_MAIN_CTRL);
    _r >>= 3;
    _r &= 1;
    return (ltr390_mode_t)_r;
}

void ltr390_setGain(ltr390_gain_t gain) {
    writeRegister(LTR390_GAIN, (uint8_t)gain);
}

ltr390_gain_t ltr390_getGain(void) {
    uint8_t _r = readRegister(LTR390_GAIN);
    _r &= 7;
    return (ltr390_gain_t)_r;
}

void ltr390_setResolution(ltr390_resolution_t res) {
    uint8_t _r = 0;
    _r |= (res << 4);
    writeRegister(LTR390_MEAS_RATE, (uint8_t)_r);
}

ltr390_resolution_t ltr390_getResolution(void) {
    uint8_t _r = readRegister(LTR390_MEAS_RATE);
    _r &= 0x70;
    _r = 7 & (_r >> 4);
    return (ltr390_resolution_t)_r;
}

bool ltr390_newDataAvailable(void) {
    uint8_t status = readRegister(LTR390_MAIN_STATUS);
    status >>= 3;
    status &= 1;
    return status; 
}

uint32_t ltr390_readUVS(void) {
    uint8_t _lsb = readRegister(LTR390_UVSDATA_LSB);
    uint8_t _msb = readRegister(LTR390_UVSDATA_MSB);
    uint8_t _hsb = readRegister(LTR390_UVSDATA_HSB);
    _hsb &= 0x0F;
    uint32_t _out = ((uint32_t)_hsb << 16) | ((uint16_t)_msb << 8) | _lsb;
    return _out;
}

uint32_t ltr390_readALS(void) {
    uint8_t _lsb = readRegister(LTR390_ALSDATA_LSB);
    uint8_t _msb = readRegister(LTR390_ALSDATA_MSB);
    uint8_t _hsb = readRegister(LTR390_ALSDATA_HSB);
    _hsb &= 0x0F;
    uint32_t _out = ((uint32_t)_hsb << 16) | ((uint16_t)_msb << 8) | _lsb;
    return _out;
}
