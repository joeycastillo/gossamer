#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "rtc.h"
#include "adc.h"
#include "tcc.h"
#include "delay.h"
#include "drivers/oso_lcd.h"
#include <stdio.h>

#define NUM_MODES 6

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static void _display_time(rtc_date_time date_time);
static void _display_battery_voltage(void);
static void _display_led_value(uint8_t channel);

HAL_GPIO_PIN(LEFT,   B, 8)
HAL_GPIO_PIN(DOWN,   B, 9)
HAL_GPIO_PIN(RIGHT,  A, 18)
HAL_GPIO_PIN(SELECT, A, 16)
HAL_GPIO_PIN(UP,     A, 19)

void app_init(void) {
    rtc_init();
}

void app_setup(void) {
    // set up the external interrupt controller
    eic_init();

    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(PORT_PMUX_PMUXE_D_Val);
    HAL_GPIO_SCL_pmuxen(PORT_PMUX_PMUXE_D_Val);
    i2c_init();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);

    // fire RTC alarm every minute
    rtc_date_time date_time = {0};
    rtc_enable_alarm_interrupt(date_time, ALARM_MATCH_SS);

    // Left Button
    HAL_GPIO_LEFT_in();
    HAL_GPIO_LEFT_pullup();
    HAL_GPIO_LEFT_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(8, INTERRUPT_TRIGGER_FALLING);

    // Down Button
    HAL_GPIO_DOWN_in();
    HAL_GPIO_DOWN_pullup();
    HAL_GPIO_DOWN_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(9, INTERRUPT_TRIGGER_FALLING);

    // Right Button
    HAL_GPIO_RIGHT_in();
    HAL_GPIO_RIGHT_pullup();
    HAL_GPIO_RIGHT_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(2, INTERRUPT_TRIGGER_FALLING);

    // Select Button
    HAL_GPIO_SELECT_in();
    HAL_GPIO_SELECT_pullup();
    HAL_GPIO_SELECT_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(3, INTERRUPT_TRIGGER_FALLING);

    // Up Button
    HAL_GPIO_UP_in();
    HAL_GPIO_UP_pullup();
    HAL_GPIO_UP_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(0, INTERRUPT_TRIGGER_FALLING);

    // configure the TCC peripheral
    tcc_setup(0, 2);

    // TCC0 waveform output 0
    HAL_GPIO_A3_out();
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_E);
    // TCC0 waveform output 1
    HAL_GPIO_A4_out();
    HAL_GPIO_A4_pmuxen(HAL_GPIO_PMUX_E);
    // TCC0 waveform output 2
    HAL_GPIO_D1_out();
    HAL_GPIO_D1_pmuxen(HAL_GPIO_PMUX_F);
    // TCC0 waveform output 3
    HAL_GPIO_D0_out();
    HAL_GPIO_D0_pmuxen(HAL_GPIO_PMUX_F);

    TCC0->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_NPWM_Val;
    TCC0->PER.reg = 100;
    TCC0->CTRLA.bit.RUNSTDBY = 1;
    tcc_enable(0);

    HAL_GPIO_A5_in();
    HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_B);
}

uint8_t mode = 4;
int8_t brightness[4] = {0};

bool app_loop(void) {
    rtc_date_time date_time = rtc_get_date_time();

    if (date_time.unit.hour > 19 || date_time.unit.hour < 3) {
        if (!TCC0->CTRLA.bit.ENABLE) {
            tcc_enable(0);
        }
        TCC0->CC[0].reg = brightness[0];
        TCC0->CC[1].reg = brightness[1];
        TCC0->CC[2].reg = brightness[2];
        TCC0->CC[3].reg = brightness[3];
    } else {
        TCC0->CC[0].reg = 0;
        TCC0->CC[1].reg = 0;
        TCC0->CC[2].reg = 0;
        TCC0->CC[3].reg = 0;
        if (TCC0->CTRLA.bit.ENABLE) {
            delay_ms(100);
            tcc_disable(0);
            HAL_GPIO_A3_clr();
            HAL_GPIO_A4_clr();
            HAL_GPIO_D0_clr();
            HAL_GPIO_D1_clr();
        }
    }

    if (!HAL_GPIO_LEFT_read()) {
        if (mode == 0) mode = NUM_MODES - 1;
        else mode = (mode - 1);
    }
    if (!HAL_GPIO_RIGHT_read()) {
        mode = (mode + 1) % NUM_MODES;
    }

    if (!HAL_GPIO_UP_read()) {
        if (mode < 4) {
            brightness[mode] = MIN(brightness[mode] + 10, 100);
            if (brightness[mode] > 100) brightness[mode] = 0;
        } else if (mode == 4) {
            date_time.unit.minute = (date_time.unit.minute + 1) % 60;
            if (date_time.unit.minute == 0) {
                date_time.unit.hour = (date_time.unit.hour + 1) % 24;
            }
            date_time.unit.second = 0;
            rtc_set_date_time(date_time);
            _display_time(date_time);
            return false;
        }
    }

    if (!HAL_GPIO_DOWN_read()) {
        if (mode < 4) {
            brightness[mode] = MAX(brightness[mode] - 10, 0);
            if (brightness[mode] > 100) brightness[mode] = 0;
            TCC0->CC[mode].reg = brightness[mode];
        } else if (mode == 4) {
            if (date_time.unit.minute == 0) {
                date_time.unit.hour = (date_time.unit.hour + 23) % 24;
                date_time.unit.minute = 59;
            } else {
                date_time.unit.minute = (date_time.unit.minute - 1) % 60;
            }
            date_time.unit.second = 0;
            rtc_set_date_time(date_time);
            _display_time(date_time);
            return false;
        }
    }

    switch (mode) {
        case 0:
        case 1:
        case 2:
        case 3:
            _display_led_value(mode);
            break;
        case 4:
            _display_time(date_time);
            break;
        case 5:
            _display_battery_voltage();
            break;
        default:
            break;
    }

    return true;    
}

static void _display_time(rtc_date_time date_time) {
    int8_t hour_12 = date_time.unit.hour % 12;
    if (!hour_12) hour_12 += 12;
    char buf[7];
    sprintf(buf, "%2d:%02d ", hour_12, date_time.unit.minute);
    oso_lcd_print(buf);
    if (date_time.unit.hour < 12) {
        oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
        oso_lcd_set_indicator(OSO_LCD_INDICATOR_AM);
    } else {
        oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
        oso_lcd_set_indicator(OSO_LCD_INDICATOR_PM);
    }
}

static void _display_battery_voltage(void) {
    oso_lcd_set_indicator(OSO_LCD_INDICATOR_DATA);
    adc_enable();
    uint16_t val = adc_get_analog_value(10);
    float voltage = ((float)val / 65535.0) * 3.3 * 2;
    char buf[8];
    sprintf(buf, "%4.2f V", voltage);
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    oso_lcd_print(buf);
}

static void _display_led_value(uint8_t channel) {
    char buf[8];
    sprintf(buf, "c%d:%3d", channel, brightness[channel]);
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    oso_lcd_print(buf);
}
