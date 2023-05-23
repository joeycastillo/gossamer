#include "app.h"
#include "dac.h"
#include "opamp.h"
#include "i2c.h"
#include "delay.h"
#include "gfx/sh1107.h"

void draw_opamp(int x, int y, int instance);

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();

    gfx_init(64, 128, 1);
    gfx_set_rotation(1);
    gfx_fill_screen(0);

    draw_opamp(0, 2, 0);
    draw_opamp(43, 2, 1);
    draw_opamp(86, 2, 2);

    gfx_draw_string(0, 32, "  POS NEG R1  POT R2", 1, 0, 1);
    gfx_draw_string(0, 40, "0 IN+ IN- NC  P0  NC", 1, 0, 1);
    gfx_draw_string(0, 48, "1 IN+ IN- NC  P0  NC", 1, 0, 1);
    gfx_draw_string(0, 56, "2 IN+ IN- NC  P0  NC", 1, 0, 1);

    sh1107_begin();
    sh1107_update();

    HAL_GPIO_AREF_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A0_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A2_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A4_pmuxen(HAL_GPIO_PMUX_B);
    HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_B);

    opamp_init();

    opamp_set_muxpos(0, OPAMP0_MUXPOS_PIN);
    opamp_set_muxneg(0, OPAMP0_MUXNEG_LADDER);
    opamp_set_res1mux(0, OPAMP0_RES1MUX_GND);
    opamp_set_res2mux(0, OPAMP_RES2MUX_OUT);
    opamp_set_potmux(0, OPAMP_POTMUX_RATIO_1_15);

    opamp_set_muxpos(1, OPAMP1_MUXPOS_OUT_0);
    opamp_set_muxneg(1, OPAMP1_MUXNEG_LADDER);
    opamp_set_res1mux(1, OPAMP1_RES1MUX_GND);
    opamp_set_res2mux(1, OPAMP_RES2MUX_OUT);
    opamp_set_potmux(1, OPAMP_POTMUX_RATIO_8_8);

    opamp_set_muxpos(2, OPAMP2_MUXPOS_OUT_1);
    opamp_set_muxneg(2, OPAMP2_MUXNEG_LADDER);
    opamp_set_res1mux(2, OPAMP2_RES1MUX_GND);
    opamp_set_res2mux(2, OPAMP_RES2MUX_OUT);
    opamp_set_potmux(2, OPAMP_POTMUX_RATIO_8_8);

    opamp_enable(0);
    opamp_enable(1);
    opamp_enable(2);
}

bool app_loop(void) {
    return false;
}

void draw_opamp(int x, int y, int instance) {
    gfx_draw_triangle(17 + x, 0 + y, 34 + x, 8 + y, 17 + x, 16 + y, 1);
    gfx_draw_fast_hline(14 + x, 5 + y, 3, 1);
    gfx_draw_fast_hline(14 + x, 11 + y, 3, 1);
    gfx_draw_fast_hline(35 + x, 8 + y, 2, 1);

    gfx_draw_small_char(19 + x, 3 + y, '+', 1, 1);
    gfx_draw_small_char(19 + x, 9 + y, '-', 1, 1);
    gfx_draw_small_char(24 + x, 6 + y, '0' + instance, 1, 1);

    gfx_draw_fast_hline(7 + x, 22 + y, 27, 1);
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
}
