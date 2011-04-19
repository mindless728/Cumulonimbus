#ifndef UTILS_H
#define UTILS_H

#include "types.h"

#define NULL 0x00
#define isPrintable(x) (x>=' ' && x<='~')

int abs(int num);
int pow(int x, int y);
int strlen(char* str);
void* memset(void* buf, uint8_t value, int size);

void console_char_chart(void);

//int sprintf(char* str, const char** f);

#endif //UTILS_H
