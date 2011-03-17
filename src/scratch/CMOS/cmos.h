//compiled using information from wiki.osdev.org/CMOS

#ifndef CMOS_H_
#define CMOS_H_
// CMOS I/O Ports
#define CMOS_REG_SELECT_PORT 0x70
#define CMOS_IO_PORT 0x71

//---- Floppy Drive information ----
//each 4 bits of the byte read from the following register
//provides information about the two floppy drives attached
// with bits 0..3 being the slave and 4..7 being the master
/* The following block was taken straight from the page:
   Value   Drive Type
    00h    no drive
    01h   360 KB 5.25 Drive
    02h  1.2 MB 5.25 Drive
    03h 720 KB 3.5 Drive
    04h    1.44 MB 3.5 Drive
    05h   2.88 MB 3.5 drive
*/
#define CMOS_FLOPPY_INFO_REGISTER 0x10

// Now the useful stuff: System Time
// When reading from seconds, verify that register A is not 0x80
#define CMOS_REG_SECONDS 0x0
#define CMOS_REG_MINUTES 0x2
#define CMOS_REG_HOURS 0x4
#define CMOS_REG_WEEKDAY 0x6   //Sunday is 1, Saturday is 7
#define CMOS_REG_MONTHDAY 0x7
#define CMOS_REG_MONTH 0x8
#define CMOS_REG_YEAR 0x9
#define CMOS_REG_CENTURY 0x32
//NOTE: In 12 hour time format bit 0x80 is set if PM, probably should be masked off.

#define CMOS_REG_STATUS_A 0xA
//These shouldn't be played with, doubt Carithers would be too happy
// the following register allows you to change the format
#define CMOS_REG_STATUS_B 0xB
// Format modes
#define CMOS_RSB_24_HOUR = 0x2
#define CMOS_RSB_BINARY_MODE = 0x4

// sample function
extern uint32_t seconds_since_midnight(void);
#endif
