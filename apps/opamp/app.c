#include "app.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "opamp.h"
#include "i2c.h"
#include "tc.h"
#include "delay.h"
#include "gfx/sh1107.h"

typedef struct opamp_state_t { 
    int8_t muxneg;
    int8_t muxpos;
    int8_t potmux;
    int8_t res1mux;
    int8_t res2mux;
} opamp_state_t;

opamp_state_t opamps[3];
int8_t selected_row = 0;
int8_t selected_column = 0;
int8_t graphed_channel = -1;
int8_t active_screen = 0;
#define NUM_SCREENS 2


uint32_t select_held = 0;

bool needs_sample = false;
uint16_t adc_sample = 0;
uint8_t xpos = 0;

void draw_opamp(int instance);
void draw_menu(int instance);
void configure_opamp(int instance);
void configure_dac(void);

void app_init(void) {
    set_cpu_frequency(16000000);
}

void app_setup(void) {
    opamps[0].res1mux = opamps[1].res1mux = opamps[2].res1mux = OPAMP_RES1MUX_NC;
    opamps[0].res2mux = opamps[1].res2mux = opamps[2].res2mux = OPAMP_RES2MUX_NC;

    // OPTIONAL: Configure the DAC to emit a sine wave. Note that I still
    // haven't figured out how to use the DAC and OPAMP0 at the same time without
    // conflicts, so I'm configuring OPAMP0 to simply buffer the DAC output.
    // configure_dac();
    // opamps[0].muxneg = OPAMP_MUXNEG_OUT;
    // opamps[0].muxpos = OPAMP0_MUXPOS_DAC;

    adc_init();
    adc_enable();

    HAL_GPIO_LED_out();
    HAL_GPIO_D10_in();
    HAL_GPIO_D10_pullup();
    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();

    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    i2c_enable();

    gfx_init(64, 128, 1);
    gfx_set_rotation(90);

    sh1107_begin();
    sh1107_update();

    HAL_GPIO_OPAMP0_OUT_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_OPAMP1_OUT_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_OPAMP2_OUT_pmuxen(HAL_GPIO_PMUX_B);

    opamp_init();

    configure_opamp(0);
    configure_opamp(1);
    configure_opamp(2);
}

bool dirty = true;
uint16_t i = 0;

bool app_loop(void) {
    int8_t updown = 0;
    int8_t select = 0;

    if (!HAL_GPIO_D10_read()) updown = -1;
    if (!HAL_GPIO_D5_read()) updown = 1;
    if (!HAL_GPIO_D6_read()) {
        select_held++;
        select = select_held == 1;
        dirty = true;
    } else {
        select_held = 0;
        dirty = true;
    }

    // On screen 0, move through rows with up/down buttons
    if (updown && active_screen == 0) {
        dirty = true;
        switch (selected_column) {
            case 0:
                selected_row = (3 + selected_row + updown) % 3;
                break;
            case 1:
                opamps[selected_row].muxpos = (7 + (opamps[selected_row].muxpos + updown)) % 7;
                if (opamps[selected_row].muxpos > 3 && selected_row != 2) {
                    opamps[selected_row].muxpos = 0;
                }
                break;
            case 2:
                opamps[selected_row].muxneg = (6 + (opamps[selected_row].muxneg + updown)) % 6;
                if (opamps[selected_row].muxneg > 3 && selected_row != 2) {
                    opamps[selected_row].muxneg = 0;
                }
                break;
            case 3:
                opamps[selected_row].res1mux = (5 + (opamps[selected_row].res1mux + updown)) % 5;
                break;
            case 4:
                opamps[selected_row].potmux = (8 + opamps[selected_row].potmux + updown) % 8;
                break;
            case 5:
                opamps[selected_row].res2mux = (3 + (opamps[selected_row].res2mux + updown)) % 3;
                break;
        }
        configure_opamp(selected_row);
    }

    // On screen 0, move through columns with select button
    if (select && active_screen == 0) {
        dirty = true;
        selected_column = (selected_column + 1) % 6;
    }

    // Graph a channel or return from the graph by holding the select button
    if (select_held == 10) {
        selected_column = 0;
        xpos = 0;
        active_screen = (active_screen + 1) % NUM_SCREENS;
        graphed_channel = selected_row;
        dirty = true;
    }

    if (active_screen == 1) {
        switch (graphed_channel) {
            case 0:
                adc_sample = adc_get_analog_value(7);
                break;
            case 1:
                adc_sample = adc_get_analog_value(2);
                break;
            case 2:
                adc_sample = adc_get_analog_value(4);
                break;
            default:
                adc_sample = 32768;
                break;
        }
        dirty = true;
    }

    if (dirty) {
        dirty = false;
        switch (active_screen) {
            case 0:
                gfx_fill_screen(0);
                gfx_draw_string(0, 33, "  POS NEG R1  POT R2", 1, 0, 1);
                for (size_t i = 0; i < 3; i++) {
                    draw_opamp(i);
                    draw_menu(i);
                }
                break;
            case 1:
                gfx_draw_pixel(xpos, 63 - adc_sample / 1024, 1);
                xpos = (xpos + 1) % 128;
                gfx_draw_vline((xpos + 1) % 128, 0, 64, 0);
                break;
        }

        sh1107_update();
    }

    return false;
}

void draw_opamp(int instance) {
    int x = (instance == 0) ? 0 : (instance == 1) ? 43 : 86;
    int y = 2;
    gfx_draw_triangle(17 + x, 0 + y, 34 + x, 8 + y, 17 + x, 16 + y, 1);

    gfx_draw_small_char(19 + x, 3 + y, '+', 1, 1);
    gfx_draw_hline(15 + x, 5 + y, 2, 1);

    gfx_draw_small_char(19 + x, 9 + y, '-', 1, 1);
    gfx_draw_hline(15 + x, 11 + y, 2, 1);

    gfx_draw_small_char(24 + x, 6 + y, '0' + instance, 1, 1);
    gfx_draw_hline(35 + x, 8 + y, (instance == 2) ? 4 : 2, 1);


    gfx_draw_hline(7 + x, 22 + y, 27, 1);
    gfx_draw_pixel(10 + x, 22 + y, 0);
    gfx_draw_pixel(12 + x, 22 + y, 0);
    gfx_draw_pixel(14 + x, 22 + y, 0);
    gfx_draw_pixel(16 + x, 22 + y, 0);
    gfx_draw_pixel(24 + x, 22 + y, 0);
    gfx_draw_pixel(26 + x, 22 + y, 0);
    gfx_draw_pixel(28 + x, 22 + y, 0);
    gfx_draw_pixel(30 + x, 22 + y, 0);

    gfx_draw_pixel(10 + x, 21 + y, 1);
    gfx_draw_pixel(12 + x, 23 + y, 1);
    gfx_draw_pixel(14 + x, 21 + y, 1);
    gfx_draw_pixel(16 + x, 23 + y, 1);

    gfx_draw_pixel(24 + x, 21 + y, 1);
    gfx_draw_pixel(26 + x, 23 + y, 1);
    gfx_draw_pixel(28 + x, 21 + y, 1);
    gfx_draw_pixel(30 + x, 23 + y, 1);

    switch (opamps[instance].res1mux) {
        case OPAMP_RES1MUX_POS:
            if (instance) {
                gfx_draw_vline(3 + x, 23 + y, 1, 1);
                gfx_draw_hline(3 + x, 22 + y, 4, 1);
                gfx_draw_string(-2 + x, 25 + y, "IN +", 1, 1, 0);
            } else {
                gfx_draw_vline(3 + x, 23 + y, 1, 1);
                gfx_draw_hline(3 + x, 22 + y, 4, 1);
                gfx_draw_string(0 + x, 25 + y, "IN\n +", 1, 1, 0);
            }
            break;
        case OPAMP_RES1MUX_NEG:
            if (instance) {
                gfx_draw_vline(3 + x, 23 + y, 1, 1);
                gfx_draw_hline(3 + x, 22 + y, 4, 1);
                gfx_draw_string(-2 + x, 25 + y, "IN -", 1, 1, 0);
            } else {
                gfx_draw_vline(3 + x, 23 + y, 1, 1);
                gfx_draw_hline(3 + x, 22 + y, 4, 1);
                gfx_draw_string(0 + x, 25 + y, "IN\n -", 1, 1, 0);
            }
            break;
        case OPAMP0_RES1MUX_DAC: // 3 is DAC on instance 0, previous output on 1 and 2
        // case OPAMP1_RES1MUX_OUT_0:
        // case OPAMP2_RES1MUX_OUT_1:
            if (instance) {
                gfx_draw_hline(-9 + x, 8 + y, 7, 1);
                gfx_draw_vline(-3 + x, 8 + y, 14, 1);
                gfx_draw_hline(-3 + x, 22 + y, 10, 1);
            } else {
                gfx_draw_vline(3 + x, 23 + y, 1, 1);
                gfx_draw_hline(3 + x, 22 + y, 4, 1);
                gfx_draw_string(0 + x, 25 + y, "DA\n C", 1, 1, 0);
            }
            break;
        case OPAMP_RES1MUX_GND:
            gfx_draw_vline(6 + x, 22 + y, 3, 1);
            gfx_draw_hline(4 + x, 25 + y, 5, 1);
            gfx_draw_hline(5 + x, 27 + y, 3, 1);
            break;
        default:
            break;
    }
    switch (opamps[instance].res2mux) {
        case OPAMP_RES2MUX_VCC:
            gfx_draw_hline(31 + x, 22 + y, 3, 1);
            gfx_draw_vline(34 + x, 19 + y, 4, 1);
            gfx_draw_hline(33 + x, 20 + y, 3, 1);
            break;
        case OPAMP_RES2MUX_OUT:
            gfx_draw_hline(31 + x, 22 + y, 7, 1);
            gfx_draw_vline(37 + x, 9 + y, 13, 1);
            gfx_draw_hline(37 + x, 8 + y, 1, 1);
            break;
        default:
            break;
    }
    char r1[3] = {0};
    char r2[3] = {0};
    
    switch (opamps[instance].potmux) {
        case OPAMP_POTMUX_RATIO_14_2:
            r1[0] = '1';
            r1[1] = '4';
            r2[0] = ' ';
            r2[1] = '2';
            break;
        case OPAMP_POTMUX_RATIO_12_4:
            r1[0] = '1';
            r1[1] = '2';
            r2[0] = ' ';
            r2[1] = '4';
            break;
        case OPAMP_POTMUX_RATIO_8_8:
            r1[0] = ' ';
            r1[1] = '8';
            r2[0] = ' ';
            r2[1] = '8';
            break;
        case OPAMP_POTMUX_RATIO_6_10:
            r1[0] = ' ';
            r1[1] = '6';
            r2[0] = '1';
            r2[1] = '0';
            break;
        case OPAMP_POTMUX_RATIO_4_12:
            r1[0] = ' ';
            r1[1] = '4';
            r2[0] = '1';
            r2[1] = '2';
            break;
        case OPAMP_POTMUX_RATIO_3_13:
            r1[0] = ' ';
            r1[1] = '3';
            r2[0] = '1';
            r2[1] = '3';
            break;
        case OPAMP_POTMUX_RATIO_2_14:
            r1[0] = ' ';
            r1[1] = '2';
            r2[0] = '1';
            r2[1] = '4';
            break;
        case OPAMP_POTMUX_RATIO_1_15:
            r1[0] = ' ';
            r1[1] = '1';
            r2[0] = '1';
            r2[1] = '5';
            break;
    }
    gfx_draw_string(10 + x, 24 + y, r1, 1, 1, 0);
    gfx_draw_string(24 + x, 24 + y, r2, 1, 1, 0);

    switch (opamps[instance].muxpos) {
        case OPAMP_MUXPOS_POS:
            if (opamps[instance].res1mux == OPAMP_RES1MUX_POS) {
                gfx_draw_string(2 + x, -1 + y, "IN+", 1, 1, 0);
            } else {
                gfx_draw_string(2 + x, 3 + y, "IN+", 1, 1, 0);
            }
            break;
        case OPAMP_MUXPOS_LADDER:
            gfx_draw_hline(0 + x, 5 + y, 16, 1);
            gfx_draw_vline(0 + x, 5 + y, 14, 1);
            gfx_draw_hline(0 + x, 19 + y, 21, 1);
            gfx_draw_vline(20 + x, 19 + y, 4, 1);
            break;
        case OPAMP0_MUXPOS_DAC: // 2 is DAC on instance 0, previous output on 1 and 2
        // case OPAMP1_MUXPOS_OUT_0:
        // case OPAMP2_MUXPOS_OUT_1:
            if (instance == 0) {
                gfx_draw_string(2 + x, 3 + y, "DAC", 1, 1, 0);
            } else {
                gfx_draw_hline(x - 6, 8 + y, 1, 1);
                gfx_draw_vline(x - 6, 5 + y, 4, 1);
                gfx_draw_hline(x - 6, 5 + y, 21, 1);
            }
            break;
        case OPAMP_MUXPOS_GND:
            gfx_draw_vline(14 + x, 2 + y, 4, 1);
            gfx_draw_hline(6 + x, 2 + y, 8, 1);
            gfx_draw_vline(6 + x, 2 + y, 2, 1);
            gfx_draw_hline(4 + x, 4 + y, 5, 1);
            gfx_draw_hline(5 + x, 6 + y, 3, 1);
            break;
        case OPAMP2_MUXPOS_POS_0: // OA0 positive (second instance only)
            if (opamps[0].muxpos == OPAMP_MUXPOS_POS) {
                gfx_draw_vline(14, -2 + y, 8, 1);
                gfx_draw_hline(14, -2 + y, 86, 1);
                gfx_draw_vline(100, -2 + y, 8, 1);
            } else {
                gfx_draw_string(2 + x, 3 + y, "0+", 1, 1, 0);
            }
            break;
        case OPAMP2_MUXPOS_POS_1: // OA1 positive (second instance only)
            if (opamps[0].muxpos == OPAMP_MUXPOS_POS) {
                gfx_draw_vline(57, -2 + y, 8, 1);
                gfx_draw_hline(57, -2 + y, 43, 1);
                gfx_draw_vline(100, -2 + y, 8, 1);
            } else {
                gfx_draw_string(2 + x, 3 + y, "1+", 1, 1, 0);
            }
            break;
        case OPAMP2_MUXPOS_LADDER_0: // OA0 tap (second instance only)
            gfx_draw_vline(100, -2 + y, 8, 1);
            gfx_draw_hline(100, -2 + y, 27, 1);
            gfx_draw_vline(127, -2 + y, 32, 1);
            gfx_draw_vline(20, 23 + y, 6, 1);
            gfx_draw_hline(20, 29 + y, 107, 1);
        break;
        default:
            break;
    }

    switch (opamps[instance].muxneg) {
        case OPAMP_MUXNEG_NEG:
            if (opamps[instance].res1mux == OPAMP_RES1MUX_NEG) {
                gfx_draw_string(2 + x, 13 + y, "IN-", 1, 1, 0);
            } else {
                gfx_draw_string(2 + x, 9 + y, "IN-", 1, 1, 0);
            }
            break;
        case OPAMP_MUXNEG_LADDER:
            gfx_draw_hline(13 + x, 11 + y, 3, 1);
            gfx_draw_vline(13 + x, 11 + y, 8, 1);
            gfx_draw_hline(13 + x, 18 + y, 8, 1);
            gfx_draw_vline(20 + x, 18 + y, 4, 1);
            break;
        case OPAMP_MUXNEG_OUT:
            gfx_draw_vline(14 + x, 11 + y, 6, 1);
            gfx_draw_hline(14 + x, 17 + y, 23, 1);
            gfx_draw_vline(37 + x, 8 + y, 10, 1);
            break;
        case OPAMP0_MUXNEG_DAC: // 3 is DAC on instance 0 and 1, 0 NEG on instance 2
        // case OPAMP1_MUXNEG_DAC:
        // case OPAMP2_MUXNEG_NEG_0:
            if (instance == 2) {
                // TODO
            } else {
                if (opamps[instance].res1mux == OPAMP_RES1MUX_NEG) {
                    gfx_draw_string(2 + x, 13 + y, "DAC", 1, 1, 0);
                } else {
                    gfx_draw_string(2 + x, 9 + y, "DAC", 1, 1, 0);
                }
            }
            break;
        case OPAMP2_MUXNEG_NEG_1: // OA1 negative (second instance only)
            if (instance == 2) {
                // TODO
            }
            break;
        case OPAMP2_MUXNEG_DAC: // DAC (second instance only)
            if (instance == 2) {
                gfx_draw_string(2 + x, 9 + y, "DAC", 1, 1, 0);
            }
            break;
        default:
            break;
    }
}

void draw_menu(int instance) {
    bool row_active = (selected_row == instance);
    bool highlighted;
    char buf[4] = {0};

    if (row_active && selected_column == 0) gfx_draw_char(0, 41 + 8 * instance, '0' + instance, 0, 1, 1);
    else gfx_draw_char(0, 41 + 8 * instance, '0' + instance, 1, 0, 1);

    highlighted = (row_active && selected_column == 1) ? 1 : 0;
    switch (opamps[instance].muxpos) {
        case 0: // OAxPOS
            gfx_draw_string(12, 41 + 8 * instance, "IN+", !highlighted, highlighted, 1);
            break;
        case 1: // OAxTAP
            buf[0] = 'R';
            buf[1] = 'L';
            buf[2] = '0' + instance;
            gfx_draw_string(12, 41 + 8 * instance, buf, !highlighted, highlighted, 1);
            break;
        case 2: // DAC or output
            if (instance == 0) {
                gfx_draw_string(12, 41 + 8 * instance, "DAC", !highlighted, highlighted, 1);
            } else if (instance == 1) {
                gfx_draw_string(12, 41 + 8 * instance, "OA0", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(12, 41 + 8 * instance, "OA1", !highlighted, highlighted, 1);
            }
            break;
        case 3: // GND
            gfx_draw_string(12, 41 + 8 * instance, "GND", !highlighted, highlighted, 1);
            break;
        case 4: // OA0 positive (second instance only)
            if (instance == 2) {
                gfx_draw_string(12, 41 + 8 * instance, "0+ ", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(12, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
            }
            break;
        case 5: // OA1 positive (second instance only)
            if (instance == 2) {
                gfx_draw_string(12, 41 + 8 * instance, "1+ ", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(12, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
            }
            break;
        case 6: // OA0 tap (second instance only)
            if (instance == 2) {
                gfx_draw_string(12, 41 + 8 * instance, "RL0", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(12, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
            }
            break;
        default:
            gfx_draw_string(12, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
    }

    highlighted = (row_active && selected_column == 2) ? 1 : 0;
    switch (opamps[instance].muxneg) {
        case 0: // OAxNEG
            gfx_draw_string(36, 41 + 8 * instance, "IN-", !highlighted, highlighted, 1);
            break;
        case 1: // OAxTAP
            buf[0] = 'R';
            buf[1] = 'L';
            buf[2] = '0' + instance;
            gfx_draw_string(36, 41 + 8 * instance, buf, !highlighted, highlighted, 1);
            break;
        case 2: // OAxOUT
            gfx_draw_string(36, 41 + 8 * instance, "OUT", !highlighted, highlighted, 1);
            break;
        case 3: // DAC (0, 1) or OA0 negative (2)
            if (instance == 2) {
                gfx_draw_string(36, 41 + 8 * instance, "0- ", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(36, 41 + 8 * instance, "DAC", !highlighted, highlighted, 1);
            }
            break;
        case 4: // OA1 negative (second instance only)
            if (instance == 2) {
                gfx_draw_string(36, 41 + 8 * instance, "1- ", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(36, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
            }
            break;
        case 5: // DAC (second instance only)
            if (instance == 2) {
                gfx_draw_string(36, 41 + 8 * instance, "DAC", !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(36, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
            }
            break;
        default:
            gfx_draw_string(36, 41 + 8 * instance, "Err", !highlighted, highlighted, 1);
    }


    highlighted = (row_active && selected_column == 3) ? 1 : 0;
    switch(opamps[instance].res1mux) {
        case 0: // OAxPOS
            gfx_draw_string(60, 41 + 8 * instance, "IN+", !highlighted, highlighted, 1);
            break;
        case 1: // OAxNEG
            gfx_draw_string(60, 41 + 8 * instance, "IN-", !highlighted, highlighted, 1);
            break;
        case 2: // DAC or previous output
            if (instance) {
                buf[0] = 'O';
                buf[1] = 'A';
                buf[2] = '0' + instance - 1;
                gfx_draw_string(60, 41 + 8 * instance, buf, !highlighted, highlighted, 1);
            } else {
                gfx_draw_string(60, 41 + 8 * instance, "DAC", !highlighted, highlighted, 1);
            }
            break;
        case 3: // GND
            gfx_draw_string(60, 41 + 8 * instance, "GND", !highlighted, highlighted, 1);
            break;
        default:
            gfx_draw_string(60, 41 + 8 * instance, "NC ", !highlighted, highlighted, 1);
    }
    
    highlighted = (row_active && selected_column == 4) ? 1 : 0;
    buf[0] = 'P';
    buf[1] = '0' + opamps[instance].potmux;
    buf[2] = ' ';
    if (highlighted) gfx_fill_rect(84, 41 + 8 * instance, 17, 8, 1);
    switch (opamps[instance].potmux) {
        case OPAMP_POTMUX_RATIO_14_2:
            gfx_draw_string(85, 42 + 8 * instance, "14 : 2", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_12_4:
            gfx_draw_string(85, 42 + 8 * instance, "12 : 4", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_8_8:
            gfx_draw_string(85, 42 + 8 * instance, "  8 : 8", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_6_10:
            gfx_draw_string(85, 42 + 8 * instance, "6 : 10", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_4_12:
            gfx_draw_string(85, 42 + 8 * instance, "4 : 12", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_3_13:
            gfx_draw_string(85, 42 + 8 * instance, "3 : 13", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_2_14:
            gfx_draw_string(85, 42 + 8 * instance, "2 : 14", !highlighted, highlighted, 0);
            break;
        case OPAMP_POTMUX_RATIO_1_15:
            gfx_draw_string(85, 42 + 8 * instance, "1 : 15", !highlighted, highlighted, 0);
            break;
    }

    highlighted = (row_active && selected_column == 5) ? 1 : 0;
    switch(opamps[instance].res2mux) {
        case 0: // VCC
            gfx_draw_string(108, 41 + 8 * instance, "VCC", !highlighted, highlighted, 1);
            break;
        case 1: // OUT
            buf[0] = 'O';
            buf[1] = 'A';
            buf[2] = '0' + instance;
            gfx_draw_string(108, 41 + 8 * instance, buf, !highlighted, highlighted, 1);
            break;
        default:
            gfx_draw_string(108, 41 + 8 * instance, "NC ", !highlighted, highlighted, 1);
    }
}

void configure_opamp(int instance) {
    opamp_disable(instance);
    opamp_set_muxpos(instance, opamps[instance].muxpos);
    opamp_set_muxneg(instance, opamps[instance].muxneg);
    opamp_set_res1mux(instance, opamps[instance].res1mux);
    opamp_set_res2mux(instance, opamps[instance].res2mux);
    opamp_set_potmux(instance, opamps[instance].potmux);
    opamp_set_analog_connection(instance, true);

    if (opamps[instance].muxpos == OPAMP_MUXPOS_POS || opamps[instance].res1mux == OPAMP_RES1MUX_POS) {
        switch (instance) {
            case 0:
                HAL_GPIO_OPAMP0_POS_pmuxen(HAL_GPIO_PMUX_B);
                break;
            case 1:
                HAL_GPIO_OPAMP1_POS_pmuxen(HAL_GPIO_PMUX_B);
                break;
            case 2:
                HAL_GPIO_OPAMP2_POS_pmuxen(HAL_GPIO_PMUX_B);
                break;
        }
    } else {
        switch (instance) {
            case 0:
                HAL_GPIO_OPAMP0_POS_pmuxdis();
                break;
            case 1:
                HAL_GPIO_OPAMP1_POS_pmuxdis();
                break;
            case 2:
                HAL_GPIO_OPAMP2_POS_pmuxdis();
                break;
        }
    }

    if (opamps[instance].muxneg == OPAMP_MUXNEG_NEG || opamps[instance].res1mux == OPAMP_RES1MUX_NEG) {
        if (instance == 0) HAL_GPIO_OPAMP0_NEG_pmuxen(HAL_GPIO_PMUX_B);
    } else {
        if (instance == 0) HAL_GPIO_OPAMP0_NEG_pmuxdis();
    }

    opamp_enable(instance);
}

static uint16_t sine_vals[] = {
    0x800,0x819,0x832,0x84b,0x864,0x87d,0x896,0x8af,0x8c8,0x8e1,0x8fa,0x913,0x92c,0x945,0x95e,0x976,0x98f,0x9a8,0x9c0,0x9d9,0x9f1,0xa09,0xa22,0xa3a,0xa52,0xa6a,0xa82,0xa9a,0xab1,0xac9,0xae0,0xaf8,0xb0f,0xb26,0xb3d,0xb54,0xb6b,0xb82,0xb98,0xbae,0xbc5,0xbdb,0xbf1,0xc06,0xc1c,0xc32,0xc47,0xc5c,0xc71,0xc86,0xc9a,0xcaf,0xcc3,0xcd7,0xceb,0xcff,0xd12,0xd26,0xd39,0xd4c,0xd5f,0xd71,0xd83,0xd95,0xda7,0xdb9,0xdca,0xddc,0xded,0xdfd,0xe0e,0xe1e,0xe2e,0xe3e,0xe4e,0xe5d,0xe6c,0xe7b,0xe8a,0xe98,0xea6,0xeb4,0xec1,0xecf,0xedc,0xee8,0xef5,0xf01,0xf0d,0xf19,0xf24,0xf30,0xf3a,0xf45,0xf4f,0xf59,0xf63,0xf6d,0xf76,0xf7f,0xf87,0xf90,0xf98,0xf9f,0xfa7,0xfae,0xfb5,0xfbb,0xfc2,0xfc8,0xfcd,0xfd3,0xfd8,0xfdc,0xfe1,0xfe5,0xfe9,0xfec,0xff0,0xff3,0xff5,0xff7,0xff9,0xffb,0xffd,0xffe,0xffe,0xfff,
    0xfff,0xfff,0xffe,0xffe,0xffd,0xffb,0xff9,0xff7,0xff5,0xff3,0xff0,0xfec,0xfe9,0xfe5,0xfe1,0xfdc,0xfd8,0xfd3,0xfcd,0xfc8,0xfc2,0xfbb,0xfb5,0xfae,0xfa7,0xf9f,0xf98,0xf90,0xf87,0xf7f,0xf76,0xf6d,0xf63,0xf59,0xf4f,0xf45,0xf3a,0xf30,0xf24,0xf19,0xf0d,0xf01,0xef5,0xee8,0xedc,0xecf,0xec1,0xeb4,0xea6,0xe98,0xe8a,0xe7b,0xe6c,0xe5d,0xe4e,0xe3e,0xe2e,0xe1e,0xe0e,0xdfd,0xded,0xddc,0xdca,0xdb9,0xda7,0xd95,0xd83,0xd71,0xd5f,0xd4c,0xd39,0xd26,0xd12,0xcff,0xceb,0xcd7,0xcc3,0xcaf,0xc9a,0xc86,0xc71,0xc5c,0xc47,0xc32,0xc1c,0xc06,0xbf1,0xbdb,0xbc5,0xbae,0xb98,0xb82,0xb6b,0xb54,0xb3d,0xb26,0xb0f,0xaf8,0xae0,0xac9,0xab1,0xa9a,0xa82,0xa6a,0xa52,0xa3a,0xa22,0xa09,0x9f1,0x9d9,0x9c0,0x9a8,0x98f,0x976,0x95e,0x945,0x92c,0x913,0x8fa,0x8e1,0x8c8,0x8af,0x896,0x87d,0x864,0x84b,0x832,0x819,
    0x800,0x7e6,0x7cd,0x7b4,0x79b,0x782,0x769,0x750,0x737,0x71e,0x705,0x6ec,0x6d3,0x6ba,0x6a1,0x689,0x670,0x657,0x63f,0x626,0x60e,0x5f6,0x5dd,0x5c5,0x5ad,0x595,0x57d,0x565,0x54e,0x536,0x51f,0x507,0x4f0,0x4d9,0x4c2,0x4ab,0x494,0x47d,0x467,0x451,0x43a,0x424,0x40e,0x3f9,0x3e3,0x3cd,0x3b8,0x3a3,0x38e,0x379,0x365,0x350,0x33c,0x328,0x314,0x300,0x2ed,0x2d9,0x2c6,0x2b3,0x2a0,0x28e,0x27c,0x26a,0x258,0x246,0x235,0x223,0x212,0x202,0x1f1,0x1e1,0x1d1,0x1c1,0x1b1,0x1a2,0x193,0x184,0x175,0x167,0x159,0x14b,0x13e,0x130,0x123,0x117,0x10a,0xfe,0xf2,0xe6,0xdb,0xcf,0xc5,0xba,0xb0,0xa6,0x9c,0x92,0x89,0x80,0x78,0x6f,0x67,0x60,0x58,0x51,0x4a,0x44,0x3d,0x37,0x32,0x2c,0x27,0x23,0x1e,0x1a,0x16,0x13,0xf,0xc,0xa,0x8,0x6,0x4,0x2,0x1,0x1,0x0,
    0x0,0x0,0x1,0x1,0x2,0x4,0x6,0x8,0xa,0xc,0xf,0x13,0x16,0x1a,0x1e,0x23,0x27,0x2c,0x32,0x37,0x3d,0x44,0x4a,0x51,0x58,0x60,0x67,0x6f,0x78,0x80,0x89,0x92,0x9c,0xa6,0xb0,0xba,0xc5,0xcf,0xdb,0xe6,0xf2,0xfe,0x10a,0x117,0x123,0x130,0x13e,0x14b,0x159,0x167,0x175,0x184,0x193,0x1a2,0x1b1,0x1c1,0x1d1,0x1e1,0x1f1,0x202,0x212,0x223,0x235,0x246,0x258,0x26a,0x27c,0x28e,0x2a0,0x2b3,0x2c6,0x2d9,0x2ed,0x300,0x314,0x328,0x33c,0x350,0x365,0x379,0x38e,0x3a3,0x3b8,0x3cd,0x3e3,0x3f9,0x40e,0x424,0x43a,0x451,0x467,0x47d,0x494,0x4ab,0x4c2,0x4d9,0x4f0,0x507,0x51f,0x536,0x54e,0x565,0x57d,0x595,0x5ad,0x5c5,0x5dd,0x5f6,0x60e,0x626,0x63f,0x657,0x670,0x689,0x6a1,0x6ba,0x6d3,0x6ec,0x705,0x71e,0x737,0x750,0x769,0x782,0x79b,0x7b4,0x7cd,0x7e6,
};

void configure_dac(void) {
    HAL_GPIO_A0_pmuxen(HAL_GPIO_PMUX_B);
    dac_init();
    dac_enable(0);

    tc_setup(1, GENERIC_CLOCK_0, TC_PRESCALER_DIV256);
    tc_set_wavegen(1, TC_WAVE_WAVEGEN_MFRQ);
    tc_count16_set_cc(1, 0, 122); // (16 MHz / 256) / (122 * 512 samples) = 1 Hz
    tc_enable(1);

    gossamer_dma_job_t dac_dma;
    dma_init();
    dma_configure(&dac_dma,
                  TC1_DMAC_ID_OVF,
                  DMA_TRIGGER_ACTION_BEAT,
                  DMA_CONFIG_LOOP);
    dma_add_descriptor(&dac_dma,
                       (void *)sine_vals,
                       (void *)&(DAC->DATA[0].reg),
                       sizeof(sine_vals) / sizeof(sine_vals[0]),
                       DMA_BEAT_SIZE_HWORD,
                       DMA_INCREMENT_SOURCE,
                       DMA_STEPSIZE_1,
                       DMA_STEPSEL_SOURCE);
    dma_start_job(&dac_dma);
}
