#ifndef CLNT_H
#define CLNT_H

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "msgq.h"
#include "threads.h"

struct params {
    int * pRunning;
    int qid;
    int pid;
    int priority;
};

int clnt(const int qid, const int priority);

void * client_control(void * params);

#endif