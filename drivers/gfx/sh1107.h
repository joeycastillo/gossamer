/** 
 * @file sh1107.h
 * @brief SH1107 OLED display driver.
 */
// Ported from Adafruit_GFX
// See LICENSE for license text

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "gfx.h"

#pragma once

// this file is a driver for the SH1107 OLED that works in conjunction with the gfx driver.

/**
 * @brief Initializes the SH1107 display.
 * @details You must configure both the GFX driver (@see `gfx_init()`) and the I2C peripheral
 * @see `i2c_init()` before calling this function.
 */
void sh1107_begin(void);

/**
 * @brief Pushes the current contents of the GFX buffer to the display.
 * @details Once you have configured the GFX driver, you will draw to that buffer using the
 *       gfx draw functions. This function merely pushes the contents of that buffer to the
 *       display, and you MUST call `sh1107_update` to see any changes you have made.
 */
void sh1107_update(void);

#define SH110X_MEMORYMODE 0x20          ///< See datasheet
#define SH110X_COLUMNADDR 0x21          ///< See datasheet
#define SH110X_PAGEADDR 0x22            ///< See datasheet
#define SH110X_SETCONTRAST 0x81         ///< See datasheet
#define SH110X_CHARGEPUMP 0x8D          ///< See datasheet
#define SH110X_SEGREMAP 0xA0            ///< See datasheet
#define SH110X_DISPLAYALLON_RESUME 0xA4 ///< See datasheet
#define SH110X_DISPLAYALLON 0xA5        ///< Not currently used
#define SH110X_NORMALDISPLAY 0xA6       ///< See datasheet
#define SH110X_INVERTDISPLAY 0xA7       ///< See datasheet
#define SH110X_SETMULTIPLEX 0xA8        ///< See datasheet
#define SH110X_DCDC 0xAD                ///< See datasheet
#define SH110X_DISPLAYOFF 0xAE          ///< See datasheet
#define SH110X_DISPLAYON 0xAF           ///< See datasheet
#define SH110X_SETPAGEADDR                                                     \
  0xB0 ///< Specify page address to load display RAM data to page address
       ///< register
#define SH110X_COMSCANINC 0xC0         ///< Not currently used
#define SH110X_COMSCANDEC 0xC8         ///< See datasheet
#define SH110X_SETDISPLAYOFFSET 0xD3   ///< See datasheet
#define SH110X_SETDISPLAYCLOCKDIV 0xD5 ///< See datasheet
#define SH110X_SETPRECHARGE 0xD9       ///< See datasheet
#define SH110X_SETCOMPINS 0xDA         ///< See datasheet
#define SH110X_SETVCOMDETECT 0xDB      ///< See datasheet
#define SH110X_SETDISPSTARTLINE                                                \
  0xDC ///< Specify Column address to determine the initial display line or
       ///< COM0.

#define SH110X_SETLOWCOLUMN 0x00  ///< See datasheet
#define SH110X_SETHIGHCOLUMN 0x10 ///< See datasheet
#define SH110X_SETSTARTLINE 0x40  ///< See datasheet
