#include "sema.h"

int create_semaphore(key_t key)
{
    int sid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    return sid;
}

int remove_semaphore(int sid)
{
    return semctl(sid, 0, IPC_RMID, 0);
}

void P(int sid)
{
    struct sembuf p;

    p.sem_num = 0;
    p.sem_op = -1;
    p.sem_flg = SEM_UNDO;

    if (semop(sid, &p, 1) == -1)
    {
        perror("semop P error");
    }
}

void V(int sid)
{
    struct sembuf v;

    v.sem_num = 0;
    v.sem_op = 1;
    v.sem_flg = SEM_UNDO;

    if (semop(sid, &v, 1) == -1)
    {
        perror("semop V error");
    }   
}