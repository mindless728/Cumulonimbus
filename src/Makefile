SUB_DIRS=drivers include services userland

make: BuildImage.o
	cd $(SUB_DIRS); make
	gcc -o OS.b $(SUB_DIRS)/*.o
	cd kernel; make
	./BuildImage -d usb -o usb.image -b kernel/bootstrap.b OS.b

BuildImage.o: BuildImage.c
	gcc -o BuildImage.o BuildImage.c

