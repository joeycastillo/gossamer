/*
 * MIT License
 *
 * Copyright (c) 2023 Joey Castillo
 *
 * Based on the SparkFun M138 Satellite Modem Arduino Library
 * by Paul Clark for SparkFun Electronics
 * also MIT-licensed, copyright January 2022
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#pragma once

#include <stdint.h>
#include <stdbool.h>

/** Timeouts for the serial commands */
#define SWARM_M138_STANDARD_RESPONSE_TIMEOUT 1500 ///< Standard command timeout: allow 1.5 seconds for the modem to respond (See issue #22. 1000ms was too short.)
#define SWARM_M138_MESSAGE_DELETE_TIMEOUT 5000    ///< Allow extra time when deleting a message
#define SWARM_M138_MESSAGE_ID_TIMEOUT 5000        ///< Allow extra time when reading the message IDs
#define SWARM_M138_MESSAGE_READ_TIMEOUT 3000      ///< Allow extra time when reading a message
#define SWARM_M138_MESSAGE_TRANSMIT_TIMEOUT 3000  ///< Allow extra time when queueing a message for transmission

/** Modem Serial Baud Rate */
#define SWARM_M138_SERIAL_BAUD_RATE 115200 ///< The modem serial baud rate is 115200 and cannot be changed

/** Default I2C address used by the Qwiic Swarm Breakout. Can be changed. */
#define SFE_QWIIC_SWARM_DEFAULT_I2C_ADDRESS 0x52 ///< The default I2C address for the SparkFun Qwiic Swarm Breakout

/** Swarm packet length */
#define SWARM_M138_MAX_PACKET_LENGTH_BYTES 192 ///< The maximum packet length - defined as binary bytes
#define SWARM_M138_MAX_PACKET_LENGTH_HEX 384   ///< The maximum packet length - encoded as ASCII Hex

/** Minimum memory allocations for each message type */
#define SWARM_M138_MEM_ALLOC_CS 30  ///< E.g. DI=0x001abe,DN=M138 . Should be 20 but maybe the modem model could be longer than 4 bytes?
#define SWARM_M138_MEM_ALLOC_FV 37  ///< E.g. 2021-12-14T21:27:41,v1.5.0-rc4 . Should be 31 but maybe each v# could be three digits?
#define SWARM_M138_MEM_ALLOC_MS 128 ///< Allocate enough storage to hold the $M138 Modem Status debug or error text. GUESS! TO DO: confirm the true max length

/** An enum defining the command result */
typedef enum
{
  SWARM_M138_ERROR_ERROR = 0,        ///< Just a plain old communication error
  SWARM_M138_ERROR_SUCCESS,          ///< Hey, it worked!
  SWARM_M138_ERROR_MEM_ALLOC,        ///< Memory allocation error
  SWARM_M138_ERROR_TIMEOUT,          ///< Communication timeout
  SWARM_M138_ERROR_INVALID_FORMAT,   ///< Indicates the command response format was invalid
  SWARM_M138_ERROR_INVALID_CHECKSUM, ///< Indicates the command response checksum was invalid
  SWARM_M138_ERROR_INVALID_RATE,     ///< Indicates the message rate was invalid
  SWARM_M138_ERROR_INVALID_MODE,     ///< Indicates the GPIO1 pin mode was invalid
  SWARM_M138_ERROR_ERR               ///< Command input error (ERR) - the error is copied into commandError
} Swarm_M138_Error_e;
#define SWARM_M138_SUCCESS SWARM_M138_ERROR_SUCCESS ///< Hey, it worked!

/** Define the maximum message 'rate' (interval) */
#define SWARM_M138_MAX_MESSAGE_RATE (0x7FFFFFFF) ///< 2147483647 (2^31 - 1)

/** A struct to hold the date and time returned by $DT */
typedef struct
{
  uint16_t YYYY; // Year:    1970..2099
  uint8_t MM;    // Month:   01..12
  uint8_t DD;    // Day:     01..31
  uint8_t hh;    // Hour:    00..23
  uint8_t mm;    // Minutes: 00..59
  uint8_t ss;    // Seconds: 00..59
  bool valid;    // flag: I == Invalid == false; V == Valid == true
} Swarm_M138_DateTimeData_t;

/** A struct to hold the GPS jamming / spoofing indication */
typedef struct
{
  uint8_t spoof_state;   // 0 Spoofing unknown or deactivated
                         // 1 No spoofing indicated
                         // 2 Spoofing indicated
                         // 3 Multiple spoofing indications
  uint8_t jamming_level; // 0 = no CW jamming, 255 = strong CW jamming
} Swarm_M138_GPS_Jamming_Indication_t;

/** A struct to hold the geospatial data returned by $GN */
typedef struct
{
  float lat;    // Degrees: +/- 90
  float lon;    // Degrees: +/- 180
  float alt;    // m
  float course; // Degrees: 0..359 : 0=north, 90=east, 180=south, and 270=west
  float speed;  // km/h
} Swarm_M138_GeospatialData_t;

/** Enum for the GPIO1 pin modes */
typedef enum
{
  SWARM_M138_GPIO1_ANALOG = 0,
  SWARM_M138_GPIO1_ADC,
  SWARM_M138_GPIO1_INPUT,
  SWARM_M138_GPIO1_EXIT_SLEEP_LOW_HIGH,
  SWARM_M138_GPIO1_EXIT_SLEEP_HIGH_LOW,
  SWARM_M138_GPIO1_OUTPUT_LOW,
  SWARM_M138_GPIO1_OUTPUT_HIGH,
  SWARM_M138_GPIO1_MESSAGES_UNREAD_LOW,
  SWARM_M138_GPIO1_MESSAGES_UNREAD_HIGH,
  SWARM_M138_GPIO1_MESSAGES_UNSENT_LOW,
  SWARM_M138_GPIO1_MESSAGES_UNSENT_HIGH,
  SWARM_M138_GPIO1_MESSAGES_UNREAD_UNSENT_LOW,
  SWARM_M138_GPIO1_MESSAGES_UNREAD_UNSENT_HIGH,
  SWARM_M138_GPIO1_SLEEP_MODE_LOW,
  SWARM_M138_GPIO1_SLEEP_MODE_HIGH,
  SWARM_M138_GPIO1_INVALID
} Swarm_M138_GPIO1_Mode_e;

/** Enum for the GPS fix type */
typedef enum
{
  SWARM_M138_GPS_FIX_TYPE_NF = 0, // No Fix
  SWARM_M138_GPS_FIX_TYPE_DR,     // Dead reckoning only solution
  SWARM_M138_GPS_FIX_TYPE_G2,     // Standalone 2D solution
  SWARM_M138_GPS_FIX_TYPE_G3,     // Standalone 3D solution
  SWARM_M138_GPS_FIX_TYPE_D2,     // Differential 2D solution
  SWARM_M138_GPS_FIX_TYPE_D3,     // Differential 3D solution
  SWARM_M138_GPS_FIX_TYPE_RK,     // Combined GNSS + dead reckoning solution
  SWARM_M138_GPS_FIX_TYPE_TT,     // Time only solution
  SWARM_M138_GPS_FIX_TYPE_INVALID
} Swarm_M138_GPS_Fix_Type_e;

/** A struct to hold the GPS fix quality */
typedef struct
{
  uint16_t hdop;     // Horizontal dilution of precision (0..9999) (integer = actual hdop * 100)
  uint16_t vdop;     //  Vertical dilution of precision (0..9999) (integer = actual vdop * 100)
  uint8_t gnss_sats; // Number of GNSS satellites used in solution (integer)
  uint8_t unused;    // Always reads as 0, unused
  Swarm_M138_GPS_Fix_Type_e fix_type;
} Swarm_M138_GPS_Fix_Quality_t;

/** A struct to hold the power staus info */
typedef struct
{
  float cpu_volts; // Voltage measured at input to the CPU
  float unused1;
  float unused2;
  float unused3;
  float temp;      // CPU Temperature in degrees C to one decimal point
} Swarm_M138_Power_Status_t;

/** A struct to hold the receive test results */
typedef struct
{
  bool background;                // If true: the struct holds the rssi_background only. If false: the struct holds everything except rssi_background.
  int16_t rssi_background;        // Received background noise signal strength in dBm for open channel (integer).
  int16_t rssi_sat;               // Received signal strength in dBm for packet from satellite (integer)
  int16_t snr;                    // Signal to noise ratio in dB for packet (integer)
  int16_t fdev;                   // Frequency deviation in Hz for packet (integer)
  Swarm_M138_DateTimeData_t time; // Date and time (UTC) of received packet (valid flag is unused - always set to true)
  uint32_t sat_id;                // Device ID of satellite heard (hexadecimal)
} Swarm_M138_Receive_Test_t;

/** An enum for the sleep mode wake cause */
typedef enum
{
  SWARM_M138_WAKE_CAUSE_GPIO = 0, // GPIO input changed from inactive to active state
  SWARM_M138_WAKE_CAUSE_SERIAL,   // Activity was detected on the RX pin of the Modem's UART
  SWARM_M138_WAKE_CAUSE_TIME,     // The S or U parameter time has been reached
  SWARM_M138_WAKE_CAUSE_INVALID
} Swarm_M138_Wake_Cause_e;

/** An enum for the modem status */
typedef enum
{
  SWARM_M138_MODEM_STATUS_BOOT_ABORT = 0, // A firmware crash occurred that caused a restart
  SWARM_M138_MODEM_STATUS_BOOT_DEVICEID,  // Displays the device ID of the Modem
  SWARM_M138_MODEM_STATUS_BOOT_POWERON,   // Power has been applied
  SWARM_M138_MODEM_STATUS_BOOT_RUNNING,   // Boot has completed and ready to accept commands
  SWARM_M138_MODEM_STATUS_BOOT_UPDATED,   // A firmware update was performed
  SWARM_M138_MODEM_STATUS_BOOT_VERSION,   // Current firmware version information
  SWARM_M138_MODEM_STATUS_BOOT_RESTART,   // Modem is restarting after $RS Restart Device
  SWARM_M138_MODEM_STATUS_BOOT_SHUTDOWN,  // Modem has shutdown after $PO Power Off. Disconnect and reconnect power to restart
  SWARM_M138_MODEM_STATUS_DATETIME,       // The first time GPS has acquired a valid date/time reference
  SWARM_M138_MODEM_STATUS_POSITION,       // The first time GPS has acquired a valid position 3D fix
  SWARM_M138_MODEM_STATUS_DEBUG,          // Debug message (data - debug text)
  SWARM_M138_MODEM_STATUS_ERROR,          // Error message (data - error text)
  SWARM_M138_MODEM_STATUS_UNKNOWN,        // A new, undocumented message
  SWARM_M138_MODEM_STATUS_INVALID
} Swarm_M138_Modem_Status_e;

void swarm_m138_begin(void);

void swarm_m138_get_device_id(void);
void swarm_m138_get_date_time(void);
void swarm_m138_transmit_data(uint8_t *message, uint16_t length);
void swarm_m138_set_gpio_1_mode(Swarm_M138_GPIO1_Mode_e mode);
void swarm_m138_get_transmit_status(void);
void swarm_m138_enter_sleep_mode_until(Swarm_M138_DateTimeData_t datetime);

void swarm_m138_handle_received_data(void);

#define SWARM_M138_MAX_CMD_ERROR_LEN 32 ///< Allocate 32 bytes to store the most recent command error

// callbacks
extern void swarm_m138_get_device_id_callback(int32_t device_id);
extern void swarm_m138_date_time_callback(Swarm_M138_DateTimeData_t datetime);
extern void swarm_m138_transmit_data_callback(bool success);
extern void swarm_m138_gpio_mode_callback(bool success);
extern void swarm_m138_transmit_status_callback(int32_t messages_pending);
extern void swarm_m138_enter_sleep_mode_callback(bool success);
