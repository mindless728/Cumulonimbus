#include "klib.h"
#include "defs.h"
#include "screen.h"

screen_descriptor_t active_screen = 0;
screen_t _screens[NUM_SCREENS];
static screen_qnode_t _screen_nodes[NUM_SCREENS];
screen_queue_t _unused_screens;
screen_queue_t _active_screens;

static screen_qnode_t* _dequeue( screen_queue_t* q ) {
    if( !q->length ) return 0;
    screen_qnode_t* n = q->first;
    --(q->length);
    q->first = n->next;
    if( n == q->last ) q->last = 0;
    return n;
}

static screen_qnode_t* _remove_node( screen_queue_t* q, screen_descriptor_t sd ) {
    screen_qnode_t* node = q->first;
    screen_qnode_t* prev = 0;

    while( node ) {
        if( node->descriptor == sd ) break;
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

static void _enqueue( screen_queue_t* q, screen_qnode_t* n ) {
    n->next = 0;
    if( q->length) {
        q->last->next = n;
        q->last = n;
    } else {
        q->first = n;
        q->last = n;
    }
    ++(q->length);
}

static void _clear_screen( screen_t* screen ) {
    _memclr( (void*)screen,sizeof(screen_t) );
    screen->owner = -1;
    screen->draw_mode = GS_DRAW_MODE_XOR;
}

void _screen_init() {
    int i;
    c_printf( "Clearing screens...\n" );
    for( i = 0; i < NUM_SCREENS; ++i ) {
        _screen_nodes[i].descriptor = i;
        _clear_screen( &_screens[i] );
    }

    c_printf( "Initializing queues...\n" );
    // initialize the queues
    _unused_screens.first = &_screen_nodes[0];
    _unused_screens.last = &_screen_nodes[NUM_SCREENS-1];
    _unused_screens.length = NUM_SCREENS;
    _active_screens.first = 0;
    _active_screens.last = 0;
    _active_screens.length = 0;

    c_printf( "connecting consecutive elements in queue...\n" );
    // form the connections in the queue and initialize the qnodes
    for( i = 0; i < NUM_SCREENS - 1; ++i ) {
        _screen_nodes[i].descriptor = i;
        _screen_nodes[i].next = &_screen_nodes[i+1];
    }
}

screen_descriptor_t _screen_dequeue() {
    screen_qnode_t* node = _dequeue( &_unused_screens );
    if( !node ) return E_EMPTY;
    _enqueue( &_active_screens, node );
    _clear_screen( &_screens[node->descriptor] );
    return node->descriptor;
}

status_t _screen_enqueue( screen_descriptor_t sd ) {
    screen_qnode_t* node = _remove_node( &_active_screens, sd ); 
    if( node ) {
        _enqueue( &_unused_screens, node );
        return E_SUCCESS;
    } else {
        return E_BAD_PARAM;
    }
}
