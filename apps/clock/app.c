#include "sam.h"
#include "pins.h"
#include "app.h"
#include "rtc.h"

void app_init(void) {
    HAL_GPIO_LED_set();
    rtc_init();
    HAL_GPIO_D8_set();
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_D8_out();
}

bool app_loop(void) {
    rtc_date_time date_time = rtc_get_date_time();
    if (date_time.unit.second % 2) {
        HAL_GPIO_D8_clr();
        HAL_GPIO_LED_set();
    } else {
        HAL_GPIO_LED_clr();
        HAL_GPIO_D8_set();
    }

    return false;
}
