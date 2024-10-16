#include "app.h"
#include "tc.h"
#include "dac.h"
#include "dma.h"
#include "eic.h"
#include "delay.h"
#include "qtouch/touch.h"
#include <stdio.h>
#include <math.h>

void eic_callback(uint8_t channel);
void reset_to_bootloader(void);

extern volatile uint8_t measurement_done_touch;

// needed for arc4random to work
int getentropy(void *buffer, size_t length);
int getentropy(void *buffer, size_t length) {
    (void)buffer;
    (void)length;
    return 0;
}

// x and y values are a union of two 16-bit values
typedef union {
    struct {
        uint16_t x;
        uint16_t y;
    } position;
    uint32_t data;
} touch_val_t;

touch_val_t live_touch_value;

void app_init(void) {
    // set up 4 MHz clock for touch controller
    GCLK->GENCTRL[1].reg = GCLK_GENCTRL_DIV(2) |
                           GCLK_GENCTRL_SRC_OSC16M |
                           GCLK_GENCTRL_RUNSTDBY |
                           GCLK_GENCTRL_GENEN;
}

void app_setup(void) {
    // configure EIC
    eic_init();
    eic_enable();

    // Configure buttons as EIC inputs with pullups
    HAL_GPIO_STOP_LIVE_in();
    HAL_GPIO_STOP_LIVE_pullup();
    HAL_GPIO_STOP_LIVE_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_STOP_LIVE_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_STOP_LIVE_pin());

    HAL_GPIO_PLAY_PAUSE_in();
    HAL_GPIO_PLAY_PAUSE_pullup();
    HAL_GPIO_PLAY_PAUSE_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_PLAY_PAUSE_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_PLAY_PAUSE_pin());

    HAL_GPIO_RECORD_in();
    HAL_GPIO_RECORD_pullup();
    HAL_GPIO_RECORD_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_RECORD_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_RECORD_pin());

    HAL_GPIO_FASTER_in();
    HAL_GPIO_FASTER_pullup();
    HAL_GPIO_FASTER_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_FASTER_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_FASTER_pin());

    HAL_GPIO_SLOWER_in();
    HAL_GPIO_SLOWER_pullup();
    HAL_GPIO_SLOWER_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_SLOWER_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_SLOWER_pin());

    eic_configure_callback(eic_callback);

    // configure DACs
    HAL_GPIO_XOUT_pmuxen(HAL_GPIO_PMUX_DAC);
    HAL_GPIO_YOUT_pmuxen(HAL_GPIO_PMUX_DAC);
    dac_init();
    dac_enable(DAC_CHANNEL_BOTH);
    live_touch_value.position.x = 2048;
    live_touch_value.position.y = 2048;

    // Configure LEDs
    HAL_GPIO_LED_BLUE_out();
    HAL_GPIO_LED_GREEN_out();
    HAL_GPIO_LED_YELLOW_out();
    HAL_GPIO_LED_RED_out();

    // Configure touch pads
    HAL_GPIO_X0_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X0_out();
    HAL_GPIO_X1_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X1_out();
    HAL_GPIO_X2_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X2_out();
    HAL_GPIO_X3_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X3_out();
    HAL_GPIO_X4_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X4_out();
    HAL_GPIO_X5_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X5_out();
    HAL_GPIO_X6_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X6_out();
    HAL_GPIO_X7_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_X7_out();
    HAL_GPIO_Y0_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y0_in();
    HAL_GPIO_Y1_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y1_in();
    HAL_GPIO_Y2_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y2_in();
    HAL_GPIO_Y3_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y3_in();
    HAL_GPIO_Y4_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y4_in();
    HAL_GPIO_Y5_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y5_in();
    HAL_GPIO_Y6_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y6_in();
    HAL_GPIO_Y7_pmuxen(HAL_GPIO_PMUX_PTC);
    HAL_GPIO_Y7_in();

    // enable PTC clock
    GCLK->PCHCTRL[PTC_GCLK_ID].reg = GCLK_PCHCTRL_GEN_GCLK1_Val | (1 << GCLK_PCHCTRL_CHEN_Pos);
    MCLK->APBDMASK.bit.PTC_ = 1;

    // set up the TC4 interrupt handler to call touch_timer_handler
    tc_init(4, GENERIC_CLOCK_0, TC_PRESCALER_DIV1024);  // 8 MHz / 1024 = 7182
    tc_set_wavegen(4, TC_WAVE_WAVEGEN_MFRQ);            // Match frequency mode, overflow at CC0
    tc_count16_set_cc(4, 0, 156);                       // 7182 / 156 roughly equals 50 Hz or once every 20 ms
    TC4->COUNT16.INTENSET.reg = TC_INTENSET_OVF;        // enable overflow interrupt
    NVIC_EnableIRQ(TC4_IRQn);
    tc_enable(4);

    // Configure qtouch library
    touch_init();
}

double i = 0;

bool app_loop(void) {
    touch_process();
    if (measurement_done_touch == 1) {
        measurement_done_touch = 0;
    }

    uint8_t status = get_surface_status();
    if (status & 1) {
        // if there is a touch, the DAC outputs a voltage proportional to the touch position
        live_touch_value.position.x = (255 - get_surface_position(VER_POS)) << 4;
        live_touch_value.position.y = (get_surface_position(HOR_POS)) << 4;
    } else {
        // otherwise, the DAC outputs a random voltage on both channels
        live_touch_value.position.x = arc4random() % 4096;
        live_touch_value.position.y = arc4random() % 4096;
    }

    // plot the live position on each loop, UNLESS the play/pause button is being held down.
    // this will freeze the vectorscope's dot at the current position.
    if (HAL_GPIO_PLAY_PAUSE_read()) {
        dac_set_analog_value(0, live_touch_value.position.x);
        dac_set_analog_value(1, live_touch_value.position.y);
    }

    return false;
}

void irq_handler_tc4(void);
void irq_handler_tc4(void) {
    touch_timer_handler();
    HAL_GPIO_LED_BLUE_toggle();
    TC4->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF;
}

void eic_callback(uint8_t channel) {
    switch (channel) {
        case 1: // stop/live
            break;
        case 2: // play/pause
            break;
        case 15: // record
            // if both STOP and PLAY are also held down, reset to bootloader
            if (!HAL_GPIO_STOP_LIVE_read() && !HAL_GPIO_PLAY_PAUSE_read()) {
                reset_to_bootloader();
            }
            break;
        case 3: // faster
            // if both STOP and PLAY are also held down, just reset
            if (!HAL_GPIO_STOP_LIVE_read() && !HAL_GPIO_PLAY_PAUSE_read()) {
                NVIC_SystemReset();
            }
            break;
        case 0: // slower
            break;
    }
}

void reset_to_bootloader(void) {
    volatile uint32_t *dbl_tap_ptr = ((volatile uint32_t *)(HSRAM_ADDR + HSRAM_SIZE - 4));
    *dbl_tap_ptr = 0xf01669ef; // from the UF2 bootloaer: uf2.h line 255
    NVIC_SystemReset();
}
