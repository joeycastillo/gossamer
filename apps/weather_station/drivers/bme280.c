/*
 * MIT License
 *
 * Copyright (c) 2023 Joey Castillo
 * Copyright (c) 2021 Brian Cooke (https://github.com/fivdi/pico-devices)
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
#include "bme280.h"
#include "pins.h"
#include "../peripherals/i2c.h"

typedef struct bme_calib_t {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    uint8_t dig_H1;
    int16_t dig_H2;
    uint8_t dig_H3;
    int16_t dig_H4;
    int16_t dig_H5;
    int8_t dig_H6;
} bme_calib_t;

static bme_calib_t calibration;

inline uint16_t make_le_16(uint16_t val) { return (val >> 8) | (val << 8); }

void bme280_i2c_write8(int16_t addr, uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;

    i2c_write(addr, (uint8_t *)&buf, 2);
}

static uint8_t bme280_i2c_read8(int16_t addr, uint8_t reg) {
    uint8_t data;

    i2c_write(addr, (uint8_t *)&reg, 1);
    i2c_read(addr, (uint8_t *)&data, 1);

    return data;
}

static uint16_t bme280_i2c_read16(int16_t addr, uint8_t reg) {
    uint16_t data;

    i2c_write(addr, (uint8_t *)&reg, 1);
    i2c_read(addr, (uint8_t *)&data, 2);

    return data;
}

static uint32_t bme280_i2c_read24(int16_t addr, uint8_t reg) {
    uint32_t data;
    data = 0;

    i2c_write(addr, (uint8_t *)&reg, 1);
    i2c_read(addr, (uint8_t *)&data, 3);

    return data << 8;
}

void bme280_begin(void) {
    bme280_i2c_write8(BME280_ADDRESS, BME280_REGISTER_SOFTRESET, BME280_SOFT_RESET_CODE);
    delay_ms(10);
}

static void _bme280_measure(void) {
    bme280_i2c_write8(BME280_ADDRESS, BME280_REGISTER_CONTROL_HUMID, BME280_CONTROL_HUMID_SAMPLING_X16);
    bme280_i2c_write8(BME280_ADDRESS, BME280_REGISTER_CONTROL, BME280_CONTROL_TEMPERATURE_SAMPLING_X16 |
                                                               BME280_CONTROL_PRESSURE_SAMPLING_X16 |
                                                               BME280_CONTROL_MODE_FORCED);
}

static uint16_t read_uint16_le(const uint8_t *data, uint8_t offset) {
    return (data[offset + 1] << 8) | data[offset];
}

static int16_t read_int16_le(const uint8_t *data, uint8_t offset) {
    return (data[offset + 1] << 8) | data[offset];
}

void bme280_read_calibration(void) {
    uint8_t calib_reg;
    uint8_t raw_cd[26];

    calib_reg = BME280_REGISTER_DIG_T1;
    i2c_write(BME280_ADDRESS, (uint8_t *)&calib_reg, 1);
    i2c_read(BME280_ADDRESS, (uint8_t *)&raw_cd, 26);

    calibration.dig_T1 = read_uint16_le(raw_cd, 0);
    calibration.dig_T2 = read_int16_le(raw_cd, 2);
    calibration.dig_T3 = read_int16_le(raw_cd, 4);

    calibration.dig_P1 = read_uint16_le(raw_cd, 6);
    calibration.dig_P2 = read_int16_le(raw_cd, 8);
    calibration.dig_P3 = read_int16_le(raw_cd, 10);
    calibration.dig_P4 = read_int16_le(raw_cd, 12);
    calibration.dig_P5 = read_int16_le(raw_cd, 14);
    calibration.dig_P6 = read_int16_le(raw_cd, 16);
    calibration.dig_P7 = read_int16_le(raw_cd, 18);
    calibration.dig_P8 = read_int16_le(raw_cd, 20);
    calibration.dig_P9 = read_int16_le(raw_cd, 22);

    calibration.dig_H1 = raw_cd[25];

    calib_reg = BME280_REGISTER_DIG_H2;
    i2c_write(BME280_ADDRESS, (uint8_t *)&calib_reg, 1);
    i2c_read(BME280_ADDRESS, (uint8_t *)&raw_cd, 7);

    calibration.dig_H2 = read_int16_le(raw_cd, 0);
    calibration.dig_H3 = raw_cd[2];
    calibration.dig_H4 = ((int8_t) raw_cd[3] << 4) | (raw_cd[4] & 0x0f);
    calibration.dig_H5 = ((int8_t) raw_cd[5] << 4) | (raw_cd[4] >> 4);
    calibration.dig_H6 = (int8_t) raw_cd[6];
}

bool bme280_is_measuring(void) {
    return (bme280_i2c_read8(BME280_ADDRESS, BME280_REGISTER_STATUS) & BME280_STATUS_UPDATING_MASK) != 0;
}

typedef struct {
    uint32_t temperature;
    uint32_t pressure;
    uint16_t humidity;
} bme280_raw_data_t;

static void read_raw_data(bme280_raw_data_t *raw_data) {
    uint8_t data[8];

    uint8_t reg = BME280_REGISTER_PRESSURE_DATA;
    i2c_write(BME280_ADDRESS, (uint8_t *)&reg, 1);
    i2c_read(BME280_ADDRESS, (uint8_t *)&data, 8);

    raw_data->pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
    raw_data->temperature = data[3] << 12 | data[4] << 4 | data[5] >> 4;
    raw_data->humidity = data[6] << 8 | data[7];
}

static float compensate_temperature(uint32_t rawt) {
    return (((float) rawt / 16384.0f - (float) calibration.dig_T1 / 1024.0f) * (float) calibration.dig_T2) +
            (((float) rawt / 131072.0f - (float) calibration.dig_T1 / 8192.0f) *
             ((float) rawt / 131072.0f - (float) calibration.dig_T1 / 8192.0f) * (float) calibration.dig_T3);
}

static float compensate_pressure(uint32_t rawp, float t_fine) {
    float var1 = t_fine / 2.0f - 64000.0f;
    float var2 = var1 * var1 * (float) calibration.dig_P6 / 32768.0f;
    var2 = var2 + var1 * (float) calibration.dig_P5 * 2.0f;
    var2 = (var2 / 4.0f) + ((float) calibration.dig_P4 * 65536.0f);
    var1 = ((float) calibration.dig_P3 * var1 * var1 / 524288.0f + (float) calibration.dig_P2 * var1) / 524288.0f;
    var1 = (1.0f + var1 / 32768.0f) * (float) calibration.dig_P1;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }

    float p = 1048576.0f - (float) rawp;
    p = (p - (var2 / 4096.0f)) * 6250.0f / var1;
    var1 = (float) calibration.dig_P9 * p * p / 2147483648.0f;
    var2 = p * (float) calibration.dig_P8 / 32768.0f;
    p = p + (var1 + var2 + (float) calibration.dig_P7) / 16.0f;

    return p;
}

static float compensate_humidity(uint32_t rawh, float t_fine) {
    float h = t_fine - 76800.0f;

    h = ((float) rawh - ((float) calibration.dig_H4 * 64.0f + (float) calibration.dig_H5 / 16384.0f * h)) *
        ((float) calibration.dig_H2 / 65536.0f * (1.0f + (float) calibration.dig_H6 / 67108864.0f * h * (1.0f + (float) calibration.dig_H3 / 67108864.0f * h)));

    h = h * (1.0f - (float) calibration.dig_H1 * h / 524288.0f);

    return h;
}

void bme280_read(bme280_reading_t *reading) {
    bme280_raw_data_t raw_data;

    _bme280_measure();
    read_raw_data(&raw_data);

    // Temperature
    const float t_fine = compensate_temperature(raw_data.temperature);
    float temperature = t_fine / 5120.0f;

    if (temperature < -40.0f) {
        temperature = -40.0f;
    } else if (temperature > 85.0f) {
        temperature = 85.0f;
    }

    reading->temperature = temperature;

    // Pressure
    float pressure = reading->pressure = compensate_pressure(raw_data.pressure, t_fine);

    if (pressure < 30000.0f) {
        pressure = 30000.0f;
    } else if (pressure > 110000.0f) {
        pressure = 110000.0f;
    }

    reading->pressure = pressure / 100;

    // Humidity
    float humidity = compensate_humidity(raw_data.humidity, t_fine);

    if (humidity < 0.0f) {
        humidity = 0.0f;
    } else if (humidity > 100.0f) {
        humidity = 100.0f;
    }

    reading->humidity = humidity;
}
