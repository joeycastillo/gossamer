#include "app.h"
#include "eic.h"

void interrupt_callback(uint8_t channel);
bool led_state = false;

void app_init(void) {
    eic_init();
    eic_enable();
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    // Enable rising interrupts on button A and B
    HAL_GPIO_BUTTON_A_in();
    HAL_GPIO_BUTTON_A_pulldown();
    HAL_GPIO_BUTTON_A_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(8, INTERRUPT_TRIGGER_RISING);
    HAL_GPIO_BUTTON_B_in();
    HAL_GPIO_BUTTON_B_pulldown();
    HAL_GPIO_BUTTON_B_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(14, INTERRUPT_TRIGGER_RISING);

    // enable rising and falling interrupts on the switch
    HAL_GPIO_SWITCH_in();
    HAL_GPIO_SWITCH_pullup();
    HAL_GPIO_SWITCH_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(15, INTERRUPT_TRIGGER_BOTH);

    // set up the callback function
    eic_configure_callback(interrupt_callback);
}

bool app_loop(void) {
    // update the LED...
    HAL_GPIO_LED_write(led_state);

    // ...and immediately enter standby mode
    return true;
}

void interrupt_callback(uint8_t channel) {
    switch (channel) {
        case 8:
        case 14:
            // buttons just toggle the LED.
            led_state = !led_state;
            break;
        case 15:
            // switch sets the LED's value to that of the switch.
            led_state = HAL_GPIO_SWITCH_read();
        default:
            break;
    }
}
