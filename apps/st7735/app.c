#include "app.h"
#include "gfx/st7735.h"
#include "spi.h"
#include "delay.h"
#include <stdio.h>

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_TFT_BACKLIGHT_out();
    HAL_GPIO_TFT_BACKLIGHT_set();
    HAL_GPIO_SCK_out();
    HAL_GPIO_MOSI_out();
    HAL_GPIO_SCK_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_MOSI_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    spi_init(SPI_MODE_0, 8000000);
    spi_enable();
    gfx_init(128, 128, 16);
}

bool app_loop(void) {
    return false;
}
