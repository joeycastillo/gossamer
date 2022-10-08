#include "app.h"
#include "delay.h"
#include "piezo.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_D10_out();
    HAL_GPIO_D10_pmuxen(HAL_GPIO_PMUX_F);
    HAL_GPIO_D11_out();
    HAL_GPIO_D11_pmuxen(HAL_GPIO_PMUX_F);
    piezo_tcc_init_differential(0, 6, 7);
    
    // designed for use with Feather M0 and the Button/Buzzer wing.
    // Hold button A to drive the piezo with one pin (decent loud)
    // Hold button C to drive the piezo differentially (extra loud)
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
}

bool app_loop(void) {
    if (!HAL_GPIO_D5_read()) {
        HAL_GPIO_D11_pmuxen(HAL_GPIO_PMUX_F);
        piezo_tcc_init_differential(0, 6, 7);
    }
    if (!HAL_GPIO_D9_read()) {
        HAL_GPIO_D11_pmuxdis();
        HAL_GPIO_D11_clr();
        piezo_tcc_init(0, 6);
    }
    piezo_tcc_on(4000);
    delay_ms(50);
    piezo_tcc_off();
    HAL_GPIO_LED_toggle();
    delay_ms(500);
    return false;
}
