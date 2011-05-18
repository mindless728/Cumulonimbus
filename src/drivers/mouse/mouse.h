/**
 * File: mouse.h
 * Author: Benjamin David Mayes
 * Description: Header file containing #defines used in mouse initialization and
 * a function used for mouse initialization.
 */

#ifndef MOUSE_H_
#define MOUSE_H_
#include <types.h>

// defines used by mouse_wait
#define WAIT_READ 0
#define WAIT_WRITE 1

// ports used for mouse communication
#define MOUSE_CMD_PORT 0x60
#define MOUSE_DATA_PORT 0x64

// commands to write to the command port
#define CMD_READ_ENABLE 0x20
#define CMD_WRITE_ENABLE 0x60
#define CMD_MOUSE_PORT_EN 0xA8


// The following commands must be preceeded with a write of 
// MOUSE_CMD_WRITE_TO_MOUSE to MOUSE_DATA_PORT followed by the write of the
// actual command.
#define MOUSE_CMD_WRITE_TO_MOUSE 0xD4
#define MOUSE_CMD_RESET 0xFF
#define MOUSE_CMD_DEFAULTS 0xF6
#define MOUSE_CMD_EN_DATA_REPORTING 0xF4
#define MOUSE_CMD_ACK 0xFA
#define MOUSE_CMD_RESET_SUCCESS 0xAA

// initialization routine
extern void _mouse_init(void);

// obtains the oldest mouse packet in the queue
uint8_t get_mouse(void);
// clearsthe mouse packet queue
void clear_mouse(void);

// translates two packets into an x offset
int32_t get_x_offset( uint8_t, uint8_t );
// translates two packets into a y offset
int32_t get_y_offset( uint8_t, uint8_t );
// translates two packets into a z offset
int32_t get_z_offset( uint8_t, uint8_t );
#endif
