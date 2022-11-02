#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "tcc.h"
#include "delay.h"
#include "../clock/drivers/oso_lcd.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

HAL_GPIO_PIN(LIGHT,    A, 17)
HAL_GPIO_PIN(BUTTON_B, A, 11)
HAL_GPIO_PIN(BUTTON_C, A, 10)

void app_init(void) {
}

void app_setup(void) {
    eic_init();
    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);

    HAL_GPIO_BUTTON_B_in();
    HAL_GPIO_BUTTON_B_pullup();
    HAL_GPIO_BUTTON_B_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(3, INTERRUPT_TRIGGER_FALLING);

    HAL_GPIO_BUTTON_C_in();
    HAL_GPIO_BUTTON_C_pullup();
    HAL_GPIO_BUTTON_C_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(2, INTERRUPT_TRIGGER_FALLING);

    // set up TCC for light dimming
    tcc_setup(0, 2);
    // TCC0 waveform output 7
    HAL_GPIO_LIGHT_out();
    HAL_GPIO_LIGHT_pmuxen(HAL_GPIO_PMUX_F);

    TCC0->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_NPWM_Val;
    TCC0->PER.reg = 100;
    TCC0->CTRLA.bit.RUNSTDBY = 1;
    tcc_enable(0);
}

int8_t val = 0;

bool app_loop(void) {
    char message[] = " 0 #o";
    if (!HAL_GPIO_BUTTON_B_read()) {
        val = MAX(0, val - 1);
        TCC0->CC[3].reg = val;
    }
    if (!HAL_GPIO_BUTTON_C_read()) {
        val = MIN(100, val + 1);        
        TCC0->CC[3].reg = val;
    }

    if (val == 100) {
        message[0] = '1';
        message[1] = message[2] = '0';
    } else {
        message[0] = (val > 10) ? ((val / 10) + '0') : ' ';
        message[1] = (val % 10) + '0';
        message[2] = ' ';
    }
    
    delay_ms(25);

    oso_lcd_print(message);

    return false;
}
