#include "app.h"
#include "i2c.h"
#include "tc.h"
#include "delay.h"
#include "../clock/drivers/oso_lcd.h"

void app_init(void) {
    set_cpu_frequency(48000000);
}

void app_setup(void) {
    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    i2c_enable();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);
    oso_lcd_print("P$J&7");

    // set up the PWM pins
    HAL_GPIO_A1_out();      // A1   is TC4/WO[0]
    HAL_GPIO_A2_out();      // A2   is TC4/WO[1]
    HAL_GPIO_SCK_out();     // SCK  is TC5/WO[1]
    HAL_GPIO_MOSI_out();    // MOSI is TC5/WO[0]
    HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_TC_TCC);
    HAL_GPIO_A2_pmuxen(HAL_GPIO_PMUX_TC_TCC);
    HAL_GPIO_SCK_pmuxen(HAL_GPIO_PMUX_TC_TCC);
    HAL_GPIO_MOSI_pmuxen(HAL_GPIO_PMUX_TC_TCC);

    const int DIV = TC_PRESCALER_DIV1024;
    const int MID_A = 105;
    const int F_SHARP = 62;

    // set up the TC peripherals
    tc_init(4, GENERIC_CLOCK_0, DIV);
    tc_set_wavegen(4, TC_WAVEGEN_NORMAL_PWM);
    tc_set_counter_mode(4, TC_COUNTER_MODE_8BIT);
    tc_set_run_in_standby(4, true);
    tc_count8_set_period(4, MID_A);
    tc_init(5, GENERIC_CLOCK_0, DIV);
    tc_set_wavegen(5, TC_WAVEGEN_NORMAL_PWM);
    tc_set_counter_mode(5, TC_COUNTER_MODE_8BIT);
    tc_set_run_in_standby(5, true);
    tc_count8_set_period(5, F_SHARP);

    tc_enable(4);
    tc_enable(5);

    // demo 1
    // tc_count8_set_cc(4, 0, MID_A/2);
    // tc_count8_set_cc(4, 1, MID_A/2);
    // tc_count8_set_cc(5, 0, F_SHARP/2);
    // tc_count8_set_cc(5, 1, F_SHARP/2);

    // demo 2
    tc_count8_set_cc(4, 0, MID_A * 71 / 100);
    tc_count8_set_cc(4, 1, MID_A * 11 / 100);
    tc_count8_set_cc(5, 0, F_SHARP * 71 / 100);
    tc_count8_set_cc(5, 1, F_SHARP * 11 / 100);
}

bool app_loop(void) {

    return false;
}
