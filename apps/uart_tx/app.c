#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init(9600);
    uart_enable();
}

bool app_loop(void) {
    uint8_t on = '1';
    uint8_t off = '0';

    uart_write(&on, 1);
    delay_ms(500);
    uart_write(&off, 1);
    delay_ms(500);

    return false;
}
