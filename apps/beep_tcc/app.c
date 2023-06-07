#include "app.h"
#include "delay.h"
#include "tcc.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    HAL_GPIO_D9_out();
    HAL_GPIO_D9_pullup();

    HAL_GPIO_D10_out();
    HAL_GPIO_D10_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    HAL_GPIO_D11_out();
    HAL_GPIO_D11_pmuxen(HAL_GPIO_PMUX_TCC_ALT);

    tcc_init(0, GENERIC_CLOCK_0, TCC_PRESCALER_DIV8);
    tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    tcc_set_channel_polarity(0, 6, TCC_CHANNEL_POLARITY_INVERTED);
    tcc_set_period(0, 250);
    tcc_set_cc(0, 2, 0);
    tcc_enable(0);
}

bool app_loop(void) {
    if (!HAL_GPIO_D9_read()) {
        HAL_GPIO_LED_set();
        tcc_set_cc(0, 2, 125);
        delay_ms(50);
        tcc_set_cc(0, 2, 0);
        delay_ms(100);
    }
    HAL_GPIO_LED_clr();

    return false;
}
