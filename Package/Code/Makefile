CC=gcc
CFLAGS=-Wall -ggdb

assign2: main.o msgq.o files.o srvr.o clnt.o sema.o
	$(CC) $(CFLAGS) main.o msgq.o files.o srvr.o clnt.o sema.o threads.h -o assign2 -lpthread

clean:
	rm -f *.o *.bak assign2

main.o:
	$(CC) $(CFLAGS) -O -c main.c

msgq.o:
	$(CC) $(CFLAGS) -O -c msgq.c

files.o:
	$(CC) $(CFLAGS) -O -c files.c

srvr.o:
	$(CC) $(CFLAGS) -O -c srvr.c

clnt.o:
	$(CC) $(CFLAGS) -O -c clnt.c

sema.o:
	$(CC) $(CFLAGS) -O -c sema.c