#include "app.h"
#include "eic.h"
#include "delay.h"
#include "component/slcd.h"
#include <stdio.h>

// Union representing a single segment mapping
// COM occupies two bits, SEG occupes the rest.
typedef union segment_mapping_t {
    struct {
        uint8_t com : 2;
        uint8_t seg : 6;
    } address;
    uint8_t value;
} segment_mapping_t;

static const uint8_t segment_does_not_exist = 0xff;

// Union representing 8 segment mappings, A-H
typedef union digit_mapping_t {
    segment_mapping_t segment[8];
    uint64_t value;
} digit_mapping_t;

static const digit_mapping_t All_Digits[] = {
    {
        .segment = {
            { .address = { .com = 0, .seg = 19 } }, // 11A
            { .address = { .com = 2, .seg = 19 } }, // 11B
            { .address = { .com = 3, .seg = 19 } }, // 11C
            { .address = { .com = 3, .seg = 20 } }, // 11D
            { .address = { .com = 2, .seg = 20 } }, // 11E
            { .address = { .com = 0, .seg = 20 } }, // 11F
            { .address = { .com = 1, .seg = 20 } }, // 11G
            { .address = { .com = 1, .seg = 19 } }, // 11H
        },
    },
    {
        .segment = {
            { .address = { .com = 0, .seg = 17 } }, // 10A
            { .address = { .com = 2, .seg = 17 } }, // 10B
            { .address = { .com = 3, .seg = 17 } }, // 10C
            { .address = { .com = 3, .seg = 18 } }, // 10D
            { .address = { .com = 2, .seg = 18 } }, // 10E
            { .address = { .com = 0, .seg = 18 } }, // 10F
            { .address = { .com = 1, .seg = 18 } }, // 10G
            { .address = { .com = 1, .seg = 17 } }, // 10H
        },
    },
    {
        .segment = {
            { .address = { .com = 0, .seg = 12 } }, // 9A
            { .address = { .com = 2, .seg = 12 } }, // 9B
            { .address = { .com = 3, .seg = 12 } }, // 9C
            { .address = { .com = 3, .seg = 13 } }, // 9D
            { .address = { .com = 2, .seg = 13 } }, // 9E
            { .address = { .com = 0, .seg = 13 } }, // 9F
            { .address = { .com = 1, .seg = 13 } }, // 9G
            { .address = { .com = 1, .seg = 12 } }, // 9H
        },
    },
    {
        .segment = {
            { .address = { .com = 0, .seg = 11 } }, // 8A
            { .address = { .com = 0, .seg = 10 } }, // 8B
            { .address = { .com = 2, .seg = 10 } }, // 8C
            { .address = { .com = 3, .seg = 11 } }, // 8D
            { .address = { .com = 2, .seg = 11 } }, // 8E
            { .address = { .com = 1, .seg = 11 } }, // 8F
            { .address = { .com = 1, .seg = 10 } }, // 8G
            { .value = segment_does_not_exist }, // 8H
        },
    },
    {
        .segment = {
            { .address = { .com = 0, .seg = 9 } }, // 7A
            { .address = { .com = 0, .seg = 8 } }, // 7B
            { .address = { .com = 2, .seg = 8 } }, // 7C
            { .address = { .com = 3, .seg = 9 } }, // 7D
            { .address = { .com = 2, .seg = 9 } }, // 7E
            { .address = { .com = 1, .seg = 9 } }, // 7F
            { .address = { .com = 1, .seg = 8 } }, // 7G
            { .value = segment_does_not_exist }, // 7H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 16 } }, // 1A
            { .address = { .com = 2, .seg = 16 } }, // 1B
            { .address = { .com = 1, .seg = 16 } }, // 1C
            { .address = { .com = 0, .seg = 16 } }, // 1D
            { .address = { .com = 1, .seg = 22 } }, // 1E
            { .address = { .com = 3, .seg = 22 } }, // 1F
            { .address = { .com = 2, .seg = 22 } }, // 1G
            { .value = segment_does_not_exist }, // 1H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 14 } }, // 2A
            { .address = { .com = 2, .seg = 14 } }, // 2B
            { .address = { .com = 1, .seg = 14 } }, // 2C
            { .address = { .com = 0, .seg = 15 } }, // 2D
            { .address = { .com = 1, .seg = 15 } }, // 2E
            { .address = { .com = 3, .seg = 15 } }, // 2F
            { .address = { .com = 2, .seg = 15 } }, // 2G
            { .value = segment_does_not_exist }, // 2H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 1 } }, // 3A
            { .address = { .com = 2, .seg = 2 } }, // 3B
            { .address = { .com = 0, .seg = 2 } }, // 3C
            { .address = { .com = 0, .seg = 1 } }, // 3D
            { .address = { .com = 1, .seg = 1 } }, // 3E
            { .address = { .com = 2, .seg = 1 } }, // 3F
            { .address = { .com = 1, .seg = 2 } }, // 3G
            { .value = segment_does_not_exist }, // 3H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 3 } }, // 4A
            { .address = { .com = 2, .seg = 4 } }, // 4B
            { .address = { .com = 0, .seg = 4 } }, // 4C
            { .address = { .com = 0, .seg = 3 } }, // 4D
            { .address = { .com = 1, .seg = 3 } }, // 4E
            { .address = { .com = 2, .seg = 3 } }, // 4F
            { .address = { .com = 1, .seg = 4 } }, // 4G
            { .value = segment_does_not_exist }, // 4H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 10 } }, // 5A
            { .address = { .com = 3, .seg =  8 } }, // 5B
            { .address = { .com = 0, .seg =  5 } }, // 5C
            { .address = { .com = 1, .seg =  5 } }, // 5D
            { .address = { .com = 3, .seg =  4 } }, // 5E
            { .address = { .com = 3, .seg =  2 } }, // 5F
            { .address = { .com = 2, .seg =  5 } }, // 5G
            { .address = { .com = 3, .seg =  5 } }, // 5H
        },
    },
    {
        .segment = {
            { .address = { .com = 3, .seg = 6 } }, // 6A
            { .address = { .com = 3, .seg = 7 } }, // 6B
            { .address = { .com = 2, .seg = 7 } }, // 6C
            { .address = { .com = 0, .seg = 7 } }, // 6D
            { .address = { .com = 0, .seg = 6 } }, // 6E
            { .address = { .com = 2, .seg = 6 } }, // 6F
            { .address = { .com = 1, .seg = 6 } }, // 6G
            { .address = { .com = 1, .seg = 7 } }, // 6H
        },
    },
};

static const uint8_t Character_Set[] =
{
    0b00000000, //  
    0b00000000, // ! (unused)
    0b00100010, // "
    0b01100011, // # (degree symbol, hash mark doesn't fit)
    0b11101101, // $ (S with a downstroke)
    0b00000000, // % (unused)
    0b00000000, // & (unused)
    0b00100000, // '
    0b00111001, // (
    0b00001111, // )
    0b11000000, // * (The + sign for use in position 0)
    0b01110000, // + (segments E, F and G; looks like ┣╸)
    0b00000100, // ,
    0b01000000, // -
    0b00001000, // . (same as _, semantically most useful)
    0b00010010, // /
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b00000000, // : (unused)
    0b00000000, // ; (unused)
    0b01011000, // <
    0b01001000, // =
    0b01001100, // >
    0b01010011, // ?
    0b11111111, // @ (all segments on)
    0b01110111, // A
    0b11001111, // B (with downstroke, only in weekday / seconds)
    0b00111001, // C
    0b10001111, // D (with downstroke, only in weekday / seconds)
    0b01111001, // E
    0b01110001, // F
    0b00111101, // G
    0b01110110, // H
    0b10001001, // I (only works in position 0)
    0b00001110, // J
    0b01110101, // K
    0b00111000, // L
    0b10110111, // M (only works in position 0)
    0b00110111, // N
    0b00111111, // O
    0b01110011, // P
    0b01100111, // Q
    0b01010000, // R (lowercase, this is the only capital we can't do)
    0b01101101, // S
    0b10000001, // T (only works in position 0; set (1, 12) to make it work in position 1)
    0b00111110, // U
    0b00111110, // V
    0b10111110, // W (only works in position 0)
    0b01111110, // X
    0b01101110, // Y
    0b00011011, // Z
    0b00111001, // [
    0b00100100, // backslash
    0b00001111, // ]
    0b00100011, // ^
    0b00001000, // _
    0b00000010, // `
    0b01011111, // a
    0b01111100, // b
    0b01011000, // c
    0b01011110, // d
    0b01111011, // e
    0b01110001, // f
    0b01101111, // g
    0b01110100, // h
    0b00010000, // i
    0b00001110, // j
    0b01110101, // k
    0b00110000, // l
    0b10110111, // m (only works in position 0)
    0b01010100, // n
    0b01011100, // o
    0b01110011, // p
    0b01100111, // q
    0b01010000, // r
    0b01101101, // s
    0b01111000, // t
    0b00011100, // u
    0b00011100, // v (looks like u)
    0b10111110, // w
    0b01111110, // x
    0b01101110, // y
    0b00011011, // z
    0b00010110, // { (open brace doesn't really work; overriden to represent the two character ligature "il")
    0b00110110, // | (overriden to represent the two character ligature "ll")
    0b00110100, // } (overriden to represent the two character ligature "li")
    0b00000001, // ~
};

static void display_character(uint8_t character, uint8_t position) {
    uint8_t charmap = Character_Set[character - 32];
    digit_mapping_t segmap = All_Digits[position];
    for (uint8_t i = 0; i < 8; i++) {
        if (segmap.segment[i].value == segment_does_not_exist) {
            continue;
        }
        uint8_t seg = segmap.segment[i].address.seg;
        switch (segmap.segment[i].address.com) {
            case 0:
                if (charmap & (1 << i)) {
                    SLCD->SDATAL0.reg |= (1 << seg);
                } else {
                    SLCD->SDATAL0.reg &= ~(1 << seg);
                }
                break;
            case 1:
                if (charmap & (1 << i)) {
                    SLCD->SDATAL1.reg |= (1 << seg);
                } else {
                    SLCD->SDATAL1.reg &= ~(1 << seg);
                }
                break;
            case 2:
                if (charmap & (1 << i)) {
                    SLCD->SDATAL2.reg |= (1 << seg);
                } else {
                    SLCD->SDATAL2.reg &= ~(1 << seg);
                }
                break;
            case 3:
                if (charmap & (1 << i)) {
                    SLCD->SDATAL3.reg |= (1 << seg);
                } else {
                    SLCD->SDATAL3.reg &= ~(1 << seg);
                }
                break;
        }
    }
}

static void display_string(const char *string) {
    uint8_t position = 0;
    int len = 11;
    if (strlen(string) < len) {
        len = strlen(string);
    }
    for (int i = 0; i < len; i++) {
        display_character(string[i], position++);
    }
}

static void clear_display(void) {
    SLCD->SDATAL0.reg = 0;
    SLCD->SDATAL1.reg = 0;
    SLCD->SDATAL2.reg = 0;
    SLCD->SDATAL3.reg = 0;
}

void app_init(void) {
    eic_init();
}

static void _sync_slcd(uint32_t reg) {
    // wait for sync of whichever flags were passed in.
    while (SLCD->SYNCBUSY.reg & reg);
}

void app_setup(void) {
    // Configure LCD pins
    HAL_GPIO_SLCD0_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD1_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD2_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD3_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD4_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD5_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD6_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD7_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD8_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD9_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD10_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD11_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD12_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD13_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD14_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD15_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD16_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD17_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD18_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD19_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD20_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD21_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD22_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD23_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD24_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD25_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_SLCD26_pmuxen(HAL_GPIO_PMUX_B);

    // Enable SLCD
    MCLK->APBCMASK.reg |= MCLK_APBCMASK_SLCD;
    if (SLCD->CTRLA.bit.ENABLE) {
        SLCD->CTRLA.bit.ENABLE = 0;
        _sync_slcd(SLCD_SYNCBUSY_ENABLE);
    }
    SLCD->CTRLA.bit.SWRST = 1;
    _sync_slcd(SLCD_SYNCBUSY_SWRST);

    // Tell the LCD driver which pins are connected to the LCD
    SLCD->LPENL.reg = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 11) | (1 << 12) | (1 << 13) | (1 << 14) | (1 << 21) | (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25) | (1 << 28) | (1 << 29) | (1 << 30) | (1 << 31);
    SLCD->LPENH.reg = (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) | (1 << 10) | (1 << 11);

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

    // enable the LCD
    SLCD->CTRLA.bit.ENABLE = 1;
    _sync_slcd(SLCD_SYNCBUSY_ENABLE);

    // set up LED
    HAL_GPIO_LED1_pmuxdis();
    HAL_GPIO_LED2_pmuxdis();
    HAL_GPIO_LED1_out();
    HAL_GPIO_LED2_out();
    HAL_GPIO_LED1_set();
    HAL_GPIO_LED2_set();

    // Interrupt for Mode Button
    eic_enable();
    HAL_GPIO_BTN_MODE_pmuxen(HAL_GPIO_PMUX_EIC);
    HAL_GPIO_BTN_MODE_in();
    HAL_GPIO_BTN_MODE_pulldown();
    eic_configure_pin(HAL_GPIO_BTN_MODE_pin(), INTERRUPT_TRIGGER_RISING);
    eic_enable_interrupt(HAL_GPIO_BTN_MODE_pin());
}

static const char *messages[] = {
    "WED17114219",
    "Frq  5997HZ",
    "TMPLo1923#F",
    "$US  3995  ",
    "LON  5785#W",
    "Phasest Qtr",
    "AUS27104123",
};

int message_index = 0;

bool app_loop(void) {
    clear_display();
    display_string(messages[message_index]);
    switch (message_index) {
        case 0:
            SLCD->SDATAL0.reg |= (1 << 0);
            SLCD->SDATAL3.reg |= (1 << 21);
            break;
        case 1:
        case 2:
        case 3:
            SLCD->SDATAL0.reg |= (1 << 14);
            break;
        case 4:
            SLCD->SDATAL0.reg |= (1 << 14) | (1 << 22);
            break;
        case 5:
            SLCD->SDATAL0.reg |= (1 << 22);
            break;
        case 6:
            SLCD->SDATAL0.reg |= (1 << 21);
            break;
    }

    message_index = (message_index + 1) % (sizeof(messages) / sizeof(messages[0]));

    return true;
}

