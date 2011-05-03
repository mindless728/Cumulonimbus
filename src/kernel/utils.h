#ifndef UTILS_H
#define UTILS_H

#include <types.h>

#define isPrintable(x) (x>=' ' && x<='~')

int strlen(char* str);
void* memset(void* buf, uint8_t value, int size);

#endif //UTILS_H
