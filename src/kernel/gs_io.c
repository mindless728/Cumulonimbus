#include "x86arch.h"
#include "gs_io.h"
#include "vbe_info.h"
#include "headers.h"
#include "process.h"
#include "screen.h"

//TODO: Integrate draw modes correctly
static gs_draw_mode_t gs_draw_mode = GS_DRAW_MODE_FLAT;

gs_draw_mode_t gs_set_draw_mode( gs_draw_mode_t mode ) {
    gs_draw_mode_t old_mode = _screens[_current->screen].draw_mode;
    _screens[_current->screen].draw_mode = mode;
    return old_mode;
}

void gs_draw_console() {
    gs_set_draw_mode( GS_DRAW_MODE_FLAT );
    unsigned short* mem = VIDEO_BASE_ADDR;
    int r = 0;
    int c = 0;
    for( r = 0; r < LEGACY_SCREEN_HEIGHT; ++r ) {
        for( c = 0; c < LEGACY_SCREEN_WIDTH; ++c ) {
            gs_putc_at( c*FONT_CHAR_WIDTH, r*FONT_CHAR_HEIGHT, (char)(*mem & 0xFF) );
            ++mem;
        }
    }   
}

void gs_draw_pixel( int x, int y, pixel_t color ) {
    //*GET_PIXEL(x,y) = color;
    pixel_t* p;
    if( _current->screen == active_screen ) {
        p = GET_PIXEL(x,y);
    } else {
        p = GET_PIXEL_BUF(x,y,&_screens[_current->screen].fb);
    }
    *p = color;
}

void gs_draw_double_pixel( int x, int y, pixel_t color_l, pixel_t color_r ) {
    double_pixel_t* p;
    if( _current->screen == active_screen ) {
        p = GET_DOUBLE_PIXEL(x,y);
    } else {
        p = GET_DOUBLE_PIXEL_BUF(x,y,&_screens[_current->screen].fb);
    }
    *p = CREATE_DOUBLE_PIXEL(color_l, color_r);
}

void gs_putc_at( int x, int y, char c ) {
    if( _current->screen == active_screen ) {
        gs_putc_at_buf(x,y,c,vesa_video_memory);
    } else {
        gs_putc_at_buf(x,y,c,&_screens[_current->screen].fb);
    }
}

void gs_puts_at( int x, int y, const char* s ) {
    while( *s ) {
        switch( *s ) {
            case '\n': y+=FONT_CHAR_HEIGHT;
            case '\r': x=0; break;
            default: 
                       //TODO: use actual screen resolutions
                       if( x > vesa_x_resolution ) {x = 0; y += FONT_CHAR_HEIGHT;}
                       if( y > vesa_y_resolution ) {y = 0; x = 0; }
                       gs_putc_at( x, y, *s ); 
                       x += FONT_CHAR_WIDTH;
                       break;
        }
        ++s;
    }
}

void gs_putc_at_buf( int x, int y, char c, gs_framebuffer_t* buf ) {
    uint32_t i = 0;
    uint32_t j = 0;
    for( i = 0; i < FONT_CHAR_HEIGHT>>1; ++i ) {
        for( j = 0; j < FONT_CHAR_WIDTH>>1; ++j ) {
            pixel_t pix = font[(uint32_t)c][i][j];
            if( pix ) pix = -1;
            if( gs_draw_mode & GS_DRAW_MODE_INVERT ) pix = ~pix;
            switch(gs_draw_mode&GS_DRAW_MODE_MASK) {
                case GS_DRAW_MODE_FLAT:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) = pix;
                    break;
                case GS_DRAW_MODE_XOR:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) ^= pix;
                    break;
                case GS_DRAW_MODE_AND:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) &= pix;
                    break;
                case GS_DRAW_MODE_OR:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) |= pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) |= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) |= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) |= pix;
                    break;
            }       
        }
    }

}

void gs_puts_at_buf( int x, int y, const char* s, gs_framebuffer_t* buf ) {
    while( *s ) {
        switch( *s ) {
            case '\n': y+=FONT_CHAR_HEIGHT;
            case '\r': x=0; break;
            default: 
                       //TODO: use actual screen resolutions
                       if( x > vesa_x_resolution ) {x = 0; y += FONT_CHAR_HEIGHT;}
                       if( y > vesa_y_resolution ) {y = 0; x = 0; }
                       gs_putc_at_buf( x += FONT_CHAR_WIDTH, y, *s, buf ); 
                       break;
        }
        ++s;
    }
}

void gs_draw_buf( const gs_framebuffer_t* buf ) {
    uint32_t x = 0;
    uint32_t y = 0;
    double_pixel_t* m = (double_pixel_t*)vesa_video_memory;
    double_pixel_t* b = (double_pixel_t*)buf;
    for(; y < vesa_y_resolution; ++y ) {
        for( x = 0; x < vesa_x_resolution; ++x ) {
            *m = *b;  
            ++m; ++b;
        }
    }
}

void gs_save_buf( gs_framebuffer_t* buf ) {
    uint32_t x = 0;
    uint32_t y = 0;
    double_pixel_t* m = (double_pixel_t*)vesa_video_memory;
    double_pixel_t* b = (double_pixel_t*)buf;
    for(; y < vesa_y_resolution; ++y ) {
        for(; x < vesa_x_resolution; ++x ) {
            *b = *m;
            ++m; ++b;
        }
    }
}


