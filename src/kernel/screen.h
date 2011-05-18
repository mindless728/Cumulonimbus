/**
 * File: screen.h
 * Author: Benjamin David Mayes
 * Description: Declarations of screen routines for the screen subsystem 
 */

#ifndef SCREEN_H_
#define SCREEN_H_
#include "gs_io.h"
#include "headers.h"
#define NUM_SCREENS 16
#define BUFSIZE 200

// the currently active screen (needed to know when to draw to video memory or
// normal memory
extern handle_t active_screen;
// the current PCB
extern pcb_t* _current;

typedef struct _screen_qnode_t screen_qnode_t;

// a node that holds a screen handle
struct _screen_qnode_t {
    handle_t handle;
    screen_qnode_t* next;
};

// a queue of screens
typedef struct _screen_queue_t {
    screen_qnode_t* first;
    screen_qnode_t* last;
    uint_t length;
} screen_queue_t;

// circular queue-type structure used for keyboard and mouse input
typedef struct _screen_input_buffer_t {
    uint8_t input_buffer[BUFSIZE]; // a circular buffer used for storing input
    volatile uint8_t* next_char;     //start of the circular buffer
    volatile uint8_t* next_space;    //end of the circular buffer
} screen_input_buffer_t;


// the screen data structure itself
typedef struct _screen_t {
    gs_framebuffer_t fb;
    gs_draw_mode_t draw_mode;
    pid_t owner;
    screen_input_buffer_t kb_buf;
    screen_input_buffer_t mouse_buf;
} screen_t;

extern screen_t _screens[NUM_SCREENS];

/**
 * Screen initialization routine.
 */
void _screen_init( void );

/**
 * Obtains a screen handle from the pool of available handle.
 *
 * @return A screen handle that is not in use of a negative number that
 * corresponds to a number whose absolute value is a status_t status.
 */
handle_t _screen_dequeue(void);

/**
 * Adds an unused screen handle to the pool of available handles.
 *
 * @param sd The screen handle to add to the pool.
 * @return E_SUCCESS if successful, a descriptive status otherwise.
 */
status_t _screen_enqueue( handle_t sd ); 
#endif
