all: net.o

net.o: i8255x_ethernet.o
	cp i8255x_ethernet.o net.o

i8255x_ethernet.o: i8255x_ethernet.c
	gcc -c i8255x_ethernet.c $(CFLAGS)

clean:
	@rm -f *.o
