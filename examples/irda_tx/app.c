#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "app.h"
#include "uart.h"
#include "delay.h"

const char filedata[] = "JBSWY3DPEHPK3PXPJBSWY3DPEHPK3PXPJBSWY3DPEHPK3PXPJBSWY3DPEE01234"; // this is test scecret, it is not real
const char filename[] = "test.otp";
char *upload_buffer;
size_t buflen = 16 + sizeof(filedata) + 2;

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_TX_out();
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    uart_init_instance(4, UART_TXPO_0, UART_RXPO_NONE, 900);
    uart_set_irda_mode_instance(4, true);
    uart_enable_instance(4);

    uint16_t checksum = 0;

    upload_buffer = malloc(buflen);
    memset(upload_buffer, 0, buflen);

    // First 16 bytes are our header.
    // Bytes 0-1: the size of the data to come after the header.
    ((uint16_t *) upload_buffer)[0] = sizeof(filedata);
    // bytes 2-13: filename with extension
    memcpy(upload_buffer + 2, filename, sizeof(filename));
    
    // compute a checksum of the header data
    for (size_t i = 0; i < 14; i++) {
        checksum += upload_buffer[i];
    }
    // bytes 14-15: the header checksum
    ((uint16_t *) upload_buffer)[7] = checksum;
    checksum = 0;

    memcpy(upload_buffer + 16, filedata, sizeof(filedata));
    for (size_t i = 16; i < buflen - 2; i++) {
        checksum += upload_buffer[i];
    }
    memcpy(upload_buffer + buflen - 2, &checksum, 2);
}

bool app_loop(void) {
    HAL_GPIO_LED_toggle();
    uart_write_instance(4, upload_buffer, buflen);
    delay_ms(50);
    HAL_GPIO_LED_toggle();
    delay_ms(1100);

    return false;
}

void irq_handler_sercom4(void);
void irq_handler_sercom4(void) {
    uart_irq_handler(4);
}