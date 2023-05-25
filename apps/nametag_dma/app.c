#include "app.h"
#include "dma.h"
#include "component/slcd.h"

const uint8_t message[] = {
    0b00000000, // [space]
    0b00000000, // [space]
    0b00000000, // [space]
    0b01111100, // H
    0b11010101, // E
    0b01000101, // L
    0b01000101, // L
    0b11101101, // 0
    0b00000000, // [space]
    0b11100100, 0b11101000, // M
    0b01111001, // Y
    0b00000000, // [space]
    0b11101100, // N
    0b11111100, // A
    0b11100100, 0b11101000, // M
    0b11010101, // E
    0b00000000, // [space]
    0b00101000, // 1
    0b11011001, // 5
    0b00000000, // [space]
    0b00101101, // J
    0b11101101, // 0
    0b11010101, // E
    0b01111001, // Y
    0b00000000, // [space]
    0b00000000, // [space]
};

static void _sync_slcd(uint32_t reg) {
    // wait for sync of whichever flags were passed in.
    while (SLCD->SYNCBUSY.reg & reg);
}

void app_init(void) {
    PM->STDBYCFG.bit.BBIASHS = 0; // disable back biasing in standby, needed for DMA
}

void app_setup(void) {
    // Configure LCD COM pins
    HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_B);    // COM0
    HAL_GPIO_A2_pmuxen(HAL_GPIO_PMUX_B);    // COM1
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_B);    // COM2
    HAL_GPIO_A2_pmuxen(HAL_GPIO_PMUX_B);    // COM3

    // Configure LCD SEG pins
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_B);    // SEG0
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_B);    // SEG1
    HAL_GPIO_D11_pmuxen(HAL_GPIO_PMUX_B);   // SEG2
    HAL_GPIO_D10_pmuxen(HAL_GPIO_PMUX_B);   // SEG3
    HAL_GPIO_D12_pmuxen(HAL_GPIO_PMUX_B);   // SEG4
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_B);    // SEG5

    // Enable SLCD peripheral clock
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_SLCD;
    if (SLCD->CTRLA.bit.ENABLE) {
        // disable it so we can configure it
        SLCD->CTRLA.bit.ENABLE = 0;
        _sync_slcd(SLCD_SYNCBUSY_ENABLE);
    }
    // then reset the LCD controller.
    SLCD->CTRLA.bit.SWRST = 1;
    _sync_slcd(SLCD_SYNCBUSY_SWRST);

    // Tell the LCD driver which pins are connected to the LCD
    SLCD->LPENL.reg = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 31);
    SLCD->LPENH.reg = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 16);

    // set LCD control registers
    SLCD->CTRLA.reg = SLCD_CTRLA_DUTY(4 - 1) |  // 1/4 duty
                      SLCD_CTRLA_WMOD_LP |      // low power waveform
                      SLCD_CTRLA_RUNSTDBY |     // run in standby
                      SLCD_CTRLA_PRESC(1) |     // prescaler 32, duty 1/4,
                      SLCD_CTRLA_CKDIV(7) |     // clock divider 8 -> 32 Hz refresh
                      SLCD_CTRLA_BIAS(3 - 1) |  // 1/3 bias
                      SLCD_CTRLA_PRF(3) |       // power refresh slowly, 250 Hz
                      SLCD_CTRLA_RRF(5);        // reference refresh slowly, 62.5 Hz
    SLCD->CTRLB.reg = SLCD_CTRLB_BBEN |         // enable bias buffer
                      SLCD_CTRLB_BBD(1);        // configure bias buffer
    SLCD->CTRLC.reg = SLCD_CTRLC_CTST(10) |     // set contrast to 3.11 volts
                      SLCD_CTRLC_ACMEN;         // enable automatic character mapping
    SLCD->CTRLD.reg = SLCD_CTRLD_FC0EN |        // enable frame counter 0
                      SLCD_CTRLD_DISPEN;        // enable the display!
    _sync_slcd(SLCD_SYNCBUSY_CTRLD);

    // clear display memory
    SLCD->SDATAL0.reg = 0;
    SLCD->SDATAL1.reg = 0;
    SLCD->SDATAL2.reg = 0;
    SLCD->SDATAL3.reg = 0;

    // Character mapping config: our characters are two SEG lines wide:
    // COM3    A        F
    // COM2    B        G
    // COM1    C        E
    // COM0    X        D < LSB
    //      SEG n+1  SEG n+0
    SLCD->CMCFG.reg = SLCD_CMCFG_DEC |          // Segment lines decrement (it's how our LCD was designed)
                      SLCD_CMCFG_NSEG(2 - 1);   // 2 segment lines per digit in the LCD

    // AUTOMATED character mapping (ACM) config!
    SLCD->ACMCFG.reg =  SLCD_ACMCFG_FCS_FC0 |                       // use frame counter 0 for ACM
                        SLCD_ACMCFG_STEPS(sizeof(message) - 2) |    // Enough steps to scroll the whole message
                        SLCD_ACMCFG_NDIG(3) |                       // 3 digits in our display
                        SLCD_ACMCFG_NDROW(3) |                      // 3 digits per row (we only have one row)
                        SLCD_ACMCFG_NCOM(4 - 1) |                   // 4 COM lines per row
                        SLCD_ACMCFG_STSEG(5) |                      // Start at SEG5 (segment lines decrement)
                        SLCD_ACMCFG_MODE_SCROLL;                    // Enable scrolling mode

    // Set up DMA for ACM (automated character mapping)
    dma_init();

    // The scroll_message job will handle transfers from the message buffer to the CMDATA register
    gossamer_dma_job_t scroll_job;

    // Configure when the job does things
    dma_configure(&scroll_job,              // Pointer to the job we want to configure
                  SLCD_DMAC_ID_ACMDRDY,     // Trigger DMA on ACM ready trigger (when FC0 overflows)
                  DMA_TRIGGER_ACTION_BEAT,  // Trigger generates a request for a beat transfer
                  DMA_CONFIG_LOOP |         // Loop the job when complete
                  DMA_CONFIG_RUNSTDBY);     // Run in STANDBY mode

    // Configure what things the job does
    dma_add_descriptor(&scroll_job,             // Pointer to the job
                       (void *)message,         // Source address: buffer with the message to scroll
                       (void *)&(SLCD->CMDATA), // Destination: the character mapping data register
                       sizeof(message),         // Beats to transfer: number of bytes in the message
                       DMA_BEAT_SIZE_BYTE,      // Beat size: transfer one byte per beat
                       DMA_INCREMENT_SOURCE,    // Increment source address
                       DMA_STEPSIZE_1,          // Step size: increment by one byte per beat
                       DMA_STEPSEL_SOURCE);     // Apply step size to source address

    dma_start_job(&scroll_job);

    // Set frame counter 0 to overflow after 8 frames, triggering ACM's DMA request
    SLCD->FC0.bit.OVF = SLCD_FC0_OVF(8);

    // enable the LCD
    SLCD->CTRLA.bit.ENABLE = 1;
    _sync_slcd(SLCD_SYNCBUSY_ENABLE);
}

bool app_loop(void) {
    return true;
}
