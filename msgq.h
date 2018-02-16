#ifndef MSGQ_H
#define MSGQ_H

#define MSGSIZE 128

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "threads.h"

struct msgbuf {
    long mtype;
    char mtext[MSGSIZE];
} msg;

int open_queue(key_t keyval);
int send_message(int msg_qid, struct msgbuf *qbuf);
int read_message(int qid, long type, struct msgbuf *qbuf);
int remove_queue(int qid);

#endif
