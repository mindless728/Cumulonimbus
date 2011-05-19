/**
 * File: screen.c
 * Author: Benjamin David Mayes
 * Description: screen routines for the screen subsystem 
 */

#include "klib.h"
#include "defs.h"
#include "screen.h"

// globals
handle_t active_screen = 0;     // the active screen
screen_t _screens[NUM_SCREENS]; // array containing all screens
static screen_qnode_t _screen_nodes[NUM_SCREENS]; // screen nodes used for queueing up screens
screen_queue_t _unused_screens; // queue for inactive screens
screen_queue_t _active_screens; // queue for active screens

/**
 * Removes the first screen from the screen queue.
 */
static screen_qnode_t* _dequeue( screen_queue_t* q ) {
    if( !q->length ) return 0;
    screen_qnode_t* n = q->first;
    --(q->length);
    q->first = n->next;
    if( n == q->last ) q->last = 0;
    return n;
}

/**
 * Removes a node that is in the screen queue from somewhere within the screen
 * queue.
 */
static screen_qnode_t* _remove_node( screen_queue_t* q, handle_t sd ) {
    screen_qnode_t* node = q->first;
    screen_qnode_t* prev = 0;

    while( node ) {
        if( node->handle == sd ) break;
        prev = node;
        node = node->next;
    }
    // we need to do a removal from the queue
    if( node ) {
        if( prev ) {
            // there is a predecessor node, therefore
            prev->next = node->next;
        } else {
            // there is no predecessor node, therefore the next node is now first
            q->first = node->next;
        }
        // node is last, that means the previous node is now the last node.
        // note that if there is no previous node then this sets it correctly
        if( node->next == 0 ) {
            q->last = prev;
        }
    } 
    --(q->length);
    return node;
}

/**
 * Queue a node up in the screen queue.
 */
static void _enqueue( screen_queue_t* q, screen_qnode_t* n ) {
    // the node will be at the end so it shouldn't have a next node
    n->next = 0;
    if( q->length) {
        // new node will not the be the first node
        q->last->next = n; // the old last node's next node is the new node
        q->last = n; // the queue's last node is the new node
    } else {
        // new node will be first (and only) element in the queue after addition
        q->first = n;
        q->last = n;
    }

    // increment the queue's length
    ++(q->length);
}

// clear a screen
static void _clear_screen( screen_t* screen ) {
    _memclr( (void*)screen,sizeof(screen_t) );
    _pid_clear(&screen->owner);
    screen->draw_mode = GS_DRAW_MODE_FLAT;
}

// _screen_init
void _screen_init() {
    int i;
    c_printf( " screen" );
    for( i = 0; i < NUM_SCREENS; ++i ) {
        _screen_nodes[i].handle = i;
        _pid_clear(&_screens[i].owner);
    }

    // initialize the queues
    _unused_screens.first = &_screen_nodes[0];
    _unused_screens.last = &_screen_nodes[NUM_SCREENS-1];
    _unused_screens.length = NUM_SCREENS;
    _active_screens.first = 0;
    _active_screens.last = 0;
    _active_screens.length = 0;

    // form the connections in the queue and initialize the qnodes
    for( i = 0; i < NUM_SCREENS - 1; ++i ) {
        _screen_nodes[i].handle = i;
        _screen_nodes[i].next = &_screen_nodes[i+1];
    }
}

// _screen_dequeue
handle_t _screen_dequeue() {
    screen_qnode_t* node = _dequeue( &_unused_screens );
    if( !node ) return E_EMPTY;
    _enqueue( &_active_screens, node );
    // prep the screen for use by a process
    _clear_screen( &_screens[node->handle] );
    // return the handle
    return node->handle;
}

// _screen_enqueue
status_t _screen_enqueue( handle_t sd ) {
    screen_qnode_t* node = _remove_node( &_active_screens, sd ); 
    if( node ) {
        _enqueue( &_unused_screens, node );
        return E_SUCCESS;
    } else {
        return E_BAD_PARAM;
    }
}
