#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_TX_out();
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init_instance(4, UART_TXPO_0, UART_RXPO_NONE, 900);
    uart_set_irda_mode_instance(4, true);
    uart_enable_instance(4);
}

bool app_loop(void) {
    char hello[20] = ">1TEST>Hello World*";
    uint8_t checksum = 0;
    for (size_t i = 0; i < strlen(hello) - 1; i++) {
        checksum += hello[i];
    }
    hello[18] = checksum;
    hello[19] = 0;
    HAL_GPIO_LED_toggle();
    uart_write_instance(4, hello, strlen(hello));
    delay_ms(50);
    HAL_GPIO_LED_toggle();
    delay_ms(1100);

    return false;
}

void irq_handler_sercom4(void);
void irq_handler_sercom4(void) {
    uart_irq_handler(4);
}