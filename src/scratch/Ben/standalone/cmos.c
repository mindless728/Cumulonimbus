#include "int_types.h"
#include "cmos.h"
#include "startup.h"

//NOTE: QEMU seems to be operating in BCD mode.
// Is this typical? Does the underlying machine have the same behavior?
// BCD mode means that the hex value of each read value corresponds to the
// amount of time in that value (i.e. 0x20 in seconds means 20 seconds has
// passed since the last minute, NOT 32.
// Also the time displayed is in UTC.
uint32_t seconds_since_midnight() {
    asm( "cli\n" ); //disable interrupts
    uint32_t ret = 0;
    __outb( CMOS_REG_SELECT_PORT, CMOS_REG_STATUS_A ); 
    while( __inb( CMOS_IO_PORT ) == 0x80 );
    __outb( CMOS_REG_SELECT_PORT, CMOS_REG_SECONDS );
    ret |= __inb( CMOS_IO_PORT );
    __outb( CMOS_REG_SELECT_PORT, CMOS_REG_MINUTES );
    ret |= __inb( CMOS_IO_PORT ) << 8;
    __outb( CMOS_REG_SELECT_PORT, CMOS_REG_HOURS );
    ret |= __inb( CMOS_IO_PORT ) << 16;
    asm( "sti\n" ); //reenable interrupts
    return ret;
}
