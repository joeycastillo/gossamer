#pragma once

#include "sam.h"

/// TODO: DMA on SAMD51
#ifndef _SAMD51_

typedef enum {
    DMA_CONFIG_LOOP     = 1 << 0,
    DMA_CONFIG_RUNSTDBY = 1 << 1,
} dma_configuration_flags_t;

typedef enum {
  DMA_INCREMENT_NONE = 0,
  DMA_INCREMENT_SOURCE = (1 << 0),
  DMA_INCREMENT_DESTINATION = (1 << 1),
  DMA_INCREMENT_BOTH = (1 << 1) | (1 << 0),
} dma_address_increment_t;


typedef enum {
  DMA_TRIGGER_ACTION_BLOCK       = DMAC_CHCTRLB_TRIGACT_BLOCK_Val,
  DMA_TRIGGER_ACTION_BEAT        = DMAC_CHCTRLB_TRIGACT_BEAT_Val,
  DMA_TRIGGER_ACTION_TRANSACTION = DMAC_CHCTRLB_TRIGACT_TRANSACTION_Val,
} dma_trigger_action_t;

typedef enum {
  // First item here is for any transfer errors. A transfer error is
  // flagged if a bus error is detected during an AHB access or when
  // the DMAC fetches an invalid descriptor
  DMA_CALLBACK_TRANSFER_ERROR,
  DMA_CALLBACK_TRANSFER_DONE,
  DMA_CALLBACK_CHANNEL_SUSPEND,
  DMA_CALLBACK_N, // Number of available callbacks
} dma_callback_type_t;

typedef enum {
  DMA_BEAT_SIZE_BYTE = 0, // 8-bit
  DMA_BEAT_SIZE_HWORD,    // 16-bit
  DMA_BEAT_SIZE_WORD,     // 32-bit
} dma_beat_size_t;

typedef enum {
  DMA_EVENT_OUTPUT_DISABLE = 0, // Disable event generation
  DMA_EVENT_OUTPUT_BLOCK,       // Event strobe when block xfer complete
  DMA_EVENT_OUTPUT_RESERVED,
  DMA_EVENT_OUTPUT_BEAT, // Event strobe when beat xfer complete
} dma_event_output_selection_t;

typedef enum {
  DMA_BLOCK_ACTION_NONE = 0,
  // Channel in normal operation and sets transfer complete interrupt
  // flag after block transfer
  DMA_BLOCK_ACTION_INTERRUPT,
  // Trigger channel suspend after block transfer and sets channel
  // suspend interrupt flag once the channel is suspended
  DMA_BLOCK_ACTION_SUSPEND,
  // Sets transfer complete interrupt flag after a block transfer and
  // trigger channel suspend. The channel suspend interrupt flag will
  // be set once the channel is suspended.
  DMA_BLOCK_ACTION_INTERRUPT_AND_SUSPEND,
} dma_block_action_t;

// DMA step selection. This bit determines whether the step size setting
// is applied to source or destination address.
typedef enum {
  DMA_STEPSEL_DESTINATION = 0,
  DMA_STEPSEL_SOURCE,
} dma_stepsel_t;

// Address increment step size. These bits select the address increment step
// size. The setting apply to source or destination address, depending on
// STEPSEL setting.
typedef enum {
  DMA_STEPSIZE_1 = 0, // beat size * 1
  DMA_STEPSIZE_2,     // beat size * 2
  DMA_STEPSIZE_4,     // beat size * 4
  DMA_STEPSIZE_8,     // etc...
  DMA_STEPSIZE_16,
  DMA_STEPSIZE_32,
  DMA_STEPSIZE_64,
  DMA_STEPSIZE_128,
} dma_stepsize_t;

// higher numbers are higher priority
typedef enum {
  DMA_PRIORITY_0, // lowest (default)
  DMA_PRIORITY_1,
  DMA_PRIORITY_2,
  DMA_PRIORITY_3, // highest
} dma_priority_t;


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
} dma_status_t;

#endif
