#include "app.h"
#include "eic.h"

void app_init(void) {
}

void app_setup(void) {
    // set LEDs as outputs
    HAL_GPIO_LED_out();
    HAL_GPIO_D8_out();

    // set up the external interrupt controller
    eic_init();

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
    // if D5 is pressed light the green LED
    if (HAL_GPIO_D5_read()) HAL_GPIO_D8_clr();
    else HAL_GPIO_D8_set();

    // if D5 is pressed light the red one
    if (HAL_GPIO_D9_read()) HAL_GPIO_LED_clr();
    else HAL_GPIO_LED_set();

    // return true to hang out in STANDBY until another button is pressed
    return true;
}
