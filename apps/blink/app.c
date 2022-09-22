#include "app.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
}

bool app_loop(void) {
    HAL_GPIO_LED_toggle();
    delay_ms(500);
    return false;
}
