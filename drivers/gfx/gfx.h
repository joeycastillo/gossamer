/** 
 * @file gfx.h
 * @brief Graphics routines
 */
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
extern uint16_t gfx_rotation;
extern uint8_t *gfx_buffer;

/**
 * @brief Initializes the GFX driver.
 * @details The GFX driver is a software abstraction layer that allows you to draw to a buffer
 *          in memory, but it does not handle pushing that buffer to a display. You will need
 *          to use a display driver (such as the SH1107 driver) to push the buffer to the
 *          display you are using. These drivers will use the `gfx_buffer` defined above.
 * @param w The width of the display in pixels.
 * @param h The height of the display in pixels.
 * @param depth The color depth of the display. Currently only 1-bit displays are supported.
*/
void gfx_init(int16_t w, int16_t h, int8_t depth);

/**
 * @brief Sets the rotation of the display.
 * @param r The rotation of the display in degrees. Only 90 degree increments are supported 
 *          (0, 90, 180, 270).
 * @details All subsequent drawing operations will take this rotation into account.
*/
void gfx_set_rotation(uint16_t r);

/**
 * @brief Draws a single pixel at the given coordinates.
 * @param x The x coordinate of the pixel.
 * @param y The y coordinate of the pixel.
 * @param color The color of the pixel. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_pixel(int16_t x, int16_t y, uint16_t color);

/**
 * @brief Draws a line from (x0, y0) to (x1, y1).
 * @param x0 The x coordinate of the first point.
 * @param y0 The y coordinate of the first point.
 * @param x1 The x coordinate of the second point.
 * @param y1 The y coordinate of the second point.
 * @param color The color of the line. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

/**
 * @brief Draws a vertical line from (x, y) to (x, y+h).
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param h The height of the line.
 * @param color The color of the line. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_vline(int16_t x, int16_t y, int16_t h, uint16_t color);

/**
 * @brief Draws a horizontal line from (x, y) to (x+w, y).
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param w The width of the line.
 * @param color The color of the line. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_hline(int16_t x, int16_t y, int16_t w, uint16_t color);

/**
 * @brief Draws a rectangle frame from (x, y) to (x+w, y+h).
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The color of the rectangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * @brief Draws a filled rectangle from (x, y) to (x+w, y+h).
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param color The color of the rectangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);

/**
 * @brief Fills the entire screen with the given color.
 * @param color The color you wish to fill. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_fill_screen(uint16_t color);

/**
 * @brief Draws the outline of a circle at (x, y) with radius r.
 * @param x The x coordinate of the center of the circle.
 * @param y The y coordinate of the center of the circle.
 * @param r The radius of the circle.
*/
void gfx_draw_circle(int16_t x, int16_t y, int16_t r, uint16_t color);

/**
 * @brief Draws a filled circle at (x, y) with radius r.
 * @param x The x coordinate of the center of the circle.
 * @param y The y coordinate of the center of the circle.
 * @param r The radius of the circle.
*/
void gfx_fill_circle(int16_t x, int16_t y, int16_t r, uint16_t color);

/**
 * @brief Draws the outline of a triangle with vertices at (x0, y0), (x1, y1) and (x2, y2).
 * @param x0 The x coordinate of the first vertex.
 * @param y0 The y coordinate of the first vertex.
 * @param x1 The x coordinate of the second vertex.
 * @param y1 The y coordinate of the second vertex.
 * @param x2 The x coordinate of the third vertex.
 * @param y2 The y coordinate of the third vertex.
 * @param color The color of the triangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

/**
 * @brief Draws a filled triangle with vertices at (x0, y0), (x1, y1) and (x2, y2).
 * @param x0 The x coordinate of the first vertex.
 * @param y0 The y coordinate of the first vertex.
 * @param x1 The x coordinate of the second vertex.
 * @param y1 The y coordinate of the second vertex.
 * @param x2 The x coordinate of the third vertex.
 * @param y2 The y coordinate of the third vertex.
 * @param color The color of the triangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_fill_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);

/**
 * @brief Draws the outline of a rounded rectangle from (x, y) to (x+w, y+h), with corners of the given radius.
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param radius The radius of the corners.
 * @param color The color of the rounded rectangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_draw_round_rect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

/**
 * @brief Fills a rounded rectangle from (x, y) to (x+w, y+h), with corners of the given radius.
 * @param x The x coordinate of the starting point.
 * @param y The y coordinate of the starting point.
 * @param w The width of the rectangle.
 * @param h The height of the rectangle.
 * @param radius The radius of the corners.
 * @param color The color of the rounded rectangle. For 1-bit displays, 0 is black and 1 is white.
*/
void gfx_fill_round_rect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);

/**
 * @brief Draws a single character at the given coordinates using the 5x7 font.
 * @param x The x coordinate of the top left corner of the character.
 * @param y The y coordinate of the top left corner of the character.
 * @param c The character to draw.
 * @param color The foreground color.
 * @param bg The background color. If you pass the same foreground and background color, the character
 *           will be transparent (only foreground pixels will be drawn, no background will be filled in).
 * @param size The size of the character. 1 is the smallest size (5x7), 2 is twice as big (10x14), etc.
 *             Uses simple pixel doubling, so the result will be blocky.
*/
uint8_t gfx_draw_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

/**
 * @brief Draws a single character at the given coordinates using the 3x5 font.
 * @param x The x coordinate of the top left corner of the character.
 * @param y The y coordinate of the top left corner of the character.
 * @param c The character to draw.
 * @param color The foreground color.
 * @param bg The background color. If you pass the same foreground and background color, the character
 *           will be transparent (only foreground pixels will be drawn, no background will be filled in).
 * @note There is no size parameter for this function, as the 5x7 font is available for larger text.
*/
uint8_t gfx_draw_small_char(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg);

/**
 * @brief Draws a string at the given coordinates using the 5x7 font.
 * @param x The x coordinate of the top left corner of the string.
 * @param y The y coordinate of the top left corner of the string.
 * @param s The string to draw.
 * @param color The foreground color.
 * @param bg The background color. If you pass the same foreground and background color, the string
 *           will be transparent (only foreground pixels will be drawn, no background will be filled in).
 * @param size The size of the font to use. 0 is the 3x5 font, 1 is the 5x7 font, and values of 2 and above
 *             will pixel double the 5x7 font to achieve larger sizes (10x14, 15x21, etc).
*/
void gfx_draw_string(int16_t x, int16_t y, char *s, uint16_t color, uint16_t bg, uint8_t size);

// void gfx_draw_bitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color, uint16_t bg);
