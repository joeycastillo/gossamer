#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "drivers/oso_lcd.h"

void app_init(void) {
}

void app_setup(void) {
    // set LEDs as outputs
    HAL_GPIO_LED_out();
    HAL_GPIO_D8_out();

    // set up the external interrupt controller
    eic_init();

    // set up the I²C peripheral...
    i2c_init();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);

    // configure button D5 as an active low interrupt
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(15, INTERRUPT_TRIGGER_BOTH);

    // same for button D9
    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(7, INTERRUPT_TRIGGER_BOTH);
}

bool app_loop(void) {
    if (!HAL_GPIO_D9_read()) oso_lcd_print("Hello");
    if (!HAL_GPIO_D5_read()) oso_lcd_print("$Jrld");

    // return true to hang out in STANDBY until another button is pressed
    return true;
}
