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
    HAL_GPIO_LED_out();
    // Enable SLCD
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_SLCD;
    if (SLCD->CTRLA.bit.ENABLE) {
        SLCD->CTRLA.bit.ENABLE = 0;
        _sync_slcd(SLCD_SYNCBUSY_ENABLE);
    }
    SLCD->CTRLA.bit.SWRST = 1;
    _sync_slcd(SLCD_SYNCBUSY_SWRST);

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

    SLCD->FC0.bit.OVF = SLCD_FC0_OVF(8);

    dma_init();
    gossamer_dma_job_t scroll_message;
    dma_configure(&scroll_message, SLCD_DMAC_ID_ACMDRDY, DMAC_CHCTRLB_TRIGACT_BEAT_Val, true);
    dma_add_descriptor(&scroll_message, (void *)message, (void *)&(SLCD->CMDATA), sizeof(message), DMA_BEAT_SIZE_BYTE, true, false, 0, true);
    dma_start_job(&scroll_message);

    // Character mapping config: our characters are two SEG lines wide:
    // COM3    A        F
    // COM2    B        G
    // COM1    C        E
    // COM0    X        D < LSB
    //      SEG n+1  SEG n+0
    SLCD->CMCFG.reg = SLCD_CMCFG_DEC | 
                      SLCD_CMCFG_NSEG(2 - 1);
    SLCD->ACMCFG.reg =  SLCD_ACMCFG_FCS_FC0 | 
                        SLCD_ACMCFG_STEPS(sizeof(message) - 3 + 1) |
                        SLCD_ACMCFG_NCOM(4 - 1) |
                        SLCD_ACMCFG_NDIG(3) |
                        SLCD_ACMCFG_NDROW(3) |
                        SLCD_ACMCFG_STSEG(5) |
                        SLCD_ACMCFG_MODE_SCROLL;


    // Tell the LCD driver which pins are connected to the LCD
    SLCD->LPENL.reg = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 31);
    SLCD->LPENH.reg = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 16);

    // clear display memory
    SLCD->SDATAL0.reg = 0;
    SLCD->SDATAL1.reg = 0;
    SLCD->SDATAL2.reg = 0;
    SLCD->SDATAL3.reg = 0;

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

    // enable the LCD
    SLCD->CTRLA.bit.ENABLE = 1;
    _sync_slcd(SLCD_SYNCBUSY_ENABLE);
}

bool app_loop(void) {
    return true;
}
