#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_RX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init(9600);
    uart_enable();
}

bool app_loop(void) {
    uint8_t data = uart_read();

    if (data == '1') HAL_GPIO_LED_set();
    if (data == '0') HAL_GPIO_LED_clr();

    return false;
}
