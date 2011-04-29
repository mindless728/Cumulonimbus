SUB_DIRS = ide pci
DRIVER_OBJS = ide/ide.o pci/pci.o

all: drivers.a


drivers.a:
	for d in $(SUB_DIRS); do \
	echo "make in $$d with flags [$(MAKEFLAGS)]..."; \
	($(MAKE) -C $$d); done
	ar q $@ ${DRIVER_OBJS} 


clean:
	@rm -f *.o
	@rm -f *.a
	@for d in $(SUB_DIRS); do ($(ECHO) Cleaning $$d; cd $$d; $(MAKE) $(MFLAGS) clean); done

