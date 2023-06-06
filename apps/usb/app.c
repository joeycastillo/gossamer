#include "app.h"
#include "tusb.h"
#include "tcc.h"
#include "usb.h"
#include "uart.h"
#include "system.h"
#include "delay.h"
#include <ctype.h>
#include <stdarg.h>

static void cdc_task(void);

void app_init(void) {
    HAL_GPIO_LED_out();
    set_cpu_frequency(48000000);
}

void app_setup(void) {
    // Set up debug UART on the UART TX pin
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM);
    uart_init(115200);
    uart_enable();

    // Output 500 Hz square wave on D12 to validate clock setup
    HAL_GPIO_D12_out();
    HAL_GPIO_D12_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    tcc_setup(0, GENERIC_CLOCK_0, TCC_CTRLA_PRESCALER_DIV1_Val);
    tcc_set_period(0, 48000000 / 1000);
    tcc_set_cc(0, 3, 48000000 / 2000);
    tcc_enable(0);

    // Set up USB
    usb_init();
    usb_enable();
}

bool app_loop(void) {
    tud_task();
    cdc_task();

    return false;
}

// echo to either Serial0 or Serial1
// with Serial0 as all lower case, Serial1 as all upper case
static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count) {
    uint8_t const case_diff = 'a' - 'A';

    for(uint32_t i=0; i<count; i++) {
        if (itf == 0) {
            // echo back 1st port as lower case
            if (isupper(buf[i])) buf[i] += case_diff;
        } else {
            // echo back 2nd port as upper case
            if (islower(buf[i])) buf[i] -= case_diff;
        }

        tud_cdc_n_write_char(itf, buf[i]);
    }
    tud_cdc_n_write_flush(itf);
}

//--------------------------------------------------------------------+
// USB CDC
//--------------------------------------------------------------------+
static void cdc_task(void) {
    uint8_t itf;

    for (itf = 0; itf < CFG_TUD_CDC; itf++) {
        if (tud_cdc_n_available(itf)) {
            uint8_t buf[64];

            uint32_t count = tud_cdc_n_read(itf, buf, sizeof(buf));

            // echo back to both serial ports
            echo_serial_port(0, buf, count);
            echo_serial_port(1, buf, count);
        }
    }
}

int debug_printf(const char *format, ...);
int debug_printf(const char *format, ...) {
    char buf[1024];
    va_list args;
    va_start(args, format);
    vsnprintf (buf, 1024, format, args);
    va_end(args);
    uart_write((uint8_t *)buf, strlen(buf));

    return strlen(buf);
}
