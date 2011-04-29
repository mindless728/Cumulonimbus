SUB_DIRS=ide
DRIVER_OBJS = ide/ide.o


all: drivers.a


drivers.a: ${DRIVER_OBJS}
	echo Building $(DRIVER_OBJS)
	for d in $(SUB_DIRS); do ($(MAKE) $(MFLAGS) -C $$d); done
	ar q $@ ${DRIVER_OBJS}

clean:
	rm -f ${DRIVER_OBJS}
	rm -f drivers.a
	-for d in $(SUB_DIRS); do ($(MAKE) $(MFLAGS) -C $$d clean); done
	
