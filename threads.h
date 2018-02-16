#ifndef THREADS_H
#define THREADS_H

#define _REENTRANT
#define DCE_COMPAT

#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#endif