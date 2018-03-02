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

        fprintf(stdout, "Use './assign2 [high|normal|low] %d [filename]' to make a request to this server\n", qid);
        fflush(stdout);

        if (srvr(qid) != 0)
        {
            remove_queue(qid);
            perror("Error with server");
            exit(1);
        }

        if (remove_queue(qid) == -1)
        {
            perror("Problem with closing the queue");
            exit(1);
        }
    }
    // Client
    else
    {
        int priority = 2;
        char * p;

        if (argc != 4)
        {
            printUsage();
            return 0;
        }

        // grab qid of server
        qid = atoi(argv[2]);

        // grab priority
        p = argv[1];
        if (!strcmp(p, "high"))
        {
            priority = HIGH;
        }
        else if (!strcmp(p, "normal"))
        {
            priority = NORMAL;
        }
        else if (!strcmp(p, "low"))
        {
            priority = LOW;
        }
        else
        {
            printUsage();
            exit(0);
        }
        

        if (clnt(qid, priority, argv[3]) != 0)
        {
            perror("Error with client");
            exit(1);
        }
    }

    return 0;
}


void printUsage()
{
    printf("Usage: ./assign2 [server|[high | normal | low] qid filename]\n");
}