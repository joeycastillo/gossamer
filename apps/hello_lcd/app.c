#include "app.h"
#include "i2c.h"
#include "delay.h"
#include "../clock/drivers/oso_lcd.h"

void app_init(void) {
}

void app_setup(void) {
    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);
}

bool app_loop(void) {
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_MOON);
    oso_lcd_print("hello");
    delay_ms(1000);
    oso_lcd_set_indicator(OSO_LCD_INDICATOR_MOON);

    return true;
}
