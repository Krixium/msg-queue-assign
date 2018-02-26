#include "srvr.h"

pthread_mutex_t mutex;

int srvr(const int qid)
{
    // queue to hold all the clients
    struct queue clntQueue;

    // Threading variable
    pthread_t controlThread;
    pthread_t acceptThread;
    int running = 1;
    struct thread_params params;

    int i;
    int res;
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

    // Start thread to check if program should stop running
    if (pthread_create(&controlThread, NULL, control_thread, (void *)&running))
    {
        perror("Could not start thread");
        return 1;
    }

    // Startng the accept clients thread
    if (pthread_create(&acceptThread, NULL, accept_clients, (void *)&params))
    {
        perror("Could not start thread");
        return 1;
    }

    while (running)
    {
        for (i = 0; i < clntQueue.size; i++)
        {
            sendBuffer.mtype = clntQueue.q[i].pid;
            res = read_file(clntQueue.q[i].file, &sendBuffer);

            if (res >= 0)
            {
                if (res == 0)
                {
                    removeClientFromQueue(&clntQueue, clntQueue.q[i].pid);
                }

                if (send_message(qid, &sendBuffer) == -1)
                {
                    perror("Problem sending message");
                    running = 0;
                    break;
                }
                usleep(5000);
            }
            else
            {
                perror("Problem reading from file");
                removeClientFromQueue(&clntQueue, clntQueue.q[i].pid);
            }
        }
    }

    return 0;
}


// Does not respond to all clients once the number of clients reaches around 7
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
        // If a new client is found...
        if (read_message(qid, C_TO_S, &buffer) > 0)
        {
            printf("New request: [%s]\n", buffer.mtext);

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
                if (addClientToQueue(pClientQueue, pid, fp) == -1)
                {
                    perror("Realloc error");
                    return NULL;
                }
                fprintf(stdout, "New client %d added\n", pid);
                fflush(stdout);
            }
        }
        else
        {
            sched_yield();
        }
    }

    return NULL;    
}


void * control_thread(void * params)
{
    char line[256];
    char command[256];
    int * pRunning = (int *)params;

    while (*pRunning)
    {
        if (fgets(line, 256, stdin))
        {
            if (sscanf(line, "%s", command) == 1)
            {
                if (!strcmp(command, "quit"))
                {
                    *pRunning = 0;
                }
            }
        }

        sched_yield();
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
    struct client_data * oldQueue;

    client.pid = pid;
    client.file = file;

    pthread_mutex_lock(&mutex);
    oldQueue = pq->q;
    pq->q = (struct client_data *)realloc(pq->q, (pq->size + 1) * sizeof(struct client_data));
    if (pq->q == NULL)
    {
        pq->q = oldQueue;
    }
    else
    {
        pq->q[pq->size] = client;
        pq->size++;
    }
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
            close_file(pq->q[i - 1].file);

            pthread_mutex_lock(&mutex);
            for (j = i; j < pq->size - 2; j++)   
            {
                pq->q[j] = pq->q[j + 1];
            }
            pthread_mutex_unlock(&mutex);

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
