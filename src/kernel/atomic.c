/**
 * File: atomic.c
 * Author: Benjamin David Mayes
 * Description: Wrapper function definitions for various useful atomic 
 * operations provided by Intel.
 */

#include "atomic.h"

// _atomic_increment
inline void _atomic_increment( volatile int32_t* operand ) {
    asm volatile( "lock incl %0\n\t" : "=m"(*operand) ); 
}

// _atomic_decrement
inline void _atomic_decrement( volatile int32_t* operand ) {
    asm volatile( "lock decl %0\n\t" : "=m"(*operand) ); 
}

// _atomic_cmpxchg
inline int _atomic_cmpxchg( volatile uint32_t* operand, int old, int new ){
    asm volatile( "lock cmpxchg %3, %0\n\t" 
        : "=m"(*operand), "=a"(old) 
        : "a"(old), "d"(new) ); 
}
