#include "delay.h"
#include "sam.h"
#include "system.h"
#include "app.h"

void delay_init(void) {
    SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
}

static void _delay_cycles(uint32_t cycles) {
    uint8_t n = cycles >> 24;
    uint32_t buf = cycles;

    while (n--) {
        SysTick->LOAD = 0xFFFFFF;
        SysTick->VAL  = 0xFFFFFF;
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) yield();

        buf -= 0xFFFFFF;
    }

    SysTick->LOAD = buf;
    SysTick->VAL  = buf;
    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) yield();
}

static uint32_t _cycles_for_ms(const uint16_t ms) {
    int32_t freq = get_cpu_frequency();
    if (freq < 10000000) {
        return (ms * (freq / 10000)) * 10;
    } else {
        return (ms * (freq / 100000)) * 100;
    }
}

void delay_ms(const uint16_t ms) {
    _delay_cycles(_cycles_for_ms(ms));
}

static uint32_t _cycles_for_us(const uint16_t us) {
    int32_t freq = get_cpu_frequency();
    if (freq < 10000000) {
		return (us * (freq / 100000) - 1) / 10 + 1;
    } else {
		return (us * (freq / 10000) - 1) / 100 + 1;
    }
}

void delay_us(const uint16_t us) {
    _delay_cycles(_cycles_for_us(us));
}

