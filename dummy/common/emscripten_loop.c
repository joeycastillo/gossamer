#include "delay.h"
#include <emscripten.h>
#include <emscripten/html5.h>
#include "system.h"
#include "app.h"

#define ANIMATION_FRAME_ID_IS_VALID(id) ((id) >= 0)
#define ANIMATION_FRAME_ID_INVALID (-1)
#define ANIMATION_FRAME_ID_SUSPENDED (-2)

static bool sleeping = true;
static volatile long animation_frame_id = ANIMATION_FRAME_ID_INVALID;
bool pin_levels[3][32];

// make compiler happy
bool main_loop_is_sleeping(void);
static void main_loop_set_sleeping(bool sleeping);
static EM_BOOL main_loop(double time, void *userData);

static inline void request_next_frame(void) {
    if (animation_frame_id == ANIMATION_FRAME_ID_INVALID) {
        animation_frame_id = emscripten_request_animation_frame(main_loop, NULL);
    }
}

static EM_BOOL main_loop(double time, void *userData) {
    if (main_loop_is_sleeping()) {
        request_next_frame();
        return EM_FALSE;
    }

    if (sleeping) {
        sleeping = false;
    }

    animation_frame_id = ANIMATION_FRAME_ID_INVALID;
    bool can_sleep = app_loop();

    if (can_sleep) {
        sleeping = true;
        animation_frame_id = ANIMATION_FRAME_ID_INVALID;
        return EM_FALSE;
    }

    request_next_frame();
    return EM_FALSE;
}

void resume_main_loop(void) {
    if (!ANIMATION_FRAME_ID_IS_VALID(animation_frame_id)) {
        animation_frame_id = emscripten_request_animation_frame(main_loop, NULL);
    }
}

void suspend_main_loop(void) {
    if (ANIMATION_FRAME_ID_IS_VALID(animation_frame_id)) {
        emscripten_cancel_animation_frame(animation_frame_id);
    }

    animation_frame_id = ANIMATION_FRAME_ID_SUSPENDED;
}

void main_loop_sleep(uint32_t ms) {
    main_loop_set_sleeping(true);
    emscripten_sleep(ms);
    main_loop_set_sleeping(false);
    animation_frame_id = ANIMATION_FRAME_ID_INVALID;
}

bool main_loop_is_sleeping(void) {
    return EM_ASM_INT({ return Module['suspended']; }) != 0;
}

static void main_loop_set_sleeping(bool sleeping) {
    EM_ASM({
        Module['suspended'] = $0;
    }, sleeping);
}

void delay_init(void) {
}

void delay_ms(const uint16_t ms) {
    main_loop_sleep(ms);
}

void delay_us(const uint16_t us) {
    main_loop_sleep(us / 1000);
}

