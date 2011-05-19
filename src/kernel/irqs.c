#include <types.h>
#include <x86arch.h>
#include "startup.h"
#include "support.h"
#include "kalloc.h"
#include "irqs.h"

//! Indicates if we are currently executing ISRs
boolean_t _handling_int = FALSE;

//! The global handler will terminate ISR execution if this becomes true;
boolean_t _terminate_isr = FALSE;

int _handled_int = -1;
_isr_handler_t* _test_handler = NULL;
int _wait_irq_count = 0;

//! Table of interrupt handlers
_isr_action_t* _isr_vector_table[INT_VECTOR_COUNT];


status_t _interrupt_init(void){
	//Disable interrupts
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);

	//Initialize interrupt vector lists
	int i = 0;
	for(; i < INT_VECTOR_COUNT; i++){
		_isr_vector_table[i] = kalloc_aligned(sizeof(_isr_action_t), Align_QWord);

		_isr_vector_table[i]->next = NULL;
		_isr_vector_table[i]->calls = 0;
		_isr_vector_table[i]->handler = NULL;
	}

	//Direct all interrupts to global handler
	for(i=0; i < INT_VECTOR_COUNT; i++){
		__install_isr(i, _interrupt_global_handler);
	}


	//Enable interrupts if needed
	_interrupt_enable(state);
	return E_SUCCESS;
}


void _interrupt_swap_idt(boolean_t basic){
	extern	void	( *__isr_stub_table[ 256 ] )( void );
	extern  void    ( *__basic_isr_stub_table[ 256 ])( void );

	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);

	int i;
	for ( i=0; i < 256; i++ ){
		if(basic == FALSE){
			set_idt_entry( i, __isr_stub_table[ i ] );
		}
		else{
			set_idt_entry( i, __basic_isr_stub_table[ i ]);
		}
	}

	//Make sure interrupts are in the same state we found them in
	_interrupt_enable(state);
}


void _interrupt_enable(boolean_t enable){
	boolean_t if_state = _interrupt_is_enabled(); //(__get_flags() & EFLAGS_IF) >> 9;

	#ifdef IRQ_DEBUG
	c_printf("DEBUG irqs_enable(%d) - EFLAGS=0x%x IF=0x%x\n", enable, __get_flags(), if_state);
	#endif

	if( if_state == enable ){
		#ifdef IRQ_DEBUG
		c_printf("INFO: irqs_enable(%d) - skipping\n", enable);
		#endif
		return;
	}

	if(enable != FALSE || enable == TRUE){
		asm("sti");
	}
	else{
		asm("cli");
	}
	return;
}


boolean_t _interrupt_is_enabled(void){
	if((__get_flags() & EFLAGS_IF) > 0){
		return TRUE;
	}
	return FALSE;
}

status_t _interrupt_wait_for_irq(uint32_t timeout_ms, uint8_t vector){
	int count = _isr_vector_table[vector]->calls;
	while((count == _isr_vector_table[vector]->calls) && (timeout_ms > 0)){
		__delay_ms(1);
		timeout_ms--;
	}

	if(timeout_ms > 0){
		return E_SUCCESS;
	}
	return E_TIMEOUT;
}


status_t _interrupt_wait(uint32_t timeout_ms, _isr_handler_t* handler){
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);

	int i=0;
	_handled_int = -1;
	_test_handler = handler;
	_wait_irq_count = 0;

	//Direct all interrupts to global handler
	for(i=0; i < INT_VECTOR_COUNT; i++){
		__install_isr(i, _interrupt_wait_handler);
	}

	#ifdef IRQ_DEBUG
	c_printf("DEBUG: irq_wait() - enabling interrupts\n");
	#endif
	_interrupt_enable(TRUE);	//Enable interrupts

	#ifdef IRQ_DEBUG
	c_printf("DEBUG: irq_wait() - delaying...\n");
	#endif
	while(_handled_int == -1 && timeout_ms > 0){
		__delay_ms(1);
		timeout_ms--;
	}

	_interrupt_enable(FALSE);
	_test_handler = NULL;


	//Direct all interrupts to global handler
	for(i=0; i < INT_VECTOR_COUNT; i++){
		__install_isr(i, _interrupt_global_handler);
	}


	//Enable interrupts if needed
	_interrupt_enable(state);

	#ifdef IRQ_DEBUG
	c_printf("DEBUG: irq_wait() - returning\n");
	#endif

	if(_wait_irq_count == 1){
		return E_SUCCESS;
	}
	else if(_wait_irq_count > 1){
		return E_ERROR;
	}

	return E_TIMEOUT;
}


void _interrupt_terminate(void){
	//Make sure we are inside of an ISR
	if(_handling_int == TRUE){
		_terminate_isr = TRUE;
	}
}


void _interrupt_wait_handler(int vector, int code){
	#ifdef IRQ_DEBUG
	c_printf("WAIT HANDLER\n");
	#endif
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);
	_wait_irq_count++;
	_handled_int = vector;
	if(_test_handler != NULL){
		_test_handler(vector, code);
	}

	#ifdef IRQ_DEBUG
	c_printf("Handled IRQ = 0x%x\n", _handled_int);
	#endif

	_interrupt_clear(vector);
	_interrupt_enable(state);
}


void _interrupt_global_handler(int vector, int code){
	#ifdef IRQ_DEBUG
	c_printf("GLOBAL ISR\n");
	#endif
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);

	#ifdef IRQ_DEBUG
	c_printf("VALIDATING\n");
	#endif

	//Validate vector
	if(vector >= INT_VECTOR_COUNT || vector < 0){
		__panic("_interrupt_global_handler - Invalid interrupt vector!\n");
	}

	//Indicate that we are no executing ISRs
	_handling_int = TRUE;

	//Lookup list in vector table;
	_isr_action_t* action = _isr_vector_table[vector];

	int value = __inb(PIC_MASTER_IMR_PORT);

	#ifdef IRQ_DEBUG
	c_printf("ISR vector=0x%x code=0x%x calls=%d value=0x%x\n", vector, code, action->calls, value);
	#endif


	while(action != NULL && _terminate_isr == FALSE){
		if(action->handler != NULL){
			if(vector != 0x6){
				//c_printf("calling 0x%x @action=0x%x\n", action->handler, action);
			}

			//Call handler
			action->handler(vector, code);
		}
		action->calls++;

		action = action->next;
	}

	//Reset the terminate flag
	_terminate_isr = FALSE;

	//Indicate that we are no longer executing ISRs
	_handling_int = FALSE;
	_interrupt_clear(vector);
	_interrupt_enable(state);
}


status_t _interrupt_add_isr(_isr_handler_t handler, int vector){
	//Validate vector
	if(vector >= INT_VECTOR_COUNT || vector < 0){
		return E_BAD_PARAM;
	}

	//Allocate action structure QWord aligned
	_isr_action_t* action = kalloc_aligned(sizeof(_isr_action_t), Align_QWord);

	if(action == NULL){
		//NOTE: Maybe we should panic here?
		return E_INSUFFICIENT_RESOURCES;
	}

	action->calls = 0;
	action->next = NULL;
	action->handler = handler;

	//Protect against race conditions
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);
	_isr_action_t* actionTemp = _isr_vector_table[vector];

	while(actionTemp->next != NULL){
		actionTemp = actionTemp->next;
	}

	actionTemp->next = action;

	//Enable interrupts if needed
	_interrupt_enable(state);

	return E_SUCCESS;
}


status_t _interrupt_del_isr(_isr_handler_t handler, int vector){
	//Validate vector
	if(vector >= INT_VECTOR_COUNT || vector < 0){
		return E_BAD_PARAM;
	}

	//Protect against race conditions
	boolean_t state = _interrupt_is_enabled();
	_interrupt_enable(FALSE);

	_isr_action_t* previous = _isr_vector_table[vector];
	_isr_action_t* action = previous->next;

	while(action != NULL){
		if(action->handler == handler){
			previous->next = action->next;
			kfree(action);
			asm( "sti" );
			return E_SUCCESS;
		}

		previous = action;
		action = action->next;
	}

	//Enable interrupts if needed
	_interrupt_enable(state);
	return E_NOT_FOUND;
}


void _interrupt_clear(int vector){
	if( vector >= 0x20 && vector < 0x30 ){
		__outb( PIC_MASTER_CMD_PORT, PIC_EOI );
		if( vector > 0x28 ){
			__outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
		}
	}
}

