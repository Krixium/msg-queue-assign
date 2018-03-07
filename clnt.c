/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         clnt.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int clnt(const int qid, const int priority, const char * filename)
--                      void * client_control(void * params)
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
-- This file contains all the code for the client.
----------------------------------------------------------------------------------------------------------------------*/
#include "clnt.h"

pthread_mutex_t mutex;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            clnt
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int clnt (const int qid, const int priority)
--                          const int qid: The message queue id of the server.
--                          const int priority: The requested priority.
--
-- RETURNS:             The exit code.
--
-- NOTES:
-- Creates a new thread for handling stdin and then spins while the program is running listening for messages.
----------------------------------------------------------------------------------------------------------------------*/
int clnt(const int qid, const int priority)
{
    pthread_t controlThread;
    struct params p;
    int pid;
    int running = 1;
    struct msgbuf mBuffer;

    pid = (int)getpid();

    // Start thread to check if program should stop running
    p.pRunning = &running;
    p.qid = qid;
    p.pid = pid;
    p.priority = priority;

    if (pthread_create(&controlThread, NULL, client_control, (void *)&p))
    {
        perror("Could not start thread");
        return 1;
    }

    // If the message is not full that means it is the last one
    while (running)
    {
        memset(&mBuffer, 0, sizeof(struct msgbuf));
        if (read_message(qid, pid, &mBuffer) <= 0)
        {
            sched_yield();
            continue;
        }

        // Check if the server returned an error opening the file
        if (!strcmp(mBuffer.mtext, "Error: Could not open file"))
        {
            // If it did, print the error and return
            printf("%s\n", mBuffer.mtext);
            fflush(stdout);
        }
        else
        {
            // Otherwise, print the first part of the file
            printf("%s", mBuffer.mtext);
            fflush(stdout);
        }
    }
    
    // Print the last message
    printf("%s", mBuffer.mtext);
    fflush(stdout);

    pthread_join(controlThread, 0);
    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            client_control
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int vlient_control(void * params)
--                          void * params: A pointer to the threading parameters.
--
-- RETURNS:             Not used.
--
-- NOTES:
-- Spins while listening for user input on stdin. If any of the quit commands are read in, the thread stops the process.
-- Otherwise, the thread assues it is a file name and makes a request to the server.
----------------------------------------------------------------------------------------------------------------------*/
void * client_control(void * params)
{
    char line[MSGSIZE];
    char command[MSGSIZE];

    struct params * p = (void *)params;
    int * pRunning = p->pRunning;
    int priority = p->priority;
    int pid = p->pid;
    int qid = p->qid;

    struct msgbuf buffer;

    while (*pRunning)
    {
        if (fgets(line, MSGSIZE, stdin))
        {
            if (sscanf(line, "%s", command) == 1)
            {
                // If it is the quit command
                if (!strcmp(command, "quit") || !strcmp(command, "stop") 
                || !strcmp(command, "q") || !strcmp(command, "s"))
                {
                    pthread_mutex_lock(&mutex);
                    *pRunning = 0;
                    pthread_mutex_unlock(&mutex);
                }
                else
                {
                    // Place the filename and child PID into buffer
                    memset(&buffer, 0, sizeof(struct msgbuf));
                    buffer.mtype = C_TO_S;
                    sprintf(buffer.mtext, "%d/%d\t%s", pid, priority, command);
                    buffer.mlen = strlen(buffer.mtext);                

                    // Send the buffer
                    if (send_message(qid, &buffer) == -1)
                    {
                        perror("Problem writing to the message queue");
                    }
                }
            }
        }
    }

    return NULL;
}