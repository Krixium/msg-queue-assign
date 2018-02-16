#include "msgq.h"

pthread_mutex_t mutex;

int open_queue(const key_t keyval)
{
    int qid;

    pthread_mutex_lock(&mutex);
	qid = msgget(keyval, IPC_CREAT | 0660);
    pthread_mutex_unlock(&mutex);

    return qid;
}


int send_message(const int msg_qid, struct msgbuf * qbuf)
{
    int result;
    int length;

    length = sizeof(struct msgbuf) - sizeof(long);

    pthread_mutex_lock(&mutex);
	result = msgsnd(msg_qid, qbuf, length, 0);
    pthread_mutex_unlock(&mutex);

    return result;
}


int read_message(const int qid, const long type, struct msgbuf * qbuf)
{
    int result;
    int length;

    length = sizeof(struct msgbuf) - sizeof(long);

	pthread_mutex_lock(&mutex);
    result = msgrcv(qid, qbuf, length, type, 0);
	pthread_mutex_unlock(&mutex);

    return result;
}


int remove_queue(const int qid)
{
	int result;

	pthread_mutex_lock(&mutex);
    result = msgctl(qid, IPC_RMID, 0);
	pthread_mutex_unlock(&mutex);

    return result;
}
