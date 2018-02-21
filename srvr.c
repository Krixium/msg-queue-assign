#include "srvr.h"


int srvr(const int qid)
{
    // queue to hold all the clients
    struct queue clntQueue;

    // Threading variable
    pthread_t acceptThread;
    int running = 1;
    struct thread_params params;

    int i;
    int sent;
    struct msgbuf sendBuffer;

    // Init the client queue
    memset(&clntQueue, 0, sizeof(struct queue));
    clntQueue.size = 0;
    clntQueue.q = (struct client_data *)malloc(sizeof(struct client_data *));

    // Init the threading variable
    memset(&params, 0, sizeof(struct thread_params));
    params.pRunning = &running;
    params.qid = qid;
    params.pClientQueue = &clntQueue;

    // Startng the accept clients thread
    if (pthread_create(&acceptThread, NULL, accept_clients, (void *)&params))
    {
        return 1;
    }

    sleep(1);

    while (clntQueue.size > 0)
    {
        sent = 0;
        for (i = 0; i < clntQueue.size; i++)
        {
            sendBuffer.mtype = clntQueue.q[i].pid;
            if (read_file(clntQueue.q[i].file, &sendBuffer) < 1)
            {
                // Remove that client from the queue
                continue;
            }

            if (send_message(qid, &sendBuffer) == -1)
            {
                // maybe instead of breaking here, just remove that client
                running = 0;
                break;
            }

            sent++;
        }

        /////////////////// This is here because removing clients from queues does not yet work ///////////////////
        if (sent < clntQueue.size)
        {
            break;
        }
        /////////////////// Remove after ///////////////////
    }

    removeClientFromQueue(&clntQueue, 0);
    running = 0;
    // When exiting free the queue structure
    sleep(2);
    return 0;
}


void * accept_clients(void * params)
{
    struct msgbuf buffer;
    char filename[MSGSIZE];
    FILE * fp;
    int pid;

    struct thread_params * p = (struct thread_params *)params;
    int * pRunning = p->pRunning;
    int qid = p->qid;
    struct queue * pClientQueue = p->pClientQueue;

    memset(&buffer, 0, sizeof(struct msgbuf));

    while (pRunning)
    {
        if (read_message(qid, C_TO_S, &buffer) > 0)
        {
            splitFilenameAndPID(buffer.mtext, filename, &pid);
            fp = open_file(filename, "r");
            if (fp == NULL)
            {
                buffer.mtype = pid;
                strcpy(buffer.mtext, "Error: Could not open file");
                buffer.mlen = 27;
                if (send_message(qid, &buffer) == -1)
                {
                    *pRunning = 0;
                    break;
                }
                sched_yield();
            }
            else
            {
                addClientToQueue(pClientQueue, pid, fp);
            }
        }
        else
        {
            sched_yield();
        }
    }

    return NULL;    
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
    struct client_data client;
    struct client_data * oldQueue;

    client.pid = pid;
    client.file = file;

    q->size++;
    oldQueue = q->q;
    q->q = (struct client_data *)realloc(q->q, sizeof(struct client_data) * q->size);
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
        close_file(q->q[i].file);
    }
    free(q->q);
    return 1;
}
