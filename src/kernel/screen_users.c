/**
 * file: screen_users.h
 * @author Benjamin Mayes
 * @description Some user functions to test the screen functionality I wrote
 */

#include "headers.h"
#include "c_io.h"
#include "screen_users.h"
#include "vesa_demo.h"
#include "gs_io.h"

//TODO: Make a user space copy of the _pid_* routines
#include "process.h"


/**
 * Represents a sample user that opens a new screen then closes it immediately
 * afterwards.
 */
static void screen_user_a(void) {
    // open a screen and check if it was successful
    handle_t screen = openscreen(); 
    if( screen >= 0 ) {
        c_printf( "screen_user_a: opened screen %d\n" );
        // close the screen and check if it was successful
        status_t s = closescreen(screen);
        if( s == E_SUCCESS ) {
            c_printf( "screen_user_a: successfully closed screen %d\n", screen );
            exit( X_SUCCESS );
        }
    } else {
        c_printf( "screen_user_a: ERROR opening screen - code %d\n", -screen );
    }
    exit( X_FAILURE );
}

/**
 * Represents a sample user that is getting and setting screens as well as
 * opening and closing them.
 */
static void screen_user_b(void) {
    // open a screen, check for success
    handle_t screen = openscreen();
    if( screen >= 0 ) { 
        // sets the screen and does a sanity check
        handle_t old_screen = setscreen(screen);
        handle_t curr_screen = getscreen();
        if( screen == curr_screen ) {
            c_printf( "screen_user_b: screen successfully set to %d (was: %d)\n", curr_screen, old_screen );
            // check to see if return value for setscreen is correct
            curr_screen = setscreen( old_screen );
            c_printf( "screen_user_b: screen set back to %d (from: %d)\n", old_screen, curr_screen );
            // close the opened screen
            status_t s = closescreen(curr_screen);
            if( s == E_SUCCESS ) {
                c_printf( "screen_user_b: successfully closed screen %d\n", curr_screen );
                exit( X_SUCCESS );
            } else {
                c_printf( "screen_user_b: ERROR - could not close screen %d\n", s );
            }
        } else {
            c_printf( "screen_user_b: ERROR - could not set screen to %d\n", screen );
        }
    } else {
        c_printf( "screen_user_b: ERROR opening screen - code %d\n", -screen );
    }
    exit( X_FAILURE );
}

/**
 * Tests what happens when a user allocates: 
 *  1). many screens 
 *  2). the last screen
 *  3). the first screen after screens are plced back on the empty queue
 */
static void screen_user_c(void) {
    int i = 0;

    // lets test queues by opening all the screens
    handle_t sds[32];
    handle_t s = openscreen();
    c_printf( "screen_user_c: opening screens -" );
    while( s >= 0 ) {
        c_printf( " %d", s );
        sds[i++] = s;
        s = openscreen();
    }
    c_printf( "\n" );

    // now lets close all the screens!
    c_printf( "screen_user_c: closing all those screens...\n" );
    while( i > 0 ) {
       if( closescreen(sds[--i]) != E_SUCCESS ) {
            c_printf( "screen_user_c: ERROR! screen %d opened but cannot be closed!\n", sds[i] );
        }
    }

    // lets test by opening then closing one last screen
    s = openscreen();
    if( s < 0 ) {
        c_printf( "screen_user_c: ERROR: cannot open a new screen!\n" );
    } else {
        c_printf( "screen_user_c: successfully opened screen %d\n", s );
        if( closescreen(s) != E_SUCCESS ) {
            c_printf( "screen_user_c: ERROR: cannot close opened screen!\n" );
        }
    }
}

/**
 * Switches a screen in and out with the main screen.
 */
static void screen_user_d(void) {
    handle_t s = openscreen();
    if( s >= 0 ) {
        c_printf( "screen_user_d: sucessfully opened screen %d\n", s );
        setscreen(s);
        gs_puts_at(0,0,"screen_user_d says, \"Hello, Cumulonimbus User!\""); 
        while( 1 ) {
            sleep(1000);
            switchscreen(s); 
            sleep(100);
            switchscreen(0);
        }
    } else {
        c_printf( "screen_user_d: ERROR - could not open screen!" );
    }
}

void user_vesa_demo( void ) {
    c_getchar();
    handle_t s = openscreen();
    handle_t sold = getscreen();
    c_printf( "VESA DEMO handle: %d, formerly: %d\n", s, sold );
    c_getchar();
    setscreen(s);
    switchscreen(s);
    print_hue_test();
    c_getchar();
    print_julia( 1.5, .25, .25 );
    exit(X_SUCCESS);
}

/**
 * Spawns the actual screen user functions. To be called by the init routine in
 * user.c.
 */
void spawn_screen_users() {
	uint32_t pid;
    #ifdef SPAWN_SCREEN_USER_A
    pid = fork(NULL);
    if( pid < 0 ) {
        c_printf( "spawn_screen_users: error forking screen_user_a" );
    } else if( pid == 0 ) {
        exec( PRIO_STANDARD, screen_user_a );
        c_printf( "spawn_screen_users: error executing screen_user_a" );
        exit( X_FAILURE );
    }
    #endif
    #ifdef SPAWN_SCREEN_USER_B
    pid = fork(NULL);
    if( pid < 0 ) {
        c_printf( "spawn_screen_users: error forking screen_user_b" );
    } else if( pid == 0 ) {
        exec( PRIO_STANDARD, screen_user_b );
        c_printf( "spawn_screen_users: error executing screen_user_b" );
        exit( X_FAILURE );
    }
    #endif
    #ifdef SPAWN_SCREEN_USER_C
    pid = fork(NULL);
    if( pid < 0 ) {
        c_printf( "spawn_screen_users: error forking screen_user_c" );
    } else if( pid == 0 ) {
        exec( PRIO_STANDARD, screen_user_c );
        c_printf( "spawn_screen_users: error executing screen_user_c" );
        exit( X_FAILURE );
    }
    #endif
    #ifdef SPAWN_SCREEN_USER_D
    pid = fork(NULL);
    if( pid < 0 ) {
        c_printf( "spawn_screen_users: error forking screen_user_d" );
    } else if( pid == 0 ) {
        exec( PRIO_STANDARD, screen_user_d );
        c_printf( "spawn_screen_users: error executing screen_user_d" );
        exit( X_FAILURE );
    }
    #endif
    
    #ifdef SPAWN_VESA_DEMO
    pid = fork(NULL);
    if( pid < 0 ) {
        //error
    } else if( pid == 0 ) {
        exec( PRIO_STANDARD, user_vesa_demo );
        exit( X_FAILURE );
    }
    #endif
}
