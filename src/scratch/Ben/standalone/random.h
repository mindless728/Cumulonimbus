/**
 * @author: Benjamin Mayes
 * @description Header file for the MT19937 Psuedo-RNG
 */

#ifndef RANDOM_H_
#define RANDOM_H_
extern void srandom( uint32_t the_seed );
extern uint32_t random(void);

typedef struct _random_state_t {
        uint32_t state[624];
        uint_t index;
} random_state_t;
#endif
