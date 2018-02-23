#include "main.h"


// Usage: ./assign2 [server|client qid]
int main(int argc, char * argv[])
{
    int pid;
    int qid;

    if (argc < 2)
    {
        printUsage();
    }

    // Server
    if (!strcmp(argv[1], "server"))
    {
        pid = (int)getpid();
        if ((qid = open_queue(pid)) == -1)
        {
            perror("Could not open queue");
            exit(1);
        }

        fprintf(stdout, "Use './assign2 client %d [filename]' to make request to this server\n", qid);
        fflush(stdout);

        if (srvr(qid) != 0)
        {
            remove_queue(qid);
            perror("Error with server");
            exit(2);
        }

        if (remove_queue(qid) == -1)
        {
            perror("Problem with closing the queue");
            exit(3);
        }
    }
    // Client
    else if (!strcmp(argv[1], "client"))
    {
        if (argc != 4)
        {
            printUsage();
            return 0;
        }

        qid = atoi(argv[2]);

        if (clnt(qid, argv[3]) != 0)
        {
            perror("Error with client");
            exit(4);
        }
    }
    else
    {
        printUsage();
    }

    return 0;
}


void printUsage()
{
    printf("Usage: ./assign2 [server|client qid filename]\n");
}