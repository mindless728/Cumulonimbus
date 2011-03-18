#include "gs_io.h"
#include "vbe_info.h"

static gs_draw_mode_t gs_draw_mode;

gs_draw_mode_t gs_set_draw_mode( gs_draw_mode_t mode ) {
    gs_draw_mode = mode;
}

void gs_putc_at( int x, int y, char c ) {
  int i = 0;
  int j = 0;
  for( i = 0; i < FONT_CHAR_HEIGHT>>1; ++i ) {
        for( j = 0; j < FONT_CHAR_WIDTH>>1; ++j ) {
            pixel_t pix = font[(uint32_t)c][i][j];
            if( gs_draw_mode & GS_DRAW_MODE_INVERT ) pix = ~pix;
            if( pix || (gs_draw_mode & GS_DRAW_MODE_DRAW_BLACK) ){
                switch(gs_draw_mode&GS_DRAW_MODE_MASK) {
                    case GS_DRAW_MODE_FLAT:
                    *GET_PIXEL(x+(j<<1),y+(i<<1)) = pix;
                    *GET_PIXEL(x+(j<<1),y+1+(i<<1)) = pix;
                    *GET_PIXEL(x+1+(j<<1),y+(i<<1)) = pix;
                    *GET_PIXEL(x+1+(j<<1),y+1+(i<<1)) = pix;
                    break;
                    case GS_DRAW_MODE_XOR:
                    *GET_PIXEL(x+(j<<1),y+(i<<1)) ^= pix;
                    *GET_PIXEL(x+(j<<1),y+1+(i<<1)) ^= pix;
                    *GET_PIXEL(x+1+(j<<1),y+(i<<1)) ^= pix;
                    *GET_PIXEL(x+1+(j<<1),y+1+(i<<1)) ^= pix;
                    break;
                    case GS_DRAW_MODE_AND:
                    *GET_PIXEL(x+(j<<1),y+(i<<1)) &= pix;
                    *GET_PIXEL(x+(j<<1),y+1+(i<<1)) &= pix;
                    *GET_PIXEL(x+1+(j<<1),y+(i<<1)) &= pix;
                    *GET_PIXEL(x+1+(j<<1),y+1+(i<<1)) &= pix;
                    break;
                    case GS_DRAW_MODE_OR:
                    *GET_PIXEL(x+(j<<1),y+(i<<1)) |= pix;
                    *GET_PIXEL(x+(j<<1),y+1+(i<<1)) |= pix;
                    *GET_PIXEL(x+1+(j<<1),y+(i<<1)) |= pix;
                    *GET_PIXEL(x+1+(j<<1),y+1+(i<<1)) |= pix;
                    break;
                }       
            }
        }
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

void gs_putc_at_buf( int x, int y, char c, pixel_t* buf ) {
  uint32_t i = 0;
  uint32_t j = 0;
  for( i = 0; i < FONT_CHAR_HEIGHT>>1; ++i ) {
        for( j = 0; j < FONT_CHAR_WIDTH>>1; ++j ) {
            pixel_t pix = font[(uint32_t)c][i][j];
            /*if(pix){
                *GET_PIXEL_BUF(x+(j<<1),y+(i<<1),buf) ^= pix;
                *GET_PIXEL_BUF(x+(j<<1),y+1+(i<<1),buf) ^= pix;
                *GET_PIXEL_BUF(x+1+(j<<1),y+(i<<1),buf) ^= pix;
                *GET_PIXEL_BUF(x+1+(j<<1),y+1+(i<<1),buf) ^= pix;
            }*/
            if( gs_draw_mode & GS_DRAW_MODE_INVERT ) pix = ~pix;
            if( pix || (gs_draw_mode & GS_DRAW_MODE_DRAW_BLACK) ){
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

}

void gs_puts_at_buf( int x, int y, const char* s, pixel_t* buf ) {
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

    double_pixel_t* m = vesa_video_memory;
    double_pixel_t* b = (double_pixel_t*)buf;
    for(; y < 1024; ++y ) {
        for( x = 0; x < 1280; ++x ) {
            *m = *b;  
            ++m; 
            ++b;
        }
    }
}


