#include "app.h"
#include "tusb.h"
#include "tcc.h"
#include "usb.h"
#include "system.h"
#include "delay.h"
#include <ctype.h>
#include <stdarg.h>

static void cdc_task(void);

void app_init(void) {
    HAL_GPIO_LED_out();
}

void app_setup(void) {
    usb_init();

    // uncomment to output a 1 Hz heartbeat on the LED to validate the clock setup.
    // HAL_GPIO_LED_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    // tcc_init(0, GENERIC_CLOCK_1, TCC_PRESCALER_DIV1024); // 48 MHz / 1024 = 46875 Hz
    // tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    // tcc_set_period(0, 46875, false); // 46875 Hz / 46875 = 1 Hz
    // tcc_set_cc(0, 2, 46875 / 100, false); // LED on TCC0/WO[6] is CC[2] on Sensor Watch
    // tcc_set_cc(0, 3, 46875 / 100, false); // LED on TCC0/WO[7] is CC[3] on Circuit Playground Express
    // tcc_enable(0);

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
