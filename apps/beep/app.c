#include "app.h"
#include "delay.h"
#include "tc.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    HAL_GPIO_D9_out();
    HAL_GPIO_D9_pullup();

    HAL_GPIO_D10_out();
    HAL_GPIO_D11_out();
    HAL_GPIO_D11_clr();

    HAL_GPIO_D10_pmuxen(HAL_GPIO_PMUX_TC_TCC);

    tc_setup(3, GENERIC_CLOCK_0, TC_PRESCALER_DIV8);
    tc_set_counter_mode(3, TC_COUNTER_MODE_8BIT);
    tc_count8_set_period(3, 250);
    tc_set_wavegen(3, TC_WAVEGEN_NORMAL_PWM);
    tc_count16_set_cc(3, 0, 0);
    tc_enable(3);
}

bool app_loop(void) {
    if (!HAL_GPIO_D9_read()) {
        HAL_GPIO_LED_set();
        tc_count16_set_cc(3, 0, 125);
        delay_ms(50);
        tc_count16_set_cc(3, 0, 0);
        delay_ms(100);
    }
    HAL_GPIO_LED_clr();

    return false;
}
