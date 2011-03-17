/*
** SCCS ID:	@(#)main.c	1.3	03/15/05
**
** File:	main.c
**
** Author:	K. Reek
**
** Contributor:	Warren R. Carithers
**
** Description:	Dummy main program
*/
#include "c_io.h"
#include "support.h"
#include "startup.h"
#include "x86arch.h"
#include "uart.h"
#include "utils.h"
#include "ethernet.h"
#include "mandelbrot.h"
#include "pci.h"


void timer_handler(int vector, int code);

#define BACKSPACE 0x08
#define NULL 0x00



int clockTicks=0;
int animOverride=0;


extern struct MandelbrotInfo mbInfo;

pci_device_list_t* pciDevices = NULL;

int main( void ) {
	c_puts( "Clearing screen\n");

	c_clearscreen();


	/*if(init_ethernet(0x8086, 0x1229) == 0){
		c_printf("Ethenet 0x1229 detected, w00t!\n");
		c_getchar();
	}
	else if(init_ethernet(0x8086, 0x1049) == 0){
		c_printf("Ethenet 0x1049 detected, w00t!\n");
		c_getchar();
	}
	else{
		c_printf("Ethernet Initialization failed to find device!\n");
		c_getchar();
	}*/


	_pci_alloc_device_list(&pciDevices);

	//c_printf("pciDevices 0x%x\n", pciDevices);

	status_t status = _pci_scan(pciDevices);

	if(status != E_SUCCESS){
		c_printf("PCI bus scan failed with error 0x%x\n", status);
		while(1);
	}

	c_printf("Detected %d PCI devices\n", pciDevices->size);

	c_getchar();

	pci_device_t* dev = pciDevices->first;

	while(dev!=NULL){
		_pci_print_config(dev);
		c_getchar();

		dev = dev->next;
	}



	c_puts( "Installing interrupt handlers...\n" );
	__install_isr(INT_VEC_TIMER, timer_handler);
	__install_isr(INT_VEC_KEYBOARD, keyboard_handler);


	drawMandelbrot();

	asm("sti");


	while(1){
		__delay(5);
	}



	return( 0 );
}



void timer_handler(int vector, int code){
	//c_printf("timer_handler vector=%d code=%d\n", vector, code);
	clockTicks++;

	if(animOverride==0){
		animateMandelbrot();
	}

	c_printf_at(0, 0, "%d ms", TIMER_MS_PER_TICK*clockTicks);
	clear_interrupt(vector);
}



