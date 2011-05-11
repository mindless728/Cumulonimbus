C_INCLUDE_PATH = $(CURDIR)/include:$(CURDIR)

#
# User compilation/assembly definable options
#
#	ISR_DEBUGGING_CODE	include context restore debugging code
#	CLEAR_BSS_SEGMENT	include code to clear all BSS space
#	SP2_CONFIG		enable SP2-specific startup variations
#	REPORT_MYSTERY_INTS	print a message on interrupt 0x27
#   NO_VESA Disables VESA activation-specific code.
#   DISABLE_FPU disables saving the FPU
#
USER_OPTIONS = -DISR_DEBUGGING_CODE -DSP2_CONFIG -DCLEAR_BSS_SEGMENT

CPP = cpp ${BUILD_TYPE}
CPPFLAGS = $(USER_OPTIONS) -nostdinc ${BUILD_FLAGS}

CC = gcc ${BUILD_TYPE}
CFLAGS = -fno-builtin -fno-stack-protector -march=i686 -m32 -Wall -Wstrict-prototypes $(CPPFLAGS)

AS = gas
ASFLAGS = --32

LD = gld
LDFLAGS = -melf_i386

RM = rm
ECHO = echo

SHELL = /bin/sh

#
#	Export all variables
#

export C_INCLUDE_PATH
export CPP CPPFLAGS AS ASFLAGS LD LDFLAGS CC CFLAGS
export RM ECHO SHELL


#.SILENT:


.SUFFIXES:	.S .b

.c.s:
	$(CC) $(CFLAGS) -S $*.c

.S.s:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S

.S.o:
	$(CPP) $(CPPFLAGS) -o $*.s $*.S
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(RM) -f $*.s

.s.b:
	$(AS) $(ASFLAGS) -o $*.o $*.s -a=$*.lst
	$(LD) $(LDFLAGS) -Ttext 0x0 -s --oformat binary -e begtext -o $*.b $*.o

.c.o:
	$(CC) $(CFLAGS) -c $*.c

