#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "rtc.h"
#include "drivers/oso_lcd.h"
#include <stdio.h>

static void _display_time(rtc_date_time date_time);

void app_init(void) {
    rtc_init();
}

void app_setup(void) {
    // set up the external interrupt controller
    eic_init();
    eic_enable();

    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_D);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_D);
    i2c_init();
    i2c_enable();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);

    // fire RTC alarm every minute
    // rtc_date_time date_time = {0};
    // rtc_enable_alarm_interrupt(date_time, ALARM_MATCH_SS);

    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(15, INTERRUPT_TRIGGER_FALLING);

    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(4, INTERRUPT_TRIGGER_FALLING);

    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_A);
    eic_configure_channel(7, INTERRUPT_TRIGGER_FALLING);
}

bool app_loop(void) {
    rtc_date_time date_time = rtc_get_date_time();

    if (!HAL_GPIO_D9_read()) {
        date_time.unit.hour = (date_time.unit.hour + 1) % 24;
        rtc_set_date_time(date_time);
    }
    if (!HAL_GPIO_D6_read()) {
        date_time.unit.minute = (date_time.unit.minute + 1) % 60;
        date_time.unit.second = 0;
        rtc_set_date_time(date_time);
    }

    _display_time(date_time);

    // return true to hang out in STANDBY until an interrupt fires
    return true;
}

static void _display_time(rtc_date_time date_time) {
    int8_t hour_12 = date_time.unit.hour % 12;
    if (!hour_12) hour_12 += 12;
    char buf[7];
    sprintf(buf, "%2d:%02d ", hour_12, date_time.unit.minute);
    oso_lcd_print(buf);
    if (date_time.unit.hour < 12) {
        oso_lcd_clear_indicator(OSO_LCD_INDICATOR_PM);
        oso_lcd_set_indicator(OSO_LCD_INDICATOR_AM);
    } else {
        oso_lcd_clear_indicator(OSO_LCD_INDICATOR_AM);
        oso_lcd_set_indicator(OSO_LCD_INDICATOR_PM);
    }
}
