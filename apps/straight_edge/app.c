#include "app.h"
#include "component/slcd.h"
#include "tc.h"
#include "qtouch/touch.h"
#include <stdio.h>

static const uint8_t CHARS[];

void app_init(void) {
    // set up 4 MHz clock for touch controller
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_DIV(2) |
                           GCLK_GENCTRL_SRC_OSC16M |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;
}

static void _sync_slcd(uint32_t reg) {
    // wait for sync of whichever flags were passed in.
    while (SLCD->SYNCBUSY.reg & reg);
}

static inline void _display_character(int8_t pos, uint8_t c) {
    SLCD->CMINDEX.reg = SLCD_CMINDEX_SINDEX(2 * pos + 2) | SLCD_CMINDEX_CINDEX(0);
    SLCD->CMDATA.reg = CHARS[c - 0x20];
    while(SLCD->STATUS.bit.CMWRBUSY);
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    // Configure LCD COM pins
    HAL_GPIO_COM0_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_COM1_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_COM2_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_COM3_pmuxen(HAL_GPIO_PMUX_B);

    // Configure LCD SEG pins
    HAL_GPIO_SEG0_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG1_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG2_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG3_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG4_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG5_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG6_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG7_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG8_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG9_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG10_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SEG11_pmuxen(HAL_GPIO_PMUX_B);

    // Configure touch pins
    HAL_GPIO_TOUCH_X0_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_X1_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_X21_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_X22_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_Y8_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_Y9_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_Y14_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_TOUCH_Y23_pmuxen(HAL_GPIO_PMUX_PTC);

    // Enable SLCD
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_SLCD;
    if (SLCD->CTRLA.bit.ENABLE) {
        SLCD->CTRLA.bit.ENABLE = 0;
        _sync_slcd(SLCD_SYNCBUSY_ENABLE);
    }
    SLCD->CTRLA.bit.SWRST = 1;
    _sync_slcd(SLCD_SYNCBUSY_SWRST);

    // Tell the LCD driver which pins are connected to the LCD
    SLCD->LPENL.reg = (1 << 3) | (1 << 6) | (1 << 7) | (1 << 11) | (1 << 12) | (1 << 21) | (1 << 30) | (1 << 31);
    SLCD->LPENH.reg = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 16) | (1 << 17) | (1 << 18) | (1 << 19);

    // set LCD control registers
    SLCD->CTRLA.reg = SLCD_CTRLA_DUTY(4 - 1) |  // 1/4 duty
                      SLCD_CTRLA_WMOD_LP |      // low power waveform
                      SLCD_CTRLA_RUNSTDBY |     // run in standby
                      SLCD_CTRLA_PRESC(1) |     // prescaler 32, duty 1/4...
                      SLCD_CTRLA_CKDIV(7) |     // ...and clock divider 8 -> 32 Hz refresh
                      SLCD_CTRLA_BIAS(3 - 1) |  // 1/3 bias
                      SLCD_CTRLA_PRF(3) |       // power refresh slowly, 250 Hz
                      SLCD_CTRLA_RRF(5);        // reference refresh slowly, 62.5 Hz
    SLCD->CTRLB.reg = SLCD_CTRLB_BBEN |         // enable bias buffer
                      SLCD_CTRLB_BBD(1);        // configure bias buffer
    SLCD->CTRLC.reg = SLCD_CTRLC_CTST(6);       // set contrast
    SLCD->CTRLD.reg = SLCD_CTRLD_FC0EN |        // enable frame counter 0
                      SLCD_CTRLD_DISPEN;        // enable the display!
    _sync_slcd(SLCD_SYNCBUSY_CTRLD);

    // Character mapping config:
    // COM3    D        E
    // COM2    G        X
    // COM1    C        F
    // COM0    B        A < LSB
    //      SEG n+1  SEG n+0
    SLCD->CMCFG.reg = SLCD_CMCFG_NSEG(2 - 1);   // our characters are two SEG lines wide
    SLCD->CMDMASK.reg = 0;                      // don't mask out any segments when character mapping

    // clear display memory
    SLCD->SDATAL0.reg = 0;
    SLCD->SDATAL1.reg = 0;
    SLCD->SDATAL2.reg = 0;
    SLCD->SDATAL3.reg = 0;

    // enable the LCD
    SLCD->CTRLA.bit.ENABLE = 1;
    _sync_slcd(SLCD_SYNCBUSY_ENABLE);

    GCLK->PCHCTRL[PTC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    MCLK->APBCMASK.bit.PTC_ = 1;

    tc_init(1, GENERIC_CLOCK_0, TC_PRESCALER_DIV1024); // 8 MHz / 1024 = 7182
    tc_set_wavegen(1, TC_WAVE_WAVEGEN_MFRQ); // Match frequency mode, overflow at CC0
    tc_count16_set_cc(1, 0, 156); // 7182 / 156 roughly equals 50 Hz or once every 20 ms
    TC1->COUNT16.INTENSET.reg = TC_INTENSET_OVF; // enable overflow interrupt
    NVIC_EnableIRQ(TC1_IRQn);
    tc_enable(1);

    // Configure touch sensors with Application specific settings
    touch_init();
}

char things[] = {'2',
                 '6',
                 'E',
                 'A',
                 '1',
                 '5',
                 'D',
                 '9',
                 '0',
                 '4',
                 '(',
                 '8',
                 '3',
                 '7',
                 'F',
                 'B',};

bool app_loop(void) {
    int pos = 0;

    touch_process();

    // // get channnel 9's raw value (number '4')
    // uint16_t raw = get_sensor_node_signal(9);
    // // print it to LCD
    // char buf[6];
    // snprintf(buf, 6, "%5d", raw);
    // for (int i = 0; i < 5; i++) {
    //     _display_character(pos++, buf[i]);
    // }


    // read all 12 touch channels
    for (int i = 0; i < DEF_NUM_SENSORS; i++) {
        if (get_sensor_state(i) & KEY_TOUCHED_MASK) {
            _display_character(pos++, things[i]);
        }
    }

    while(pos < 5) {
        _display_character(pos++, ' ');
    }

    return false;
}

// Segment data for printable characters.
static const uint8_t CHARS[] = {
    //DEGXCFBA
    0b00000000, // [space]
    0b00010100, // !
    0b00000110, // "
    0b00100111, // # (°)
    0b11001100, // $ (Like an L with an extra segment, use $J to make a W)
    0b00001100, // %
    0b01000111, // & (Like an F without a cross stroke, use &7 to make an M)
    0b00000010, // '
    0b11000101, // (
    0b10001011, // )
    0b00000000, // * (Currently unused)
    0b00101010, // +
    0b01000000, // ,
    0b00100000, // -
    0b00010000, // .
    0b01000010, // /
    0b11001111, // 0
    0b00001010, // 1
    0b11100011, // 2
    0b10101011, // 3
    0b00101110, // 4
    0b10101101, // 5
    0b11101101, // 6
    0b00001111, // 7
    0b11101111, // 8
    0b10101111, // 9
    0b00000000, // : (RESERVED: special character that turns on the colon)
    0b00000100, // ; (Like the apostrophe, but on the other side, use 'foo; to say ‘foo’)
    0b11000000, // <
    0b10100000, // =
    0b10001000, // >
    0b01110011, // ?
    0b11111111, // @
    0b01101111, // A
    0b11101100, // B
    0b11100000, // C
    0b11101010, // D
    0b11100101, // E
    0b01100101, // F
    0b10101111, // G
    0b01101100, // H
    0b00001000, // I
    0b11001010, // J
    0b01101101, // K
    0b11000100, // L
    0b01001111, // M
    0b01101000, // N
    0b11101000, // O
    0b01100111, // P
    0b00101111, // Q
    0b01100000, // R
    0b10001101, // S
    0b11100100, // T
    0b11001000, // U
    0b11001110, // V
    0b11101110, // W
    0b01101110, // X
    0b10101110, // Y
    0b11000011, // Z
};

void irq_handler_tc1(void);
void irq_handler_tc1(void) {
    TC1->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
    touch_timer_handler();
}