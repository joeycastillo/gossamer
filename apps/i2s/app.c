#include "app.h"
#include "i2s.h"
#include "i2c.h"
#include "../clock/drivers/oso_lcd.h"
#include <stdio.h>

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_D0_pmuxen(HAL_GPIO_PMUX_COM);
    HAL_GPIO_D1_pmuxen(HAL_GPIO_PMUX_COM);
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_COM);

    i2s_init(0, 8000, 32, I2S_MODE_NORMAL);
    i2s_enable_receiver(0);

    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    i2c_enable();

    oso_lcd_begin();
    oso_lcd_fill(0);
}

bool app_loop(void) {
    char buf[10];
    uint16_t data = I2S->DATA[0].reg >> 17;
    sprintf(buf, "%5d", data);
    oso_lcd_print(buf);

    return false;
}
