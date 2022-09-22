#include "sam.h"
#include "pins.h"
#include "app.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_D8_out();
}

bool app_loop(void) {
    HAL_GPIO_D8_clr();
    HAL_GPIO_LED_set();
    delay_ms(500);
    HAL_GPIO_LED_clr();
    HAL_GPIO_D8_set();
    delay_ms(500);

    return false;
}
