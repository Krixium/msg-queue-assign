/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         main.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int main(int argc, char * argv[])
--                      void printUsage()
--
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- NOTES:
-- This is the main entry point for both the client and the server. 
-- The server will spin until the "quit" command is given. While the server is spinning, it will listen for requests
-- on the message queue it is listen on and if a new request is found the server will try to open the file and read the
-- file's contents to the message queue. If the file could not be openned, an error message is written instead. The
-- server is also responsible for openning and closing the message queue.
--
-- The client, when started, will request a file from the server. After this request is made the client will read all
-- messages from the message queue that are for it. Once it reads a message that is not full it is assumed that is the
-- last message and will exit. The client is able to request priorty from the server.
--
-- Usage:
-- ./assign2 server - Start the server
-- ./assign2 [high | normal | low] [qid] [filename] - Ask the server with message queue id [qid] to serve [filename]
--                                                    with  [high | normal | low] priority.
----------------------------------------------------------------------------------------------------------------------*/
#include "main.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            main
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int main (int argc, char * argv[])
--                          int argc: Number of command line arguements.
--                          char * argv[]: Array of command line arguements.               
--
-- RETURNS:             The exit code.
--
-- NOTES:
-- The main entry point of the program. Main will parse the command line arguements and start the client or server.
-- If server is specified, this function will also create the queue.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char * argv[])
{
    int qid;

    if (argc < 2)
    {
        printUsage();
    }

    // Server
    if (!strcmp(argv[1], "server"))
    {
        return srvr();
    }
    // Client
    else
    {
        int priority = 2;
        char * p;

        if (argc != 3)
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

        if (clnt(qid, priority) != 0)
        {
            perror("Error with client");
            exit(1);
        }
    }

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            printUsage
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int printUsage ()
--
-- RETURNS:             void.
--
-- NOTES:
-- Prints how to run the program to stdout.
----------------------------------------------------------------------------------------------------------------------*/

void printUsage()
{
    printf("Usage: ./assign2 [server|[high | normal | low] qid]\n");
}