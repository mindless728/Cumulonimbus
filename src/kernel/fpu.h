/**
 * file: fpu.h
 * @author Benjamin Mayes
 * @description declarations of functions wrapping fpu instructions for some 
 * common tasks.
 */

#ifndef FPU_H_
#define FPU_H_
#include "types.h"

// Register access functions
extern inline uint16_t get_fpu_status(void);
extern inline uint16_t get_fpu_control(void);

// mathematical functions
extern inline double abs( double x );
extern inline double sqrt( double x );
extern inline double rndint( double x );
extern inline double sin( double x );
extern inline double cos( double x );
extern inline double patan( double x, double y );
extern inline double log2( double x );
extern inline double logn( double x, double base );
extern inline double pow( double x, double y );

#endif
