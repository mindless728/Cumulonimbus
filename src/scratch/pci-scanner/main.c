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



extern struct MandelbrotInfo mbInfo;

int main( void ) {
	c_puts( "Clearing screen\n");

	c_clearscreen();


	if(init_ethernet(0x8086, 0x1229) == 0){
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
	}

	asm("sti");

	pciScan();

	c_puts( "Installing interrupt handlers...\n" );
	__install_isr(INT_VEC_TIMER, timer_handler);
	__install_isr(INT_VEC_KEYBOARD, keyboard_handler);

	drawMandelbrot();


	while(1){
		__delay(5);
	}



	return( 0 );
}



void timer_handler(int vector, int code){
	//c_printf("timer_handler vector=%d code=%d\n", vector, code);
	clockTicks++;
	c_printf_at(0, 0, "%d ms", TIMER_MS_PER_TICK*clockTicks);
	clear_interrupt(vector);
}



