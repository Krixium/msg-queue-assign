/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         clnt.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int clnt(const int qid, const int priority, const char * filename)
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
-- INTERFACE:           int clnt (const int qid, const int priority, const char * filename)
--                          const int qid: The message queue id of the server.
--                          const int priority: The requested priority.
--                          const char * filename: The name of the requested file.
--
-- RETURNS:             The exit code.
--
-- NOTES:
-- This is the main entry point for the client. The client performs the following:
--
-- 1) Collect the requsted priority, filename, and pid of itself and sends it to the server.
-- 2) Listen for a response from the server. If the response is an error then the client exits.
-- 3) Listen for messages as long as the incoming messages are full message and display them to stdout.
--      Note: The client only listens for messages where the type is its pid.
----------------------------------------------------------------------------------------------------------------------*/
int clnt(const int qid, const int priority, const char * filename)
{
    int pid;
    struct msgbuf mBuffer;

    pid = (int)getpid();
    memset(&mBuffer, 0, sizeof(struct msgbuf));

    // Place the filename and child PID into buffer
    mBuffer.mtype = C_TO_S;
    sprintf(mBuffer.mtext, "%d/%d\t%s", pid, priority, filename);
    mBuffer.mlen = strlen(mBuffer.mtext);

    // Send the buffer
    if (send_message(qid, &mBuffer) == -1)
    {
        perror("Problem writing to the message queue");
        return 1;
    }

    memset(&mBuffer, 0, sizeof(struct msgbuf));
    // Check if server returns with error message
    if (read_message_blocking(qid, pid, &mBuffer) == -1)
    {
        perror("Reading from message queue failed");
        return 1;
    }

    // Check if the server returned an error opening the file
    if (!strcmp(mBuffer.mtext, "Error: Could not open file"))
    {
        // If it did, print the error and return
        printf("%s\n", mBuffer.mtext);
        fflush(stdout);
        return 0;
    }
    else
    {
        // Otherwise, print the first part of the file
        printf("%s", mBuffer.mtext);
        fflush(stdout);
        if (mBuffer.mlen < MSGSIZE)
        {
            return 0;
        }
    }

    // If the message is not full that means it is the last one
    while (read_message_blocking(qid, pid, &mBuffer) == MSGSIZE)
    {
        if (mBuffer.mlen > 1)
        {
            printf("%s", mBuffer.mtext);
            fflush(stdout);
        }
    }

    printf("%s", mBuffer.mtext);
    fflush(stdout);

    return 0;
}