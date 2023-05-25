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
#include "dma_util.h"

typedef struct {
    uint8_t channel;                            // (0 to DMAC_CH_NUM-1, or 0xFF)
    volatile dma_status_t jobStatus;            // Last known DMA job status
    bool hasDescriptors;                        // 'true' if one or more descriptors assigned
    bool loopFlag;                              // 'true' if descriptor chain loops back to start
    void (*callbacks[DMA_CALLBACK_N])(void *);  // Callback function (is passed an instance of this struct)
} gossamer_dma_job_t;

/**
 * @brief Initialize the DMA system.
 * @details Sets up peripheral clocks and points the DMA controller at an empty
 *          list of descriptors.
 *          
 */
void dma_init(void);

/**
 * @brief Configures a DMA job.
 * @details Configures a DMA job with the given peripheral trigger and trigger action.
 *          You must configure the job before adding descriptors to it.
 * @param dmaJob A pointer to the DMA job to configure. This will be modified in place.
 * @param peripheralTrigger The peripheral trigger to use for this job. These are
 *                          defined in your microcontroller's data sheet, in the DMAC's
 *                          register details section, under a headling like "Trigger Source"
 * @param triggerAction The trigger action to use for this job. One of: 
 *                      * DMA_TRIGGER_ACTION_BLOCK
 *                      * DMA_TRIGGER_ACTION_BEAT
 *                      * DMA_TRIGGER_ACTION_TRANSACTION
 * @param flags A OR'ed bitmask of flags to use for this job. Available flags are:
 *              * DMA_CONFIG_LOOP - Loop the job when it completes
 *              * DMA_CONFIG_RUNSTDBY - Keep the job running in standby mode
 */
bool dma_configure(gossamer_dma_job_t *dmaJob, uint8_t peripheralTrigger, dma_trigger_action_t triggerAction, dma_configuration_flags_t flags);

/**
 * @brief Adds a descriptor to a DMA job.
 * @details Adds a descriptor to a DMA job. You must configure the job before adding
 *         descriptors to it.
 * @param dmaJob A pointer to the DMA job. Must be previously configured with dma_configure().
 * @param src The source address for this descriptor.
 * @param dst The destination address for this descriptor.
 * @param count The number of beats to transfer.
 * @param size The beat size. One of:
 *             * DMA_BEAT_SIZE_BYTE - One byte per beat
 *             * DMA_BEAT_SIZE_HWORD - One half word (two bytes) per beat
 *             * DMA_BEAT_SIZE_WORD - One word (four bytes) per beat
 * @param srcInc Whether to increment the source address after each beat.
 * @param dstInc Whether to increment the destination address after each beat.
 * @param stepSize Address increment step size. One of:
 *                 * DMA_STEPSIZE_1 - Increment by 1 * beat size
 *                 * DMA_STEPSIZE_2 - Increment by 2 * beat size
 *                 * DMA_STEPSIZE_4 - Increment by 4 * beat size
 *                 * DMA_STEPSIZE_8 - ...you get the idea
 *                 * DMA_STEPSIZE_16
 *                 * DMA_STEPSIZE_32
 *                 * DMA_STEPSIZE_64
 *                 * DMA_STEPSIZE_128
 * @param stepSel Which address to increment by stepSize after each step. You can only have one:
 *                * DMA_STEPSEL_SOURCE - Increment the source address after each step
 *                * DMA_STEPSEL_DESTINATION - Increment the destination address after each step
 * 
*/
DmacDescriptor *dma_add_descriptor(gossamer_dma_job_t *dmaJob, void *src, void *dst, uint32_t count, dma_beat_size_t size, dma_address_increment_t addressIncrement, dma_stepsize_t stepSize, dma_stepsel_t stepSel);

bool dma_start_job(gossamer_dma_job_t *dmaJob);
