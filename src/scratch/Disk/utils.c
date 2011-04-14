#include "utils.h"
#include "c_io.h"
#include "startup.h"
#include "x86arch.h"

/**
*	@return	Absolute value of num
*/
int abs(int num){
	if(num < 0){
		num *= -1;
	}
	return num;
}

/**
*
*	@param	x	Values to raise to y
*	@param	y	Power to raise x
*
*	@return	Returns x raised to the power of y
*/
int pow(int x, int y){
	if(y==0){
		return 1;
	}

	return x*pow(x, --y);
}

/**
*	Counts the number of characters in the null-terminated string
*
*	@param	str	String to count characters in
*	@return	The number of bytes in str
*/
int strlen(char* str){
	int i=0;
	while(str[i] != 0x00){
		i++;
	}
	return i;
}

/**
*	Sets size bytes of buf to value
*
*	@param	buf		Buffer to modify
*	@param	value	Value to set to buffer
*	@param	size	Size of buffer
*/
void* memset(void* buf, int value, int size){
	int i=0;
	for(i=0; i<size; i++){
		((char*)buf)[i] = value;
	}
	return buf;
}


/**
*	Prints a full character map
*/
void console_char_chart(void){
	int i=0;
	c_printf(" 0 1 2 3 4 5 6 7 8 9 A B C D E F\n");

	for(i=0; i<=255; i++){
		if( (i%0x10) == 0 ){
			c_printf("\n%1x ", (i>>4));
		}

		c_putchar((char)i);

		c_putchar(' ');
	}
}


