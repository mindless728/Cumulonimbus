#ifndef FPU_H_
#define FPU_H_
#include "int_types.h"

extern uint16_t get_fpu_status(void);
extern uint16_t get_fpu_control(void);
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
