#include "app.h"
#include "rtc.h"
#include "i2c.h"
#include "eic.h"
#include "adc.h"
#include "tc.h"
#include "delay.h"
#include "../../drivers/lcd/oso_lcd.h"
#include "drivers/bme280.h"
#include "drivers/ltr390.h"
#include "drivers/pmsa003i.h"
#include "drivers/m138.h"
#include <stdio.h>
#include <string.h>

bool woke_for_measurement = false;
bool woke_for_rainfall = false;
bool display_needs_update = true;
bool time_synced = false;

uint16_t rain_this_hour = 0;
uint16_t wind_speeds_this_hour[60] = {0};
uint8_t wind_directions_this_hour[60] = {0};
uint16_t visible_light_this_hour[60] = {0};
uint16_t uv_light_this_hour[60] = {0};
uint16_t temperature_this_hour[60] = {0};
uint16_t humidity_this_hour[60] = {0};
uint16_t pressure_this_hour[60] = {0};
uint16_t pm10_this_hour[60] = {0};
uint16_t pm25_this_hour[60] = {0};
uint16_t pm100_this_hour[60] = {0};

float battery_voltage;

typedef enum weather_station_display_mode_t {
    DISPLAY_MODE_TIME = 0,
    DISPLAY_MODE_PENDING_MESSAGES,
    DISPLAY_MODE_BATTERY_VOLTAGE,
    DISPLAY_MODE_TEMP,
    DISPLAY_MODE_SEND_TEST_MESSAGE,
    DISPLAY_MODE_CLEAR_QUEUE,
    DISPLAY_MODE_NUMBER_OF_MODES
} weather_station_display_mode_t;

weather_station_display_mode_t display_mode = 0;

typedef enum wind_direction_t {
    WIND_DIRECTION_NORTH = 0,
    WIND_DIRECTION_NORTH_NORTHEAST,
    WIND_DIRECTION_NORTHEAST,
    WIND_DIRECTION_EAST_NORTHEAST,
    WIND_DIRECTION_EAST,
    WIND_DIRECTION_EAST_SOUTHEAST,
    WIND_DIRECTION_SOUTHEAST,
    WIND_DIRECTION_SOUTH_SOUTHEAST,
    WIND_DIRECTION_SOUTH,
    WIND_DIRECTION_SOUTH_SOUTHWEST,
    WIND_DIRECTION_SOUTHWEST,
    WIND_DIRECTION_WEST_SOUTHWEST,
    WIND_DIRECTION_WEST,
    WIND_DIRECTION_WEST_NORTHWEST,
    WIND_DIRECTION_NORTHWEST,
    WIND_DIRECTION_NORTH_NORTHWEST,
    WIND_DIRECTION_UNKNOWN
} wind_direction_t;

// define a struct for storing weather data
typedef struct weather_data_t
{
    uint32_t magic;                 // 0
    uint32_t timestamp;             // 1
    int16_t battery_temperature;    // 2
    int16_t temperature_min;        // 3
    int16_t temperature_max;        // 4
    int16_t temperature_average;    // 5
    uint16_t battery_voltage;       // 6
    uint16_t visible_light_min;     // 7
    uint16_t visible_light_max;     // 8
    uint16_t visible_light_average; // 9
    uint16_t uv_light_min;          // 10
    uint16_t uv_light_max;          // 11
    uint16_t uv_light_average;      // 12
    uint16_t humidity_min;          // 13
    uint16_t humidity_max;          // 14
    uint16_t humidity_average;      // 15
    uint16_t pressure_min;          // 16
    uint16_t pressure_max;          // 17
    uint16_t pressure_average;      // 18
    uint16_t wind_speed_min;        // 19
    uint16_t wind_speed_max;        // 20
    uint16_t wind_speed_average;    // 21
    uint16_t rainfall;              // 22
    uint16_t particles_1_0_min;     // 23
    uint16_t particles_1_0_max;     // 24
    uint16_t particles_1_0_average; // 25
    uint16_t particles_2_5_min;     // 26
    uint16_t particles_2_5_max;     // 27
    uint16_t particles_2_5_average; // 28
    uint16_t particles_10_0_min;    // 29
    uint16_t particles_10_0_max;    // 30
    uint16_t particles_10_0_average;// 31
    uint8_t wind_directions[30];    // 32
    uint8_t bad_wind_readings;      // 33
    uint8_t checksum;               // 34
} weather_data_t;

void eic_callback(uint8_t channel);
void rtc_callback(uint8_t source);

static uint16_t get_max_value_for_hour(uint16_t *datapoints) {
    uint16_t max = 0;
    for (uint8_t i = 0; i < 60; i++) {
        if ((datapoints[i] != 0xFFFF) && (datapoints[i] > max)) {
            max = datapoints[i];
        }
    }

    return max;
}

static uint16_t get_min_value_for_hour(uint16_t *datapoints) {
    uint16_t min = 0xffff;
    for (uint8_t i = 0; i < 60; i++) {
        if ((datapoints[i] != 0xFFFF) && (datapoints[i] < min)) {
            min = datapoints[i];
        }
    }

    return min;
}

static uint16_t get_mean_value_for_hour(uint16_t *datapoints) {
    uint32_t sum = 0;
    uint8_t denominator = 0;
    for (uint8_t i = 0; i < 60; i++) {
        sum += datapoints[i];
        denominator++;
    }

    if (denominator) return sum / denominator;
    else return 0;
}

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

    // init the RTC    
    rtc_init();
    rtc_enable();

    swarm_m138_get_date_time();
}

void app_setup(void) {
    // start communication with the M138 satellite modem
    swarm_m138_begin();

    // set up a wake every minute
    rtc_date_time alarm_time = {0};
    rtc_enable_alarm_interrupt(alarm_time, ALARM_MATCH_SS);
    rtc_configure_callback(rtc_callback);

    // set up the external interrupt controller
    eic_init();
    eic_enable();
    eic_configure_callback(eic_callback);

    // Button Interrupts on pins D5, D6, and D9
    HAL_GPIO_D5_in();
    HAL_GPIO_D5_pullup();
    HAL_GPIO_D5_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D5_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D5_pin());
    HAL_GPIO_D6_in();
    HAL_GPIO_D6_pullup();
    HAL_GPIO_D6_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D6_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D6_pin());
    HAL_GPIO_D9_in();
    HAL_GPIO_D9_pullup();
    HAL_GPIO_D9_pmuxen(HAL_GPIO_PMUX_EIC);
    eic_configure_pin(HAL_GPIO_D9_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_D9_pin());

    // Rainfall Interrupt on pin A4
    HAL_GPIO_A4_in();
    HAL_GPIO_A4_pmuxen(HAL_GPIO_PMUX_EIC);
    HAL_GPIO_A4_pullup();
    eic_configure_pin(HAL_GPIO_A4_pin(), INTERRUPT_TRIGGER_FALLING);
    eic_enable_interrupt(HAL_GPIO_A4_pin());

    // Wind Event on pin D10
    HAL_GPIO_D10_in();
    HAL_GPIO_D10_pmuxen(HAL_GPIO_PMUX_EIC);
    HAL_GPIO_D10_pullup();
    eic_configure_pin(HAL_GPIO_D10_pin(), INTERRUPT_TRIGGER_FALLING);
    // eic_enable_interrupt(HAL_GPIO_D10_pin()); // Debug purposes only: wakes with each revolution of the anemometer

    // Have the External Interrupt Controller generate an event when the anemometer switch (on EIC channel 3) closes.
    eic_enable_event(HAL_GPIO_D10_pin());
    // TC4's event 0 input is the user of the event. Connect this user to the Event System's channel 0.
    EVSYS->USER[EVSYS_ID_USER_TC4_EVU].reg = EVSYS_USER_CHANNEL(0 + 1);
    // Set up channel 0:
    EVSYS->CHANNEL[0].reg = EVSYS_CHANNEL_EVGEN(EVSYS_ID_GEN_EIC_EXTINT_3) | // External interrupt 6 generates events on channel 0
                            EVSYS_CHANNEL_RUNSTDBY |                         // The channel should run in standby sleep mode...
                            EVSYS_CHANNEL_PATH_ASYNCHRONOUS;                 // on the asynchronous path (event drives action directly)

    // Pin 12 was active HIGH enable of secondary regulator...
    // but we're not using it.
    HAL_GPIO_D12_out();
    HAL_GPIO_D12_clr();

    // initialize TC4
    tc_init(4, GENERIC_CLOCK_3, TC_PRESCALER_DIV1);
    tc_set_run_in_standby(4, true);
    tc_set_counter_mode(4, TC_COUNTER_MODE_16BIT);

    // configure TC4's event control:
    TC4->COUNT16.EVCTRL.reg = TC_EVCTRL_TCEI | TC_EVCTRL_EVACT_COUNT;   // Event 0 should increment the count
    tc_enable(4);

    // LED for debugging
    HAL_GPIO_LED_out();

    // set up the I²C peripheral...
    HAL_GPIO_SDA_pmuxen(HAL_GPIO_PMUX_SERCOM);
    HAL_GPIO_SCL_pmuxen(HAL_GPIO_PMUX_SERCOM);
    i2c_init();
    i2c_enable();
    // ...and the Oddly Specific LCD
    oso_lcd_begin();
    oso_lcd_fill(0);

    bme280_begin();
    bme280_read_calibration();

    // the sensor is set to a gain of 3 and a resolution of 16 bits by default
    ltr390_init();
}

static wind_direction_t direction_from_adc_get_analog_value(uint16_t value) {
    if (value > 59000 && value < 60300) {
        return WIND_DIRECTION_NORTH;
    } else if (value > 43000 && value < 44300) {
        return WIND_DIRECTION_NORTH_NORTHEAST;
    } else if (value > 46000 && value < 47300) {
        return WIND_DIRECTION_NORTHEAST;
    } else if (value > 13400 && value < 14700) {
        return WIND_DIRECTION_EAST_NORTHEAST;
    } else if (value > 14700 && value < 16000) {
        return WIND_DIRECTION_EAST;
    } else if (value > 10700 && value < 12000) {
        return WIND_DIRECTION_EAST_SOUTHEAST;
    } else if (value > 25600 && value < 26900) {
        return WIND_DIRECTION_SOUTHEAST;
    } else if (value > 19000 && value < 20300) {
        return WIND_DIRECTION_SOUTH_SOUTHEAST;
    } else if (value > 35000 && value < 36300) {
        return WIND_DIRECTION_SOUTH;
    } else if (value > 31400 && value < 32700) {
        return WIND_DIRECTION_SOUTH_SOUTHWEST;
    } else if (value > 53700 && value < 55000) {
        return WIND_DIRECTION_SOUTHWEST;
    } else if (value > 52400 && value < 53700) {
        return WIND_DIRECTION_WEST_SOUTHWEST;
    } else if (value > 63100 && value < 64400) {
        return WIND_DIRECTION_WEST;
    } else if (value > 60300 && value < 61500) {
        return WIND_DIRECTION_WEST_NORTHWEST;
    } else if (value > 61800 && value < 62900) {
        return WIND_DIRECTION_NORTHWEST;
    } else if (value > 56300 && value < 57300) {
        return WIND_DIRECTION_NORTH_NORTHWEST;
    } else {
        return WIND_DIRECTION_UNKNOWN;
    }
}

static void update_display(void) {
    char buf[20];
    rtc_date_time datetime = rtc_get_date_time();
    bme280_reading_t bme_reading;

    oso_lcd_fill(0);
    switch (display_mode) {
        case DISPLAY_MODE_PENDING_MESSAGES:
            oso_lcd_set_indicator(OSO_LCD_INDICATOR_DATA);
            oso_lcd_print("     ");
            swarm_m138_get_transmit_status();
            break;
        case DISPLAY_MODE_TIME:
        {
            uint8_t hour = (datetime.unit.hour + 18) % 12;
            sprintf(buf, "%2d:%02d ", hour ? hour : 12, datetime.unit.minute);
            oso_lcd_print(buf);
            if (datetime.unit.hour < 12) {
                oso_lcd_set_indicator(OSO_LCD_INDICATOR_AM);
            } else {
                oso_lcd_set_indicator(OSO_LCD_INDICATOR_PM);
            }
        }
            break;
        case DISPLAY_MODE_TEMP:
            bme280_read(&bme_reading);
            sprintf(buf, "te:%3.1f", bme_reading.temperature);
            oso_lcd_print(buf);
            break;
        // case DISPLAY_MODE_HUMID:
        // {
        //     bme280_read(&bme_reading);
        //     sprintf(buf, "hu:%3.1f", bme_reading.humidity);
        //     oso_lcd_print(buf);
        //     break;
        // }
        // case DISPLAY_MODE_PRESSURE:
        // {
        //     bme280_read(&bme_reading);
        //     sprintf(buf, "%5.1f", bme_reading.pressure);
        //     oso_lcd_print(buf);
        //     break;
        // }
        // case DISPLAY_MODE_WIND_SPEED:
        //         TC4->COUNT16.CTRLBSET.reg = TC_CTRLBSET_CMD_READSYNC;
        //         while (TC4->COUNT16.SYNCBUSY.bit.CTRLB);
        //         while (TC4->COUNT16.SYNCBUSY.bit.COUNT);
        //         sprintf(buf, "$J:%3d", TC4->COUNT16.COUNT.reg);
        //         oso_lcd_print(buf);
        //     break;
        // case DISPLAY_MODE_WIND_DIRECTION:
        //     if (datetime.unit.minute == 0) {
        //         oso_lcd_print("nodata");
        //     } else {
        //         sprintf(buf, "$J:d%2d", wind_directions_this_hour[datetime.unit.minute - 1]);
        //         oso_lcd_print(buf);
        //     }
        //     break;
        // case DISPLAY_MODE_RAINFALL_THIS_HOUR:
        //     sprintf(buf, "rf:%3d", rain_this_hour);
        //     oso_lcd_print(buf);
        //     break;
        case DISPLAY_MODE_BATTERY_VOLTAGE:
        {
            sprintf(buf, "bt:%03.1f", 2 * 3.3 * battery_voltage / 65535.0);
            oso_lcd_print(buf);
        }
            break;
        case DISPLAY_MODE_SEND_TEST_MESSAGE:
            oso_lcd_print("Test.?");
            break;
        case DISPLAY_MODE_CLEAR_QUEUE:
            oso_lcd_print("qclr.?");            
            break;
        default:
            oso_lcd_print(" .?.?.? ");
            break;
    }
}

static void add_checksum(weather_data_t *data) {
    uint8_t checksum = 0;
    uint8_t *dataptr = (uint8_t *)data;
    for (uint8_t i = 0; i < sizeof(weather_data_t) - 1; i++) {
        checksum += dataptr[i];
    }

    data->checksum = checksum;
}

static void commit_action(void) {
    char buf[20];
    rtc_date_time datetime = rtc_get_date_time();
    bme280_reading_t bme_reading;

    oso_lcd_fill(0);
    switch (display_mode) {
        case DISPLAY_MODE_SEND_TEST_MESSAGE:
        {
            weather_data_t test_data = {0};
            test_data.magic = 0x54534554; // "TEST"
            test_data.timestamp = datetime.reg;
            add_checksum(&test_data);
            swarm_m138_transmit_data((uint8_t *)&test_data, sizeof(test_data));
        }
            break;
        case DISPLAY_MODE_CLEAR_QUEUE:
            // TODO: clear queue
            break;
        default:
            break;
    }
}

bool app_loop(void) {
    if (woke_for_rainfall) {
        woke_for_rainfall = false;
        return true; // nothing to do here; go back to sleep
    }

    swarm_m138_handle_received_data();

    // on button A press, mode --
    if (HAL_GPIO_D9_read() == 0) {
        display_mode = (display_mode + DISPLAY_MODE_NUMBER_OF_MODES - 1) % DISPLAY_MODE_NUMBER_OF_MODES;
        display_needs_update = true;
        delay_ms(100);
    }

    // on button B press, mode ++
    if (HAL_GPIO_D6_read() == 0) {
        display_mode = (display_mode + 1) % DISPLAY_MODE_NUMBER_OF_MODES;
        display_needs_update = true;
        delay_ms(100);
    }

    // on button C press, execute mode if needed
    if (HAL_GPIO_D5_read() == 0) {
        display_needs_update = true;
        commit_action();
        delay_ms(100);
    }

    if (woke_for_measurement) {
        woke_for_measurement = false;
        rtc_date_time datetime = rtc_get_date_time();
        char buf[20];

        // enable ADC
        adc_init();
        adc_enable(); // disabled below

        // get battery voltage on A5
        HAL_GPIO_A5_pmuxen(HAL_GPIO_PMUX_ADC);
        adc_set_sampling_length(63);
        battery_voltage = adc_get_analog_value(HAL_GPIO_A5_pin());
        battery_voltage = 2 * 3.3 * battery_voltage / 65535.0;
        HAL_GPIO_A5_pmuxdis();

        // enable wind direction pin
        HAL_GPIO_A1_pmuxen(HAL_GPIO_PMUX_ADC);
        adc_set_sampling_length(16);
        uint16_t wind_reading = adc_get_analog_value(HAL_GPIO_A1_pin());
        // disable wind direction pin
        HAL_GPIO_A1_pmuxdis();

        // get temperature and humidity
        bme280_reading_t bme_reading;
        // bme280_read(&bme_reading);
        float temperature = bme_reading.temperature;
        float humidity = bme_reading.humidity;
        float pressure = bme_reading.pressure;

        // get air quality
        pms_aqi_data_t air_quality_data = {0};
        pmsa003i_get_data(&air_quality_data);

        uint32_t visible_light = ltr390_readALS();
        uint32_t uv_light = ltr390_readUVS();

        // store the temperature for this minute
        temperature_this_hour[datetime.unit.minute] = (int16_t)(temperature * 100);
        // store the humidity for this minute
        humidity_this_hour[datetime.unit.minute] = (uint16_t)(humidity * 100);
        // store the barometric pressure for this minute
        pressure_this_hour[datetime.unit.minute] = (uint16_t)(pressure * 10);

        // store air quality for this minute
        pm10_this_hour[datetime.unit.minute] = air_quality_data.pm10_standard;
        pm25_this_hour[datetime.unit.minute] = air_quality_data.pm25_standard;
        pm100_this_hour[datetime.unit.minute] = air_quality_data.pm100_standard;

        /// store the light level for this minute
        visible_light_this_hour[datetime.unit.minute] = visible_light;
        uv_light_this_hour[datetime.unit.minute] = uv_light;

        // store the wind data for this minute
        wind_directions_this_hour[datetime.unit.minute] = direction_from_adc_get_analog_value(wind_reading);
        wind_speeds_this_hour[datetime.unit.minute] = tc_count16_get_count(4);
        // Reset the wind speed count
        tc_count16_set_count(4, 0);

        // 5 minutes after the top of the hour, sync clock with satellite
        if (!time_synced || datetime.unit.minute == 5) {
            swarm_m138_get_date_time();
        }

        // at the end of each hour, uplink data to the satellite
        if (datetime.unit.minute == 59) {
            // get battery temperature on TS
            HAL_GPIO_TS_pmuxen(HAL_GPIO_PMUX_ADC);
            uint16_t battery_temperature = adc_get_analog_value(HAL_GPIO_TS_pin());
            HAL_GPIO_TS_pmuxdis();

            // assemble the data
            weather_data_t outgoing_data = {0};
            // outgoing_data.magic = 0x5343594E; // "NYCS"
            // outgoing_data.magic = 0x53434C53; // "SLCS"
            outgoing_data.magic = 0x5351415A; // "ZAQS"
            outgoing_data.timestamp = datetime.reg;
            outgoing_data.battery_voltage = (uint16_t)(battery_voltage * 1000);
            outgoing_data.battery_temperature = battery_temperature; 
            outgoing_data.visible_light_average = get_max_value_for_hour(visible_light_this_hour);
            outgoing_data.visible_light_min = get_min_value_for_hour(visible_light_this_hour);
            outgoing_data.visible_light_max = get_mean_value_for_hour(visible_light_this_hour);
            outgoing_data.uv_light_average = get_max_value_for_hour(uv_light_this_hour);
            outgoing_data.uv_light_min = get_min_value_for_hour(uv_light_this_hour);
            outgoing_data.uv_light_max = get_mean_value_for_hour(uv_light_this_hour);
            outgoing_data.temperature_max = get_max_value_for_hour(temperature_this_hour);
            outgoing_data.temperature_min = get_min_value_for_hour(temperature_this_hour);
            outgoing_data.temperature_average = get_mean_value_for_hour(temperature_this_hour);
            outgoing_data.humidity_max = get_max_value_for_hour(humidity_this_hour);
            outgoing_data.humidity_min = get_min_value_for_hour(humidity_this_hour);
            outgoing_data.humidity_average = get_mean_value_for_hour(humidity_this_hour);
            outgoing_data.pressure_min = get_min_value_for_hour(pressure_this_hour);
            outgoing_data.pressure_max = get_max_value_for_hour(pressure_this_hour);
            outgoing_data.pressure_average = get_mean_value_for_hour(pressure_this_hour);
            outgoing_data.wind_speed_max = get_max_value_for_hour(wind_speeds_this_hour);
            outgoing_data.wind_speed_min = get_min_value_for_hour(wind_speeds_this_hour);
            outgoing_data.wind_speed_average = get_mean_value_for_hour(wind_speeds_this_hour);
            for (uint8_t i = 0; i < 60; i += 2) {
                uint8_t wind_direction_0 = wind_directions_this_hour[i];
                uint8_t wind_direction_1 = wind_directions_this_hour[i + 1];
                if (wind_direction_0 == WIND_DIRECTION_UNKNOWN) {
                    outgoing_data.bad_wind_readings++;
                    // if we have a bad reading, use the previous reading
                    wind_direction_0 = i ? wind_directions_this_hour[i - 1] : 0;
                }
                if (wind_direction_1 == WIND_DIRECTION_UNKNOWN) {
                    outgoing_data.bad_wind_readings++;
                    wind_direction_1 = wind_directions_this_hour[i];
                }
                outgoing_data.wind_directions[i / 2] = ((wind_direction_0 & 0xF) << 4) | (wind_direction_1 & 0xF);
            }
            outgoing_data.rainfall = rain_this_hour;
            outgoing_data.particles_1_0_min = get_min_value_for_hour(pm10_this_hour);
            outgoing_data.particles_1_0_max = get_max_value_for_hour(pm10_this_hour);
            outgoing_data.particles_1_0_average = get_mean_value_for_hour(pm10_this_hour);
            outgoing_data.particles_2_5_min = get_min_value_for_hour(pm25_this_hour);
            outgoing_data.particles_2_5_max = get_max_value_for_hour(pm25_this_hour);
            outgoing_data.particles_2_5_average = get_mean_value_for_hour(pm25_this_hour);
            outgoing_data.particles_10_0_min = get_min_value_for_hour(pm100_this_hour);
            outgoing_data.particles_10_0_max = get_max_value_for_hour(pm100_this_hour);
            outgoing_data.particles_10_0_average = get_mean_value_for_hour(pm100_this_hour);

            // add checksum
            add_checksum(&outgoing_data);

            // uplink data
            swarm_m138_transmit_data((uint8_t *)&outgoing_data, sizeof(weather_data_t));

            // reset all the hourly data
            rain_this_hour = 0;
            for (size_t i = 0; i < 60; i++) {
                wind_speeds_this_hour[i] = 0xFFFF;
                wind_directions_this_hour[i] = WIND_DIRECTION_UNKNOWN;
                visible_light_this_hour[i] = 0xFFFF;
                uv_light_this_hour[i] = 0xFFFF;
                temperature_this_hour[i] = 0xFFFF;
                humidity_this_hour[i] = 0xFFFF;
                pressure_this_hour[i] = 0xFFFF;
                pm10_this_hour[i] = 0xFFFF;
                pm25_this_hour[i] = 0xFFFF;
                pm100_this_hour[i] = 0xFFFF;
            }
            
            memset(wind_speeds_this_hour, 0, sizeof(wind_speeds_this_hour));
            memset(wind_directions_this_hour, 0, sizeof(wind_directions_this_hour));
            memset(visible_light_this_hour, 0, sizeof(visible_light_this_hour));
            memset(temperature_this_hour, 0, sizeof(temperature_this_hour));
        }

        adc_disable();
    }

    if (display_needs_update) {
        display_needs_update = false;
        update_display();
    }

    /// TODO: MAKE SURE SERCOM RUNS IN STANDBY AND WAKES US!!
    // return false;
    return true;
}

void eic_callback(uint8_t channel) {
    if (channel == 6) {
        rain_this_hour++;
        woke_for_rainfall = true;
    }
}

void rtc_callback(uint8_t source) {
    (void) source;
    woke_for_measurement = true;
    display_needs_update = true;
}

void swarm_m138_get_device_id_callback(int32_t device_id) {
    char buf[20];
    oso_lcd_fill(0);
    sprintf(buf, "%05x", (int16_t)device_id);
    oso_lcd_print(buf);
}

void swarm_m138_date_time_callback(Swarm_M138_DateTimeData_t datetime) {
    if (datetime.valid == 0) {
        return;
    }
    rtc_date_time rtcDatetime = {
        .unit = {
            .year = datetime.YYYY,
            .month = datetime.MM,
            .day = datetime.DD,
            .hour = datetime.hh,
            .minute = datetime.mm,
            .second = datetime.ss
        }
    };
    rtc_set_date_time(rtcDatetime);
    time_synced = true;
}

void swarm_m138_transmit_data_callback(bool success) {
    oso_lcd_fill(0);
    if (success) {
        oso_lcd_print("5ent.");
    } else {
        oso_lcd_print("Error");
    }
}

void swarm_m138_gpio_mode_callback(bool success) {
    (void)success;
    // TODO: do something with this
}

void swarm_m138_transmit_status_callback(int32_t messages_pending) {
    char buf[20];
    oso_lcd_fill(0);
    sprintf(buf, "tp:%3ld", messages_pending);
    oso_lcd_print(buf);
}

void swarm_m138_enter_sleep_mode_callback(bool success) {
    if (success) {
        oso_lcd_set_indicator(OSO_LCD_INDICATOR_MOON);
    } else {
        oso_lcd_clr_indicator(OSO_LCD_INDICATOR_MOON);
    }
}

