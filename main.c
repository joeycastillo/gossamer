#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "sam.h"
#include "pins.h"
#include "app.h"
#include "system.h"
#include "delay.h"

void _enter_standby_mode(void);

int main(void) {
    // set up system clocks
    sys_init();

    // set up system tick
    systick_init();

    // allow application to do its own initialization
    app_init();

    // TODO: restore from BACKUP state

    // allow application to do its initial setup
    app_setup();

    while (1) {
        bool can_sleep = app_loop();

        if (can_sleep) {
            _enter_standby_mode();
        }
    }

    return 0;
}
