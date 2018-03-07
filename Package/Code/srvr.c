/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         srvr.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--
--                      int srvr()
--                      void * server_control(void * params)
--                      void catchSig(int sig)
--                      int acceptClients(int * pPid, int * pPriority, char * pFile)
--                      void parseClientRequest(const char * message, int * pid, int * priority, char * filename)
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
-- This file contains all the code for the server.
----------------------------------------------------------------------------------------------------------------------*/
#include "srvr.h"
#include <errno.h>

static int serverQID;
static int globalSID;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            srvr
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int srvr ()
--
-- RETURNS:             The exit code.
--
-- NOTES:
-- The main entry point of the server. The server will:
-- 1) Create the control thread, semaphore and message queue.
-- 2) Check if there is a new client request.
--      If there is this function will fork a new process to serve it
-- 3) Take care of any house keeping when user wants to quit.
--
-- The server will only exit when the user types quit, stop, q, or s.
----------------------------------------------------------------------------------------------------------------------*/
int srvr()
{
    // Threading variable
    pthread_t controlThread;
    int running = 1;

    // Client
    int pid;
    int priority;
    char filename[MSGSIZE];
    FILE * file;

    int i;
    int res;
    int returnCode = 0;
    int pleaseQuit = 0;
    struct msgbuf sendBuffer;

    // Exit signal
    signal(SIGINT, catchSig);

    // Start thread to check if program should stop running
    if (pthread_create(&controlThread, NULL, server_control, (void *)&running))
    {
        perror("Could not start thread");
        return 1;
    }

    // Open queue
    if ((serverQID = open_queue((int)getpid())) == -1)
    {
        perror("Could not open queue");
        return 1;
    }
    else
    {
        fprintf(stdout, "Use './assign2 [high|normal|low] %d' to make a request to this server\n", serverQID);
        fflush(stdout);
    }

    if ((globalSID = create_semaphore((int)getpid())) < 0)
    {
        perror("Could not create semaphore");
        return 1;
    }

    V(globalSID);

    while (running)
    {
        if (!acceptClients(&pid, &priority, filename))
        {
            sched_yield();
            continue;
        }

        // Fork and serve if it is the child
        if (!fork())
        {
            file = fopen(filename, "r");

            if (file == NULL)
            {
                sendBuffer.mtype = pid;
                strcpy(sendBuffer.mtext, "Error: Could not open file");
                sendBuffer.mlen = 27;

                if (send_message(serverQID, &sendBuffer) == -1)
                {
                    perror("Problem sending to client");
                }

                perror("Could not open file");
                return 0;
            }

            printf("%d> child started\n", getpid());
            sendBuffer.mtype = pid;

            while (!feof(file))
            {
                P(globalSID);
                for (i = 0; i < priority; i++)
                {
                    res = read_file(file, &sendBuffer);

                    if (res == 0)
                    {
                        returnCode = 0;
                        pleaseQuit = 1;
                    }

                    if (res < 0)
                    {
                        perror("Problem reading from file");
                        returnCode = pleaseQuit = 1;
                    }

                    if (send_message(serverQID, &sendBuffer) == -1)
                    {
                        perror("Problem sending message");
                        returnCode = pleaseQuit = 1;
                    }
                }
                V(globalSID);

                sched_yield();

                if (pleaseQuit)
                {
                    break;
                }
            }

            printf("%d> child is finished and exiting\n", getpid());
            return returnCode;
        }
    }

    if (remove_semaphore(globalSID) == -1)
    {
        perror("Could not remove semaphore");
    }

    // Close queue when parent exits
    if (remove_queue(serverQID) == -1)
    {
        perror("Problem with closing the queue");
        return(1);
    }

    pthread_join(controlThread, 0);
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            server_control
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void * server_control (void * params)
--                          void * params: The parameters.
--
-- RETURNS:             NULL.
--
-- NOTES:
-- This is the callback for the control thread. When the users types in a command this is the thread that handles the
-- input. Currently only quit, stop, q, and s are supported. They all tell the server to clean up and exit.
----------------------------------------------------------------------------------------------------------------------*/
void * server_control(void * params)
{
    char line[256];
    char command[256];
    int * pRunning = (int *)params;

    while (*pRunning)
    {
        if (fgets(line, 256, stdin))
        {
            if (sscanf(line, "%s", command) == 1)
            {
                if (!strcmp(command, "quit") || !strcmp(command, "stop") 
                || !strcmp(command, "q") || !strcmp(command, "s"))
                {
                    kill(0, SIGINT);
                    *pRunning = 0;
                }
            }
        }

        sched_yield();
    }

    return NULL;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            server_control
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void catchSig (int sig)
--                          int sig: The signal.
--
-- RETURNS:             void.
--
-- NOTES:
-- This function catches the SIGINT signal. Once caught, this function will close the message queue and remove the 
-- semaphore.
----------------------------------------------------------------------------------------------------------------------*/
void catchSig(int sig)
{
    remove_semaphore(globalSID);
    remove_queue(serverQID);
    exit(0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            acceptClients
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int acceptCleints (int * pPid, int * pPriority, char * filename)
--                          int * pPid: A pointer to where to store the client pid.
--                          int * pPriority: A pointer to where to store the cleint priority.
--                          char * filename: A pointer to where to store the requested filename.
--
-- RETURNS:             1 if a client was added, 0 otherwise.
--
-- NOTES:
-- This function will make a blocking read to the message queue to check if there are any new requests from clients.
-- If there is a new request from a new client, this funciton will parse the pid, desired priority, and filename and
-- fill the approriate variables.
----------------------------------------------------------------------------------------------------------------------*/
int acceptClients(int * pPid, int * pPriority, char * filename)
{
    struct msgbuf buffer;

    memset(&buffer, 0, sizeof(struct msgbuf));

    // If a new client is found...
    if (read_message_blocking(serverQID, C_TO_S, &buffer) > 0)
    {
        printf("server> New request: [%s]\n", buffer.mtext);

        // Grab the filename and pid
        memset(filename, 0, MSGSIZE);
        parseClientRequest(buffer.mtext, pPid, pPriority, filename);
        return 1;
    }

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            parseClientRequest
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void parseClientRequest (const char * message, int * pid, int * priority, char * filename)
--                          const char * message: The message sent to the server by the client.
--                          int * pid: Where the parsed process id will be placed.
--                          int * priority: Where the parsed priority value will be placed.
--                          char * filename: Where the pared filename will be placed.
--
-- RETURNS:             void.
--
-- NOTES:
-- Parsed the client's initial request message into its process id, priority and filename.
----------------------------------------------------------------------------------------------------------------------*/
void parseClientRequest(const char * message, int * pid, int * priority, char * filename)
{
    int i;
    char tmp[MSGSIZE];
    char * fileStart = NULL;
    char * pidStart = NULL;
    char * priorityStart = NULL;

    strcpy(tmp, message);
    pidStart = tmp;

    for (i = 0; tmp[i]; i++)
    {
        if (tmp[i] == '/')
        {
            tmp[i] = '\0';
            priorityStart = tmp + i + 1;
        }

        if (tmp[i] == '\t')
        {
            tmp[i] = '\0';
            fileStart = tmp + i + 1;
        }
    }

    *pid = atoi(pidStart);
    *priority = atoi(priorityStart);
    memcpy(filename, fileStart, strlen(fileStart));
}