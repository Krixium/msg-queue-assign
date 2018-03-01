#include "srvr.h"

pthread_mutex_t mutex;

int srvr(const int qid)
{
    // queue to hold all the clients
    struct queue clntQueue;

    // Threading variable
    pthread_t controlThread;
    int running = 1;

    int i;
    int res;
    struct msgbuf sendBuffer;

    // Init the client queue
    memset(&clntQueue, 0, sizeof(struct queue));
    clntQueue.size = 0;
    clntQueue.q = (struct client_data *)malloc(0);

    // Start thread to check if program should stop running
    if (pthread_create(&controlThread, NULL, control_thread, (void *)&running))
    {
        perror("Could not start thread");
        return 1;
    }

    while (running)
    {
        acceptClients(qid, &clntQueue);

        for (i = 0; i < clntQueue.size; i++)
        {
            sendBuffer.mtype = clntQueue.q[i].pid;
            res = read_file(clntQueue.q[i].file, &sendBuffer);

            if (res >= 0)
            {
                if (res == 0)
                {
                    printf("server> client %d is finished, removing\n", clntQueue.q[i].pid);
                    removeClientFromQueue(&clntQueue, clntQueue.q[i].pid);
                }

                if (send_message(qid, &sendBuffer) == -1)
                {
                    perror("Problem sending message");
                    running = 0;
                    break;
                }
            }
            else
            {
                perror("Problem reading from file");
                removeClientFromQueue(&clntQueue, clntQueue.q[i].pid);
            }
        }
        // only for testing completion speed
        // usleep(2000);
    }

    clearQueue(&clntQueue);
    return 0;
}


void acceptClients(int qid, struct queue * pClientQueue)
{
    struct msgbuf buffer;
    char filename[MSGSIZE];
    FILE * fp;
    int pid;

    memset(&buffer, 0, sizeof(struct msgbuf));

    // If a new client is found...
    if (read_message(qid, C_TO_S, &buffer) > 0)
    {
        printf("server> New request: [%s]\n", buffer.mtext);

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
                return;
            }
        }
        else
        {
            if (addClientToQueue(pClientQueue, pid, fp) == -1)
            {
                perror("Realloc error");
                return;
            }
            printf("server> New client %d added\n", pid);
            fflush(stdout);
        }
    }

    return;    
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
                    pthread_mutex_lock(&mutex);
                    *pRunning = 0;
                    pthread_mutex_unlock(&mutex);
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

    printf("server> Removing all instances of %d from queue\n", pid);

    x = 0;
    for (i = pq->size; i > 0; i--)
    {
        if (pq->q[i - 1].pid == pid)
        {
            close_file(&(pq->q[i - 1].file));

            pthread_mutex_lock(&mutex);
            for (j = i - 1; j < pq->size - 1; j++)   
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


int clearQueue(struct queue * pq)
{
    int i;
    for (i = 0; i < pq->size; i++)
    {
        close_file(&(pq->q[i].file));
    }
    free(pq->q);
    return 1;
}
