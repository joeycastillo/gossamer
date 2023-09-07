/*
 * MIT License
 *
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
#include "pmsa003i.h"
#include "pins.h"
#include "../peripherals/i2c.h"

#define PMSA003I_ADDRESS 0x12

bool pmsa003i_get_data(pms_aqi_data_t *data) {
    uint8_t buffer[32];
    uint16_t sum = 0;

    I2CResult result = i2c_read(PMSA003I_ADDRESS, (uint8_t *)&buffer, 32);

    if (result != I2C_RESULT_SUCCESS) {
        return false;
    }

    if (!data) return false;

    for (uint8_t i = 0; i < 30; i++) {
        sum += buffer[i];
    }

    uint16_t buffer_u16[15];
    for (uint8_t i = 0; i < 15; i++) {
        buffer_u16[i] = buffer[2 + i * 2 + 1];
        buffer_u16[i] += (buffer[2 + i * 2] << 8);
    }

    memcpy((void *)data, (void *)buffer_u16, 30);

    if (sum != data->checksum) return false;

    return true;
}
