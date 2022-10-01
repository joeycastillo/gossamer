#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "rtc.h"
#include "adc.h"
#include "tcc.h"
#include "drivers/oso_lcd.h"
#include <stdio.h>

#define NUM_MODES 6

static void _display_time(rtc_date_time date_time);
static void _display_battery_voltage(void);
static void _display_led_value(uint8_t channel);

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

    // Button C
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(15, INTERRUPT_TRIGGER_FALLING);

    // Button B
    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(4, INTERRUPT_TRIGGER_FALLING);

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
uint8_t brightness[4] = {0};

bool app_loop(void) {
    rtc_date_time date_time = rtc_get_date_time();

    if (!HAL_GPIO_D6_read()) {
        mode = (mode + 1) % NUM_MODES;
    }

    if (!HAL_GPIO_D5_read()) {
        if (mode < 4) {
            brightness[mode] += 10;
            if (brightness[mode] > 100) brightness[mode] = 0;
            TCC0->CC[mode].reg = brightness[mode];
        } else if (mode == 4) {
            date_time.unit.minute = (date_time.unit.minute + 1) % 60;
            if (date_time.unit.minute == 0) {
                date_time.unit.hour = (date_time.unit.hour + 1) % 24;
            }
            date_time.unit.second = 0;
            rtc_set_date_time(date_time);
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
            // return true to hang out in STANDBY until an interrupt fires
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