CC = gcc
DEBUG= NODEBUG
CFLAGS = -Wall -c -lpthread -D$(DEBUG)
LFLAGS = -Wall -lpthread -D$(DEBUG)
all: CFS-emulator
CFS-emulator: CFS-emulator.o util.o 
	$(CC) $(LFLAGS) CFS-emulator.o util.o -o CFS-emulator
CFS-emulator.o: CFS-emulator.c util.h
	$(CC) $(CFLAGS) CFS-emulator.c
util.o: util.c util.h sched.h
	$(CC) $(CFLAGS) util.c
clean:
	rm -rf *.o
