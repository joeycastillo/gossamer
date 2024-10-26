/*
 * MIT License
 *
 * Copyright (c) 2024 Joey Castillo
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
#include "evsys.h"

void evsys_configure_channel(uint8_t channel, uint8_t generator, uint8_t user, bool run_in_standby, bool asynchronous) {
#if defined(_SAMD11_) || defined(_SAMD21_) || defined(_SAMD51_)
    // TODO: these devices have a different way of configuring events
    (void)channel;
    (void)generator;
    (void)user;
    (void)run_in_standby;
    (void)asynchronous;
#else
    EVSYS->USER[user].reg = EVSYS_USER_CHANNEL(channel + 1);
    EVSYS->CHANNEL[0].reg = EVSYS_CHANNEL_EVGEN(generator) |
                            (run_in_standby ? EVSYS_CHANNEL_RUNSTDBY : 0) |
                            (asynchronous ? EVSYS_CHANNEL_PATH_ASYNCHRONOUS : 0);
#endif
}
