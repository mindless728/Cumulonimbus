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
#define BUFSIZE 300 // should be a multiple of 3

// the currently active screen (needed to know when to draw to video memory or
// normal memory
extern handle_t active_screen;
// the current PCB
extern pcb_t* _current;

// typedef of the qnode type so that it can be used within its own structure
typedef struct _screen_qnode_t screen_qnode_t;

// a lightweight node that holds a screen handle
struct _screen_qnode_t {
    handle_t handle; // handle associated with the qnode
    screen_qnode_t* next; // the next qnode in the linked data structure
};

// a queue of screens
typedef struct _screen_queue_t {
    screen_qnode_t* first;  // front of the queue
    screen_qnode_t* last;   // back of the queue
    uint_t length;  // number of queued elements
} screen_queue_t;

// circular queue-type structure used for keyboard and mouse input
typedef struct _screen_input_buffer_t {
    uint8_t input_buffer[BUFSIZE]; // a circular buffer used for storing input
    volatile uint8_t* next_char;     //start of the circular buffer
    volatile uint8_t* next_space;    //end of the circular buffer
} screen_input_buffer_t;


// the screen data structure itself
typedef struct _screen_t {
    gs_framebuffer_t fb;    // the framebuffer
    gs_draw_mode_t draw_mode;   // the drawing mode for textures onto the fb
    pid_t owner;                // the owner of this screen
    screen_input_buffer_t kb_buf;   // the keyboard buffer
    screen_input_buffer_t mouse_buf;    // the mouse buffer
} screen_t;

// declaration of the array containing all the intial screens
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
