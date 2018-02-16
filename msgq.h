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

int open_queue(const key_t keyval);
int send_message(const int msg_qid, struct msgbuf *qbuf);
int read_message(const int qid, const long type, struct msgbuf *qbuf);
int remove_queue(const int qid);

#endif
