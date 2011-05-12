#include "screen.h"
#include "gs_io.h"
#include "vesa_framebuffer.h"

// The screen the manager uses
static handle_t manager_screen = -1;

/**
 * Initializes the scree manager by allocating a screen.
 */
void _manager_init(void) {
    manager_screen = openscreen();
}

/**
 * Draws the manager's screen.
 */
void _draw_screen_manager(void) {
    //if( manager_screen == -1 ) _manager_init();
    handle_t newscreen = openscreen();
    handle_t oldscreen = getscreen();
    setscreen(newscreen);
    switchscreen(newscreen);
    //TODO: Make this adaptive and display ceil(sqrt(NUM_SCREENS))^2 screen spaces

    // There is no reason to draw the screen manager if the manager is not the
    // active screen
    //if( active_screen == manager_screen ) {
        screen_t* s = _screens;
        int r, c;
        int x, y;
        int a, b;
        // we are arranging the screen in 4x4 mini-screens
        for( c = 0; c < 4; ++c ) {
            for( r = 0; r < 4; ++r ) {
                gs_framebuffer_t* current = &(s->fb);
                for( x = 0; x < 320 ; ++x ) { // 1024/16 = 2^6 = 64
                    for( y = 0; y < 256; ++y ) { // 1280/16=256/16*5= 80
                        // For each cluster of 4x4 screens we went to take
                        // average color values and draw a single pixel of the
                        // average color
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
                    }
                }
                s++;
            }    
        }
    //}
    c_getchar();
    setscreen(oldscreen);
    switchscreen(oldscreen);
    closescreen(newscreen);
}
