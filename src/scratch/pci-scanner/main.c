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
#include "broadcom_ethernet.h"


#define BACKSPACE 0x08


void dummy_isr(int vector, int code);

int animOverride=0;


extern struct MandelbrotInfo mbInfo;

pci_device_list_t* pciDevices = NULL;

int main( void ) {
	c_puts( "Clearing screen\n");

	c_clearscreen();

	__install_isr(0x2a, dummy_isr);


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

	/*if(status != E_SUCCESS){
		c_printf("PCI bus scan failed with error 0x%x\n", status);
		while(1);
	}*/


	c_printf("Detected %d PCI devices\n", pciDevices->size);



	//int index = 0;
	pci_device_t* dev = pciDevices->first;

	while(dev!=NULL){
		_pci_print_config(dev);
		char scan = c_getchar();
		c_clearscreen();
		c_moveto(0,0);

		switch(scan){
			case 0x34:		//Left arrow
				if(dev->prev == NULL){
					dev=pciDevices->last;
				}
				else{
					dev = dev->prev;
				}
				break;
			case 0x36:		//Right arrow
				if(dev->next == NULL){
					dev = pciDevices->first;
				}
				else{
					dev = dev->next;
				}
				break;
			default:
				c_printf("Got key=0x%x\n", scan);
				__delay(20);
		}
		__delay(2);
	}



	return( 0 );
}


void dummy_isr(int vector, int code){
	clear_interrupt(vector);
}

