/*
 * MIT License
 *
 * Copyright (c) 2020 Joey Castillo
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

#define OSO_LCD_INDICATOR_AM (0b10000000)
#define OSO_LCD_INDICATOR_PM (0b01000000)
#define OSO_LCD_INDICATOR_BATTERY (0b00100000)
#define OSO_LCD_INDICATOR_BELL (0b00001000)
#define OSO_LCD_INDICATOR_WIFI (0b00000100)
#define OSO_LCD_INDICATOR_DATA (0b00000010)
#define OSO_LCD_INDICATOR_MOON (0b00000001)
#define OSO_LCD_INDICATOR_ALL (0b11101111)

void oso_lcd_begin(void);
void oso_lcd_show(void);
void oso_lcd_show_partial(uint8_t length, uint8_t pos);
void oso_lcd_fill(bool on);
void oso_lcd_print(char *s);
void oso_lcd_set_indicator(uint8_t indicator);
void oso_lcd_clear_indicator(uint8_t indicator);
void oso_lcd_toggle_colon(void);
