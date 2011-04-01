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
#include "gs_io.h"
#include "timer_isr.h"
#include "random.h"
#include "cmos.h"
#include "vbe_info.h"
#include "vesa_framebuffer.h"
#include "x86arch.h"
#include "support.h"
#include "startup.h"
#include "util.h"
#include "math.h"

extern gs_framebuffer_t framebuffer;
void remove_usb_isr( int vector, int code ) {
    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
    __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
}

int main( void ) {
        asm("sti");
        __install_isr( 0x2A, remove_usb_isr );
        //c_puts( "\nHello, world!\n" );
        //c_printf( "Seconds since midnight: %d\n", seconds_since_midnight() );
        _init_timer(); // Y U NO WORK QEMU?
        //_print_vesa_controller_info();
        //_print_vesa_mode_info();
        //_scan_all_pci_devices();
        //srandom(seconds_since_midnight());
    #ifndef NNO_VESA
        _vesa_init();
        _print_hue_test();
        _print_mandelbrot(1.0);
    #else 
        while(1);
    #endif
        return 0;
}
