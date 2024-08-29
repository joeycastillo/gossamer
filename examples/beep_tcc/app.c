#include "app.h"
#include "delay.h"
#include "tcc.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_A1_out();
    HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_TC_TCC);

    // 8 Mhz / 250 = 32 kHz
    tcc_init(0, GENERIC_CLOCK_0, TCC_PRESCALER_DIV1);
    tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    tcc_set_period(0, 250, false);
    tcc_set_cc(0, 1, 125, false);
    tcc_enable(0);
}

bool app_loop(void) {
    return false;
}
