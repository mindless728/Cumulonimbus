#include "screen.h"
#include "gs_io.h"
#include "vesa_framebuffer.h"

void _manager_init(void) {
    handle_t sd = openscreen();
    setscreen(sd);
}

void _draw_screen_manager(void) {
    handle_t oldscreen = getscreen();
    handle_t newscreen = openscreen();
    setscreen( newscreen );
    switchscreen( newscreen );
    screen_t* s = _screens;
    int r, c;
    int x, y;
    int a, b;
    for( c = 0; c < 4; ++c ) {
        for( r = 0; r < 4; ++r ) {
            gs_framebuffer_t* current = &(s->fb);
            for( x = 0; x < 320 ; ++x ) { // 1024/16 = 2^6 = 64
                for( y = 0; y < 256; ++y ) { // 1280/16=256/16*5= 80
                    int avgR = 0;
                    int avgG = 0;
                    int avgB = 0;
                    for( b = 0; b < 4; ++b ) {
                        for( a = 0; a < 4; ++a ) {
                           avgR += EXTRACT_RED(s->fb.buffer[a+4*y][b+4*x]);
                           avgG += EXTRACT_GREEN(s->fb.buffer[a+4*y][b+4*x]);
                           avgB += EXTRACT_BLUE(s->fb.buffer[a+4*y][b+4*x]);
                        }
                    }
                    avgR >>= 4;
                    avgG >>= 4;
                    avgB >>= 4;
                    gs_draw_pixel( c*320 + x, r*256 + y, CREATE_PIXEL( avgR, avgG, avgB ) ); 
                    //gs_draw_pixel( x, y, CREATE_PIXEL( avgR, avgG, avgB ) ); 
                }
            }
            s++;
       }
    }
    c_getchar();
    setscreen( oldscreen );
    switchscreen( oldscreen );
    closescreen(newscreen);
    
}
