/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
 * Based heavily on Adafruit_ZeroDMA
 * Written by Phil "PaintYourDragon" Burgess for Adafruit Industries,
 * based partly on DMA insights from Atmel ASFCORE 3.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
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

#include <string.h>
#include <malloc.h>
#include "pins.h"
#include "system.h"
#include "dma.h"

void dma_init(void) {
}

bool dma_configure(gossamer_dma_job_t *dmaJob, uint8_t peripheralTrigger, dma_trigger_action_t triggerAction, dma_configuration_flags_t flags) {
    return false;
}

DmacDescriptor *dma_add_descriptor(gossamer_dma_job_t *dmaJob, void *src, void *dst, uint32_t count, dma_beat_size_t size, dma_address_increment_t addressIncrement, dma_stepsize_t stepSize, dma_stepsel_t stepSel) {
    return 0;
}

bool dma_start_job(gossamer_dma_job_t *dmaJob) {
    return false;
}

void DMAC_Handler(void) {
}
