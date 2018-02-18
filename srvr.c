#include "srvr.h"


int srvr(const int qid)
{
    int childPID;
    char filename[MSGSIZE];
    FILE * file;
    struct msgbuf mBuffer;

    memset(&mBuffer, 0, sizeof(struct msgbuf));
    mBuffer.mtype = 1;

    // This is for testing
    sleep(1);

    // Grab the PID of child to use as mtype and file to read
    if (read_message(qid, mBuffer.mtype, &mBuffer) == -1)
    {
        return 1;
    }
    sscanf(mBuffer.mtext, "%s %d", filename, &childPID);

    if (!(file = open_file(filename, "r")))
    {
        mBuffer.mtype = S_TO_C;
        strcpy(mBuffer.mtext, "Could not open file");
        mBuffer.mlen = 20;

        send_message(qid, &mBuffer);

        return 2;
    }

    // Error with server: Resource temporarily unavailable
    memset(&mBuffer, 0, sizeof(struct msgbuf));
    mBuffer.mtype = childPID;
    while (read_file(file, &mBuffer) > 0)
    {
        if (send_message(qid, &mBuffer))
        {
            return 3;
        }
    }

    // Sleep so that queue isnt closed before client can read it
    // Probably replace with a semaphore
    sleep(2);
    return 0;
}