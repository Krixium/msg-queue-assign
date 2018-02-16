#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msgq.h"
#include "files.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char * argv[])
{
    int mkey;
    int id;
    struct msgbuf msg;
    struct msgbuf rcv;
	char * filename;
	FILE * inFile;
	FILE * outFile;

    if (argc != 4)
    {
        fprintf(stderr, "Usage: showmsg keyval inFilename outFilename\n");
    }

    mkey = (key_t) atoi(argv[1]);
	
	// Open files
	filename = argv[2];
	if (!(inFile = open_file(filename, "r")))
	{
		exit(5);
	}
	filename = argv[3];
	if (!(outFile = open_file(filename, "w+")))
	{
		close_file(inFile);
		exit(6);
	}
    
    // Init structures
    memset(&msg, 0, sizeof(struct msgbuf));
    memset(&rcv, 0, sizeof(struct msgbuf));
    msg.mtype = 1;

	// Read from file
	read_file(inFile, &msg);

    // Open message queue
    id = open_queue(mkey);
    if (id == -1)
    {
		close_file(inFile);
		close_file(outFile);
        exit(1);
    }

	// Write from queue
	if (send_message(id, &msg) == -1)
	{
		remove_queue(id);
		close_file(inFile);
		close_file(outFile);
		exit(2);
	}

	// Read to queue
	if (read_message(id, msg.mtype, &rcv) == -1)
	{
		remove_queue(id);
		close_file(inFile);
		close_file(outFile);
		exit(3);
	}

	// Write to file
	write_file(&rcv, outFile);

    // Close message queue
    if (remove_queue(id) == -1)
    {
		close_file(inFile);
		close_file(outFile);
        exit(4);
    }

	close_file(inFile);
	close_file(outFile);
    exit(0);
}
