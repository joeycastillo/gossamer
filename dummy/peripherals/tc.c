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
#include "tc.h"
#include "sam.h"

static void tc_sync(uint8_t instance) {
}

bool tc_init(uint8_t instance, generic_clock_generator_t clocksource, tc_prescaler_value_t prescaler) {
    return false;
}

void tc_set_counter_mode(uint8_t instance, tc_counter_mode_t mode) {
}

void tc_set_run_in_standby(uint8_t instance, bool runStandby) {
}

#ifndef _SAMD11_
void tc_set_run_on_demand(uint8_t instance, bool onDemand) {
}
#endif

void tc_set_wavegen(uint8_t instance, tc_wavegen_t mode) {
}

void tc_set_channel_polarity(uint8_t instance, uint8_t channel, tc_channel_polarity_t polarity) {
}

void tc_enable(uint8_t instance) {
}

bool tc_is_enabled(uint8_t instance) {
    return false;
}

void tc_count8_set_period(uint8_t instance, uint8_t period) {
}

uint8_t tc_count8_get_period(uint8_t instance) {
    return 0;
}

void tc_count8_set_cc(uint8_t instance, uint8_t channel, uint8_t value) {
}

void tc_count16_set_cc(uint8_t instance, uint8_t channel, uint16_t value) {
}

void tc_count32_set_cc(uint8_t instance, uint8_t channel, uint32_t value) {
}

void tc_count8_set_count(uint8_t instance, uint8_t value) {
}

void tc_count16_set_count(uint8_t instance, uint16_t value) {
}

void tc_count32_set_count(uint8_t instance, uint32_t value) {
}

static void _tc_request_read_count(uint8_t instance) {
}

uint8_t tc_count8_get_count(uint8_t instance) {
    return 0;
}

uint16_t tc_count16_get_count(uint8_t instance) {
    return 0;
}

uint32_t tc_count32_get_count(uint8_t instance) {
    return 0;
}

void tc_set_event_action(uint8_t instance, tc_event_action_t action) {
}

void tc_stop(uint8_t instance) {
}

void tc_retrigger(uint8_t instance) {
}

void tc_disable(uint8_t instance) {
}

// TODO: every peripheral should have a deinit function, make public when that's done
/*
void tc_deinit(uint8_t instance) {
}
*/
