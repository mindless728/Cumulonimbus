#ifndef GS_IO_H_
#define GS_IO_H_
#include "vesa_framebuffer.h"
#include "gs_font.h"
#define FONT_CHAR_WIDTH  12
#define FONT_CHAR_HEIGHT 16

// A buffer that allows for compact storage of text
typedef struct _gs_text_buffer_t {
    char buf[106][64];
} gs_textbuffer_t;

typedef union _gs_frame_buffer_t {
    pixel_t buf[1280][1024];
    double_pixel_t dpbuf[1280][1024];
} gs_framebuffer_t;

typedef uint32_t gs_draw_mode_t;
#define GS_DRAW_MODE_FLAT 0x0
#define GS_DRAW_MODE_AND 0x1
#define GS_DRAW_MODE_OR 0x2
#define GS_DRAW_MODE_XOR 0x3
#define GS_DRAW_MODE_MASK 0x3
#define GS_DRAW_MODE_DRAW_BLACK 0x4
#define GS_DRAW_MODE_INVERT 0x8

extern gs_draw_mode_t gs_set_draw_mode( gs_draw_mode_t );

extern void gs_puts_at( int x, int y, const char* s );
extern void gs_printf_at( int x, int y, const char* s );
extern void gs_putc_at( int x, int y, char c );
extern void gs_puts_at_buf( int x, int y, const char* s, pixel_t* buf );
extern void gs_putc_at_buf( int x, int y, char c, pixel_t* buf );
extern void gs_draw_buf(const gs_framebuffer_t* buf);
extern void gs_save_buf(gs_framebuffer_t* buf);
extern void gs_put_text_buffer( gs_textbuffer_t* buf );

#endif
