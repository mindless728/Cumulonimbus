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


	asm( "sti" );



	_pci_alloc_device_list(&pciDevices);

	status_t status = _pci_scan(pciDevices);

	if(status != E_SUCCESS){
		c_printf("PCI bus scan failed with error 0x%x\n", status);
		while(1);
	}


	c_printf("Detected %d PCI devices\n", pciDevices->size);


	//bcm_driver_init(pciDevices);
	i8255x_driver_init(pciDevices);

	pci_device_t* dev = pciDevices->first;

	while(dev!=NULL){
		char scan = c_getchar();

		switch(scan){
			case 0x34:		//Left arrow
				if(dev->prev == NULL){
					dev=pciDevices->last;
				}
				else{
					dev = dev->prev;
				}
				c_clearscreen();
				c_moveto(0,0);
				_pci_print_config(dev);
				break;
			case 0x36:		//Right arrow
				if(dev->next == NULL){
					dev = pciDevices->first;
				}
				else{
					dev = dev->next;
				}
				c_clearscreen();
				c_moveto(0,0);
				_pci_print_config(dev);
				break;
			case 0x0A:		//Enter key
				//Mask device's interrupt
				_pci_mask_inta(dev);
				c_clearscreen();
				_pci_read_config(dev->address, &dev->config);
				c_moveto(0,0);
				_pci_print_config(dev);
				c_printf("MASKED");
				break;
			default:
				c_printf_at(78, 0, "%x", (int)scan);
				break;
		}
		//__delay(2);
	}



	return( 0 );
}


void dummy_isr(int vector, int code){
	clear_interrupt(vector);
}

