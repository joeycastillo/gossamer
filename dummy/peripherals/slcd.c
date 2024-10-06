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

#include "slcd.h"

#ifdef SLCD

static void _slcd_sync(uint32_t reg) {
}

void slcd_init(uint64_t lcd_pins, slcd_bias_value_t bias, slcd_duty_value_t duty, slcd_clocksource_value_t clocksource, slcd_prescaler_value_t prescaler, slcd_clockdiv_value_t clkdiv) {
}

void slcd_set_contrast(uint8_t contrast) {
}

void slcd_enable(void) {
}

void slcd_clear(void) {
}

void slcd_set_segment(uint8_t com, uint8_t seg) {
}

void slcd_clear_segment(uint8_t com, uint8_t seg) {
}

void slcd_configure_frame_counter(uint8_t fc, uint8_t overflow_count, bool prescale) {
}

void slcd_set_frame_counter_enabled(uint8_t fc, bool enabled) {
}

void slcd_configure_blink(bool blink_all, uint8_t bss0, uint8_t bss1, uint8_t fc) {
}

void slcd_set_blink_enabled(bool enabled) {
}

void slcd_configure_circular_shift_animation(uint16_t initial_value, uint8_t size, slcd_csrshift_value_t shift_dir, uint8_t fc) {
}

void slcd_set_circular_shift_animation_enabled(bool enabled) {
}

void slcd_disable(void) {
}

#endif // SLCD