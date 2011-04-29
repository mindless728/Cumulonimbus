#ifndef DRIVERS_INTERRUPTS_H
#define DRIVERS_INTERRUPTS_H

#include <types.h>

#define INT_VECTOR_COUNT 256
#define MAX_IRQ_FIND_TRIES 3

typedef void (_isr_handler_t)(int vector, int code);

//!	Describes an action to take in response to an IRQ
typedef struct isr_action{
	_isr_handler_t* handler;
	uint32_t calls;
	struct isr_action* next;
} _isr_action_t;


/**
  *	Initializes the interrupt subsystem. After this call interrupts are
  *	enabled.
  *
  *	@return	Always returns E_SUCCESS
  */
status_t _interrupt_init(void);


/**
  *	Waits at most timeout_ms milliseconds for an interrupt to be triggered
  *	on the specified vector.
  *
  *	@param	timeout_ms	Number of milliseconds to spend waiting for an interrupt
  *	@param	vector		Interrupt vector that should be waited on
  *
  *	@return	Returns E_SUCCESS if the specified interrupt occured before the
  *			timeout expired, otherwise E_TIMEOUT is returned.
  */
status_t _interrupt_wait_for_irq(uint32_t timeout_ms, uint8_t vector);


/**
  *	Waits at most timeout_ms milliseconds for any interrupt to be triggered,
  *	this function will first use the supplied isr to handle the interrupt. This
  *	function should only be used when trying to identify what IRQ is associated
  *	with a device which has a software triggerable interrupt.
  *
  *	@param	timeout_ms	Number of milliseconds to spedn waiting for an interrupt
  *	@param	handler		ISR to run if an interrupt occurs.
  *
  *	@return	Returns E_SUCCESS if a single interrupt occured before the
  *			timeout expired. If the timeout expires before an IRQ is serviced
  *			then E_TIMEOUT is returned. If multiple, different, IRQs are
  *			serviced then E_ERROR is returned.
  */
status_t _interrupt_wait(uint32_t timeout_ms, _isr_handler_t* handler);


/**
  *	Special interrupt handler which is only used during a call _interrupt_wait.
  *	This should never be called outside of an ISR.
  *
  *	@param	vector
  *	@param	code
  */
void _interrupt_wait_handler(int vector, int code);


/**
  *	This routine is registered for all possible IRQs. When an IRQ is triggered
  *	all _isr_action_t which are registered for the given vector is executed in
  *	the order they were added.
  *
  *	@param	vector
  *	@param	code
  */
void _interrupt_global_handler(int vector, int code);


/**
  *	This maybe called from within an ISR to prevent subsequent handlers from
  *	from being called.
  */
void _interrupt_terminate(void);


/**
  *	Adds a new interrupt handler to the end of the list of handlers
  *	associated with the provided vector.
  *
  *	@param	handler	ISR
  *	@param	vector	Interrupt vector to add ISR for
  *
  *	@return	Returns E_SUCCESS if everything works
  */
status_t _interrupt_add_isr(_isr_handler_t handler, int vector);


/**
  *	Deletes the first interrupt handler from the list of handlers
  *	associated with the provided vector which calls the specified ISR.
  *
  *	@param	handler	ISR
  *	@param	vector	Interrupt vector to add ISR for
  *
  *	@return	Returns E_SUCCESS if everything works
  */
status_t _interrupt_del_isr(_isr_handler_t handler, int vector);


/**
  *	Clears pending interrupt. Should never be called anywhere other than
  *	in interrupts.c
  *
  *	@param	vector	Interrupt vector to clear
  */
void _interrupt_clear(int vector);

#endif	//DRIVERS_INTERRUPTS_H
