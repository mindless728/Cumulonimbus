#ifndef UTILS_H
#define UTILS_H

#include <types.h>

#define isPrintable(x) (x>=' ' && x<='~')

int strlen(char* str);
void* memset(void* buf, uint8_t value, int size);
int memcmp(const void *s1, const void *s2, int size);
void *memcpy(void *dest, const void *src, int n);
void strcpy(char * dst, char * src);
#endif //UTILS_H
