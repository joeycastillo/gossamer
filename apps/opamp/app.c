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
    opamp_enable(0);
    opamp_enable(1);
    opamp_enable(2);
}

bool app_loop(void) {
    return false;
}

void draw_opamp(int x, int y, int instance) {
    (void) instance; // need to add text to GFX

    gfx_draw_triangle(17 + x, 0 + y, 34 + x, 8 + y, 17 + x, 16 + y, 1);
    gfx_draw_fast_hline(14 + x, 5 + y, 3, 1);
    gfx_draw_fast_hline(14 + x, 11 + y, 3, 1);
    gfx_draw_fast_hline(35 + x, 8 + y, (instance == 2) ? 4 : 2, 1);

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
