#include "msgq.h"

int open_queue(key_t keyval)
{
    int qid;

    pthread_mutex_lock(&mutex);
    if ((qid = msgget(keyval, IPC_CREAT | 0660)) == -1)
    {
        return -1;
    }
    pthread_mutex_unlock(&mutex);

    return qid;
}


int send_message(int msg_qid, struct msgbuf * qbuf)
{
    int result;
    int length;

    length = sizeof(struct msgbuf) - sizeof(long):

    pthread_mutex_lock(&mutex);
    if ((result = msgsnd(msg_qid, qbuf, length, 0)) == -1)
    {
        return -1;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}


int read_message(int qid, long type, struct msgbuf * qbuf)
{
    int result;
    int length;

    length = sizeof(struct msgbuf) - sizeof(long);

    if ((result = msgrcv(qid, qbuf, length, type, 0)) == -1)
    {
        return -1;
    }

    return result;
}


int remove_queue(int qid)
{
    if (msgctl(qid, IPC_RMID, 0) == -1)
    {
        return -1;
    }

    return 0;
}