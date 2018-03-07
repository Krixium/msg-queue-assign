#ifndef SEMA_H
#define SEMA_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>


int create_semaphore(key_t key);
int remove_semaphore(int sid);

void P(int sid);
void V(int sid);

#endif