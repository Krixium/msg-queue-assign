#include "clnt.h"


int clnt(const int qid)
{
    int pid;
    int result;
    struct msgbuf mBuffer;
    char filename[MSGSIZE];

    pid = (int)getpid();
    memset(&mBuffer, 0, sizeof(struct msgbuf));
    memset(filename, 0, MSGSIZE);

    // Create atomic functions for getting user input to replace this
    sprintf(filename, "%s %d", "test.in", pid);

    // Set the message buffer
    mBuffer.mlen = strlen(filename);
    mBuffer.mtype = (long)1;
    strcpy(mBuffer.mtext, filename);

    // Send filename and PID to server
    if (send_message(qid, &mBuffer) == -1)
    {
        perror("Could not write to message queue");
        return 1;
    }   

    // This is for testing
    // Wait so that the server can write something
    // Replace with a semaphore
    sleep(2);

    memset(&mBuffer, 0, sizeof(struct msgbuf));
    while ((result = read_message(qid, pid, &mBuffer) > 0))
    {
        printf("%s", mBuffer.mtext);
    }
    printf("\n");
    fflush(stdout);

    return 0;
}