#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_TX_LED_out();
    HAL_GPIO_TX_LED_clr();
    HAL_GPIO_RX_LED_out();
    HAL_GPIO_RX_LED_clr();
    HAL_GPIO_TX_out();

    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM);
    uart_init_instance(0, UART_TXPO_2, UART_RXPO_NONE, 300);
    uart_set_irda_mode_instance(0, true);
    uart_enable_instance(0);
}

bool app_loop(void) {
    char hello[] = "Hello, world!\r\n";
    HAL_GPIO_TX_LED_toggle();
    uart_write_instance(0, hello, strlen(hello));
    delay_ms(50);
    HAL_GPIO_TX_LED_toggle();
    delay_ms(950);

    return false;
}

void irq_handler_sercom0(void);
void irq_handler_sercom0(void) {
    uart_irq_handler(0);
}