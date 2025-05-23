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

#include <stddef.h>
#include "eic.h"
#include "hal_gpio.h"

static void _eic_sync(void) {
}

void eic_init(void) {
}

void eic_enable(void) {
}

bool eic_is_enabled(void) {
    return true;
}

int8_t eic_configure_pin(const uint8_t pin, eic_interrupt_trigger_t trigger, bool filten) {
    return 0;
}

bool eic_enable_interrupt(const uint8_t pin) {
    return false;
}

bool eic_disable_interrupt(const uint8_t pin) {
    return false;
}

bool eic_enable_event(const uint8_t pin) {
    return false;
}

bool eic_disable_event(const uint8_t pin) {
    return false;
}

void eic_disable(void) {
}

void eic_configure_callback(eic_cb_t callback) {
}

void irq_handler_eic(void);
void irq_handler_eic(void) {
}
