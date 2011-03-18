#ifndef TYPES_H_
#define TYPES_H_
    // set length integers
    // 8 bit
    typedef char int8_t;
    typedef unsigned char uint8_t;
    typedef uint8_t byte_t;
    // 16 bit
    typedef short int16_t;
    typedef unsigned short uint16_t;
    typedef uint16_t word_t;
    // 32 bit
    typedef long int32_t;
    typedef unsigned long uint32_t;
    typedef uint32_t dword_t;
    // 64 bit
    typedef long long int64_t;
    typedef unsigned long long uint64_t;
    typedef uint64_t qword_t;

    // the standard integer types
    typedef int32_t int_t;
    typedef uint32_t uint_t;
 
    // the standard boolean type (which, for efficiency, is the "fastest" int to work with:.
    typedef uint_t boolean_t;
#endif
