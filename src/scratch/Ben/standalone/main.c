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
#include "pci_utils.h"
#include "random.h"
#include "cmos.h"
#include "math.h"
#include "vbe_info.h"
#include "vesa_framebuffer.h"
#include "x86arch.h"
#include "support.h"
#include "startup.h"
#include "util.h"
uint32_t aaaadonottouchmemory[4024][1024];

extern gs_framebuffer_t framebuffer;
void remove_usb_isr( int vector, int code ) {
    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
    __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
}

int main( void ) {
        __install_isr( 0x2A, remove_usb_isr );
        //c_puts( "\nHello, world!\n" );
        //c_printf( "Seconds since midnight: %d\n", seconds_since_midnight() );
        _init_timer(); // Y U NO WORK QEMU?
        //_scan_all_pci_devices();
        //srandom(seconds_since_midnight());
        _vesa_init();
        gs_puts_at(0,0, "Vesa successfully initialized.");
        gs_puts_at(0,16, "Address of Framebuffer: 0x");
        gs_puts_at(0,32, "Address of memory to not touch: 0x");
        gs_puts_at(26*12,16, itohex((uint32_t)&framebuffer));
        gs_puts_at(34*12,32, itohex((uint32_t)&aaaadonottouchmemory));
        c_getchar();
        gs_puts_at(0,0, "Vesa successfully initialized.");
        gs_puts_at(0,16, "Address of Framebuffer: 0x");
        gs_puts_at(0,32, "Address of memory to not touch: 0x");
        gs_puts_at(26*12,16, itohex((uint32_t)&framebuffer));
        gs_puts_at(34*12,32, itohex((uint32_t)&aaaadonottouchmemory));
        
        _vesa_text_demo();
        _print_vesa_demo();
        //_print_vesa_controller_info();
        //_print_vesa_mode_info();
        return 0;
}
