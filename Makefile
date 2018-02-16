CC=gcc
CFLAGS=-Wall -ggdb

assign2: main.o msgq.o
	$(CC) $(CFLAGS) main.o msgq.o threads.h -o assign2

clean:
	rm -f *.o *.bak assign2

main.o:
	$(CC) $(CFLAGS) -O -c main.c

msgq.o:
	$(CC) $(CFLAGS) -O -c msgq.c
