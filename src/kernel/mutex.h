/**
 * File: mutex.h
 * Author: Benjamin David Mayes
 * Description: Declarations of functions and data types for use with a very 
 * primitive (yet fully functional) binary mutex type.
 */

#ifndef MUTEX_H_
#define MUTEX_H_
#include "types.h"

/**
 * A very simple mutex type.
 */
typedef uint32_t mutex_lock_t;

/**
 * Attempts to lock the given mutex_lock_t. This is a blocking function call.
 *
 * @param s The mutex lock to attempt to grab.
 */
void lock( mutex_lock_t* s );

/**
 * Attempts to lock the given mutex_lock_t. This is a non-blocking function 
 * call.
 *
 * @param s The mutex lock to attempt to grab.
 * @return 1 if the lock was obtained. 0 otherwise.
 */
int lock_nonblock( mutex_lock_t* s );

/**
 * Unlocks the given mutex_lock_t.
 *
 * WARNING: You probably don't want to unlock mutexes you don't own.
 *
 * @param s The mutex to unlock.
 */
void unlock( mutex_lock_t* s );
#endif
