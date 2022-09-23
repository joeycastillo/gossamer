#include "app.h"
#include "eic.h"

void app_init(void) {
    eic_init();
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    HAL_GPIO_BTN_ALARM_in();
    HAL_GPIO_BTN_ALARM_pullup();
    HAL_GPIO_BTN_ALARM_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(2, INTERRUPT_TRIGGER_FALLING);
}

bool app_loop(void) {
    HAL_GPIO_LED_toggle();
    return true;
}
