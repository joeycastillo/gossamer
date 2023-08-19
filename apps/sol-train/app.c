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

void app_init(void) {
    // Confugure brown-out detector to put us into
    // BACKUP mode when main power fails
    SUPC->BOD33.bit.ACTION = SUPC_BOD33_ACTION_BKUP_Val;
    // Enable sampling in BACKUP mode
    SUPC->BOD33.bit.PSEL = SUPC_BOD33_PSEL_DIV256_Val;
    // Run brown-out detector in backup mode, so that
    // we can detect power restoration
    SUPC->BOD33.bit.RUNBKUP = 1;
    // Enable brown-out detector
    SUPC->BOD33.bit.ENABLE = 1;

    // Setting WAKEEN in will ensure that we wake up
    // when power is restored.
    SUPC->BBPS.bit.WAKEEN = 1;

    rtc_init();
    rtc_enable();
}

void app_setup(void) {
    HAL_GPIO_LED_out();

    // set up the external interrupt controller
    eic_init();
    eic_enable();

    // set up (but do not enable) the ADC
    adc_init();
    // we have a 200K voltage divider on the battery input, so we
    // need to increase the sampling time to 6 ADC clock cycles
    ADC->SAMPCTRL.bit.SAMPLEN = 6;

    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_C);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_C);
    i2c_init();
    i2c_enable();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(1);

    // fire RTC alarm every minute
    rtc_date_time date_time = {0};
    rtc_enable_alarm_interrupt(date_time, ALARM_MATCH_SS);

    // Down Button
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(15, INTERRUPT_TRIGGER_FALLING);

    // Up Button
    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(4, INTERRUPT_TRIGGER_FALLING);

    // Mode Button
    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(7, INTERRUPT_TRIGGER_FALLING);

    // configure the TCC peripheral
    tcc_init(0, GENERIC_CLOCK_2, TCC_PRESCALER_DIV1);

    tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    tcc_set_period(0, 100);
    tcc_set_run_in_standby(0, true);

    tcc_enable(0);

    // TCC0 waveform output 0
    HAL_GPIO_A3_out();
    HAL_GPIO_A3_pmuxen(HAL_GPIO_PMUX_TC_TCC);
    // TCC0 waveform output 1
    HAL_GPIO_A4_out();
    HAL_GPIO_A4_pmuxen(HAL_GPIO_PMUX_TC_TCC);
    // TCC0 waveform output 2
    HAL_GPIO_D1_out();
    HAL_GPIO_D1_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    // TCC0 waveform output 3
    HAL_GPIO_D0_out();
    HAL_GPIO_D0_pmuxen(HAL_GPIO_PMUX_TCC_ALT);

    tcc_enable(0);

    HAL_GPIO_A5_in();
    HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_B);
}

uint8_t mode = 4;
int8_t brightness[4] = {0};

bool app_loop(void) {
    rtc_date_time date_time = rtc_get_date_time();

    if (!HAL_GPIO_D9_read()) {
        mode = (mode + 1) % NUM_MODES;
    }

    if (!HAL_GPIO_D6_read()) {
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

    if (!HAL_GPIO_D5_read()) {
        if (mode < 4) {
            brightness[mode] = MAX(brightness[mode] - 10, 0);
            if (brightness[mode] > 100) brightness[mode] = 0;
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

    if (date_time.unit.hour > 19 || date_time.unit.hour < 3) {
        if (!tcc_is_enabled(0)) {
            tcc_enable(0);
        }
        for (uint8_t i = 0; i < 4; i++) {
            tcc_set_cc(0, i, brightness[i]);
        }
    } else {
        for (uint8_t i = 0; i < 4; i++) {
            tcc_set_cc(0, i, 0);
        }
        if (TCC0->CTRLA.bit.ENABLE) {
            delay_ms(100);
            tcc_disable(0);
            HAL_GPIO_A3_clr();
            HAL_GPIO_A4_clr();
            HAL_GPIO_D0_clr();
            HAL_GPIO_D1_clr();
        }
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
