#include <stdio.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"
#include "usb.h"
#include "usb_cdc.h"

void app_init(void) {
}

void app_setup(void) {
    usb_init();
    usb_enable();
    HAL_GPIO_IR_ENABLE_out();
    HAL_GPIO_IR_ENABLE_clr();
    HAL_GPIO_IRSENSE_in();
    HAL_GPIO_IRSENSE_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init_instance(0, UART_TXPO_NONE, UART_RXPO_0, 900);
    uart_set_irda_mode_instance(0, true);
    uart_enable_instance(0);
}

bool app_loop(void) {
    uint8_t buf[32];
    size_t bytes_read = uart_read_instance(0, &buf, 32);
    if (bytes_read) {
        buf[31] = 0;
        // dump as hex
        for(int i = 0; i < bytes_read; i++) {
            printf("%c", buf[i]);
        }
    }

    yield();

    return false;
}

void irq_handler_sercom0(void);
void irq_handler_sercom0(void) {
    uart_irq_handler(0);
}

void yield(void) {
    tud_task();
    cdc_task();
}
