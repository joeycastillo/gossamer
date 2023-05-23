// Ported from Adafruit_GFX
// See LICENSE for license text

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#pragma once

extern int16_t gfx_raw_width;
extern int16_t gfx_raw_height;
extern int16_t gfx_width;
extern int16_t gfx_height;
extern int8_t gfx_depth;
extern int8_t gfx_rotation;
extern uint8_t *gfx_buffer;

extern void ((*transaction_start_function)(void));
extern void ((*transaction_end_function)(void));

void gfx_init(int16_t w, int16_t h, int8_t gfx_depth);
void gfx_set_rotation(uint8_t r);

void gfx_start_write(void);
void gfx_write_pixel(int16_t x, int16_t y, uint16_t color);
void gfx_write_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_write_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_write_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);
void gfx_write_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void gfx_end_write(void);

void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color);
void gfx_draw_fast_vline(int16_t x, int16_t y, int16_t h, uint16_t color);
void gfx_draw_fast_hline(int16_t x, int16_t y, int16_t w, uint16_t color);
void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void gfx_fill_screen(uint16_t color);
void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

void gfx_draw_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_draw_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void gfx_fill_circle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void gfx_fill_circle_helper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void gfx_fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
void gfx_draw_round_rect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
void gfx_fill_round_rect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

void gfx_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
uint8_t gfx_draw_small_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg);
void gfx_draw_string(int16_t x, int16_t y, char *s, uint16_t color, uint16_t bg, uint8_t size);

// void gfx_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);
// void gfx_get_text_bounds(const char *string, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
// void gfx_set_text_size(uint8_t s);
// void setFont(const GFXfont *f = NULL);
