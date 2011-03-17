/**
 * @file random.c
 * @author Benjamin Mayes
 * @description An implementation of the MT19937 Mersenne Twister Algorithm
 * Additional notes: Written with the help of the Pseudocode at http://en.wikipedia.org/wiki/Mersenne_twister
 */
#include "int_types.h"
#include "random.h"

// State variables for the internal state of the RNG
static uint32_t state[624];
static uint_t index;

static void generate_numbers(void);

void srandom( uint32_t the_seed ) {
    index = 0;
    state[0] = the_seed;
    uint_t i;
    for( i = 1; i < 624; ++i ) {
       state[i] = (uint32_t)( 0x6c078965l*(state[i-1]^(state[i-1] >> 30)) + i );
    }
}

uint32_t random() {
    if( index == 0 ) {
        generate_numbers();
    }

    uint32_t ret = state[index];
    ret ^= ret >> 11;
    ret ^= (ret << 7) & 0x9D2C5680;
    ret ^= (ret << 15) & 0xEFC60000;
    ret ^= ret >> 18;

    index = (index+1) % 624;
    return ret;
}

static void generate_numbers() {
    uint_t i;
    for( i = 0; i < 624; ++i ) {
        uint32_t n = (0x8000000 & state[i]) | (0x7FFFFFFF & state[(i+1) % 624]);
        state[i] = state[(i+397) % 624] ^ (n >> 1);
        if( n & 0x1 ) {
            state[i] = state[i] ^ 0x9908b0df;
        }
    }
}
