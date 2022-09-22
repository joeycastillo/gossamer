/*
 * MIT License
 *
 * Copyright (c) 2022 Joey Castillo
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

#include <samd21.h>
#include <string.h>
#include <ctype.h>
#include "oso_lcd.h"
#include "../peripherals/i2c.h"

#define BU9796_ADDRESS 0x3e

static uint8_t _buffer[7] = {0};

static void _write_cmd(uint8_t cmd) {
    i2c_write(BU9796_ADDRESS, &cmd, 1);
}

static void _set_buffer(uint8_t pos, uint8_t value) {
    _buffer[pos + 1] = value;
}

static uint8_t _get_buffer(uint8_t pos) {
    return _buffer[pos + 1];
}

void oso_lcd_begin() {
    _write_cmd(0b00111100); // Configure for lowest power consumption
    _write_cmd(0b01001000); // display ON, 1/3 bias
}

void oso_lcd_show(void) {
    i2c_write(BU9796_ADDRESS, _buffer, 7);
}

void oso_lcd_show_partial(uint8_t length, uint8_t pos) {
    if (pos) {
        uint8_t temp[6];
        temp[0] = pos * 2;
        memcpy(temp + 1, _buffer + pos + 1, length);
        i2c_write(BU9796_ADDRESS, temp, length + 1);
    } else {
        i2c_write(BU9796_ADDRESS, _buffer, length + 1);
    }
}

void oso_lcd_fill(bool on) {
    for (int i = 0; i < 6; i++) {
        _buffer[i + 1] = on ? 0xFF : 0x00;
    }
    oso_lcd_show();
}

static void _oso_lcd_update_byte_0() {
    oso_lcd_show_partial(1, 0);
}

static const uint8_t CHARS[] = {
    0b00000000, // [space]
    0b00101100, // !
    0b01001000, // "
    0b11001010, // # (°)
    0b01010101, // $ (Like an L with an extra segment, use $J to make a W)
    0b01000100, // %
    0b11011000, // & (Like an F without a cross stroke, use &7 to make an M)
    0b00001000, // '
    0b11010001, // (
    0b10001101, // )
    0b00000000, // * (Currently unused)
    0b01010010, // +
    0b00000100, // ,
    0b00000010, // -
    0b00100000, // .
    0b00001100, // /
    0b11011101, // 0
    0b00001100, // 1
    0b10011011, // 2
    0b10001111, // 3
    0b01001110, // 4
    0b11000111, // 5
    0b11010111, // 6
    0b11001100, // 7
    0b11011111, // 8
    0b11001111, // 9
    0b00000000, // : (RESERVED: special character that turns on the colon)
    0b01000000, // ; (Like the apostrophe, but on the other side, use 'foo; to say ‘foo’)
    0b00010001, // <
    0b00000011, // =
    0b00000101, // >
    0b10011010, // ?
    0b11111111, // @
    0b11011110, // A
    0b01010111, // B
    0b00010011, // C
    0b00011111, // D
    0b11010011, // E
    0b11010010, // F
    0b11001111, // G
    0b01010110, // H
    0b00010000, // I
    0b00011101, // J
    0b11010110, // K
    0b01010001, // L
    0b11011100, // M
    0b00010110, // N
    0b00010111, // O
    0b11011010, // P
    0b11001110, // Q
    0b00010010, // R
    0b11000101, // S
    0b01010011, // T
    0b00010101, // U
    0b01011101, // V
    0b01011111, // W
    0b01011110, // X
    0b01001111, // Y
    0b10011001, // Z
};

void oso_lcd_print(char *s) {
    char value[10];
    for(size_t i = 0; i < strlen(s); i++) value[i] = toupper(s[i]);

    bool neg = value[0] == '-';
    int pos = neg ? 1 : 0;
    bool d = false;
    int i = 1;
    _set_buffer(0, _get_buffer(0) & ~0b00010000);

    while(i < 6) {
        int b = 0;
        char c = value[pos];
        pos++;
        if (c == '.') {
            d = true;
            continue;
        } else if (c == ':') {
            if (i == 3) _set_buffer(0, _get_buffer(0) | 0b00010000);
            continue;
        }
        b = CHARS[c - 32];
        _set_buffer(i, b | (((neg && i == 1) || d ) ? 0b00100000 : 0));
        i += 1;
        d = false;
    }

    oso_lcd_show();
}

void oso_lcd_set_indicator(uint8_t indicator) {
    _set_buffer(0, _get_buffer(0) | indicator);
    _oso_lcd_update_byte_0();
}

void oso_lcd_clear_indicator(uint8_t indicator) {
    _set_buffer(0, _get_buffer(0) & ~indicator);
    _oso_lcd_update_byte_0();
}

void oso_lcd_toggle_colon(void) {
    uint8_t indicators = _get_buffer(0);
    _set_buffer(0, (indicators & 0b00010000) ? (indicators & ~0b00010000) : (indicators | 0b00010000));
    _oso_lcd_update_byte_0();
}
