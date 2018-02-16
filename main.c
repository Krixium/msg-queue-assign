#include "main.h"


int main(int argc, char * argv[])
{
	int mkey;
	int qid;

	// Check arguements
	if (argc != 2) 
	{
		printf("%s\n", "Usage: ./assign2 [server|client]");
		exit(1);
	}

	// Set message queue key based on PID
	mkey = (int)getpid();
	if ((qid = open_queue(mkey)) == -1)
	{
		perror("Could not open queue");
		exit(1);
	}

	// Start mode
	if (!strcmp("server", argv[1]))
	{
		if (srvr())
		{
			perror("Error with server");
		}
	}
	else if (!strcmp("client", argv[1]))
	{
		if (clnt())
		{
			perror("Error with client");
		}
	}
	else
	{
		printf("%s\n", "Usage: ./assign2 [server|client]");
	}

	// Close queue
	if (remove_queue(qid) == -1)
	{
		perror("Could not close queue");
	}

	exit(0);
}
