#include "app.h"
#include "i2c.h"
#include "gfx/sh1107.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    i2c_init();
    i2c_enable();

    gfx_init(64, 128, 1);
    gfx_set_rotation(90);

    sh1107_begin();
    sh1107_update();

    // fill the screen with a checkerboard pattern
    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 128; j++) {
            gfx_draw_pixel(j, i, (i + j) % 2);
        }
    }

    sh1107_update();
}

bool app_loop(void) {
    return true;
}
