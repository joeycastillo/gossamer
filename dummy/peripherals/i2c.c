/*
    Cribbed from the Castor & Pollux Gemini firmware:
    https://github.com/wntrblm/Castor_and_Pollux/
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "pins.h"
#include "system.h"
#include "sercom.h"
#include "i2c.h"

#if defined(I2C_SERCOM)

void i2c_init(void) {
}

void i2c_enable(void) {
}

i2c_result_t i2c_write(uint8_t address, uint8_t* data, size_t len) {
    return 0;
}

i2c_result_t i2c_read(uint8_t address, uint8_t* data, size_t len) {
    return 0;
}

void i2c_disable(void) {
}

#endif

void i2c_init_instance(uint8_t sercom, uint32_t baud) {
}

void i2c_enable_instance(uint8_t sercom) {
}

i2c_result_t i2c_write_instance(uint8_t sercom, uint8_t address, uint8_t* data, size_t len) {
    return 0;
}

i2c_result_t i2c_read_instance(uint8_t sercom, uint8_t address, uint8_t* data, size_t len) {
    return 0;
}

void i2c_disable_instance(uint8_t sercom) {
}
