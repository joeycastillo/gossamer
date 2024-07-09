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

#include "sam.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "m138.h"
#include "pins.h"
#include "../peripherals/uart.h"

/** Suported Commands */
const char SWARM_M138_COMMAND_CONFIGURATION[] = "$CS";   ///< Configuration Settings
const char SWARM_M138_COMMAND_DATE_TIME_STAT[] = "$DT";  ///< Date/Time Status
const char SWARM_M138_COMMAND_FIRMWARE_VER[] = "$FV";    ///< Retrieve Firmware Version
const char SWARM_M138_COMMAND_GPS_JAMMING[] = "$GJ";     ///< GPS Jamming/Spoofing Indication
const char SWARM_M138_COMMAND_GEOSPATIAL_INFO[] = "$GN"; ///< Geospatial Information
const char SWARM_M138_COMMAND_GPIO1_CONTROL[] = "$GP";   ///< GPIO1 Control
const char SWARM_M138_COMMAND_GPS_FIX_QUAL[] = "$GS";    ///< GPS Fix Quality
const char SWARM_M138_COMMAND_MSG_RX_MGMT[] = "$MM";     ///< Messages Received Management
const char SWARM_M138_COMMAND_MSG_TX_MGMT[] = "$MT";     ///< Messages to Transmit Management
const char SWARM_M138_COMMAND_POWER_OFF[] = "$PO";       ///< Power Off
const char SWARM_M138_COMMAND_POWER_STAT[] = "$PW";      ///< Power Status
const char SWARM_M138_COMMAND_RX_DATA_MSG[] = "$RD";     ///< Receive Data Message
const char SWARM_M138_COMMAND_RESTART[] = "$RS";         ///< Restart Device
const char SWARM_M138_COMMAND_RX_TEST[] = "$RT";         ///< Receive Test
const char SWARM_M138_COMMAND_SLEEP[] = "$SL";           ///< Sleep Mode
const char SWARM_M138_COMMAND_MODEM_STAT[] = "$M138";    ///< Modem Status
const char SWARM_M138_COMMAND_TX_DATA[] = "$TD";         ///< Transmit Data

bool _printDebug = false;
char commandError[SWARM_M138_MAX_CMD_ERROR_LEN + 1] = {0};

// Add the two NMEA checksum bytes and line feed to a command
static void addChecksumLF(char *command);
static void addChecksumLF(char *command) {
    char *dollar = strchr(command, '$'); // Find the $

    if (dollar == NULL) // Return now if the $ was not found
        return;

    char *asterix = strchr(dollar, '*'); // Find the *

    if (asterix == NULL) // Return now if the * was not found
        return;

    // Check for a second asterix ($MM C=**)
    if (*(asterix + 1) == '*')
        asterix++;

    char checksum = 0;

    dollar++; // Point to the char after the $

    while (dollar < asterix) // Calculate the checksum
    {
        checksum ^= *dollar;
        dollar++;
    }

    // Add the checksum bytes to the command
    *(asterix + 1) = (checksum >> 4) + '0';
    if (*(asterix + 1) >= ':') // Hex a-f
        *(asterix + 1) = *(asterix + 1) + 'a' - ':';
    *(asterix + 2) = (checksum & 0x0F) + '0';
    if (*(asterix + 2) >= ':') // Hex a-f
        *(asterix + 2) = *(asterix + 2) + 'a' - ':';

    // Add the line feed
    *(asterix + 3) = '\n';

    // Add a \0 - just in case
    *(asterix + 4) = 0;
}

// Check if the response / message format and checksum is valid
static Swarm_M138_Error_e checkChecksum(char *startPosition);
static Swarm_M138_Error_e checkChecksum(char *startPosition) {
    char *dollar = strchr(startPosition, '$'); // Find the $

    if (dollar == NULL) // Return now if the $ was not found
    {
        if (_printDebug == true)
        {
            printf(("checkChecksum: $ not found!\n"));
        }
        return (SWARM_M138_ERROR_INVALID_FORMAT);
    }

    char *asterix = strchr(dollar, '*'); // Find the *

    if (asterix == NULL) // Return now if the * was not found
    {
        if (_printDebug == true)
        {
            printf(("checkChecksum: * not found!\n"));
        }
        return (SWARM_M138_ERROR_INVALID_FORMAT);
    }

    char checksum = 0;

    dollar++; // Point to the char after the $

    while (dollar < asterix) // Calculate the checksum
    {
        checksum ^= *dollar;
        dollar++;
    }

    char expectedChecksum;

    char checksumChar = *(asterix + 1); // Get the first checksum character

    if ((checksumChar >= '0') && (checksumChar <= '9')) // Convert to binary
        expectedChecksum = (checksumChar - '0') << 4;
    else if ((checksumChar >= 'a') && (checksumChar <= 'f'))
        expectedChecksum = (checksumChar + 10 - 'a') << 4;
    else if ((checksumChar >= 'A') && (checksumChar <= 'F'))
        expectedChecksum = (checksumChar + 10 - 'A') << 4;
    else
    {
        if (_printDebug == true)
        {
            printf(("checkChecksum: invalid checksum char 1\n"));
        }
        return (SWARM_M138_ERROR_INVALID_FORMAT);
    }

    checksumChar = *(asterix + 2); // Get the second checksum character

    if ((checksumChar >= '0') && (checksumChar <= '9')) // Convert to binary
        expectedChecksum |= (checksumChar - '0');
    else if ((checksumChar >= 'a') && (checksumChar <= 'f'))
        expectedChecksum |= (checksumChar + 10 - 'a');
    else if ((checksumChar >= 'A') && (checksumChar <= 'F'))
        expectedChecksum |= (checksumChar + 10 - 'A');
    else
    {
        if (_printDebug == true)
        {
            printf(("checkChecksum: invalid checksum char 2\n"));
        }
        return (SWARM_M138_ERROR_INVALID_FORMAT);
    }

    if (checksum != expectedChecksum)
    {
        if (_printDebug == true)
        {
            printf(("checkChecksum: invalid checksum\n"));
        }
        return (SWARM_M138_ERROR_INVALID_CHECKSUM);
    }

    return (SWARM_M138_ERROR_SUCCESS);
}

void swarm_m138_begin(void) {
    uint32_t dev_ID = 0;

    HAL_GPIO_TX_out();
    HAL_GPIO_TX_clr();
    HAL_GPIO_TX_pmuxen(HAL_GPIO_PMUX_SERCOM_ALT);
    HAL_GPIO_RX_in();
    HAL_GPIO_RX_pullup();
    HAL_GPIO_RX_pmuxen(HAL_GPIO_PMUX_SERCOM);
    uart_init(115200);
    uart_set_run_in_standby(true);
    uart_enable();
}

void swarm_m138_get_device_id(void) {
    char command[16] = {0};
    sprintf(command, "%s*", SWARM_M138_COMMAND_CONFIGURATION);  // Copy the command, add the asterix
    addChecksumLF(command);                                     // Add the checksum bytes and line feed
    uart_write(command, strlen(command));                       // Send the command
}

void swarm_m138_get_date_time(void) {
    char command[16] = {0};
    sprintf(command, "%s @*", SWARM_M138_COMMAND_DATE_TIME_STAT);   // Copy the command, add the asterix
    addChecksumLF(command);                                         // Add the checksum bytes and line feed
    uart_write(command, strlen(command));                           // Send the command
}

void swarm_m138_transmit_data(uint8_t *message, uint16_t length) {
    char command[512] = {0};
    char scratchpad[16] = {0};

    sprintf(command, "%s ", SWARM_M138_COMMAND_TX_DATA);// Copy the command. Append the space
    strcat(command, "HD=172800,");                      // Hold message for 48 hours
    for (size_t i = 0; i < length; i++) {
        char c1 = (message[i] >> 4) + '0';              // Convert the MS nibble to ASCII
        if (c1 >= ':') c1 = c1 + 'A' - ':';
        char c2 = (message[i] & 0x0F) + '0';            // Convert the LS nibble to ASCII
        if (c2 >= ':') c2 = c2 + 'A' - ':';
        sprintf(scratchpad, "%c%c", c1, c2);
        strcat(command, scratchpad);                    // Append each data byte as an ASCII hex char pair
    }
    strcat(command, "*");                               // Append the asterix
    addChecksumLF(command);                             // Add the checksum bytes and line feed
    uart_write(command, strlen(command));               // Send the command
}

void swarm_m138_set_gpio_1_mode(Swarm_M138_GPIO1_Mode_e mode) {
    char command[16] = {0};
    // Check mode is within bounds
    if ((int)mode >= SWARM_M138_GPIO1_INVALID) return;
    sprintf(command, "%s %u*", SWARM_M138_COMMAND_GPIO1_CONTROL, (int)mode);    // Copy the command, add the asterix
    addChecksumLF(command);                                                     // Add the checksum bytes and line feed
    uart_write((uint8_t *)command, strlen(command));                            // Send the command
}

void swarm_m138_get_transmit_status(void) {
    char command[16] = {0};
    sprintf(command, "%s C=U*", SWARM_M138_COMMAND_MSG_TX_MGMT);    // Copy the command, add the asterix
    addChecksumLF(command);                                         // Add the checksum bytes and line feed
    uart_write((uint8_t *)command, strlen(command));                // Send the command
}

void swarm_m138_enter_sleep_mode_until(Swarm_M138_DateTimeData_t datetime) {
    char command[32] = {0};
    sprintf(command, "%s %u,%u,%u,%u,%u,%u*", SWARM_M138_COMMAND_SLEEP, datetime.YYYY, datetime.MM, datetime.DD, datetime.hh, datetime.mm, datetime.ss); // Copy the command, add the asterix
    addChecksumLF(command); // Add the checksum bytes and line feed
    uart_write((uint8_t *)command, strlen(command)); // Send the command
}

void swarm_m138_handle_received_data(void) {
    static uint8_t response[512];
    char *responseStart;
    char *responseEnd = NULL;
    static size_t pos = 0;
    bool needs_processing = false;

    if (uart_read_byte(response + pos)) {
        if (response[pos] == '\n') {
            response[pos] = '\0';
            needs_processing = true;
        }
        pos++;
    }
    if (needs_processing) {
        needs_processing = false;
        pos = 0;
        // treat the first four digits of the buffer as a magic number
        uint32_t magic = *(uint32_t *)response;
        // compare it to the magic numbers for the supported commands, like SWARM_M138_COMMAND_CONFIGURATION
        switch (magic) {
            case 0x20534324: // $CS - Configuration Settings
            {
                uint32_t dev_ID = 0;
                responseStart = strstr((const char *)response, "$CS DI=0x");
                if (responseStart != NULL)
                    responseEnd = strchr(responseStart, ','); // Stop at the comma
                if ((responseStart == NULL) || (responseEnd == NULL)) return;

                // Extract the ID
                responseStart += 9; // Point at the first digit
                while (responseStart < responseEnd)
                {
                    dev_ID <<= 4;            // Shuffle the existing value along by 4 bits
                    char c = *responseStart; // Get the digit
                    if ((c >= '0') && (c <= '9'))
                        dev_ID |= c - '0';
                    else if ((c >= 'a') && (c <= 'f'))
                        dev_ID |= c + 10 - 'a';
                    else if ((c >= 'A') && (c <= 'F'))
                        dev_ID |= c + 10 - 'A';
                    responseStart++;
                }

                swarm_m138_get_device_id_callback(dev_ID);
            }
                break;
            case 0x20544424: // $DT - Date/Time Status
            {
                Swarm_M138_DateTimeData_t datetime;
                responseStart = strstr((const char *)response, "$DT ");
                if (responseStart != NULL) responseEnd = strchr(responseStart, '*'); // Stop at the asterix
                if ((responseStart == NULL) || (responseEnd == NULL) || (responseEnd < (responseStart + 20))) {
                    // Check we have enough data
                    return;
                }

                // Extract the Date, Time and flag
                int year, month, day, hour, minute, second;
                char valid;

                int ret = sscanf(responseStart, "$DT %4d%2d%2d%2d%2d%2d,%c*", &year, &month, &day, &hour, &minute, &second, &valid);

                if (ret < 7)
                {
                return;
                }

                datetime.YYYY = (uint16_t)year;
                datetime.MM = (uint8_t)month;
                datetime.DD = (uint8_t)day;
                datetime.hh = (uint8_t)hour;
                datetime.mm = (uint8_t)minute;
                datetime.ss = (uint8_t)second;
                datetime.valid = valid == 'V' ? 1 : 0;

                swarm_m138_date_time_callback(datetime);
            }
                break;
            case 0x20564624: // $FV - Retrieve Firmware Version
                // Not implemented
                break;
            case 0x204A4724: // $GJ - GPS Jamming/Spoofing Indication
                // Not implemented
                break;
            case 0x204E4724: // $GN - Geospatial Information
                // Not implemented
                break;
            case 0x20504724: // $GP - GPIO1 Control
                /// TODO: implement this
                break;
            case 0x20534724: // $GS - GPS Fix Quality
                // Not implemented
                break;
            case 0x204D4D24: // $MM - Messages Received Management
                // Not implemented
                break;
            case 0x20544D24: // $MT - Messages to Transmit Management
            {
                responseStart = strstr((const char *)response, "$MT ");
                if (responseStart != NULL) responseEnd = strchr(responseStart, '*'); // Stop at the asterix
                if ((responseStart == NULL) || (responseEnd == NULL)) {
                    swarm_m138_transmit_status_callback(-1);
                }

                // Extract the count
                char c;
                uint16_t count = 0;
                responseStart += 4; // Point at the first digit of the count

                c = *responseStart; // Get the first digit of the count
                while ((c != '*') && (c != ',')) {
                    // Keep going until we hit the asterix
                    if ((c >= '0') && (c <= '9')) {
                        // Extract the count one digit at a time
                        count = count * 10;
                        count += (uint16_t)(c - '0');
                    }
                    responseStart++;
                    c = *responseStart; // Get the next digit of the count
                }

                if (c == ',') {
                    // If we hit a comma, this must be a different $MT message
                    swarm_m138_transmit_status_callback(-1);
                }
                else {
                    // call callback
                    swarm_m138_transmit_status_callback(count);
                }
            }
                break;
            case 0x204F5024: // $PO - Power Off
                break;
            case 0x20575024: // $PW - Power Status
                break;
            case 0x20445224: // $RD - Receive Data Message
                break;
            case 0x20535224: // $RS - Restart Device
                break;
            case 0x20545224: // $RT - Receive Test
                break;
            case 0x204C5324: // $SL - Sleep Mode
                break;
            case 0x33314D24: // $M13 - Modem Status
                break;
            case 0x20445424: // $TD - Transmit Data
                break;
            default:
                break;
        }
    }
}

void irq_handler_sercom5(void);
void irq_handler_sercom5(void) {
    uart_irq_handler(5);
}
