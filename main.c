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
	FILE * file;

    if (argc != 3)
    {
        fprintf(stderr, "Usage: showmsg keyval filename\n");
    }

    mkey = (key_t) atoi(argv[1]);
	filename = argv[2];

	// Open file
	if (!(file = open_file(filename, "r")))
	{
		exit(5);
	}
    
    // Init structures
    memset(&msg, 0, sizeof(struct msgbuf));
    memset(&rcv, 0, sizeof(struct msgbuf));

    msg.mtype = 1;
	fgets(msg.mtext, MSGSIZE, file);

    // Open message queue
    id = open_queue(mkey);
    if (id == -1)
    {
        exit(1);
    }

	// Write
	if (send_message(id, &msg) == -1)
	{
		remove_queue(id);
		close_file(file);
		exit(2);
	}

	// Read
	if (read_message(id, msg.mtype, &rcv) == -1)
	{
		remove_queue(id);
		close_file(file);
		exit(3);
	}

	fprintf(stdout, "Message: %s\n", rcv.mtext);

    // Close message queue
    if (remove_queue(id) == -1)
    {
		close_file(file);
        exit(4);
    }

	close_file(file);
    exit(0);
}
