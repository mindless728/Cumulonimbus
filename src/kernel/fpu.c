/**
 * File: fpu.c
 * @author Benjamin Mayes
 * @description A collection of functions wrapping FPU instructions for some 
 * common tasks.
 */

#include "fpu.h"
#include "types.h"

#define SINGLE_ARGUMENT_FPU_ROUTINE( NAME ) inline double NAME( double x ) {  \
    asm volatile( "finit\n\t"                           \
                  "fldl %1\n\t"                         \
                  "f" #NAME "\n\t"                      \
                  "fstpl %0\n\t" : "=m"(x) : "m"(x));   \
        return x;                                      \
    } 

                  
#define DOUBLE_ARGUMENT_FPU_ROUTINE( NAME ) inline double NAME( double x, double y ) {  \
    asm volatile( "finit\n\t"                           \
                  "fldl %2\n\t"                         \
                  "fldl %1\n\t"                         \
                  "f" #NAME "\n\t"                      \
                  "fstpl %0\n\t" : "=m"(x) : "m"(x), "m"(y));   \
        return x;                                        \
    } 



/**
 * Obtains the fpu status register. Can be used for error checking purposes.
 *
 * @return The floating point status register's value.
 */
inline uint16_t get_fpu_status(void) {
    register uint16_t ret;
    asm volatile("fstsw %%ax\n\t" : "=r"(ret) );
    return ret;
}

/**
 * Obtains the FPU control register. Can be used for FPU state checking purposes.
 *
 * @return The floating point control register's value.
 */
inline uint16_t get_fpu_control(void) {
    uint16_t ret;
    asm volatile("fstcw %0\n\t" 
    : "=m"(ret) );
    return ret;
}
/**
 * Obtains the absolute value of x.
 *
 * @param x The number to obtain the absolute value of.
 * @return The sine of x.
 */

SINGLE_ARGUMENT_FPU_ROUTINE(abs)
/**
 * Obtains the cosine of x.
 *
 * @param x The number to obtain the cosine of.
 * @return The sine of x.
 */

SINGLE_ARGUMENT_FPU_ROUTINE(cos)
/**
 * Obtains the sine of x.
 *
 * @param x The number to obtain the sine of.
 * @return The sine of x.
 */

SINGLE_ARGUMENT_FPU_ROUTINE(sin)
/**
 * Obtains the square root of x.
 *
 * @param x The number to obtain the square root of.
 * @return The square root of x.
 */
SINGLE_ARGUMENT_FPU_ROUTINE(sqrt)
/**
 * Rounds x to the nearest integer double.
 *
 * @param x The number to round to the nearest integer.
 */
SINGLE_ARGUMENT_FPU_ROUTINE(rndint)
/**
 * Obtains the angle from (0,0) to (x,y).
 *
 * @param x The x coordinate.
 * @param y The y coordinate.
 */
DOUBLE_ARGUMENT_FPU_ROUTINE(patan)

/**
 * Obtains log base 2 of x.
 *
 * @param x The number to obtain the log2 of.
 * @return The log2 of x.
 */
inline double log2( double x ) {
    asm volatile(    "finit\n\t"
            "fld1\n\t" 
            "fldl %1\n\t" 
            "fyl2x\n\t"
            "fstpl %0\n\t"
            : "=m"(x)
            : "m"(x) );
    return x;
}

/**
 * Obtains log base n of x.
 *
 * @param x The number to obtain the logn of.
 * @return The logn of x.
 */
inline double logn( double x, double n ) {
    // We could use log2 as a subroutine but to be safe let's maintain precision
    // by staying in the FPU.
    asm volatile(    "finit\n\t"
            "fld1\n\t" 
            "fldl %2\n\t"    
            "fyl2x\n\t"         // calculate log2(n)
            "fld1\n\t"
            "fldl %1\n\t"
            "fyl2x\n\t"         // calculate log2(x)
            "fdivp\n\t"         // calculate log2(x)/log2(n)
            "fstpl %0\n\t"      // pop the desired value off the stack
            : "=m"(x)
            : "m"(x), "m"(n) );
    return x;
}

/**
 * Obtains the value of x to the y power.
 *
 * @param x The base.
 * @param y The exponent.
 * @returns x raised to the yth power.
 */
inline double pow( double x, double y ) {
    asm volatile(
        "finit\n\t"         //initialize FPU - probably not needed but good practice
        "fldl %2\n\t"       
        "fldl %1\n\t"       // STACK: st(0) = x, st(1) = y
        "fyl2x\n\t"         // st(0) = y*l2(x)
        "fld %%st(0)\n\t"
        "frndint\n\t"       // st(0) = (int)(y*l2(x)), st(1) = y*l2(x)
        "fxch %%st(1)\n\t"
        "fsub %%st(1), %%st(0)\n\t" //st(0) = (frac)(y*l2(x)), st(1) = (int)(y*l2x)
        "f2xm1\n\t"  // st(0) = (frac pow of 2) - 1, st(1) = (int)(y*l2x)
        "fld1\n\t"
        "faddp\n\t"
        "fscale\n\t" //st(0) = x^y, st(1) = (int)(y*l2x)
        "fstp %%st(1)\n\t" // clean off the stack
        "fstpl %0\n\t"
        : "=m"(x)
        : "m"(x), "m"(y) );

    return x;
}
