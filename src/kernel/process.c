/*
** SCCS ID:	@(#)process.c	1.1	03/31/11
**
** File:	process.c
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Process-related routines
*/

#define	__KERNEL__20103__

#include "headers.h"
#include "utils.h"
#include "process.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

static queue_t _pcb_avail;

/*
** PUBLIC GLOBAL VARIABLES
*/

// this must be available to allow traversals in exit() and wait()

pcb_t _pcbs[ N_PCBS ];		// all PCBs in the system
pcb_t *_current;		// the currently executing process
pid_t _next_pid;		// next available PID

// process queues (ready queue is in the scheduler module)

queue_t _sleep;		// processes who called sleep()
queue_t _reading;	// processes waiting for input
queue_t _waiting;	// processes waiting for children
queue_t _zombie;	// terminated processes

/*
** PRIVATE FUNCTIONS
*/

/*
** PUBLIC FUNCTIONS
*/

void _pid_clear(pid_t* p){
	memset(p, 0x0, sizeof(pid_t));
}

void _pid_cpy(pid_t* dest, pid_t* src){
	memcpy(dest, src, sizeof(pid_t));
}

int _pid_cmp(pid_t* p1, pid_t* p2){
	int ret = memcmp(&p1->host, &p2->host, sizeof(mac_address_t));
	if(ret == 0){
		ret = memcmp(&p1->id, &p2->id, sizeof(uint32_t));
	}
	return ret;
}

status_t _pid_next(pid_t* pid){

	memcpy((void*) &pid->host, (void*) &_local_mac, sizeof(mac_address_t));
	pid->id = _next_pid.id++;

	return E_SUCCESS;
}


/*
** status = _pcb_alloc( &pcb )
**
** allocate a PCB and place a pointer to it into 'pcb'
**
** returns the allocation status
*/

status_t _pcb_alloc( pcb_t **pcb ) {
	return( _q_remove( &_pcb_avail, (void **) pcb ) );
}

/*
** status = _pcb_free( pcb )
**
** return 'pcb' to the free PCB list
**
** returns the deallocation status
*/

status_t _pcb_free( pcb_t *pcb ) {
	key_t key;
	if( pcb != NULL ) {
		key.u = 0;
		_memclr( (void *)pcb, sizeof(pcb_t) );
		return( _q_append( &_pcb_avail, (void *) pcb, key ) );
	}
	return( E_BAD_PARAM );
}

/*
** _pcb_find(pid)
**
** locate the PCB for the specified PID
**
** returns a pointer to the PCB, or NULL
*/

pcb_t *_pcb_find( pid_t pid ) {
	pcb_t *pcb;
	int i;

	for( i = 0, pcb = &_pcbs[0]; i < N_PCBS; ++i, ++pcb ) {
		if( pcb->state != FREE && _pid_cmp(&pcb->pid, &pid) == 0 ) {
			return( pcb );
		}
	}

	return( NULL );

}

/*
** _pcb_init()
**
** initialize the process module
*/

void _pcb_init( void ) {
	status_t stat;
	key_t key;
	int i;

	memset(&_next_pid, 0x0, sizeof(pid_t));
	memcpy(&_next_pid.host, &_local_mac, sizeof(mac_address_t));
	_next_pid.id = PID_INIT;

	_q_reset( &_pcb_avail, NULL, NULL );
	_q_reset( &_sleep, _compare_uint_asc, _remove_if_le_u );
	_q_reset( &_reading, _compare_uint_asc, _remove_if_equal );
	_q_reset( &_waiting, _compare_uint_asc, _remove_if_equal );
	_q_reset( &_zombie, _compare_uint_asc, _remove_if_equal );

	key.u = 0;

	for( i = 0; i < N_PCBS; ++i ) {
		stat = _pcb_free( &_pcbs[i] );
		if( stat != E_SUCCESS ) {
			_kpanic( "_pcb_init", "_pcb_free status %s", stat );
		}
	}

	c_puts( " process" );
}
