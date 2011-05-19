/**
 * File: screen_manager.c
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: Manages screens. Displays a screen selector that a user can use
 * the mouse to select a screen.
 */
#include "screen.h"
#include "../drivers/mouse/mouse.h"
#include "gs_io.h"
#include "vesa_framebuffer.h"

// The screen the manager uses
static handle_t manager_screen = -1;
static int mouse_x = 100;
static int mouse_y = 100;
static int draw_cursor = 0;
static pid_t zero;

/**
 * Initializes the screen manager by allocating a screen if necesary then
 * setting this process' screen and then finally setting that screen to the
 * active screen.
 */
static void manager_init(void) {
    if( manager_screen == -1 ) {
        // a screen was never opened, open one.
        manager_screen = openscreen();
    }
    // set this process' screen and switch to it.
    setscreen(manager_screen);
    switchscreen(manager_screen);
}

/**
 * Handles the drawing of the cursor and processing of input from the mouse
 * to the current screen.
 */
static void cursor_func(void) {
    // we have to remember the previous info packet in order to correctly do clicks
    int prev_pktinfo;
    int i = 0;
    int j = 0;
    while(1) {
        for( i = 0; i < 6; ++i ) {
            for( j = 0; j < 6; ++j ) {
                gs_draw_pixel( mouse_x+i, mouse_y+j, 0xFFFF ^ gs_get_pixel(mouse_x+i, mouse_y+j));
            }
        }
        //gs_putc_at( mouse_x, mouse_y, 0x3);
        
        int8_t pktinfo = get_mouse();
        int8_t pktx = get_mouse();
        int8_t pkty = get_mouse();
        int8_t pktz = get_mouse();
        for( i = 0; i < 6; ++i ) {
            for( j = 0; j < 6; ++j ) {
                gs_draw_pixel( mouse_x+i, mouse_y+j, 0xFFFF ^ gs_get_pixel(mouse_x+i, mouse_y+j));
            }
        }
        // if the new packet is not registering a click but the old one is,
        // clearly a click was just completed by the user!
        if( (pktinfo & 0xF) == 0x8 && (prev_pktinfo & 0xF) == 0x9 ) {
            // screens are in the form:
            // 0 1 2 3
            // 4 5 6 7
            // 8 9 A B
            // C D E F
            // and are of size 320x256
            handle_t dst_screen = (handle_t)(4*(mouse_y/256) + (mouse_x/320));

            // we don't want to go to our current screen, it'd be redundant 
            // and causes problems. we also probably don't want to go to a
            // screen without an owner
            if( dst_screen != getscreen() && _pid_cmp(&zero,&_screens[dst_screen].owner)  ) {
                //c_printf( "(%d,%d)->%d\n", getscreen(), mouse_x, mouse_y, dst_screen );
                // switch to the selected screen 
                switchscreen( dst_screen ); 
                break;
                //__delay(5);
                //switchscreen(old_screen);
            }
        } else {
            // move the mouse by obtaining x and y deltas from the mouse 
            // packets

            // x value
            mouse_x += get_x_offset(pktinfo, pktx);
            // clipping
            if( mouse_x >= 1280 ){ 
                mouse_x = 1279;
            } else if( mouse_x < 0 ) {
                mouse_x = 0;
            }

            // y value
            mouse_y -= get_y_offset(pktinfo, pkty);
            // clipping
            if( mouse_y >= 1024 ) {
                mouse_y = 1023;
            } else if( mouse_y < 0 ) {
                mouse_y = 0;
            }
        }

        // save the previous info packet
        prev_pktinfo = pktinfo;
    }

    // we want to exit because this process was forked into
    exit(X_SUCCESS);
}

/**
 * Draws the manager's screen.
 */
void screen_manager(void) {
    // initialize the process
    manager_init();
    draw_cursor = 1;
    int pid = fork(NULL);
    if( pid > 0 ) {
        exec( PRIO_STANDARD, cursor_func );
    }

    //TODO: Make this adaptive and display ceil(sqrt(NUM_SCREENS))^2 screen spaces

    // There is no reason to draw the screen manager if the manager is not the
    // active screen
    while( active_screen == manager_screen ) {
        screen_t* s = _screens;
        int r, c;
        int x, y;
        int a, b;
        // we are arranging the screen in 4x4 mini-screens (TODO: Don't hard-code this) 
        for( r = 0; r < 4; ++r ) {
            for( c = 0; c < 4; ++c ) {
                if( _pid_cmp(&zero,&s->owner) ) {
                    gs_framebuffer_t* current = &(s->fb);
                    for( x = 0; x < 320 ; ++x ) { // 1024/16 = 2^6 = 64
                        for( y = 0; y < 256; ++y ) { // 1280/16=256/16*5= 80
                            // For each cluster of 4x4 screens we went to take
                            // average color values and draw a single pixel of the
                            // average color
                            int avgR = 0;
                            int avgG = 0;
                            int avgB = 0;
                            // average of 16 pixels is expensive, let's just take
                            // the one in the upper-righthand corner until we work
                            // on efficiency.

                            avgR = EXTRACT_RED(s->fb.buffer[4*y][4*x]);
                            avgG += EXTRACT_GREEN(s->fb.buffer[4*y][4*x]);
                            avgB += EXTRACT_BLUE(s->fb.buffer[4*y][4*x]);
                            //for( b = 0; b < 4; ++b ) {
                            //    for( a = 0; a < 4; ++a ) {
                            //avgR += EXTRACT_RED(s->fb.buffer[a+4*y][b+4*x]);
                            //avgG += EXTRACT_GREEN(s->fb.buffer[a+4*y][b+4*x]);
                            //avgB += EXTRACT_BLUE(s->fb.buffer[a+4*y][b+4*x]);
                            //    }
                            //}
                            //avgR >>= 4;
                            //avgG >>= 4;
                            //avgB >>= 4;
                            // draw the pixel at the correct location
                            gs_draw_pixel( c*320 + x, r*256 + y, CREATE_PIXEL( avgR, avgG, avgB ) ); 
                        }
                    }
                } 
                s++;
            }    
        }
    }
    exit(0);
}
