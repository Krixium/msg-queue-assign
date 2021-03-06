/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         msgq.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      int open_queue(const key_t keyval);
--                      int send_message(const int msg_qid, msgbug * qbuf);
--                      int read_message(const int qid, const ong type, msgbuf * qbuf);
--                      int read_message_blocking(const int qid, const long type, msgbuf * qbuf)
--                      int remove_queue(const int qid)
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
-- This file containts all the wrapper functions for interacting with the message queue.
----------------------------------------------------------------------------------------------------------------------*/
#include "msgq.h"

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            open_queue
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int open_queue (const key_t keyval)
--                          const key_t keyval: The key to open the message queue with.
--
-- RETURNS:             -1 if the queue failed to open, otherwise a positive integer.
--
-- NOTES:
-- Opens a new message queue.
----------------------------------------------------------------------------------------------------------------------*/
int open_queue(const key_t keyval)
{
    int qid;

	qid = msgget(keyval, IPC_CREAT | 0660);

    return qid;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            send_message
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int send_message (const int msg_qid, struct msgbuf * qbuf)
--                          const int msg_qid: The id of the message queue.
--                          strcut msgbuf * qbuf: A pointer the the message struct to use.
--
-- RETURNS:             The number of bytes written to the message queue, -1 if writing failed.
--
-- NOTES:
-- Writes a message to the message queue with a blocking call.
----------------------------------------------------------------------------------------------------------------------*/
int send_message(const int msg_qid, struct msgbuf * qbuf)
{
    int result;

	result = msgsnd(msg_qid, qbuf, qbuf->mlen, 0);

    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            read_message
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int read_message (const int qid, const long type, struct msgbuf * qbuf)
--                          const int qid: The id of the message queue.
--                          const long type: The message type.
--                          struct msgbuf * qbuf: A pointer to where the message will be stored.
--
-- RETURNS:             The number of bytes read from the message queue, -1 if reading failed.
--
-- NOTES:
-- Reads a message from the message queue with a non-blocking call.
----------------------------------------------------------------------------------------------------------------------*/
int read_message(const int qid, const long type, struct msgbuf * qbuf)
{
    int result;

    result = msgrcv(qid, qbuf, MSGSIZE, type, IPC_NOWAIT);
    qbuf->mlen = result;

    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            read_message_blocking
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int read_message_blocking (const int qid, const long type, struct msgbuf * qbuf)
--                          const int qid: The id of the message queue.
--                          const long type: The message type.
--                          struct msgbuf * qbuf: A pointer to where the message will be stored.
--
-- RETURNS:             The number of bytes read from the message queue, -1 if reading failed.
--
-- NOTES:
-- Reads a message from the message queue with a blocking call.
----------------------------------------------------------------------------------------------------------------------*/
int read_message_blocking(const int qid, const long type, struct msgbuf * qbuf)
{
    int result;

    result = msgrcv(qid, qbuf, MSGSIZE, type, 0);
    qbuf->mlen = result;

    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            remove_queue
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           int remove_queue (const int qid)
--                          const int qid: The qid of the message queue.
--
-- RETURNS:             -1 if removing failed, a positive integer otherwise.
--
-- NOTES:
-- Removes a message queue.
----------------------------------------------------------------------------------------------------------------------*/
int remove_queue(const int qid)
{
	int result;

    result = msgctl(qid, IPC_RMID, 0);

    return result;
}
