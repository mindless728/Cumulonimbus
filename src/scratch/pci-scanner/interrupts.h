#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "types.h"

#define INT_VECTOR_COUNT 256
#define MAX_IRQ_FIND_TRIES 3

typedef void (_isr_handler_t)(int vector, int code);

typedef struct isr_action{
	_isr_handler_t* handler;
	uint32_t calls;
	struct isr_action* next;
} _isr_action_t;



status_t _interrupt_init(void);

status_t _interrupt_wait_for_irq(uint32_t timeout_ms, uint8_t vector);

status_t _interrupt_wait(uint32_t timeout_ms, _isr_handler_t* handler);

void _interrupt_wait_handler(int vector, int code);

void _interrupt_global_handler(int vector, int code);

void _interrupt_terminate(void);

status_t _interrupt_add_isr(_isr_handler_t handler, int vector);

status_t _interrupt_del_isr(_isr_handler_t handler, int vector);


/**
*	Clears pending interrupt
*
*	@param	vector	Interrupt vector to clear
*/
void _interrupt_clear(int vector);

#endif	//INTERRUPTS_H
