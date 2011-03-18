#include "x86arch.h"
#include "startup.h"
#include "support.h"
#include "timer_isr.h"
#include "c_io.h"

static unsigned long timer_ticks;

static void _timer_isr( int vector, int code ) {
    ++timer_ticks;
    //c_printf_at( 0,0, "%dms", timer_ticks );
    __outb( PIC_MASTER_CMD_PORT, PIC_EOI );
}

void _init_timer() {
    __install_isr( INT_VEC_TIMER, _timer_isr );
}

unsigned long _get_time() {
    return timer_ticks;
}
