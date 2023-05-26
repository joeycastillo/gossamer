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
#include "sam.h"
#include "system.h"
#include "dma.h"

static volatile uint32_t _channelMask = 0; // Bitmask of allocated channels

// DMA descriptor list entry point (and writeback buffer) per channel
__attribute__((__aligned__(16))) static DmacDescriptor ///< 128 bit alignment
    _descriptor[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR,  ///< Descriptor table
    _writeback[DMAC_CH_NUM] SECTION_DMAC_DESCRIPTOR;   ///< Writeback table

gossamer_dma_job_t *_dmaPtr[DMAC_CH_NUM] = {0};

// Adapted from ASF3 interrupt_sam_nvic.c:

static volatile unsigned long cpu_irq_critical_section_counter = 0;
static volatile unsigned char cpu_irq_prev_interrupt_state = 0;

static void cpu_irq_enter_critical(void) {
    if (!cpu_irq_critical_section_counter) {
        if (__get_PRIMASK() == 0) { // IRQ enabled?
            __disable_irq();          // Disable it
            __DMB();
            cpu_irq_prev_interrupt_state = 1;
        } else {
            // Make sure the to save the prev state as false
            cpu_irq_prev_interrupt_state = 0;
        }
    }

    cpu_irq_critical_section_counter++;
}

static void cpu_irq_leave_critical(void) {
    // Check if the user is trying to leave a critical section
    // when not in a critical section
    if (cpu_irq_critical_section_counter > 0) {
        cpu_irq_critical_section_counter--;

        // Only enable global interrupts when the counter
        // reaches 0 and the state of the global interrupt flag
        // was enabled when entering critical state */
        if ((!cpu_irq_critical_section_counter) && cpu_irq_prev_interrupt_state) {
            __DMB();
            __enable_irq();
        }
    }
}


void dma_init(void) {
#if defined(_SAMD21_) || defined(_SAMD11_)
    PM->AHBMASK.bit.DMAC_ = 1;
    PM->APBBMASK.bit.DMAC_ = 1;
#else
    MCLK->AHBMASK.reg |= MCLK_AHBMASK_DMAC;
#endif
    DMAC->CTRL.bit.DMAENABLE = 0;
    DMAC->CTRL.bit.SWRST = 1;

    // Initialize descriptor list addresses
    DMAC->BASEADDR.bit.BASEADDR = (uint32_t)_descriptor;
    DMAC->WRBADDR.bit.WRBADDR = (uint32_t)_writeback;
    memset(_descriptor, 0, sizeof(_descriptor));
    memset(_writeback, 0, sizeof(_writeback));

    // Re-enable DMA controller with all priority levels
    DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xF);

    // Enable DMA interrupt at lowest priority
    NVIC_EnableIRQ(DMAC_IRQn);
    NVIC_SetPriority(DMAC_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
}

bool dma_configure(gossamer_dma_job_t *dmaJob, uint8_t peripheralTrigger, dma_trigger_action_t triggerAction, dma_configuration_flags_t flags) {
    uint8_t channel;
    for (channel = 0; (channel < DMAC_CH_NUM) && (_channelMask & (1 << channel)); channel++) {
        // do nothing, just find the first available channel
    }
    if (channel >= DMAC_CH_NUM) return false;

    cpu_irq_enter_critical();

    _channelMask |= 1 << channel;   // Mark channel as allocated
    _dmaPtr[channel] = dmaJob;   // Channel-index-to-object pointer

    // Reset the allocated channel
    DMAC->CHID.bit.ID = channel;
    DMAC->CHCTRLA.bit.ENABLE = 0;
    DMAC->CHCTRLA.bit.SWRST = 1;

    // Clear software trigger
    DMAC->SWTRIGCTRL.reg &= ~(1 << channel);

    // Configure default behaviors
    DMAC->CHCTRLB.bit.LVL = 0;
    DMAC->CHCTRLB.bit.TRIGSRC = peripheralTrigger;
    DMAC->CHCTRLB.bit.TRIGACT = triggerAction;

#if defined(_SAML21_) || defined(_SAML22_)
    // run in standby if asked
    DMAC->CHCTRLA.bit.RUNSTDBY = !!(flags & DMA_CONFIG_RUNSTDBY);
#endif

    cpu_irq_leave_critical();

    dmaJob->channel = channel;
    dmaJob->jobStatus = DMA_STATUS_OK;
    dmaJob->hasDescriptors = false; // No descriptors allocated yet
    dmaJob->loopFlag = !!(flags & DMA_CONFIG_LOOP);
    memset(dmaJob->callbacks, 0, sizeof(dmaJob->callbacks));

    return true;
}

DmacDescriptor *dma_add_descriptor(gossamer_dma_job_t *dmaJob, void *src, void *dst, uint32_t count, dma_beat_size_t size, dma_address_increment_t addressIncrement, dma_stepsize_t stepSize, dma_stepsel_t stepSel) {
    if (dmaJob->channel >= DMAC_CH_NUM)
        return NULL;

    // Can't do while job's busy
    if (dmaJob->jobStatus == DMA_STATUS_BUSY)
        return NULL;

    DmacDescriptor *desc;

    // Scan descriptor list to find last entry.  If an entry's
    // DESCADDR value is 0, that's the end of the list and it's
    // currently un-looped.  If the DESCADDR value is the same
    // as the first entry, that's the end of the list and it's
    // looped.  Either way, set the last entry's DESCADDR value
    // to the new descriptor, and the descriptor's own DESCADDR
    // will be set later either to 0 or the list head.
    if (dmaJob->hasDescriptors) {
        // DMA descriptors must be 128-bit (16 byte) aligned.
        // memalign() is considered 'obsolete' but it's replacements
        // (aligned_alloc() or posix_memalign()) are not currently
        // available in the version of ARM GCC in use, but this is,
        // so here we are.
        if (!(desc = (DmacDescriptor *)memalign(16, sizeof(DmacDescriptor))))
            return NULL;
        DmacDescriptor *prev = &_descriptor[dmaJob->channel];
        while (prev->DESCADDR.reg && (prev->DESCADDR.reg != (uint32_t)&_descriptor[dmaJob->channel])) {
            prev = (DmacDescriptor *)prev->DESCADDR.reg;
        }
        prev->DESCADDR.reg = (uint32_t)desc;
    }
    else {
        desc = &_descriptor[dmaJob->channel];
    }
    dmaJob->hasDescriptors = true;

    uint8_t bytesPerBeat; // Beat transfer size IN BYTES
    switch (size) {
        default:
            bytesPerBeat = 1;
            break;
        case DMA_BEAT_SIZE_HWORD:
            bytesPerBeat = 2;
            break;
        case DMA_BEAT_SIZE_WORD:
            bytesPerBeat = 4;
            break;
    }

    desc->BTCTRL.bit.VALID = true;
    desc->BTCTRL.bit.EVOSEL = DMA_EVENT_OUTPUT_DISABLE;
    desc->BTCTRL.bit.BLOCKACT = DMA_BLOCK_ACTION_NONE;
    desc->BTCTRL.bit.BEATSIZE = size;
    desc->BTCTRL.bit.SRCINC = !!(addressIncrement & DMA_INCREMENT_SOURCE);
    desc->BTCTRL.bit.DSTINC = !!(addressIncrement & DMA_INCREMENT_DESTINATION);
    desc->BTCTRL.bit.STEPSEL = stepSel;
    desc->BTCTRL.bit.STEPSIZE = stepSize;
    desc->BTCNT.reg = count;
    desc->SRCADDR.reg = (uint32_t)src;

    if (addressIncrement & DMA_INCREMENT_SOURCE) {
        if (stepSel) {
            desc->SRCADDR.reg += bytesPerBeat * count * (1 << stepSize);
        } else {
            desc->SRCADDR.reg += bytesPerBeat * count;
        }
    }

    desc->DSTADDR.reg = (uint32_t)dst;

    if (addressIncrement & DMA_INCREMENT_DESTINATION) {
        if (!stepSel) {
            desc->DSTADDR.reg += bytesPerBeat * count * (1 << stepSize);
        }
        else {
            desc->DSTADDR.reg += bytesPerBeat * count;
        }
    }

    desc->DESCADDR.reg = dmaJob->loopFlag ? (uint32_t)&_descriptor[dmaJob->channel] : 0;

    return desc;
}

bool dma_start_job(gossamer_dma_job_t *dmaJob) {
    bool status = true;

    cpu_irq_enter_critical(); // Job status is volatile

    if (dmaJob->jobStatus == DMA_STATUS_BUSY) {
        status = false; // Resource is busy
    } else if (dmaJob->channel >= DMAC_CH_NUM) {
        status = false; // Channel not in use
    } else if (!dmaJob->hasDescriptors || (_descriptor[dmaJob->channel].BTCNT.reg <= 0)) {
        status = false; // Bad transfer size
    } else {
        uint8_t i, interruptMask = 0;
        for (i = 0; i < DMA_CALLBACK_N; i++)
            if (dmaJob->callbacks[i])
                interruptMask |= (1 << i);
        dmaJob->jobStatus = DMA_STATUS_BUSY;
        DMAC->CHID.bit.ID = dmaJob->channel;
        DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK & interruptMask;
        DMAC->CHINTENCLR.reg = DMAC_CHINTENCLR_MASK & ~interruptMask;
        DMAC->CHCTRLA.bit.ENABLE = 1; // Enable the transfer channel
    }

    cpu_irq_leave_critical();

    return status;
}

void DMAC_Handler(void) {
    cpu_irq_enter_critical();

    uint8_t channel = DMAC->INTPEND.bit.ID;
    if (channel < DMAC_CH_NUM) {
        gossamer_dma_job_t *dmaJob;
        if ((dmaJob = _dmaPtr[channel])) {
            DMAC->CHID.bit.ID = channel;
            // Call IRQ handler with interrupt flag(s)
            uint8_t flags = DMAC->CHINTFLAG.reg;
            if (flags & DMAC_CHINTENCLR_TERR) {
                // Clear error flag
                DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
                dmaJob->jobStatus = DMA_STATUS_ERR_IO;
                if (dmaJob->callbacks[DMA_CALLBACK_TRANSFER_ERROR]) {
                    dmaJob->callbacks[DMA_CALLBACK_TRANSFER_ERROR](dmaJob);
                }
            } else if (flags & DMAC_CHINTENCLR_TCMPL) {
                DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL;
                dmaJob->jobStatus = DMA_STATUS_OK;
                if (dmaJob->callbacks[DMA_CALLBACK_TRANSFER_DONE]) {
                    dmaJob->callbacks[DMA_CALLBACK_TRANSFER_DONE](dmaJob);
                }
            } else if (flags & DMAC_CHINTENCLR_SUSP) {
                DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
                dmaJob->jobStatus = DMA_STATUS_SUSPEND;
                if (dmaJob->callbacks[DMA_CALLBACK_CHANNEL_SUSPEND]) {
                    dmaJob->callbacks[DMA_CALLBACK_CHANNEL_SUSPEND](dmaJob);
                }
            }
        }
    }

    cpu_irq_leave_critical();
}
