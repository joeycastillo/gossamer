#include "app.h"
#include "ptc.h"

struct ptc_config config;

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    ptc_get_config_default(&config);
    config.pin = 4;
    config.yline = 2;
    ptc_init(PTC, &config);
}

bool app_loop(void) {
    ptc_start_conversion(PTC, &config);
    while (!ptc_is_conversion_finished(PTC));
    uint16_t val = ptc_get_conversion_result(PTC);

    if (val >= 2048) HAL_GPIO_LED_set();
    else HAL_GPIO_LED_clr();
    
    return false;
}
