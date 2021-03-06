#ifndef SRVR_H
#define SRVR_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "files.h"
#include "msgq.h"
#include "threads.h"
#include "sema.h"


int srvr();

void * server_control(void * params);
void catchSig(int sig);

int acceptClients(int * pPid, int * pPriority, char * pFile);
void parseClientRequest(const char * message, int * pid, int * priority, char * filename);

#endif