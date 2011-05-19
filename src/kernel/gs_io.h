/**
  * File: gs_io.h
  * Author: Benjamin David Mayes
  * Description: Drawing routines for user-level applications (user-level is an
  * after-thought). 
  */

#ifndef GS_IO_H_
#define GS_IO_H_
#include "gs_font.h"
#include "vesa_framebuffer.h"
// size of the legacy console
#define LEGACY_SCREEN_WIDTH 106
#define LEGACY_SCREEN_HEIGHT 64

// size of the font in pixels
#define FONT_CHAR_WIDTH  12
#define FONT_CHAR_HEIGHT 16

// A buffer that allows for compact storage of text in a manner that hopefully
// will be usable with existing c_io functions.
typedef struct _gs_text_buffer_t {
    uint8_t buffer[106][64];            
    uint32_t scroll_min_x, scroll_min_y;
    uint32_t scroll_max_x, scroll_max_y;
    uint32_t curr_x, curr_y;
    uint32_t min_x, min_y;
    uint32_t max_x, max_y;
} gs_textbuffer_t;

// a framebuffer type
typedef union _gs_frame_buffer_t {
    pixel_t buffer[1024][1280];         //read/write a single pixel at a time
    double_pixel_t dpbuffer[640][1024]; //take advantage of the 32 bit architecture to read/write faster
} gs_framebuffer_t;

// drawing modes
typedef uint32_t gs_draw_mode_t;
#define GS_DRAW_MODE_FLAT 0x0       // straight from the font buffer
#define GS_DRAW_MODE_AND 0x1        // anded into current pixels
#define GS_DRAW_MODE_OR 0x2         // ored into current pixels
#define GS_DRAW_MODE_XOR 0x3        // xored into current pixels
#define GS_DRAW_MODE_MASK 0x3       // mask for the drawing mode types
#define GS_DRAW_MODE_DRAW_BLACK 0x4 // do we want to draw black pixels?
#define GS_DRAW_MODE_INVERT 0x8     // invert what is drawn


// graphics system functions
extern void gs_draw_console(void);
extern gs_draw_mode_t gs_set_draw_mode( gs_draw_mode_t );
extern pixel_t gs_get_pixel( int x, int y );
extern pixel_t gs_get_pixel_buf( int x, int y, gs_framebuffer_t* buf );
extern void gs_draw_pixel( int x, int y, pixel_t color );
extern void gs_draw_double_pixel( int x, int y, pixel_t color_l, pixel_t color_r );
extern void gs_puts_at( int x, int y, const char* s, gs_draw_mode_t dm );
extern void gs_putc_at( int x, int y, char c, gs_draw_mode_t dm );
extern void gs_puts_at_buf( int x, int y, const char* s, gs_framebuffer_t* buf, gs_draw_mode_t dm );
extern void gs_putc_at_buf( int x, int y, char c, gs_framebuffer_t* buf, gs_draw_mode_t dm );
extern void gs_draw_buf(const gs_framebuffer_t* buf);
extern void gs_save_buf(gs_framebuffer_t* buf);
extern void gs_clear(void);
extern void gs_clear_buf( gs_framebuffer_t* buf );
#endif
