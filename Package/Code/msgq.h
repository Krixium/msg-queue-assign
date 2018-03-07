#ifndef MSGQ_H
#define MSGQ_H

#define S_TO_C 1
#define C_TO_S 1

#define MSGHEADERSIZE (sizeof(int) + sizeof(long))
#define MSGSIZE (4096 - MSGHEADERSIZE)

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "threads.h"

struct msgbuf {
    long mtype;
    char mtext[MSGSIZE];
    int mlen;
} msg;

int open_queue(const key_t keyval);
int send_message(const int msg_qid, struct msgbuf *qbuf);
int read_message(const int qid, const long type, struct msgbuf *qbuf);
int read_message_blocking(const int qid, const long type, struct msgbuf *qbuf);
int remove_queue(const int qid);

#endif
