#ifndef KALLOC_H
#define KALLOC_H

#include <types.h>

enum AlignmentValue{
	Align_Byte = 0x01,
	Align_Word = 0x02,
	Align_DWord = 0x04,
	Align_QWord = 0x08,
	Align_Paragraph = 0x10
};

//TODO: IMPLEMENT
void kfree(void* ptr);

void* kalloc(uint32_t bytes);

void* kalloc_aligned(uint32_t bytes, uint8_t alignment);

void* kalloc_set_base(void* base_ptr);

//void* _kalloc_base;

#endif	//KALLOC_H
