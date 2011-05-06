SUB_DIRS = ide pci mouse fat64
DRIVER_OBJS = ide/ide.o pci/pci.o mouse/mouse.o fat64/fat64.o

all: drivers.a drivers.nl


drivers.a:
	for d in $(SUB_DIRS); do \
	echo "make in $$d with flags [$(MAKEFLAGS)]..."; \
	($(MAKE) -C $$d); done
	ar rcs $@ ${DRIVER_OBJS} 

drivers.nl:
	nm -Bng drivers.a > drivers.nl

clean:
	@rm -f *.o
	@rm -f *.a
	@rm -f *.nl
	@for d in $(SUB_DIRS); do ($(ECHO) Cleaning $$d; cd $$d; $(MAKE) $(MFLAGS) clean); done

