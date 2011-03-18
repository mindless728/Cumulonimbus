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
#include "timer_isr.h"
#include "pci_utils.h"
#include "random.h"
#include "cmos.h"
#include "math.h"
#include "vbe_info.h"
#include "vesa_framebuffer.h"

int main( void ) {
        //c_puts( "\nHello, world!\n" );
        //c_printf( "Seconds since midnight: %d\n", seconds_since_midnight() );
        //_init_timer(); // Y U NO WORK QEMU?
        //_scan_all_pci_devices();
        //srandom(seconds_since_midnight());
        _vesa_init();
        _print_vesa_demo();
        //_print_vesa_controller_info();
        //_print_vesa_mode_info();
}
