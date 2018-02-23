#include "srvr.h"

pthread_mutex_t mutex;

int srvr(const int qid)
{
    // queue to hold all the clients
    struct queue clntQueue;

    // Threading variable
    pthread_t acceptThread;
    int running = 1;
    struct thread_params params;

    int i;
    struct msgbuf sendBuffer;

    // Init the client queue
    memset(&clntQueue, 0, sizeof(struct queue));
    clntQueue.size = 0;
    clntQueue.q = (struct client_data *)malloc(0);

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
        for (i = 0; i < clntQueue.size; i++)
        {
            sendBuffer.mtype = clntQueue.q[i].pid;
            if (read_file(clntQueue.q[i].file, &sendBuffer) < 1)
            {
                removeClientFromQueue(&clntQueue, clntQueue.q[i].pid);
                continue;
            }

            if (send_message(qid, &sendBuffer) == -1)
            {
                running = 0;
                break;
            }
        }
    }

    running = 0;
    // When exiting free the queue structure
    sleep(1);
    return 0;
}


// Potentially not working
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
        int oldSize;

        // If a new client is found...
        if (read_message(qid, C_TO_S, &buffer) > 0)
        {
            // Grab the filename and pid
            splitFilenameAndPID(buffer.mtext, filename, &pid);
            fp = open_file(filename, "r");

            // If the file couldn't be oppened
            if (fp == NULL)
            {
                // Respond with an error
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
                oldSize = pClientQueue->size;
                if (addClientToQueue(pClientQueue, pid, fp) == oldSize)
                {
                    perror("Could not add client to queue");
                }
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


int addClientToQueue(struct queue * pq, int pid, FILE * file)
{
    struct client_data client;

    client.pid = pid;
    client.file = file;

    pthread_mutex_lock(&mutex);
    pq->size++;
    pq->q = (struct client_data *)realloc(pq->q, pq->size * sizeof(struct client_data));
    pq->q[pq->size - 1] = client;
    pthread_mutex_unlock(&mutex);

    return pq->size;
}


int removeClientFromQueue(struct queue * pq, int pid)
{
    int i;
    int j;
    int x;

    x = 0;
    for (i = pq->size; i > 0; i--)
    {
        if (pq->q[i - 1].pid == pid)
        {
            /*
            close_file(pq->q[i - 1].file);
            */

            for (j = i; j < pq->size - 2; j++)   
            {
                pq->q[j] = pq->q[j + 1];
            }

            x++;
        }
    }

    pthread_mutex_lock(&mutex);
    pq->size -= x;
    pq->q = (struct client_data *)realloc(pq->q, sizeof(struct client_data) * pq->size);
    pthread_mutex_unlock(&mutex);

    return x;
}


int clearQueue(struct queue * q)
{
    int i;
    for (i = 0; i < q->size; i++)
    {
        close_file(q->q[i].file);
    }
    free(q->q);
    return 1;
}
