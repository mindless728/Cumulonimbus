/**
 * @author: Benjamin Mayes
 * @description Header file for the MT19937 Psuedo-RNG
 */

#ifndef RANDOM_H_
#define RANDOM_H_
extern void srandom( uint32_t the_seed );
extern uint32_t random(void);
#endif
