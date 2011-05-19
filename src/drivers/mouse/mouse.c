/**
 * File: mouse.c
 * Author: Benjamin David Mayes <bdm8233@rit.edu>
 * Description: A simple mouse driver. Sadly it's a bit more coupled with the
 * operating system than I would like.
 */

#include <x86arch.h>
#include "../kernel/support.h"
#include "../kernel/startup.h"
#include "../kernel/c_io.h"
#include "../kernel/screen.h"
#include "mouse.h"

/**
 * An ISR for a PS/2 Mouse
 *
 * @param vector    the vector that triggered this interrupt (unused)
 * @param code      the code associated with this interrupt (unused)
 */
static void _isr_mouse( int vector, int code ) {
    // let's be loud and brag about how awesome the ISR is.    
    uint8_t packet = __inb( MOUSE_CMD_PORT );

    screen_input_buffer_t* cq = &(_screens[active_screen].mouse_buf); 
    *(cq->next_space) = packet;
    if( ++(cq->next_space) >= cq->input_buffer + BUFSIZE ) {
        cq->next_space = cq->input_buffer;
    }

    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
    __outb( PIC_SLAVE_CMD_PORT, PIC_EOI );
}

/**
 * Obtains the next mouse packet from the current process' screen's mouse queue.
 *
 * WARNING: This function call blocks.
 *
 * @return The next mouse packet.
 */
uint8_t get_mouse() {
    screen_input_buffer_t* cq = &(_screens[_current->screen].mouse_buf); 
    while( cq->next_char == cq->next_space );
    uint8_t packet = *cq->next_char;
    if( ++(cq->next_char) >= cq->input_buffer + BUFSIZE ) {
        cq->next_char = cq->input_buffer;
    }
    return packet;
}

/**
 * Clears the mouse's input buffer.
 */
void clear_mouse() {
    screen_input_buffer_t* cq = &(_screens[_current->screen].mouse_buf); 
    cq->next_char = cq->next_space;
}

/**
 * Obtains the number of mouse packets in the input buffer
 *
 * @return the number of input packets in the input buffer
 */
int size_mouse() {
    screen_input_buffer_t* cq = &(_screens[_current->screen].mouse_buf); 
    int offset = cq->next_char - cq->next_space;
    return offset >= 0 ? offset : BUFSIZE + offset;
}


/**
 * Obtains the x offset of the mouse movement as a 32 bit signed integer.
 *
 * @return The x offset of the current mouse packet.
 */
int32_t get_x_offset( uint8_t packet1, uint8_t packetpos ) {
    // was there an overflow? commonly if there is the packet is 
    // considered bad and ignored   
    // we check bit 6 in p1 to see if there is an x overflow
    int overflow = (packet1 & 0x40);
    if( overflow == 0 ) {
        // is our offset negative? if so we will want to sign extend the byte
        // we check bit 4 in p1 to see if the x offset is negative
        //int32_t sign = packetpos; 
        if( packet1 & 0x10 ) {
            return 0xFFFFFF00 | packetpos;
        } else {
            return packetpos;
        }
    }
    return 0;
}

/**
 * Obtains the y offset of the mouse movement as a 32 bit signed integer.
 *
 * @return The y offset of the current mouse packet.
 */
int32_t get_y_offset( uint8_t packet1, uint8_t packetpos ) {
    // was there an overflow? commonly if there is the packet is 
    // considered bad and ignored   
    // we check bit 7 in p1 to see if there is an x overflow
    int overflow = (packet1 & 0x80);
    if( overflow == 0 ) {
        // is our offset negative? if so we will want to sign extend the byte
        // we check bit 5 in p1 to see if the x offset is negative
        int32_t sign; 
        if( packet1 & 0x20 ) {
            return 0xFFFFFF00 | packetpos;
        } else {
            return packetpos;
        }
    }
    return 0;
}

/**
 * Obtains the z(scroll) offset of the mouse movement as a 32 bit signed 
 * integer.
 *
 * @return the z offset of the current mouse packet.
 */
int32_t get_z_offset( uint8_t packet1, uint8_t packetpos ) {
    if( packetpos == 0xFF ) {
        return -1;
    } else if( packetpos ){
        return 1;
    } else {
        return 0;
    }
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
    
    // we want to read the command register, this is done by writing 
    // CMD_READ_ENABLE to port MOUSE_DATA_PORT then reading from port 
    // MOUSE_CMD_PORT
    mouse_wait(WAIT_WRITE);
    __outb( MOUSE_DATA_PORT, CMD_READ_ENABLE );
    mouse_wait(WAIT_READ);
    uint8_t status_byte = __inb(MOUSE_CMD_PORT);

    // we want to enable mouse IRQ 12, this is done by setting bit 1 in the 
    // command register
    status_byte |= 2;

    // to write to the command register we write MOUSE_CMD_PORT to port 
    // MOUSE_DATA_PORT then write the new command register to 0x60
    mouse_wait(WAIT_WRITE);
    __outb(MOUSE_DATA_PORT, CMD_WRITE_ENABLE);
    mouse_wait(WAIT_WRITE);
    __outb(MOUSE_CMD_PORT, status_byte );

    mouse_write(0xF3);
    mouse_write(200);
    mouse_write(0xF3);
    mouse_write(100);
    mouse_write(0xF3);
    mouse_write(80);

    // writing 0xF6 to the mouse controller sets the default values
    mouse_write( MOUSE_CMD_DEFAULTS );
    // writing 0xF4 to the mouse controller starts data reporting
    mouse_write( MOUSE_CMD_EN_DATA_REPORTING );
}
