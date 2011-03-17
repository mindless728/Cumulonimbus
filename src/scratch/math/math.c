#include "math.h"
#include "int_types.h"

// Seems to work just fine.
double exp(double x) {
    double prev = -1.0;
    double ret = 1.0;
    double factorial = 1.0;
    uint32_t exponent = 1;
    
    // lets do a taylor series expansion
    do {
        prev = ret;
        factorial *= exponent;
        ret += powl( x, exponent ) / factorial;
        ++exponent;
    } while( ret != prev );

    return ret;
}

double log(double x) {
    x -= 1.0;
    double ret = 0.0;
    double prev = 0.0;
    double tmp = 1.0;
    uint32_t div = 1;

    do { 
        prev = ret;
        ret += tmp / div;
        tmp *= x;
        ++div;
        ret -= tmp / div;
        tmp *= x;
        ++div;
    } while( ret != prev );
    return ret;
}

double powl( double b, uint32_t n ) {
    double ret = 1.0;
    double tmp = b;
    while( n ) {
        if( 0x1 & n ) {
            ret *= tmp;
        }
        n >>= 1;
        tmp *= tmp;
    }
    return ret;
}

/*double pow( double b, double n ) {
    //TODO: Make this not suck
    //IDEA: Square an multiply for integer portion of double and
    //Babylonian method for the
    double ret = 1;
    int i = 0;
    for( ; i < n; ++i ) {
       ret *= b; 
    }
    return ret;
}*/
