// Ported from Adafruit_GFX
// See LICENSE for license text

#include <math.h>
#include <string.h>
#include "i2c.h"
#include "sh1107.h"
#include "delay.h"

#define SH1107_ADDRESS (0x3C)
#define DC_BIT_COMMAND (0x00)
#define DC_BIT_DATA (0x40)

static int16_t window_x1;
static int16_t window_y1;
static int16_t window_x2;
static int16_t window_y2;

#ifndef min
#define min(a,b) ((a) <= (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) >= (b) ? (a) : (b))
#endif

static void _write_commands(uint8_t *cmd, uint8_t len) {
    i2c_write(SH1107_ADDRESS, cmd, len);
}

void sh1107_begin() {
    window_x1 = 0;
    window_y1 = 0;
    window_x2 = gfx_raw_width - 1;
    window_y2 = gfx_raw_height - 1;

    static const uint8_t set_contrast[] = {
        DC_BIT_COMMAND,
        SH110X_SETCONTRAST, 0x4F
    };
    static const uint8_t init[] = {
        DC_BIT_COMMAND,
        SH110X_DISPLAYOFF,               // 0xAE
        SH110X_SETDISPLAYCLOCKDIV, 0x51, // 0xd5, 0x51,
        SH110X_MEMORYMODE,               // 0x20
        SH110X_SETCONTRAST, 0x4F,        // 0x81, 0x4F
        SH110X_DCDC, 0x8A,               // 0xAD, 0x8A
        SH110X_SEGREMAP,                 // 0xA0
        SH110X_COMSCANINC,               // 0xC0
        SH110X_SETDISPSTARTLINE, 0x0,    // 0xDC 0x00
        SH110X_SETDISPLAYOFFSET, 0x60,   // 0xd3, 0x60,
        SH110X_SETPRECHARGE, 0x22,       // 0xd9, 0x22,
        SH110X_SETVCOMDETECT, 0x35,      // 0xdb, 0x35,
        SH110X_SETMULTIPLEX, 0x3F,       // 0xa8, 0x3f,
        SH110X_DISPLAYALLON_RESUME, // 0xa4
        SH110X_NORMALDISPLAY,       // 0xa6
    };
    static const uint8_t display_on[] = {
        DC_BIT_COMMAND,
        SH110X_DISPLAYON
    };

    _write_commands((uint8_t *)set_contrast, sizeof(set_contrast));
    _write_commands((uint8_t *)init, sizeof(init));
    delay_ms(100);
    _write_commands((uint8_t *)display_on, sizeof(display_on));
}

void sh1107_update(void) {
    uint8_t *ptr = gfx_buffer;
    uint8_t pages = ((gfx_raw_height + 7) / 8);

    uint8_t bytes_per_page = gfx_raw_width;
    uint8_t first_page = window_y1 / 8;
    uint8_t page_start = min(bytes_per_page, (uint8_t)window_x1);
    uint8_t page_end = (uint8_t)max((int)0, (int)window_x2);

    uint8_t data_buf[257];
    data_buf[0] = DC_BIT_DATA;

    for (uint8_t p = first_page; p < pages; p++) {
        uint16_t bytes_remaining = bytes_per_page;
        ptr = gfx_buffer + (uint16_t)p * (uint16_t)bytes_per_page;
        // fast forward to dirty rectangle beginning
        ptr += page_start;
        bytes_remaining -= page_start;
        // cut off end of dirty rectangle
        bytes_remaining -= (gfx_raw_width - 1) - page_end;

        uint8_t cmds[] = {
            DC_BIT_COMMAND,
            (uint8_t)(SH110X_SETPAGEADDR + p),
            (uint8_t)(0x10 + ((page_start) >> 4)),
            (uint8_t)((page_start) & 0xF)};

        _write_commands(cmds, sizeof(cmds));

        while (bytes_remaining) {
            uint8_t to_write = min(bytes_remaining, sizeof(data_buf) - 1);
            memcpy(data_buf + 1, ptr, to_write);
            i2c_write(SH1107_ADDRESS, data_buf, to_write + 1);
            // ptr += to_write;
            bytes_remaining -= to_write;
        }
    }
}
