#include "utils.h"
#include "c_io.h"
#include "startup.h"
#include <x86arch.h>

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

	for(i = 0; i < size; ++i)
		((uint8_t*)buf)[i] = value;

	/*if(size < 0x4){
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

		//loops = size & 0x3;
		i = loops << 2;		//Multiply by for to get current index

		for(; i<size; i++){
			((uint8_t*)buf)[i] = value;
		}
	}*/
	return buf;
}

int memcmp(const void *s1, const void *s2, int size){
	int i=0;

	if(size < 0x4){
		for(i=0; i<size; i++){
			if(((int8_t*)s1)[i] != ((int8_t*)s2)[i]){
				return (((int8_t*)s1)[i] - ((int8_t*)s2)[i]);
			}
		}
	}
	else{
		int loops = size>>2;	//Divide by four

		for(i=0; i<loops; i++){
			if(((int32_t*)s1)[i] != ((int32_t*)s2)[i]){
				return (((int32_t*)s1)[i] - ((int32_t*)s2)[i]);
			}

		}

		i = loops << 2;		//Multiply by four to get current index
		for(i=0; i<size; i++){
			if(((int8_t*)s1)[i] != ((int8_t*)s2)[i]){
				return (((int8_t*)s1)[i] - ((int8_t*)s2)[i]);
			}

		}
	}

	return 0;
}

void *memcpy(void *dest, const void *src, int size){
	int i=0;

	for(i = 0; i < size; ++i)
		((uint8_t*)dest)[i] = ((uint8_t*)src)[i];

	/*if(size < 0x4){
		for(i=0; i<size; i++){
			((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
		}
	}
	else{
		int loops = size>>2;	//Divide by four

		for(i=0; i<loops; i++){
			((uint32_t*) dest)[i] = ((uint32_t*) src)[i];
		}

		i = loops << 2;

		for(; i<size; i++){
			((uint8_t*) dest)[i] = ((uint8_t*) src)[i];
		}
	}*/
	return dest;
}

void strcpy(char * dst, char * src) {
	uint32_t i = 0;
	while(dst[i] = src[i])
		++i;
}

