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
#include "system.h"
#include "tcc.h"

static void tcc_sync(uint8_t instance) {
}

bool tcc_init(uint8_t instance, generic_clock_generator_t clocksource, tcc_prescaler_value_t prescaler) {
    return 0;
}

void tcc_set_run_in_standby(uint8_t instance, bool runStandby) {
}

void tcc_set_wavegen(uint8_t instance, tcc_wavegen_t mode) {
}

void tcc_set_output_matrix(uint8_t instance, tcc_output_matrix_t mode) {
}

void tcc_set_channel_polarity(uint8_t instance, uint8_t channel, tcc_channel_polarity_t polarity) {
}

void tcc_enable(uint8_t instance) {
}

bool tcc_is_enabled(uint8_t instance) {
    return false;
}

void tcc_set_period(uint8_t instance, uint32_t period, bool buffered) {
}

uint32_t tcc_get_period(uint8_t instance) {
    return 0;
}

void tcc_set_cc(uint8_t instance, uint8_t channel, uint32_t value, bool buffered) {
}

void tcc_set_count(uint8_t instance, uint32_t value) {
}

uint32_t tcc_get_count(uint8_t instance) {
    return 0;
}

void tcc_stop(uint8_t instance) {
}

void tcc_retrigger(uint8_t instance) {
}

void tcc_update(uint8_t instance) {
}

void tcc_disable(uint8_t instance) {
}

// TODO: every peripheral should have a deinit function, make public when that's done
/*
void tcc_deinit(uint8_t instance) {
}
*/
