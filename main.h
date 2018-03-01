#ifndef MAIN_H
#define MAIN_H

#define HIGH    4
#define NORMAL  2
#define LOW     1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "msgq.h"
#include "files.h"

#include "srvr.h"
#include "clnt.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void printUsage();

#endif