#include "int_types.h"
#include "fpu.h"

#define BUF_SIZE 128
static char buf[BUF_SIZE];

const char* itoa( int32_t n ) {
    buf[BUF_SIZE-1] = '\0';
    int i = BUF_SIZE-1;
    if( n == 0 ) {
        buf[BUF_SIZE-2] = '0';
        return &buf[BUF_SIZE-2];
    }
    int neg = n < 0;
    if( neg ) n = -n;
    while( n ) {
        buf[--i] = '0' + n % 10;
        n /= 10;
    }
    if( neg ) buf[--i] = '-';
    return &buf[i];
}

const char* uitoa( uint32_t n ) {
    buf[BUF_SIZE-1] = '\0';
    int i = BUF_SIZE-1;
    if( n == 0 ) {
        buf[BUF_SIZE-2] = '0';
        return &buf[BUF_SIZE-2];
    }
    while( n ) {
        buf[--i] = '0' + n % 10;
        n /= 10;
    }
    return &buf[i];
}

const char* itohex( uint32_t n ) {
    buf[BUF_SIZE-1] = '\0';
    int i = BUF_SIZE-1;
    if( n == 0 ) {
        buf[BUF_SIZE-2] = '0';
        return &buf[BUF_SIZE-2];
    }
    while( n ) {
        char c = '0' + (n & 0xF);
        if( c > '9' ) c += 7;
        buf[--i] = c;
        n >>= 4;
    }

    return &buf[i];
}
