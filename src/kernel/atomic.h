/**
 * File: atomic.h
 * Author: Benjamin David Mayes
 * Description: Wrapper function declarations for various useful atomic 
 * operations provided by Intel.
 */

#ifndef ATOMIC_H_
#define ATOMIC_H_
#include "types.h"

/**
 * Increments a value in memory
 *
 * This is done atomically.
 *
 * @param operand The memory address with the value to increment.
 */
inline void _atomic_increment( volatile int32_t* operand );

/**
 * Decrements a value in memory
 *
 * This is done atomically.
 *
 * @param operand The memory address with the value to decrement.
 */
inline void _atomic_decrement( volatile int32_t* operand );

/**
 * Sets the value at operand to new if *operand == old.
 *
 * This is done atomically.
 *
 * @param operand The operand to set to new provided it is equal to old.
 * @param old     The value of the operand if the operand is to be changed.
 * @param new     The new value of the operand if the operand is to be changed.
 * @return        old if the swap occureed, new if the swap did not occur.
 */
inline int _atomic_cmpxchg( volatile uint32_t* operand, int old, int new );

#endif
