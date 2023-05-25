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

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <malloc.h> // memalign() function
#include <stdlib.h>
#include "dma_util.h"

typedef enum {
  DMA_STATUS_OK = 0,
  DMA_STATUS_ERR_NOT_FOUND,
  DMA_STATUS_ERR_NOT_INITIALIZED,
  DMA_STATUS_ERR_INVALID_ARG,
  DMA_STATUS_ERR_IO,
  DMA_STATUS_ERR_TIMEOUT,
  DMA_STATUS_BUSY,
  DMA_STATUS_SUSPEND,
  DMA_STATUS_ABORTED,
  DMA_STATUS_JOBSTATUS = -1 // For printStatus() function
} gossamer_dma_status_t;

typedef struct {
    uint8_t channel;                            // (0 to DMAC_CH_NUM-1, or 0xFF)
    volatile gossamer_dma_status_t jobStatus;   // Last known DMA job status
    bool hasDescriptors;                        // 'true' if one or more descriptors assigned
    bool loopFlag;                              // 'true' if descriptor chain loops back to start
    void (*callbacks[DMA_CALLBACK_N])(void *); // Callback func *s
} gossamer_dma_job_t;


void dma_init(void);

bool dma_configure(gossamer_dma_job_t *dmaJob, uint8_t peripheralTrigger, uint8_t triggerAction, bool looping);

DmacDescriptor *dma_add_descriptor(gossamer_dma_job_t *dmaJob, void *src, void *dst, uint32_t count, dma_beat_size size, bool srcInc, bool dstInc, uint32_t stepSize, bool stepSel);

bool dma_start_job(gossamer_dma_job_t *dmaJob);
