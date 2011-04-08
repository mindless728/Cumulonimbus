/**
 * File: mutex.c
 * Author: Benjamin David Mayes
 * Description: Functions for use with a very primitive (yet fully functional)
 * binary mutex type.
 */

#include "mutex.h"
#include "atomic.h"

// lock
void lock( mutex_lock_t* l ) {
    while( _atomic_cmpxchg( (uint32_t*)l, 0, 1) );
}

// lock_nonblock
int lock_nonblock( mutex_lock_t* l ) {
    return !_atomic_cmpxchg( (uint32_t*)l, 0, 1);
}

// unlock
void unlock( mutex_lock_t* l ) {
    _atomic_cmpxchg( (uint32_t*)l, 1, 0 );
}

/*
 *   Keeping this test here in case if I ever have to reuse it for another
 *   mutex type
 */

/*mutex_lock_t m = 0;

void test_handler( int signal ) {
    printf( "Caught SIGCONT, unlocking mutex...\n" );
    unlock( &m );    
}
int main () {
    signal( SIGCONT, test_handler ); // the resume signal is strangely suited for this
    printf( "Before first lock.\n" );
    lock( &m );
    if( lock_nonblock( &m ) ) {
        printf( "ERROR: Nonblocking lock succeeded but should have failed!\n" );
    } else {
        printf( "Non-blocking lock failed\n" );
    }
    printf( "Initiating deadlock.\n" );
    lock( &m );
    printf( "YAY! It unlocked!\n",m );
    unlock( &m );
    if( lock_nonblock( &m ) ) {
        printf( "Non-block lock obtained.\n" );
    } else {
        printf( "ERROR: Nonblocking lock failed but should have succeeded!\n" );
    }
    printf( "Initiating deadlock.\n" );
    lock( &m );
    printf( "YAY! It unlocked!\n" );
}*/
