/**
 * @author Benjamin Mayes
 * @description A collection of functions wrapping FPU instructions for some 
 * common mathematical tasks.
 */

#include "int_types.h"

/**
 * Obtains the fpu status register. Can be used for error checking purposes.
 *
 * @return The floating point status register's value.
 */
uint16_t get_fpu_status(void) {
    uint16_t ret = 0xBEEF;
    asm("fstsw %%ax\n\t" : "=r"(ret) );
    return ret;
}

/**
 * Obtains the FPU control register. Can be used for FPU state checking purposes.
 *
 * @return The floating point control register's value.
 */
uint16_t get_fpu_control(void) {
    uint16_t ret = 0xBEEF;
    asm("fstcw %0\n\t" 
    : "=m"(ret) );
    return ret;
}

/**
 * Obtains the square root of x.
 *
 * @param x The number to obtain the square root of.
 * @return The square root of x.
 */
double sqrt( double x ) {
    asm volatile(    "finit\n\t"
            "fldl %1\n\t" 
            "fsqrt\n\t"
            "fstpl %0\n\t"
            : "=m"(x)
            : "m"(x) );
    return x;
}

/**
 * Obtains the sine of x.
 *
 * @param x The number to obtain the sine of.
 * @return The sine of x.
 */
double sin( double x ) {
    asm volatile(    "finit\n\t"
            "fldl %1\n\t" 
            "fsin\n\t"
            "fstpl %0\n\t"
            : "=m"(x)
            : "m"(x) );
    return x;
}

/**
 * Obtains the cosine of x.
 *
 * @param x The number to obtain the cosine of.
 * @return The cosine of x.
 */
double cos( double x ) {
    asm volatile(    "finit\n\t"
            "fldl %1\n\t" 
            "fcos\n\t"
            "fstpl %0\n\t"
            : "=m"(x)
            : "m"(x) );
    return x;
}

/**
 * Obtains log base 2 of x.
 *
 * @param x The number to obtain the log2 of.
 * @return The log2 of x.
 */
double log2( double x ) {
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
double logn( double x, double n ) {
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
double pow( double x, double y ) {
    asm volatile( 
            "finit\n\t"
            "fldl %2\n\t"
            "fldl %1\n\t"
            "fyl2x\n\t"                         // calculates y*log2(x)
            "fld %%st(0)\n\t"                   // copy to the top of the stack 
            "frndint\n\t"                       // round the copy to the nearest integer 
            "fsub %%st(1), %%st(0)\n\t"         // ST(0) = ST(1) - ST(0)
            "fxch %%st(1)\n\t"                  // ST(0) = Frac[y*log2(x) ST(1) = y*log2(x)
            "f2xm1\n\t"                          
            "fld1\n\t"
            "faddp\n\t"                         // ST(0) = 2^(frac*log2(x)) ST(1) = y*log2(x)
            "fscale\n\t"
            "fstpl %0\n\t" 
            : "=m"(x)
            : "m"(x), "m"(y) );
    //c_printf( "%d: 0x%x\n", (int)x, get_fpu_status()  );
    return x;
}
