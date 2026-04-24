#include "app.h"
#include "i2cd.h"

#define DEVICE_ADDRESS 0x30
#define REGISTER_LED   0x10

static uint8_t reg_address = 0;
static bool have_reg_address = false;

void on_address_match(uint8_t sercom, bool direction) {
    (void)sercom;
    (void)direction;
    have_reg_address = false;
}

void on_data_received(uint8_t sercom, uint8_t data) {
    (void)sercom;
    if (!have_reg_address) {
        reg_address = data;
        have_reg_address = true;
    } else {
        if (reg_address == REGISTER_LED) {
            if (data) {
                HAL_GPIO_LED_set();
            } else {
                HAL_GPIO_LED_clr();
            }
        }
        reg_address++;
    }
}

void on_stop(uint8_t sercom) {
    (void)sercom;
    have_reg_address = false;
}

void irq_handler_sercom2(void) {
    i2c_device_irq_handler(2);
}

void app_init(void) {
}

void app_setup(void) {
    HAL_GPIO_LED_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);

    i2c_device_set_callbacks(on_address_match, on_data_received, NULL, on_stop, NULL);
    i2c_device_init(DEVICE_ADDRESS);
    i2c_device_enable();
}

bool app_loop(void) {
    return false;
}
