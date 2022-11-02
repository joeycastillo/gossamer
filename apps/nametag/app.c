#include "app.h"
#include "component/slcd.h"

static const uint8_t CHARS[];

static void _sync_slcd(uint32_t reg) {
    // wait for sync of whichever flags were passed in.
    while (SLCD->SYNCBUSY.reg & reg);
}

static inline void _display_character(uint8_t pos, uint8_t c) {
    // segment indices are 4, 2 and 0 for positions 0, 1 and 2, respectively.
    // common indices are all 0
    SLCD->CMINDEX.reg = SLCD_CMINDEX_SINDEX(2 * (2 - pos)) | SLCD_CMINDEX_CINDEX(0);

    // write character data (lookup table starts at ' ', 0x20)
    SLCD->CMDATA.reg = CHARS[c - 0x20];

    // CMWRBUSY = character mapping write busy. wait for it to finish.
    while(SLCD->STATUS.bit.CMWRBUSY);
}

void app_init(void) {
}

void app_setup(void) {
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
    SLCD->CTRLB.reg = SLCD_CTRLB_BBEN | SLCD_CTRLB_BBD(1); // configure bias buffer
    SLCD->CTRLC.reg = SLCD_CTRLC_CTST(10);      // set contrast to 3.11 volts
    SLCD->CTRLD.reg = SLCD_CTRLD_FC0EN |        // enable frame counter 0
                      SLCD_CTRLD_DISPEN;        // enable the display!
    _sync_slcd(SLCD_SYNCBUSY_CTRLD);

    SLCD->FC0.bit.OVF = SLCD_FC0_OVF(8);

    // Character mapping config: our characters are two SEG lines wide:
    // COM3    F        A
    // COM2    G        B
    // COM1    E        C
    // COM0    D        X < LSB
    //      SEG n+1  SEG n+0
    SLCD->CMCFG.reg = SLCD_CMCFG_NSEG(2 - 1);

    // don't mask out any segments when character mapping
    SLCD->CMDMASK.reg = 0;

    // Tell the LCD driver which pins are connected to the LCD
    SLCD->LPENL.reg = (1 << 2) | (1 << 3) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 31);
    SLCD->LPENH.reg = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 16);

    // clear display memory
    SLCD->SDATAL0.reg = 0;
    SLCD->SDATAL1.reg = 0;
    SLCD->SDATAL2.reg = 0;
    SLCD->SDATAL3.reg = 0;

    // set blink mode to only blink selected characters
    // (none selected bt default)
    SLCD->BCFG.bit.MODE = 1;

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

    // enable interrupt on frame counter 0 overflow.
    // this will wake the run loop from standby mode.
    SLCD->INTENSET.reg = SLCD_INTENSET_FC0O;
    NVIC_ClearPendingIRQ(SLCD_IRQn);
    NVIC_EnableIRQ(SLCD_IRQn);

    // enable the LCD
    SLCD->CTRLA.bit.ENABLE = 1;
    _sync_slcd(SLCD_SYNCBUSY_ENABLE);
}

int scrollpos = 0;
const char message[] = "   XELL0 &7Y MA&7E 15 J0EY   ";

bool app_loop(void) {
    for(int i = 0; i < 3; i++) {
        _display_character(i, message[scrollpos + i]);
    }
    scrollpos = (scrollpos + 1) % (sizeof(message) - 3);

    return true;
}

// this is the inerr
void irq_handler_slcd(void);

void irq_handler_slcd(void) {
    // all we do here: clear all interrupt flags
    SLCD->INTFLAG.reg = SLCD_INTFLAG_MASK;
}

// Segment data for printable characters.
static const uint8_t CHARS[] = {
    0b00000000, // [space]
    0b00000000, // ! (unused)
    0b10010000, // "
    0b11110000, // # (°)
    0b10001110, // $ (Like an L with an extra segment, use $J to make a W)
    0b00011000, // %
    0b11011000, // & (Like an F without a cross stroke, use &7 to make an M)
    0b00010000, // '
    0b11001010, // (
    0b01010110, // )
    0b00000000, // * (Currently unused)
    0b10101000, // +
    0b00000100, // ,
    0b00100000, // -
    0000000000, // . (unused)
    0b00011000, // /
    0b11011110, // 0
    0b00010100, // 1
    0b01111010, // 2
    0b01110110, // 3
    0b10110100, // 4
    0b11100110, // 5
    0b11101110, // 6
    0b11010100, // 7
    0b11111110, // 8
    0b11110110, // 9
    0b00000000, // : (unavailable / unused)
    0b01000000, // ; (Like the apostrophe, but on the other side, use 'foo; to say ‘foo’)
    0000000000, // < (unused)
    0b00100010, // =
    0000000000, // > (unused)
    0000000000, // ? (unused)
    0000000000, // @ (unused)
    0b11111100, // A
    0b10101110, // B (lowercase)
    0b11001010, // C (lowercase)
    0b00111110, // D (lowercase)
    0b11101010, // E
    0b11101000, // F
    0b11001110, // G
    0b10101100, // H (lowercase)
    0b00001000, // I (lowercase)
    0b00011110, // J
    0b11101100, // K
    0b10001010, // L
    0b11011100, // M (looks like tall N)
    0b00101100, // N (lowercase)
    0b00101110, // O (lowercase)
    0b11111000, // P
    0b11110100, // Q (lowercase)
    0b00101000, // R (lowercase)
    0b11000110, // S (looks like 5 without the center segment)
    0b10101010, // T (lowercase)
    0b00001110, // U (lowercase)
    0b10011110, // V (looks like tall U)
    0b10111110, // W (looks like upside down A)
    0b10111100, // X (looks like uppercase H)
    0b10110110, // Y
    0b01011010, // Z (looks like 2 without the center segment)
};
