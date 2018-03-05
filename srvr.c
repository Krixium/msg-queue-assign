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
    int j;
    int res;
    int pleaseRemove;
    struct msgbuf sendBuffer;
    struct client_data * pc;

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

    pleaseRemove = 0;
    while (running)
    {
        acceptClients(qid, &clntQueue);

        for (i = 0; i < clntQueue.size; i++)
        {
            pc = &clntQueue.q[i];

            if (pc->finished)
            {
                continue;
            }

            sendBuffer.mtype = pc->pid;

            for (j = 0; j < pc->priority; j++)
            {
                res = read_file(pc->file, &sendBuffer);
                if (res >= 0)
                {
                    if (res == 0)
                    {
                        printf("server> client %d is finished, flagging\n", pc->pid);
                        fflush(stdout);
                        pc->finished = 1;
                        pleaseRemove = 1;
                        break;
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
                    pc->finished = 1;
                    pleaseRemove = 1;
                    break;
                }
            }
        }

        if (pleaseRemove)
        {
            removeFinishedClients(&clntQueue);
            pleaseRemove = 0;
        }
    }

    clearQueue(&clntQueue);
    return 0;
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
                if (!strcmp(command, "quit") || !strcmp(command, "stop") 
                || !strcmp(command, "q") || !strcmp(command, "s"))
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


void acceptClients(int qid, struct queue * pq)
{
    int tmp;
    struct msgbuf buffer;
    char filename[MSGSIZE];
    int pid;
    int priority;
    FILE * fp;

    memset(&buffer, 0, sizeof(struct msgbuf));

    // If a new client is found...
    if (read_message(qid, C_TO_S, &buffer) > 0)
    {
        printf("server> New request: [%s]\n", buffer.mtext);

        // Grab the filename and pid
        parseClientRequest(buffer.mtext, &pid, &priority, filename);
        fp = open_file(filename, "r");

        // If the file couldn't be oppened
        if (fp == NULL)
        {
            // Respond with an error
            buffer.mtype = pid;
            strcpy(buffer.mtext, "Error: Could not open file");
            buffer.mlen = 27;

            perror("Could not open file");

            if (send_message(qid, &buffer) == -1)
            {
                perror("Problem sending error message to client");
                return;
            }
        }
        else
        {
            tmp = pq->size;
            if (addClientToQueue(pq, pid, priority, fp) == tmp)
            {
                perror("Realloc error");
                return;
            }
            printf("server> New client %d added\n", pid);
            fflush(stdout);
        }
    }
}


void parseClientRequest(const char * message, int * pid, int * priority, char * filename)
{
    int i;
    char tmp[MSGSIZE];
    char * fileStart = NULL;
    char * pidStart = NULL;
    char * priorityStart = NULL;

    strcpy(tmp, message);
    pidStart = tmp;

    for (i = 0; tmp[i]; i++)
    {
        if (tmp[i] == '/')
        {
            tmp[i] = '\0';
            priorityStart = tmp + i + 1;
        }

        if (tmp[i] == '\t')
        {
            tmp[i] = '\0';
            fileStart = tmp + i + 1;
        }
    }

    *pid = atoi(pidStart);
    *priority = atoi(priorityStart);
    memcpy(filename, fileStart, strlen(fileStart));
}


int addClientToQueue(struct queue * pq, int pid, int priority, FILE * file)
{
    struct client_data client;
    struct client_data * oldQueue;

    client.pid = pid;
    client.finished = 0;
    client.priority = priority;
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


int removeFinishedClients(struct queue * pq)
{
    int i;
    int j;
    int x;

    x = 0;
    pthread_mutex_lock(&mutex);
    for (i = pq->size; i > 0; i--)
    {
        if (pq->q[i - 1].finished)
        {
            close_file_unsafe(&(pq->q[i - 1].file));

            for (j = i - 1; j < pq->size - 1; j++)   
            {
                pq->q[j] = pq->q[j + 1];
            }

            x++;
        }
    }
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
