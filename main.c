#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "sam.h"
#include "pins.h"
#include "app.h"
#include "system.h"
#include "delay.h"

#if __EMSCRIPTEN__
void resume_main_loop(void);
#endif

int main(void) {
    // set up system clocks
    sys_init();

    // set up system tick
    delay_init();

    // allow application to do its own initialization
    app_init();

    // restore from BACKUP state
    app_wake_from_backup();

    // allow application to do its initial setup
    app_setup();

#if __EMSCRIPTEN__
    resume_main_loop();
#else
    while (1) {
        bool can_sleep = app_loop();

        if (can_sleep) {
            _enter_standby_mode();
        }
    }
#endif

    return 0;
}

static void __empty() {
	// Empty
}

void app_wake_from_backup(void) __attribute__ ((weak, alias("__empty")));

void yield(void) __attribute__ ((weak, alias("__empty")));
