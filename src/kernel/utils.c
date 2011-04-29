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
void* memset(void* buf, uint8_t value, int size){
	int i=0;

	if(size < 0x4){
		for(i=0; i<size; i++){
			((uint8_t*)buf)[i] = value;
		}
	}
	else{
		int loops = size>>2;	//Divide by four
		uint32_t val;
		((uint8_t*) &val)[0] = value;
		((uint8_t*) &val)[1] = value;
		((uint8_t*) &val)[2] = value;
		((uint8_t*) &val)[3] = value;

		for(i=0; i<loops; i++){
			((uint32_t*) buf)[i] = val;
		}

		loops = size &0x3;

		for(i=0; i<loops; i++){
			((uint8_t*)buf)[i] = value;
		}
	}
	return buf;
}

