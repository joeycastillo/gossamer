// Ported from Adafruit_GFX
// See LICENSE for license text

#include "gfx.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

int16_t gfx_raw_width = 0;
int16_t gfx_raw_height = 0;
int16_t gfx_width = 0;
int16_t gfx_height = 0;
int8_t gfx_depth = 0;
int8_t gfx_rotation = 0;
uint8_t *gfx_buffer = NULL;

void ((*transaction_start_function)(void)) = NULL;
void ((*transaction_end_function)(void)) = NULL;

void gfx_init(int16_t w, int16_t h, int8_t gfx_depth) {
    gfx_raw_width = w;
    gfx_raw_height = h;
    gfx_width = gfx_raw_width;
    gfx_height = gfx_raw_height;
    gfx_depth = gfx_depth;
    // one bit only for now
    if (gfx_depth == 1) {
        uint32_t bytes = ((w + 7) / 8) * h;
        if ((gfx_buffer = (uint8_t *)malloc(bytes))) {
            memset(gfx_buffer, 0x00, bytes);
        }
    }
}

void gfx_set_rotation(uint8_t r) {
    gfx_rotation = (r & 3);
    switch (gfx_rotation) {
        case 0:
        case 2:
            gfx_width = gfx_raw_width;
            gfx_height = gfx_raw_height;
            break;
        case 1:
        case 3:
            gfx_width = gfx_raw_height;
            gfx_height = gfx_raw_width;
            break;
    }
}

void gfx_start_write(void) {
    if (transaction_start_function != NULL) transaction_start_function();
}

void gfx_write_pixel(int16_t x, int16_t y, uint16_t color) {
    gfx_draw_pixel(x, y, color);
}

void gfx_write_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    gfx_fill_rect(x, y, w, h, color);
}

void gfx_write_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color) {
    gfx_draw_fast_vline(x, y, h, color);
}

void gfx_write_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    gfx_draw_fast_hline(x, y, w, color);
}

void gfx_write_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);

    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) ystep = 1;
    else ystep = -1;

    for (; x0 <= x1; x0++) {
        if (steep) {
            gfx_write_pixel(y0, x0, color);
        } else {
            gfx_write_pixel(x0, y0, color);
        }
        
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void gfx_end_write(void) {
    if (transaction_end_function != NULL) transaction_start_function();
}

#define grayoled_swap(a, b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color) {
    if ((x >= 0) && (x < gfx_width) && (y >= 0) && (y < gfx_height)) {
        // Pixel is in-bounds. Rotate coordinates if needed.
        switch (gfx_rotation) {
            case 1:
                grayoled_swap(x, y);
                x = gfx_raw_width - x - 1;
                break;
            case 2:
                x = gfx_raw_width - x - 1;
                y = gfx_raw_height - y - 1;
                break;
            case 3:
                grayoled_swap(x, y);
                y = gfx_raw_height - y - 1;
                break;
        }

        switch (color) {
            case 0:
                gfx_buffer[x + (y / 8) * gfx_raw_width] &= ~(1 << (y & 7));
                break;
            case 1:
                gfx_buffer[x + (y / 8) * gfx_raw_width] |= (1 << (y & 7));
                break;
            case 3:
                gfx_buffer[x + (y / 8) * gfx_raw_width] ^= (1 << (y & 7));
                break;
        }
    }
}

void gfx_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color) {
    gfx_start_write();
    gfx_write_line(x, y, x, y + h - 1, color);
    gfx_end_write();
}

void gfx_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color) {
    gfx_start_write();
    gfx_write_line(x, y, x + w - 1, y, color);
    gfx_end_write();
}

void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    gfx_start_write();
    for (int16_t i = x; i < x + w; i++) {
        gfx_write_fast_vline(i, y, h, color);
    }
    gfx_end_write();
}

void gfx_fill_screen(uint16_t color) {
    gfx_fill_rect(0, 0, gfx_width, gfx_height, color);
}

void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    if (x0 == x1) {
        if (y0 > y1) _swap_int16_t(y0, y1);
        gfx_draw_fast_vline(x0, y0, y1 - y0 + 1, color);
    } else if (y0 == y1) {
        if (x0 > x1) _swap_int16_t(x0, x1);
        gfx_draw_fast_hline(x0, y0, x1 - x0 + 1, color);
    } else {
        gfx_start_write();
        gfx_write_line(x0, y0, x1, y1, color);
        gfx_end_write();
    }
}

void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    gfx_start_write();
    gfx_write_fast_hline(x, y, w, color);
    gfx_write_fast_hline(x, y + h - 1, w, color);
    gfx_write_fast_vline(x, y, h, color);
    gfx_write_fast_vline(x + w - 1, y, h, color);
    gfx_end_write();
}

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    gfx_start_write();
    gfx_write_pixel(x0, y0 + r, color);
    gfx_write_pixel(x0, y0 - r, color);
    gfx_write_pixel(x0 + r, y0, color);
    gfx_write_pixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        gfx_write_pixel(x0 + x, y0 + y, color);
        gfx_write_pixel(x0 - x, y0 + y, color);
        gfx_write_pixel(x0 + x, y0 - y, color);
        gfx_write_pixel(x0 - x, y0 - y, color);
        gfx_write_pixel(x0 + y, y0 + x, color);
        gfx_write_pixel(x0 - y, y0 + x, color);
        gfx_write_pixel(x0 + y, y0 - x, color);
        gfx_write_pixel(x0 - y, y0 - x, color);
    }
    gfx_end_write();
}

void gfx_draw_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            gfx_write_pixel(x0 + x, y0 + y, color);
            gfx_write_pixel(x0 + y, y0 + x, color);
        }
        if (cornername & 0x2) {
            gfx_write_pixel(x0 + x, y0 - y, color);
            gfx_write_pixel(x0 + y, y0 - x, color);
        }
        if (cornername & 0x8) {
            gfx_write_pixel(x0 - y, y0 + x, color);
            gfx_write_pixel(x0 - x, y0 + y, color);
        }
        if (cornername & 0x1) {
            gfx_write_pixel(x0 - y, y0 - x, color);
            gfx_write_pixel(x0 - x, y0 - y, color);
        }
    }
}

void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
    gfx_write_fast_vline(x0, y0 - r, 2 * r + 1, color);
    gfx_fill_circle_helper(x0, y0, r, 3, 0, color);
}

void gfx_fill_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t corners, int16_t delta, uint16_t color) {
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;
    int16_t px = x;
    int16_t py = y;

    delta++; // Avoid some +1's in the loop

    while (x < y) {
    if (f >= 0) {
        y--;
        ddF_y += 2;
        f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;
    // These checks avoid double-drawing certain lines, important
    // for the SSD1306 library which has an INVERT drawing mode.
    if (x < (y + 1)) {
        if (corners & 1)
        gfx_write_fast_vline(x0 + x, y0 - y, 2 * y + delta, color);
        if (corners & 2)
        gfx_write_fast_vline(x0 - x, y0 - y, 2 * y + delta, color);
    }
    if (y != py) {
        if (corners & 1)
        gfx_write_fast_vline(x0 + py, y0 - px, 2 * px + delta, color);
        if (corners & 2)
        gfx_write_fast_vline(x0 - py, y0 - px, 2 * px + delta, color);
        py = y;
    }
    px = x;
    }
}

void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    gfx_draw_line(x0, y0, x1, y1, color);
    gfx_draw_line(x1, y1, x2, y2, color);
    gfx_draw_line(x2, y2, x0, y0, color);
}

void gfx_fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }
    if (y1 > y2) {
        _swap_int16_t(y2, y1);
        _swap_int16_t(x2, x1);
    }
    if (y0 > y1) {
        _swap_int16_t(y0, y1);
        _swap_int16_t(x0, x1);
    }

    gfx_start_write();
    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a)
        a = x1;
        else if (x1 > b)
        b = x1;
        if (x2 < a)
        a = x2;
        else if (x2 > b)
        b = x2;
        gfx_write_fast_hline(a, y0, b - a + 1, color);
        gfx_end_write();
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0, dx02 = x2 - x0, dy02 = y2 - y0,
            dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) last = y1; // Include y1 scanline
    else last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
        _swap_int16_t(a, b);
        gfx_write_fast_hline(a, y, b - a + 1, color);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = (int32_t)dx12 * (y - y1);
    sb = (int32_t)dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        /* longhand:
        a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        */
        if (a > b)
        _swap_int16_t(a, b);
        gfx_write_fast_hline(a, y, b - a + 1, color);
    }
    gfx_end_write();
}

void gfx_draw_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius) r = max_radius;

    gfx_start_write();
    gfx_write_fast_hline(x + r, y, w - 2 * r, color);         // Top
    gfx_write_fast_hline(x + r, y + h - 1, w - 2 * r, color); // Bottom
    gfx_write_fast_vline(x, y + r, h - 2 * r, color);         // Left
    gfx_write_fast_vline(x + w - 1, y + r, h - 2 * r, color); // Right
    // draw four corners
    gfx_draw_circle_helper(x + r, y + r, r, 1, color);
    gfx_draw_circle_helper(x + w - r - 1, y + r, r, 2, color);
    gfx_draw_circle_helper(x + w - r - 1, y + h - r - 1, r, 4, color);
    gfx_draw_circle_helper(x + r, y + h - r - 1, r, 8, color);

    gfx_end_write();
}

void gfx_fill_round_rect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color) {
    int16_t max_radius = ((w < h) ? w : h) / 2; // 1/2 minor axis
    if (r > max_radius) r = max_radius;

    gfx_start_write();
    gfx_write_fill_rect(x + r, y, w - 2 * r, h, color);
    gfx_fill_circle_helper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1, color);
    gfx_fill_circle_helper(x + r, y + r, r, 2, h - 2 * r - 1, color);

    gfx_end_write();
}

// void gfx_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg) {
//
// }

// void gfx_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) {
//
// }

// void gfx_get_text_bounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
//
// }

// void gfx_set_text_size(uint8_t s) {
//
// }
