#ifndef UTILS_H
#define UTILS_H

#define NULL 0x00
#define isPrintable(x) (x>=' ' && x<='~')

int abs(int num);
int pow(int x, int y);
int strlen(char* str);
void* memset(void* buf, int value, int size);
void clear_interrupt(int vector);

void console_char_chart(void);

#endif //UTILS_H
