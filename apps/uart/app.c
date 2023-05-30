#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_RX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init(9600);
    uart_enable();
}

bool app_loop(void) {
    static uint16_t i = 0;
    char buf[32];

    sprintf(buf, "Hello, world! %d loops.\n", i++);
    uart_write((uint8_t *)buf, strlen(buf));
    delay_ms(1000);

    return false;
}
