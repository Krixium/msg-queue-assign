#include "srvr.h"


int srvr(const int qid)
{
    struct queue q;

    int childPID;
    FILE * file;
    char filename[MSGSIZE];
    struct msgbuf mBuffer;

    memset(&q, 0, sizeof(struct queue));
    q.size = 0;
    q.q = (struct client_data *)malloc(sizeof(struct client_data *));

    memset(&mBuffer, 0, sizeof(struct msgbuf));

    // This is for testing, creates a delay so client has time to post filename
    sleep(2);

    // Grab the PID of child to use as mtype and file to read
    if (read_message_blocking(qid, C_TO_S, &mBuffer) == -1)
    {
        return 1;
    }

    // Grab the pid of the child and the name of the desired file to be opened
    splitFilenameAndPID(mBuffer.mtext, filename, &childPID);

    // Add the first client to the queue
    if (!addClientToQueue(&q, childPID, file))
    {
        return 2;
    }

    // Spin up a thread to start sending to all clients in the queue

    // On this thread, listen for more connections



    // When exiting free the queue structure
    sleep(2);
    return 0;
}

void splitFilenameAndPID(const char * message, char * filename, int * pid)
{
    int i;
    const char * tmp;

    for (i = 0; message[i]; i++)
    {
        if (message[i] == '/')
        {
            break;
        }
    }

    tmp = message + i + 1;
    *pid = atoi(tmp);

    memcpy(filename, message, i);
    *(filename + i) = '\0';
}


int addClientToQueue(struct queue * q, int pid, FILE * file)
{
    int result;
    struct client_data client;
    struct client_data * oldQueue;

    client.pid = pid;
    client.file = file;

    q->size++;

    oldQueue = q->q;
    q->q = (struct client_data *)realloc(q, sizeof(struct client_data) * q->size);
    if (q->q == NULL)
    {
        q->q = oldQueue;
        return 0;
    }

    q->q[q->size - 1] = client;

    return 1;
}


int removeClientFromQueue(struct queue * q, int pid)
{
    int i;
    for (i = 0; i < q->size; i++)
    {
        free(q->q);
    }
    return 1;
}
