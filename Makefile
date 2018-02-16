CC=gcc
CFLAGS=-Wall -ggdb

assign2: msgq.o threads.o
	$(CC) $(CFLAGS) msgq.o threads.o -o assign2

clean:
	rm -f *.o *.bak assign2

msgq.o:
	$(CC) $(CFLAGS) -O -c msgq.c

threads.o:
	$(CC) $(CFLAGS) -O -c threads.h