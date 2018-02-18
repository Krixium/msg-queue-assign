#include "main.h"


int main(int argc, char * argv[])
{
	int i;
	int childCount;
	int pid;
	int qid;

	// Check arguements
	if (argc != 2) 
	{
		printf("%s\n", "Usage: ./assign2 number_of_clients");
		exit(1);
	}

	// Set message queue key based on PID
	pid = (int)getpid();
	if ((qid = open_queue(pid)) == -1)
	{
		perror("Could not open queue");
		exit(1);
	}

	// Create children in fan pattern
	childCount = atoi(argv[1]);
	for (i = 0; i < childCount; i++)
	{
		if (!(pid = fork()))
		{
			clnt(qid);
			break;
		}
	}

	// If this is the parent
	if (pid)
	{
		if (srvr(qid))
		{
			perror("Error with server");
		}

		if (remove_queue(qid) == -1)
		{
			perror("Could not close queue");
		}
	}

	exit(0);
}
