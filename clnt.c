#include "clnt.h"


int clnt(const int qid)
{
    int pid;
    struct msgbuf mBuffer;

    pid = (int)getpid();
    memset(&mBuffer, 0, sizeof(struct msgbuf));

    // Get filename here somehow

    // Place the filename and child PID into buffer
    mBuffer.mtype = C_TO_S;
    sprintf(mBuffer.mtext, "%s/%d", "test.in", pid);
    mBuffer.mlen = strlen(mBuffer.mtext);

    printf("Starting child: %d\n", pid);

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
    if (!strcmp(mBuffer.mtext, "Could not open file"))
    {
        // If it did, print the error and return
        printf("%s\n", mBuffer.mtext);
        return 0;
    }
    else
    {
        // Otherwise, print the first part of the file
        printf("[PID: %d]%s\n", pid, mBuffer.mtext);
    }

    // BUG: loop does not exit
    // If the message is not full that means it is the last one
    while (read_message_blocking(qid, pid, &mBuffer) == MSGSIZE)
    {
        printf("[PID: %d]%s\n", pid, mBuffer.mtext);
    }

    return 0;
}