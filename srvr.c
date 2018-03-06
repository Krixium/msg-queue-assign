/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         main.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int srvr(const int qid)
--                      void * control_thread(void * params)
--                      void acceptClients(const int qid, struct queue * pClientQueue)
--                      void parseClientRequest(const char * message, int * pid, int * priority, char * filename)
--                      int addClientToQueue(struct queue * pq, const int pid, const int priotiy, FILE * file)
--                      int removeFinishedClients(struct queue * pq)
--                      void clearQueue(struct queue * pq)
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

pthread_mutex_t mutex;

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
-- INTERFACE:           int srvr (const int qid)
--                          const int qid: The message queue id of the server.
--
-- RETURNS:             The exit code.
--
-- NOTES:
-- The main entry point of the server. The server will:
-- 1) Create the control thread.
-- 2) Check if there are new clients
-- 3) Serve any existing clients
-- 4) Remove any old clients
--
-- The server will only exit when the user types quit, stop, q, or s.
----------------------------------------------------------------------------------------------------------------------*/
int srvr(const int qid)
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

    // Start thread to check if program should stop running
    if (pthread_create(&controlThread, NULL, control_thread, (void *)&running))
    {
        perror("Could not start thread");
        return 1;
    }

    while (running)
    {
        if (!acceptClients(qid, &pid, &priority, filename))
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
                perror("Could not open file");
                return 0;
            }

            printf("%d> child started\n", getpid());
            sendBuffer.mtype = pid;

            while (!feof(file))
            {
                // lock semaphore here
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

                    if (send_message(qid, &sendBuffer) == -1)
                    {
                        perror("Problem sending message");
                        returnCode = pleaseQuit = 1;
                    }
                    usleep(5000);
                }
                // unlock semaphore here

                if (pleaseQuit)
                {
                    break;
                }
            }

            printf("%d> child is finished and exiting\n", getpid());
            return returnCode;
        }
    }

    if (remove_queue(qid) == -1)
    {
        perror("Problem with closing the queue");
        return(1);
    }

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            control_thread
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void * control_thread (void * params)
--                          void * params: The parameters.
--
-- RETURNS:             NULL.
--
-- NOTES:
-- This is the callback for the control thread. When the users types in a command this is the thread that handles the
-- input. Currently only quit, stop, q, and s are supported. They all tell the server to clean up and exit.
----------------------------------------------------------------------------------------------------------------------*/
void * control_thread(void * params)
{
    // TODO: SEND KILL SIGNAL TO ALL CHILDREN
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
                    pthread_mutex_lock(&mutex);
                    *pRunning = 0;
                    pthread_mutex_unlock(&mutex);
                }
            }
        }

        sched_yield();
    }

    kill(0, SIGINT);
    return NULL;
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
-- INTERFACE:           int acceptCleints (const int qid, int * pPid, int * pPriority, FILE * pFile)
--                          const int qid: The id of the message queue.
--                          int * pPid: A pointer to where to store the client pid.
--                          int * pPriority: A pointer to where to store the cleint priority.
--                          FILE * pFile: A pointer to where to store the requested file.
--
-- RETURNS:             1 if a client was added, 0 otherwise.
--
-- NOTES:
-- TODO: Fill out documentation.
----------------------------------------------------------------------------------------------------------------------*/
int acceptClients(const int qid, int * pPid, int * pPriority, char * filename)
{
    struct msgbuf buffer;

    memset(&buffer, 0, sizeof(struct msgbuf));

    // If a new client is found...
    if (read_message(qid, C_TO_S, &buffer) > 0)
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