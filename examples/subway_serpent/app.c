#include "app.h"
#include "i2c.h"
#include "eic.h"
#include "rtc.h"
#include "adc.h"
#include "tcc.h"
#include "delay.h"
#include "sunriset.h"
#include "lcd/oso_lcd.h"
#include <stdio.h>
#include <math.h>

typedef enum {
    MODE_CHANNEL_0 = 0,
    MODE_CHANNEL_1,
    MODE_CHANNEL_2,
    MODE_CHANNEL_3,
    MODE_TIME_SET,
    MODE_YEAR_SET,
    MODE_MONTH_SET,
    MODE_DAY_SET,
    MODE_MAIN_VOLTAGE,
    MODE_BATTERY_VOLTAGE,
    MODE_NUM_MODES
} serpent_mode_t;

#define DEFAULT_MODE MODE_TIME_SET

#define EST_OFFSET_MINUTES -300
#define REFERENCE_YEAR 2020
#define TIME_SET_DELAY 50

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

static void rtc_alarm_callback(uint8_t source);

static void _display_time(rtc_date_time_t date_time);
static void _display_year(rtc_date_time_t date_time);
static void _display_month(rtc_date_time_t date_time);
static void _display_day(rtc_date_time_t date_time);
static void _display_main_voltage(void);
static void _display_battery_voltage(void);
static void _display_led_value(uint8_t channel);
static void _recalculate_riset(void);

uint32_t date_time_to_unix_time(rtc_date_time_t date_time, uint32_t utc_offset);

serpent_mode_t mode = DEFAULT_MODE;
int8_t brightness[4] = {0, 15, 15, 50};
bool serpent_is_lit = false;
bool first_run = true;
bool lights_need_update = true;
bool riset_needs_update = true;
uint32_t todays_sunset;     // epoch time of next sunset, lights ON
uint32_t tomorrow_2am;      // epoch time of 2:00 AM tomorrow, lights OFF
uint32_t tomorrows_sunrise; // epoch time of next sunrise, lights OFF

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

    // Setting CONF to APWS lets the Automatic Power Switch handle power failure.
    SUPC->BBPS.bit.CONF = SUPC_BBPS_CONF_APWS;
    // Setting WAKEEN in ensures that we wake up when power is restored.
    SUPC->BBPS.bit.WAKEEN = 1;

    // enable the voltage reference
    SUPC->VREF.bit.VREFOE = 1;
    // configure SUPC to use a 2.5V reference voltage (we use this to check battery voltage)
    SUPC->VREF.bit.SEL = SUPC_VREF_SEL_2V5_Val;
    // and have it run only on demand
    SUPC->VREF.bit.ONDEMAND = 1;

    // initialize and enable the RTC
    rtc_init();
    rtc_enable();
}

void app_setup(void) {
    // set up the LED for general purpose use
    HAL_GPIO_LED_out();
    HAL_GPIO_LED_clr();

    // fire RTC alarm every minute
    rtc_date_time_t date_time = {0};
    rtc_enable_alarm_interrupt(date_time, ALARM_MATCH_SS);
    rtc_configure_callback(rtc_alarm_callback);

    // set up the ADC
    adc_init();
    // pin A5 is our main battery monitor
    HAL_GPIO_A5_in();
    HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_ADC);
    // we have a 940K voltage divider on the battery input, so we need to increase
    // the sampling time to 32 ADC clock cycles for a max input impedance of 1MOhm.
    adc_set_sampling_length(31); // 32 - 1
    // also configure it to use the 2.5V reference voltage we set up earlier
    /// FIXME: this should be wrapped in a gossamer function call
    ADC->REFCTRL.bit.REFSEL = ADC_REFCTRL_REFSEL_INTREF_Val;
    adc_enable();

    // set up the I²C peripheral...
    HAL_GPIO_SDA_out();
    HAL_GPIO_SCL_out();
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
    i2c_init();
    i2c_enable();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(1);

    // set up the external interrupt controller
    eic_init();

    // Up Button
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D5_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D5_pin());

    // Down Button
    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D6_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D6_pin());

    // Mode Button
    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D9_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D9_pin());

    // enable the EIC
    eic_enable();

    // configure the TCC peripheral:
    // 32768 Hz clock, no divider
    tcc_init(0, GENERIC_CLOCK_2, TCC_PRESCALER_DIV1);
    // normal PWM: a duty cycle for the LEDs
    tcc_set_wavegen(0, TCC_WAVEGEN_NORMAL_PWM);
    // Periods based on a count to 100 (makes it easy set a percentage of full power)
    tcc_set_period(0, 100, false);
    // run the LEDs in standby mode
    tcc_set_run_in_standby(0, true);

    // now configure the LED pins to act as outputs:
    // TCC0 waveform output 0
    HAL_GPIO_MOSI_out();
    HAL_GPIO_MOSI_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    // TCC0 waveform output 1
    HAL_GPIO_SCK_out();
    HAL_GPIO_SCK_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    // TCC0 waveform output 2
    HAL_GPIO_MISO_out();
    HAL_GPIO_MISO_pmuxen(HAL_GPIO_PMUX_TCC_ALT);
    // TCC0 waveform output 3
    HAL_GPIO_RX_out();
    HAL_GPIO_RX_pmuxen(HAL_GPIO_PMUX_TCC_ALT);

    // enable the TCC
    tcc_enable(0);
}

bool app_loop(void) {
    rtc_date_time_t date_time = rtc_get_date_time();
    uint32_t timestamp = date_time_to_unix_time(date_time, EST_OFFSET_MINUTES * 60);

    if (riset_needs_update) {
        _recalculate_riset();
        riset_needs_update = false;
    }

    if (!HAL_GPIO_D9_read()) {
        mode = (mode + 1) % MODE_NUM_MODES;
        delay_ms(TIME_SET_DELAY * 5);
    }

    // increase
    if (!HAL_GPIO_D5_read()) {
        switch (mode) {
            case MODE_CHANNEL_0:
            case MODE_CHANNEL_1:
            case MODE_CHANNEL_2:
            case MODE_CHANNEL_3:
                brightness[mode] = MIN(brightness[mode] + 1, 100);
                if (brightness[mode] > 100) brightness[mode] = 0;
                lights_need_update = true;
                break;
            case MODE_TIME_SET:
                date_time.unit.minute = (date_time.unit.minute + 1) % 60;
                if (date_time.unit.minute == 0) {
                    date_time.unit.hour = (date_time.unit.hour + 1) % 24;
                }
                date_time.unit.second = 0;
                rtc_set_date_time(date_time);
                _display_time(date_time);
                _recalculate_riset();
                lights_need_update = true;
                return false;
            case MODE_YEAR_SET:
                date_time.unit.year = (date_time.unit.year + 1) % 64;
                rtc_set_date_time(date_time);
                _display_year(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 4);
                break;
            case MODE_MONTH_SET:
                date_time.unit.month = 1 + (date_time.unit.month) % 12;
                rtc_set_date_time(date_time);
                _display_month(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 3);
                break;
            case MODE_DAY_SET:
                date_time.unit.day = 1 + (date_time.unit.day) % 31;
                rtc_set_date_time(date_time);
                _display_day(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 3);
                break;
            case MODE_MAIN_VOLTAGE:
            case MODE_BATTERY_VOLTAGE:
                _recalculate_riset();
                lights_need_update = true;
                break;
            case MODE_NUM_MODES:
                // nothing to do here
                break;
        }
    }

    // decrease
    if (!HAL_GPIO_D6_read()) {
        switch (mode) {
            case MODE_CHANNEL_0:
            case MODE_CHANNEL_1:
            case MODE_CHANNEL_2:
            case MODE_CHANNEL_3:
                brightness[mode] = MAX(brightness[mode] - 1, 0);
                if (brightness[mode] > 100) brightness[mode] = 0;
                lights_need_update = true;
                break;
            case MODE_TIME_SET:
                if (date_time.unit.minute == 0) {
                    date_time.unit.hour = (date_time.unit.hour + 23) % 24;
                    date_time.unit.minute = 59;
                } else {
                    date_time.unit.minute = (date_time.unit.minute - 1) % 60;
                }
                date_time.unit.second = 0;
                rtc_set_date_time(date_time);
                _display_time(date_time);
                _recalculate_riset();
                lights_need_update = true;
                return false;
            case MODE_YEAR_SET:
                date_time.unit.year = (date_time.unit.year + 63) % 64;
                rtc_set_date_time(date_time);
                _display_year(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 4);
                break;
            case MODE_MONTH_SET:
                date_time.unit.month = 1 + ((date_time.unit.month + 10) % 12);
                rtc_set_date_time(date_time);
                _display_month(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 3);
                break;
            case MODE_DAY_SET:
                date_time.unit.day = 1 + ((date_time.unit.day + 29) % 31);
                rtc_set_date_time(date_time);
                _display_day(date_time);
                _recalculate_riset();
                lights_need_update = true;
                delay_ms(TIME_SET_DELAY * 3);
                break;
            case MODE_MAIN_VOLTAGE:
            case MODE_BATTERY_VOLTAGE:
                _recalculate_riset();
                lights_need_update = true;
                break;
            case MODE_NUM_MODES:
                // nothing to do here
                break;
        }
    }

    switch (mode) {
        case MODE_CHANNEL_0:
        case MODE_CHANNEL_1:
        case MODE_CHANNEL_2:
        case MODE_CHANNEL_3:
            _display_led_value(mode);
            break;
        case MODE_TIME_SET:
            _display_time(date_time);
            break;
        case MODE_YEAR_SET:
            _display_year(date_time);
            break;
        case MODE_MONTH_SET:
            _display_month(date_time);
            break;
        case MODE_DAY_SET:
            _display_day(date_time);
            break;
        case MODE_MAIN_VOLTAGE:
            _display_main_voltage();
            break;
        case MODE_BATTERY_VOLTAGE:
            _display_battery_voltage();
            break;
        default:
            break;
    }

    // LOGIC:
    // The headlights should be on from sunset to 2am.
    // The sign should remain on at all times.
    // We should just set serpents_is_lit lights_need_update here; then below we'll update the lights if necessary.
    if (timestamp > todays_sunset && timestamp < tomorrow_2am) {
        if (first_run || !serpent_is_lit) {
            serpent_is_lit = true;
            lights_need_update = true;
        }
    } else {
        if (first_run || serpent_is_lit) {
            serpent_is_lit = false;
            lights_need_update = true;
        }
    }

    // now update the lights if necessary
    // OUTPUTS:
    // Channel 3 controls the sign in the front window
    // Channels 1 and 2 control the headlights of the subway serpent.
    // Channel 0 is unused and should remain at 0 always.
    if (lights_need_update) {
        // print that lights need update and the current timestamp
        lights_need_update = false;
        if (serpent_is_lit) {
            for (uint8_t i = 0; i < 4; i++) {
                tcc_set_cc(0, i, brightness[i], false);
            }
        } else {
            for (uint8_t i = 0; i < 4; i++) {
                tcc_set_cc(0, i, i == 3 ? brightness[i] : 0, false);
            }
        }
    }

    first_run = false;

    return true;    
}

static void _display_time(rtc_date_time_t date_time) {
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

static void _display_year(rtc_date_time_t date_time) {
    char buf[7];
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    sprintf(buf, "Yr:%2d ", date_time.unit.year + REFERENCE_YEAR - 2000);
    oso_lcd_print(buf);
}

static void _display_month(rtc_date_time_t date_time) {
    char buf[7];
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    sprintf(buf, "&7:%2d ", date_time.unit.month);
    oso_lcd_print(buf);
}

static void _display_day(rtc_date_time_t date_time) {
    char buf[7];
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    sprintf(buf, "Dy:%2d ", date_time.unit.day);
    oso_lcd_print(buf);
}

static void _display_main_voltage(void) {
    oso_lcd_set_indicator(OSO_LCD_INDICATOR_DATA);
    uint16_t val = adc_get_analog_value(HAL_GPIO_A5_pin());
    float voltage = ((float)val / 65535.0) * 2.5 * 2;
    char buf[8];
    sprintf(buf, "%4.2f V", voltage);
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    oso_lcd_print(buf);
}

static void _display_battery_voltage(void) {
    oso_lcd_set_indicator(OSO_LCD_INDICATOR_DATA);
    // Not in headers: 0x1D is SCALEDVBAT, 1/4 Scaled VBAT Supply
    uint16_t val = adc_get_analog_value_for_channel(0x1D);
    float voltage = ((float)val / 65535.0) * 2.5 * 4;
    char buf[8];
    sprintf(buf, "%4.2f V", voltage);
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    oso_lcd_set_indicator(OSO_LCD_INDICATOR_BATTERY);
    oso_lcd_print(buf);
}

static void _display_led_value(uint8_t channel) {
    char buf[8];
    sprintf(buf, "c%d:%3d", channel, brightness[channel]);
    oso_lcd_clear_indicator(OSO_LCD_INDICATOR_ALL);
    oso_lcd_print(buf);
}

#define LEAPOCH (946684800LL + 86400*(31+29))

#define DAYS_PER_400Y (365*400 + 97)
#define DAYS_PER_100Y (365*100 + 24)
#define DAYS_PER_4Y   (365*4   + 1)

rtc_date_time_t _date_time_from_unix_time(uint32_t timestamp, uint32_t utc_offset);
rtc_date_time_t _date_time_from_unix_time(uint32_t timestamp, uint32_t utc_offset) {
    rtc_date_time_t retval;
    retval.reg = 0;
    int32_t days, secs;
    int32_t remdays, remsecs, remyears;
    int32_t qc_cycles, c_cycles, q_cycles;
    int32_t years, months;
    int32_t wday, yday, leap;
    static const int8_t days_in_month[] = {31,30,31,30,31,31,30,31,30,31,31,29};
    timestamp += utc_offset;

    secs = timestamp - LEAPOCH;
    days = secs / 86400;
    remsecs = secs % 86400;
    if (remsecs < 0) {
        remsecs += 86400;
        days--;
    }

    wday = (3+days)%7;
    if (wday < 0) wday += 7;

    qc_cycles = (int)(days / DAYS_PER_400Y);
    remdays = days % DAYS_PER_400Y;
    if (remdays < 0) {
        remdays += DAYS_PER_400Y;
        qc_cycles--;
    }

    c_cycles = remdays / DAYS_PER_100Y;
    if (c_cycles == 4) c_cycles--;
    remdays -= c_cycles * DAYS_PER_100Y;

    q_cycles = remdays / DAYS_PER_4Y;
    if (q_cycles == 25) q_cycles--;
    remdays -= q_cycles * DAYS_PER_4Y;

    remyears = remdays / 365;
    if (remyears == 4) remyears--;
    remdays -= remyears * 365;

    leap = !remyears && (q_cycles || !c_cycles);
    yday = remdays + 31 + 28 + leap;
    if (yday >= 365+leap) yday -= 365+leap;

    years = remyears + 4*q_cycles + 100*c_cycles + 400*qc_cycles;

    for (months=0; days_in_month[months] <= remdays; months++)
        remdays -= days_in_month[months];

    years += 2000;

    months += 2;
    if (months >= 12) {
        months -=12;
        years++;
    }

    if (years < 2020 || years > 2083) return retval;
    retval.unit.year = years - REFERENCE_YEAR;
    retval.unit.month = months + 1;
    retval.unit.day = remdays + 1;

    retval.unit.hour = remsecs / 3600;
    retval.unit.minute = remsecs / 60 % 60;
    retval.unit.second = remsecs % 60;

    return retval;
}

static uint8_t is_leap(uint16_t y)
{
	y += 1900;
	return !(y%4) && ((y%100) || !(y%400));
}

static uint16_t _days_since_new_year(uint16_t year, uint8_t month, uint8_t day) {
    uint16_t DAYS_SO_FAR[] = {
        0,   // Jan
        31,  // Feb
        59,  // March
        90,  // April
        120, // May
        151, // June
        181, // July
        212, // August
        243, // September
        273, // October
        304, // November
        334  // December
    };

    return (is_leap(year) && (month > 2) ? 1 : 0) + DAYS_SO_FAR[month - 1] + day;
}

uint32_t _convert_to_unix_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t utc_offset);
uint32_t _convert_to_unix_time(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t minute, uint8_t second, uint32_t utc_offset) {
    uint32_t year_adj = year + 4800;
    uint32_t leap_days = 1 + (year_adj / 4) - (year_adj / 100) + (year_adj / 400);
    uint32_t days = 365 * year_adj + leap_days + _days_since_new_year(year, month, day) - 1;
    days -= 2472692;  /* Adjust to Unix epoch. */

    uint32_t timestamp = days * 86400;
    timestamp += hour * 3600;
    timestamp += minute * 60;
    timestamp += second;
    timestamp -= utc_offset;

    return timestamp;
}

uint32_t date_time_to_unix_time(rtc_date_time_t date_time, uint32_t utc_offset) {
    return _convert_to_unix_time(date_time.unit.year + REFERENCE_YEAR, date_time.unit.month, date_time.unit.day, date_time.unit.hour, date_time.unit.minute, date_time.unit.second, utc_offset);
}

static rtc_date_time_t _convert_zone(rtc_date_time_t date_time, uint32_t origin_utc_offset, uint32_t destination_utc_offset) {
    uint32_t timestamp = date_time_to_unix_time(date_time, origin_utc_offset);
    return _date_time_from_unix_time(timestamp, destination_utc_offset);
}

static void _recalculate_riset(void) {
    double rise, set, minutes, seconds;

    rtc_date_time_t date_time = rtc_get_date_time(); // the current local date / time
    date_time.unit.hour = 6;
    date_time.unit.minute = 0;
    rtc_date_time_t utc_today = _convert_zone(date_time, EST_OFFSET_MINUTES * 60, 0); // the current date / time in UTC
    rtc_date_time_t scratch_time; // scratchpad, contains different values at different times
    scratch_time.reg = utc_today.reg;

    // Beach 60th
    double lat = 40.59238533557047;
    double lon = -73.78927690293212;

    // sunriset returns the rise/set times as signed decimal hours in UTC.
    // this can mean hours below 0 or above 31, which won't fit into a rtc_date_time_t struct.
    // to deal with this, we set aside the offset in hours, and add it back before converting it to a rtc_date_time_t.
    double hours_from_utc = ((double)EST_OFFSET_MINUTES) / 60.0;

    sun_rise_set(scratch_time.unit.year + REFERENCE_YEAR, scratch_time.unit.month, scratch_time.unit.day, lon, lat, &rise, &set);

    rise += hours_from_utc;
    set += hours_from_utc;

    minutes = 60.0 * fmod(set, 1);
    seconds = 60.0 * fmod(minutes, 1);
    scratch_time.unit.hour = floor(set);
    if (seconds < 30) scratch_time.unit.minute = floor(minutes);
    else scratch_time.unit.minute = ceil(minutes);

    if (scratch_time.unit.minute == 60) {
        scratch_time.unit.minute = 0;
        scratch_time.unit.hour = (scratch_time.unit.hour + 1) % 24;
    }
    todays_sunset = date_time_to_unix_time(scratch_time, EST_OFFSET_MINUTES * 60);

    minutes = 60.0 * fmod(rise, 1);
    seconds = 60.0 * fmod(minutes, 1);
    scratch_time.unit.hour = floor(rise);
    if (seconds < 30) scratch_time.unit.minute = floor(minutes);
    else scratch_time.unit.minute = ceil(minutes);

    if (scratch_time.unit.minute == 60) {
        scratch_time.unit.minute = 0;
        scratch_time.unit.hour = (scratch_time.unit.hour + 1) % 24;
    }
    // this is technically today's sunrise, but we want tomorrow's.
    // hacky! just advance scratch_time by 24 hours.
    // this may be off by a minute or so but whatever.
    tomorrows_sunrise = date_time_to_unix_time(scratch_time, EST_OFFSET_MINUTES * 60);
    tomorrows_sunrise += 86400;

    // now make 2:00 AM tomorrow
    scratch_time = _date_time_from_unix_time(tomorrows_sunrise, 0);
    scratch_time.unit.hour = 2;
    scratch_time.unit.minute = 0;
    scratch_time.unit.second = 0;
    tomorrow_2am = date_time_to_unix_time(scratch_time, EST_OFFSET_MINUTES * 60);
}

static void rtc_alarm_callback(uint8_t source) {
    (void)source;

    // at noon, at :00, recalculate the sunset time
    if (rtc_get_date_time().unit.hour == 12 && rtc_get_date_time().unit.minute == 0) {
        riset_needs_update = true;
    }
}