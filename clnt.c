#include "clnt.h"


int clnt(const int qid, const int priority, const char * filename)
{
    int pid;
    struct msgbuf mBuffer;

    pid = (int)getpid();
    memset(&mBuffer, 0, sizeof(struct msgbuf));

    // Place the filename and child PID into buffer
    mBuffer.mtype = C_TO_S;
    sprintf(mBuffer.mtext, "%d/%d/%s", pid, priority, filename);
    mBuffer.mlen = strlen(mBuffer.mtext);

    // Send the buffer
    send_message(qid, &mBuffer);

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
        return 0;
    }
    else
    {
        // Otherwise, print the first part of the file
        printf("%s", mBuffer.mtext);
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