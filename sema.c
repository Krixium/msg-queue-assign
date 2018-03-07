/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         sema.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int create_semaphore(key_t key)
--                      int remove_semaphore(int sid)
--                      void P(int sid)
--                      void V(int sid)
--
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- NOTES:
-- This file contains all the code for using semaphores.
----------------------------------------------------------------------------------------------------------------------*/
#include "sema.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            create_semaphore
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int create_semaphore (key_t key)
--                          key_t key: The key for the semaphore.
--
-- RETURNS:             Non-negative integer that is the semaphore id if a semaphore was created, otherwise -1.
--
-- NOTES:
-- This is a wrapper function for creating a semaphore.
----------------------------------------------------------------------------------------------------------------------*/
int create_semaphore(key_t key)
{
    int sid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    return sid;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            remove_semaphore
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int remove_semaphore (int sid)
--                          int sid: The id of the semaphore.
--
-- RETURNS:             Non-negative integer if a semaphore was removed, otherwise -1.
--
-- NOTES:
-- This is a wrapper function for removing a semaphore.
----------------------------------------------------------------------------------------------------------------------*/
int remove_semaphore(int sid)
{
    return semctl(sid, 0, IPC_RMID, 0);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            P
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void P (int sid)
--                          int sid: The id of the semaphore.
--
-- RETURNS:             void.
--
-- NOTES:
-- This is a wrapper function for decrementing the semaphore.
----------------------------------------------------------------------------------------------------------------------*/
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

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            V
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           void V (int sid)
--                          int sid: The id of the semaphore.
--
-- RETURNS:             void.
--
-- NOTES:
-- This is a wrapper function for incrementing the semaphore.
----------------------------------------------------------------------------------------------------------------------*/
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