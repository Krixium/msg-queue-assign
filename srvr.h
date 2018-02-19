#ifndef SRVR_H
#define SRVR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "files.h"
#include "msgq.h"

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

int srvr(const int qid);
void splitFilenameAndPID(const char * message, char * filename, int * pid);

int addClientToQueue(struct queue * q, int pid, FILE * file);
int removeClientFromQueue(struct queue * q, int pid);

#endif