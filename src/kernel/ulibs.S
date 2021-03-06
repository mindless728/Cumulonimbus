/*
** sccs id:	@(#)ulibs.S	1.1	03/31/11
**
** File:	ulibs.S
**
** Author:	4003-506 class of 20103
**
** Contributor:
**
** Description:	Assembler implementations of user-level library routines
**
** These are library routines written in assembly language either
** for speed, or because they must do things that can't be done in C.
*/

#define	__ASM__20103__

#include "syscall.h"

/*
** System call stubs
**
** All have the same structure:
**
**      move a code into EAX
**      generate the interrupt
**      return to the caller
**
** As these are simple "leaf" routines, we don't use
** the standard enter/leave method to set up a stack
** frame - that takes time, and we don't really need it.
*/

#define SYSCALL(name)			\
	.globl	name			; \
name:					; \
	movl    $SYS_##name, %eax       ; \
	int     $INT_VEC_SYSCALL        ; \
	ret

/*
** for debugging purposes, this could be changed to
** (e.g., for write) something like this:
**
**      .globl  write
**      .globl  c_printf
** write:       movl    $SYS_write, %eax
**      int     $INT_VEC_SYSCALL
**      pushl   $fmt
**      call    c_printf
**      addl    $4, %esp
**      ret
** fmt: .string "write() is returning to %08x after printing %08x\n"
*/

SYSCALL(exit)
SYSCALL(sleep)
SYSCALL(fork)
SYSCALL(exec)
SYSCALL(wait)
SYSCALL(readc)
SYSCALL(writec)
SYSCALL(reads)
SYSCALL(writes)
SYSCALL(kill)
SYSCALL(getpid)
SYSCALL(getppid)
SYSCALL(getprio)
SYSCALL(gettime)
SYSCALL(setprio)
SYSCALL(settime)
SYSCALL(yield)
SYSCALL(getscreen)
SYSCALL(setscreen)
SYSCALL(switchscreen)
SYSCALL(openscreen)
SYSCALL(closescreen)
SYSCALL(ide_read)
SYSCALL(ide_write)
SYSCALL(fat64_user_init)
SYSCALL(sendmsg)

/*
** This is a bogus system call; it's here so that we can test
** our handling of out-of-range syscall codes in the syscall ISR.
*/

SYSCALL(bogus)

/*
** get_ebp - return current contents of EBP at the time of the call
*/

	.globl	get_ebp
get_ebp:
	movl	%ebp, %eax
	ret
