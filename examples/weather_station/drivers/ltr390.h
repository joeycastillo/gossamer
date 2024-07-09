/*
 * MIT License
 *
 * Copyright (c) 2021 Igor Levkov
 * Copyright (c) 2023 Joey Castillo
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


#define UV_SENSITIVITY  1400

#define WFAC            1
/*
*  For device under tinted window with coated-ink of flat transmission rate at 400-600nm wavelength,  
*  window factor  is to  compensate light  loss due to the  lower  transmission rate from the coated-ink. 
*     a. WFAC = 1 for NO window / clear window glass. 
*     b. WFAC >1 device under tinted window glass. Calibrate under white LED. 
*/

#define LTR390_ADDRESS 0x53         ///< I2C address
#define LTR390_MAIN_CTRL 0x00       ///< Main control register
#define LTR390_MEAS_RATE 0x04       ///< Resolution and data rate
#define LTR390_GAIN 0x05            ///< ALS and UVS gain range
#define LTR390_PART_ID 0x06         ///< Part id/revision register
#define LTR390_MAIN_STATUS 0x07     ///< Main status register
#define LTR390_ALSDATA_LSB 0x0D     ///< ALS data lowest byte
#define LTR390_ALSDATA_MSB 0x0E     ///< ALS data middle byte
#define LTR390_ALSDATA_HSB 0x0F     ///< ALS data highest byte
#define LTR390_UVSDATA_LSB 0x10     ///< UVS data lowest byte
#define LTR390_UVSDATA_MSB 0x11     ///< UVS data middle byte
#define LTR390_UVSDATA_HSB 0x12     ///< UVS data highest byte
#define LTR390_INT_CFG 0x19         ///< Interrupt configuration
#define LTR390_INT_PST 0x1A         ///< Interrupt persistance config
#define LTR390_THRESH_UP 0x21       ///< Upper threshold, low byte
#define LTR390_THRESH_LOW 0x24      ///< Lower threshold, low byte

/*!    @brief  Whether we are measuring ambient or UV light  */
typedef enum {
  LTR390_MODE_ALS,
  LTR390_MODE_UVS,
} ltr390_mode_t;

/*!    @brief  Sensor gain for UV or ALS  */
typedef enum {
  LTR390_GAIN_1 = 0,
  LTR390_GAIN_3,
  LTR390_GAIN_6,
  LTR390_GAIN_9,
  LTR390_GAIN_18,
} ltr390_gain_t;

/*!    @brief Measurement resolution (higher res means slower reads!)  */
typedef enum {
  LTR390_RESOLUTION_20BIT,
  LTR390_RESOLUTION_19BIT,
  LTR390_RESOLUTION_18BIT,
  LTR390_RESOLUTION_17BIT,
  LTR390_RESOLUTION_16BIT,
  LTR390_RESOLUTION_13BIT,
} ltr390_resolution_t;


bool ltr390_init();

bool ltr390_reset(void);

void ltr390_enable(bool en);
bool ltr390_enabled(void);

void ltr390_setMode(ltr390_mode_t mode);
ltr390_mode_t ltr390_getMode(void);

void ltr390_setGain(ltr390_gain_t gain);
ltr390_gain_t ltr390_getGain(void);

void ltr390_setResolution(ltr390_resolution_t res);
ltr390_resolution_t ltr390_getResolution(void);

bool ltr390_newDataAvailable(void);

uint32_t ltr390_readUVS(void);
uint32_t ltr390_readALS(void);
