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
#include "fpu.h"
//uint32_t aaaadonottouchmemory[8096][1024];

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
    #ifndef NO_VESA
        _vesa_init();
        //gs_puts_at(0,0, "Vesa successfully initialized.");
        //gs_puts_at(0,16, "Address of Framebuffer: 0x");
        //gs_puts_at(0,32, "Address of memory to not touch: 0x");
        //gs_puts_at(26*12,16, itohex((uint32_t)&framebuffer));
        //gs_puts_at(34*12,32, itohex((uint32_t)&aaaadonottouchmemory));
        //gs_puts_at(20*12,64, itohex(get_fpu_status()));
        //gs_puts_at(20*12,80, itohex(get_fpu_control()));
        //gs_puts_at(20*12,112, itoa(1000*cos(1.414/2)));
        //c_getchar();
        //gs_puts_at(0,0, "Vesa successfully initialized.");
        //gs_puts_at(0,16, "Address of Framebuffer: 0x");
        //gs_puts_at(0,32, "Address of memory to not touch: 0x");
        //gs_puts_at(26*12,16, itohex((uint32_t)&framebuffer));
        //gs_puts_at(34*12,32, itohex((uint32_t)&aaaadonottouchmemory));
        //gs_puts_at(20*12,64, itohex(get_fpu_status()));
        //gs_puts_at(20*12,80, itohex(get_fpu_control()));
        //gs_puts_at(20*12,112, itoa(1000*cos(1.414/2)));
        
        //_vesa_text_demo();
        //_print_vesa_demo();
        _print_hue_test();
        _print_mandelbrot(1.0);
    #else 
        int i = 0;
        for( i = 0; i < 1000000; ++i ) {
            c_printf( "%d\n", (int)(1000000*exp(0.5*logn(i,2.718182))));//logn( 2.0, 2.718182 ))));
        }
        //_print_vesa_controller_info();
        //c_printf("%d\n", (int)sqrt(2500.0) );
        //c_printf("0x%x\n", get_fpu_status() );
        //int base = (int)log2(66234.0);
        //c_printf("0x%x\n", (int)base );
        //c_printf("%d\n", base);
    #endif
        return 0;
}
