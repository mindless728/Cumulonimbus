#include "kalloc.h"

#define KALLOC_BASE_ADDR 0x20000000

uint32_t _kalloc_base = (uint32_t) KALLOC_BASE_ADDR;


void kfree(void* ptr){
	//TODO: IMPLEMENT!!!!!!
}

void* kalloc(uint32_t bytes){
	int retVal = _kalloc_base;
	_kalloc_base += bytes;

	return (void*) retVal;
}

void* kalloc_aligned(uint32_t bytes, uint8_t alignment){
	uint32_t retVal = _kalloc_base;

	if(alignment == Align_Word){
		if((0x1 & retVal) != 0x0){
			retVal++;
		}
	}
	else if(alignment == Align_DWord){
		if((0x3 & retVal) != 0x0){
			retVal &= (~0x00000003);
			retVal += 0x04;
		}
	}
	else if(alignment == Align_QWord){
		if((0x7 & retVal) != 0x0){
			retVal &= (~0x00000007);
			retVal += 0x08;
		}
	}
	else if(alignment == Align_Paragraph){
		if((0x0f & retVal) != 0x0){
			retVal &= (~0x0000000f);
			retVal += 0x10;
		}
	}

	_kalloc_base = retVal + bytes;
	return (void*) retVal;
}

void* kalloc_set_base(void* base_ptr){
	uint32_t retVal = _kalloc_base;
	_kalloc_base = (uint32_t) base_ptr;

	return (void*)retVal;
}
