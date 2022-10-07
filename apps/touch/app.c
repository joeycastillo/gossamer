#include "app.h"
#include "ptc.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_A3_in();
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_B);
    ptc_init();
    ptc_enable_channel(2);
}

bool app_loop(void) {
    uint16_t val = ptc_get_value(2);

    if (val >= 2048) HAL_GPIO_LED_set();
    else HAL_GPIO_LED_clr();
    
    return false;
}
