#include "c_io.h"
#include "x86arch.h"
#include "support.h"
#include "startup.h"
#include "types.h"

#define WAIT_WRITE 1
#define WAIT_READ 0
#define MOUSE_TIMEOUT 2000000000

void _isr_mouse( int vector, int code ) {
    c_printf( "MOUSE: 0x%02x\n", __inb( 0x60 )  );
    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
    __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
}


static void mouse_wait( uint8_t type) {
    //uint32_t i = MOUSE_TIMEOUT;
    switch( type ) {
        case WAIT_READ:
            while( ((__inb(0x64) & 1)!=1) );
            //c_printf( "%d\n", i );
            break;
        case WAIT_WRITE:
            while( ((__inb(0x64) & 2)!=0) );
            //c_printf( "%d\n", i );
            break;
        default:
            break;
    }
}

static void mouse_write( uint8_t write_byte ) {
    mouse_wait(WAIT_WRITE);
    __outb( 0x64, 0xD4 );
    mouse_wait(WAIT_WRITE);
    __outb( 0x60, write_byte );
    uint8_t ch;

    // read an ACK from the mouse
    do {
        mouse_wait(WAIT_READ);
        ch = __inb( 0x60 );
        //c_printf( "%02x\n", ch );
    } while( ch != 0xFA );
}

static uint8_t mouse_read(void) {
    mouse_wait(WAIT_READ);
    return __inb( 0x60 );
}

void mouse_init() {
    c_printf( "Installing Mouse ISR\n" );
    __install_isr( INT_VEC_MOUSE, _isr_mouse );
    //c_printf( "Enabling the mouse.\n" );
    // Enable the mouse port
    mouse_wait(WAIT_WRITE);
    __outb( 0x64, 0xA8 );
    
    // reset the mouse
    mouse_wait(WAIT_WRITE);
    __outb( 0x64, 0xD4 );
    mouse_wait(WAIT_WRITE);
    __outb( 0x60, 0xFF );

    //Wait for reset completion
    uint8_t ch;
    do {
        mouse_wait(WAIT_READ);
        ch = __inb(0x60);
    } while( ch != 0xAA ); 

    
    // we want to read the command register, this is done by writing 0x20 to port 0x64 then reading from port 0x60
    //c_printf( "Reading command register from the mouse.\n" );
    mouse_wait(WAIT_WRITE);
    __outb( 0x64, 0x20 );
    mouse_wait(WAIT_READ);
    uint8_t status_byte = __inb(0x60);

    // we want to enable mouse IRQ 12, this is done by setting bit 1 in the command register
    //c_printf( "Enabling IRQ12 %d (old cmd)  -> %d (new cmd) \n", status_byte, status_byte | 2 );
    status_byte |= 2;

    // to write to the command register we write 0x60 to port 0x64 then write the new command register to 0x
    mouse_wait(WAIT_WRITE);
    __outb(0x64, 0x60);
    mouse_wait(WAIT_WRITE);
    //c_printf( "writing status byte\n" );
    __outb(0x60, status_byte );

    // writing 0xF6 to the mouse controller sets the default values
    mouse_write( 0xF6 );
    // writing 0xF4 to the mouse controller starts data reporting
    mouse_write( 0xF4 );
}
