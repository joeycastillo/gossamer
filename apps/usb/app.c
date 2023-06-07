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
    set_cpu_frequency(48000000);
}

void app_setup(void) {
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
