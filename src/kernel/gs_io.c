/**
 * File: gs_io.c
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: User-level functions for writing to a screen.
 */

#include <x86arch.h>
#include "gs_io.h"
#include "vbe_info.h"
#include "headers.h"
#include "process.h"
#include "screen.h"


/**
 * Draws the system console onto the current screen.
 */
void gs_draw_console() {
    // iterate through the remapped video memory and pull out console output
    unsigned short* mem = (unsigned short*)VIDEO_BASE_ADDR;
    int r = 0;
    int c = 0;
    for( r = 0; r < LEGACY_SCREEN_HEIGHT; ++r ) {
        for( c = 0; c < LEGACY_SCREEN_WIDTH; ++c ) {
            gs_putc_at( c*FONT_CHAR_WIDTH, r*FONT_CHAR_HEIGHT, (char)(*mem & 0xFF), GS_DRAW_MODE_FLAT );
            ++mem;
        }
    }   
}

/**
 * Obtains a pixel at the specified location.
 *
 * @param x The column of the pixel.
 * @param y The row of the pixel.
 * @return The pixel at (x,y) in the screen's buffer.
 */
pixel_t gs_get_pixel( int x, int y ) {
    // get a pixel based on whether or not the screen's buffer is in or not in
    // memory
    pixel_t* p;
    if( _current->screen == active_screen ) {
        p = GET_PIXEL(x,y);
    } else {
        p = GET_PIXEL_BUF(x,y,&_screens[_current->screen].fb);
    }
    return *p;
}

/**
 * Obtains a pixel at a specified location in the buffer.
 *
 * @param x The column of the pixel.
 * @param y The row of the pixel.
 * @param buf The buffer.
 */
pixel_t gs_get_pixel_buf( int x, int y, gs_framebuffer_t* buf ) {
    return *GET_PIXEL_BUF(x,y,buf);
}

/**
 * Draws a pixel of the chosen color at (x,y).
 *
 * @param x The column of the pixel.
 * @param y The row of the pixel.
 * @param colo The color of the pixel.
 */
void gs_draw_pixel( int x, int y, pixel_t color ) {
    // set a pixel based on whether or not the screen's buffer is in or not in
    // memory
    pixel_t* p;
    if( _current->screen == active_screen ) {
        p = GET_PIXEL(x,y);
    } else {
        p = GET_PIXEL_BUF(x,y,&_screens[_current->screen].fb);
    }
    *p = color;
}

/**
 * Draws a double pixel at location (2*x,y) and (2*x,y) with colors color_l
 * color_r respectively.
 *
 * @param x The column in an array grouping two pixels in each column.
 * @param y The row into the array.
 * @param color_l The color to assign to the left pixel in the double-column
 * @param color_r The color to assign to the right pixel in the double-column
 */
void gs_draw_double_pixel( int x, int y, pixel_t color_l, pixel_t color_r ) {
    // set a pixel based on whether or not the screen's buffer is in or not in
    // memory
    double_pixel_t* p;
    if( _current->screen == active_screen ) {
        p = GET_DOUBLE_PIXEL(x,y);
    } else {
        p = GET_DOUBLE_PIXEL_BUF(x,y,&_screens[_current->screen].fb);
    }
    *p = CREATE_DOUBLE_PIXEL(color_l, color_r);
}

/**
 * Draws a given character with (x,y) being the upper-left pixel.
 * An optional draw mode can be specified.
 *
 * @param x The column to draw the character.
 * @param y The row to draw the character.
 * @param c The character to draw.
 * @param dm The drawing mode.
 */
void gs_putc_at( int x, int y, char c, gs_draw_mode_t dm ) {
    if( _current->screen == active_screen ) {
        gs_putc_at_buf(x,y,c,(gs_framebuffer_t*)vesa_video_memory, dm);
    } else {
        gs_putc_at_buf(x,y,c,&_screens[_current->screen].fb,dm);
    }
}

/**
 * Draws the given string with the first character having (x,y) as its 
 * upper-left pixel.
 *
 * @param x The column to start drawing the string
 * @param y The row to start drawing the string
 * @param s The null-terminated string to draw.
 */
void gs_puts_at( int x, int y, const char* s, gs_draw_mode_t dm ) {
    // draw each character
    while( *s ) {
        switch( *s ) {
            case '\n': y+=FONT_CHAR_HEIGHT;
            case '\r': x=0; break;
            default: 
                       if( x > vesa_x_resolution ) {x = 0; y += FONT_CHAR_HEIGHT;}
                       if( y > vesa_y_resolution ) {y = 0; x = 0; }
                       // draw the character
                       gs_putc_at( x, y, *s, dm ); 
                       x += FONT_CHAR_WIDTH;
                       break;
        }
        ++s;
    }
}

/**
 * Draws a given character at the given buffer with (x,y) being the upper-left
 * pixel. Optionally a draw_mode can be specified.
 *
 * @param x The column to draw the character.
 * @param y The row to draw the character.
 * @param c The character to draw.
 * @param buf The buffer to put the character onto
 * @param dm The drawing mode to draw the font with. 
 */
void gs_putc_at_buf( int x, int y, char c, gs_framebuffer_t* buf, gs_draw_mode_t dm ) {
    uint32_t i = 0;
    uint32_t j = 0;
    // each single pixel of the font is drawn as a 2x2 pixel in this function,
    // so we want to use half of the heights and widths
    for( i = 0; i < FONT_CHAR_HEIGHT>>1; ++i ) {
        for( j = 0; j < FONT_CHAR_WIDTH>>1; ++j ) {
            // extract out the given pixel
            pixel_t pix = font[(uint32_t)c][i][j];

            // we want the pixel to be completely white and all bits to be set
            if( pix ) pix = -1;

            // does the draw mode specify pixel color inversion?
            if( dm & GS_DRAW_MODE_INVERT ) pix = ~pix;
            switch(dm&GS_DRAW_MODE_MASK) {
                // flat draw mode
                //  - just draws the pixel without modification
                case GS_DRAW_MODE_FLAT:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) = pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) = pix;
                    break;
                // xor draw mode
                //  - can be used to put something on the screen in a readable
                //    fashion then can be removed by redrawing it with xor in
                //    the same spot
                case GS_DRAW_MODE_XOR:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) ^= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) ^= pix;
                    break;
                // and draw mode
                //  - probably only useful with inversion
                case GS_DRAW_MODE_AND:
                    *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) &= pix;
                    *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) &= pix;
                    break;
                // or draw mode
                //  - won't draw black
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

/**
 * Draws the given string with the first character having (x,y) as its 
 * upper-left pixel.
 *
 * @param x The column to start drawing the string
 * @param y The row to start drawing the string
 * @param s The null-terminated string to draw.
 * @param buf The buffer to draw the string on.
 * @param dm The drawing mode for each character.
 */
void gs_puts_at_buf( int x, int y, const char* s, gs_framebuffer_t* buf, gs_draw_mode_t dm ) {
    while( *s ) {
        switch( *s ) {
            case '\n': y+=FONT_CHAR_HEIGHT;
            case '\r': x=0; break;
            default: 
                       //TODO: use actual screen resolutions
                       if( x > vesa_x_resolution ) {x = 0; y += FONT_CHAR_HEIGHT;}
                       if( y > vesa_y_resolution ) {y = 0; x = 0; }
                       gs_putc_at_buf( x += FONT_CHAR_WIDTH, y, *s, buf, dm ); 
                       break;
        }
        ++s;
    }
}

/**
 * Copies the current buffer onto the active screen.
 *
 * @param buf The buffer to draw onto the active screen.
 */
void gs_draw_buf( const gs_framebuffer_t* buf ) {
    uint32_t x = 0;
    uint32_t y = 0;

    // select the current screen
    double_pixel_t* m;
    if( _current->screen == active_screen ) {
        m = (double_pixel_t*)vesa_video_memory;
    } else {
        m = _screens[_current->screen].fb.dpbuffer;
    }
    
    // perform the copy
    double_pixel_t* b = (double_pixel_t*)buf;
    for(; y < vesa_y_resolution; ++y ) {
        for( x = 0; x < vesa_x_resolution; ++x ) {
            *m = *b;  
            ++m; ++b;
        }
    }
}

/**
 * Copies the active screen into the given buf.
 *
 * @param buf The buffer to draw the active screen onto.
 */
void gs_save_buf( gs_framebuffer_t* buf ) {
    uint32_t x = 0;
    uint32_t y = 0;

    // select the current screen
    double_pixel_t* m;
    if( _current->screen == active_screen ) {
        m = (double_pixel_t*)vesa_video_memory;
    } else {
        m = _screens[_current->screen].fb.dpbuffer;
    }

    // perform the copy
    double_pixel_t* b = (double_pixel_t*)buf;
    for(; y < vesa_y_resolution; ++y ) {
        for(; x < vesa_x_resolution; ++x ) {
            *b = *m;
            ++m; ++b;
        }
    }
}


