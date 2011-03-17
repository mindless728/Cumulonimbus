#include "kalloc.h"

#define KALLOC_BASE_ADDR 0xA00000

int _kalloc_base = (int) KALLOC_BASE_ADDR;

void* kalloc(int bytes){
	int retVal = _kalloc_base;
	_kalloc_base += bytes;

	return (void*)retVal;
}
