#include "app.h"
#include "ptc.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    // set PTC pin mux on analog on all pins but A0
    HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A2_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A4_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A6_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_A7_pmuxen(HAL_GPIO_PMUX_PTC);

    // set up touch inputs
    ptc_init();
    ptc_enable(3);
    ptc_enable(4);
    ptc_enable(5);
    ptc_enable(8);
    ptc_enable(9);
    ptc_enable(14);
    ptc_enable(15);
}

bool app_loop(void) {
    if (ptc_get_value(3) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(4) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(5) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(8) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(9) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(14) >= 2048) HAL_GPIO_LED_set();
    else if (ptc_get_value(15) >= 2048) HAL_GPIO_LED_set();
    else HAL_GPIO_LED_clr();
    
    return false;
}
