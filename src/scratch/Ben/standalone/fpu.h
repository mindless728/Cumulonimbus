#ifndef FPU_H_
#define FPU_H_
extern uint16_t get_fpu_status(void);
extern uint16_t get_fpu_control(void);
extern double sqrt( double x );
extern double sin( double x );
extern double cos( double x );
extern double log2( double x );
extern double logn( double x, double base );
extern double pow( double x, double y );
#endif
