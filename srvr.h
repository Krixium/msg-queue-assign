#ifndef SRVR_H
#define SRVR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "files.h"
#include "msgq.h"
#include "threads.h"

struct queue
{
    int size;
    struct client_data * q;
};

struct client_data
{
    int pid;
    FILE * file;
};

struct thread_params
{
    int * pRunning;
    int qid;
    struct queue * pClientQueue;
};

int srvr(const int qid);

void * control_thread(void * params);

void splitFilenameAndPID(const char * message, char * filename, int * pid);

void acceptClients(int qid, struct queue * pClientQueue);
int addClientToQueue(struct queue * pq, int pid, FILE * file);
int removeClientFromQueue(struct queue * pq, int pid);
int clearQueue(struct queue * pq);


#endif