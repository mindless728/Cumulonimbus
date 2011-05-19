all: userland.o

userland.o: shell.o
	cp shell.o userland.o

shell.o: shell.c
	gcc -c shell.c $(CFLAGS)

clean:
	@rm -f *.o
