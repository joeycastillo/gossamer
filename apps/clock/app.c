#include "sam.h"
#include "pins.h"
#include "app.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_LED_set();
}

bool app_loop(void) {
    return true;
}
