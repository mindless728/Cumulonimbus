#include "c_io.h"
#include "x86arch.h"
#include "support.h"
#include "startup.h"
#include "types.h"
#include "mouse.h"

/**
 * An ISR for a PS/2 Mouse
 *
 * @param vector    the vector that triggered this interrupt (unused)
 * @param code      the code associated with this interrupt (unused)
 */
static void _isr_mouse( int vector, int code ) {
    // let's be loud and brag about how awesome the ISR is.    
    c_printf( "MOUSE: 0x%02x\n", __inb( MOUSE_CMD_PORT )  );
    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
    __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
}


/**
 * Waits for either the output buffer of the mouse/keyboard controller to be 
 * full or the input buffer to be empty depending on the argument.
 * 
 * @param type  Waits on the controller's output buffer if WAIT_READ, waits
 * on the controller's input buffer if WAIT_WRITE.
 */
static void mouse_wait( uint8_t type) {
    switch( type ) {
        case WAIT_READ:
            while( ((__inb(MOUSE_DATA_PORT) & 1)!=1) );
            break;
        case WAIT_WRITE:
            while( ((__inb(MOUSE_DATA_PORT) & 2)!=0) );
            break;
        default:
            break;
    }
}

/**
 * Writes the given byte to the mouse.
 *
 * @param write_byte    The byte to write
 */
static void mouse_write( uint8_t write_byte ) {
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_DATA_PORT, MOUSE_CMD_WRITE_TO_MOUSE );
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_CMD_PORT, write_byte );
    uint8_t ch;

    // read an ACK from the mouse
    do {
        mouse_wait(WAIT_READ);
        ch = __inb( MOUSE_CMD_PORT );
    } while( ch != MOUSE_CMD_ACK );
}

/**
 * Reads a byte from the mouse.
 *
 * @return The byte read from the mouse.
 */
static uint8_t mouse_read(void) {
    mouse_wait(WAIT_READ);
    return __inb( MOUSE_CMD_PORT );
}

// _mouse_init
void _mouse_init() {
    c_printf( "Installing Mouse ISR\n" );
    __install_isr( INT_VEC_MOUSE, _isr_mouse );
    // Enable the mouse port
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_DATA_PORT, CMD_MOUSE_PORT_EN );
    
    // reset the mouse
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_DATA_PORT, MOUSE_CMD_WRITE_TO_MOUSE );
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_CMD_PORT, MOUSE_CMD_RESET );

    //Wait for reset completion
    uint8_t ch;
    do {
        mouse_wait(WAIT_READ);
        ch = __inb(MOUSE_CMD_PORT);
    } while( ch != MOUSE_CMD_RESET_SUCCESS ); 

    
    // we want to read the command register, this is done by writing CMD_READ_ENABLE to port MOUSE_DATA_PORT then reading from port MOUSE_CMD_PORT
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_DATA_PORT, CMD_READ_ENABLE );
    mouse_wait(WAIT_READ);
    uint8_t status_byte = __inb(MOUSE_CMD_PORT);

    // we want to enable mouse IRQ 12, this is done by setting bit 1 in the command register
    status_byte |= 2;

    // to write to the command register we write MOUSE_CMD_PORT to port MOUSE_DATA_PORT then write the new command register to 0x60
    mouse_wait(WAIT_WRITE);
    __outb(MOUSE_DATA_PORT, CMD_WRITE_ENABLE);
    mouse_wait(WAIT_WRITE);
    __outb(MOUSE_CMD_PORT, status_byte );

    // writing 0xF6 to the mouse controller sets the default values
    mouse_write( MOUSE_CMD_DEFAULTS );
    // writing 0xF4 to the mouse controller starts data reporting
    mouse_write( MOUSE_CMD_EN_DATA_REPORTING );
}
