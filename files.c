/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:         files.c - A client server application that utilizes message queues.
--
-- PROGRAM:             assign2
--
-- FUNCTIONS:
--                      FILE * open_file(const char * filename, const char * flags)
--                      int close_file(FILE ** fp)
--                      int close_file_unsafe(FILE ** fp)
--                      size_t read_file(FILE * fp, struct msgbuf * msg)
--                      int write_file(const struct * msg, FILE * file)
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
-- This file contains all the wrapper functions for interacting with files.
----------------------------------------------------------------------------------------------------------------------*/
#include "files.h"

pthread_mutex_t mutex;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            open_file
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           FILE * open_file (const char * filename, const char * flags)
--                          const char * filename: The name of the file to open.
--                          const char * flags: The flags for how the file should open. See fopen().
--
-- RETURNS:             The pointer to the openned file. If the file could not be openned NULL is returned.
--
-- NOTES:
-- Opens a file and gives the pointer to it. This function is thread safe.
----------------------------------------------------------------------------------------------------------------------*/
FILE * open_file(const char * filename, const char * flags)
{
    FILE * fp;

    pthread_mutex_lock(&mutex);
    fp = fopen(filename, flags);
    pthread_mutex_unlock(&mutex);

    return fp;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            close_file
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           FILE * close_file (FILE ** fp)
--                          FILE ** fp: A pointer to the file pointer.
--
-- RETURNS:             Returns 0 if the file is closed, EOF otherwise.
--
-- NOTES:
-- Closes a file and sets its pointer to NULL. This function is thread safe.
----------------------------------------------------------------------------------------------------------------------*/
int close_file(FILE ** fp)
{
    int result = -1;

    pthread_mutex_lock(&mutex);
    if (*fp != NULL)
    {
        result = fclose(*fp);
        if (!result)
        {
            *fp = NULL;
        }
    }
    pthread_mutex_unlock(&mutex);

    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            close_file_unsafe
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           FILE * close_file_unsafe (FILE ** fp)
--                          FILE ** fp: A pointer to the file pointer.
--
-- RETURNS:             Returns 0 if the file is closed, EOF otherwise.
--
-- NOTES:
-- Closes a file and sets its pointer to NULL. This function is not thread safe.
----------------------------------------------------------------------------------------------------------------------*/
int close_file_unsafe(FILE ** fp)
{
    int result = -1;

    if (*fp != NULL)
    {
        result = fclose(*fp);
        *fp = NULL;
    }

    return result;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:            read_file
--
-- DATE:                March 5, 2018
--
-- REVISIONS:           N/A
--
-- DESIGNER:            Benny Wang
--
-- PROGRAMMER:          Benny Wang
--
-- INTERFACE:           size_t read_file (FILE * file, struct msgbuf * msg)
--                          FILE * file: The file to read from.
--                          struct msgbuf * msg: The message to fill.
--
-- RETURNS:             The number of bytes read from the file.
--
-- NOTES:
-- This function will attempt to fill the message with contents from the file. Only the mtext and mlen properties of the
-- message struct is touched by this function. This function is thread safe.
----------------------------------------------------------------------------------------------------------------------*/
size_t read_file(FILE * file, struct msgbuf * msg)
{
    size_t result = 0;

    pthread_mutex_lock(&mutex);
    if (file != NULL)
    {
        memset(msg->mtext, 0, sizeof(char) * MSGSIZE);
        result = fread(msg->mtext, sizeof(char), MSGSIZE - 1, file);
        msg->mtext[result] = '\0';
        msg->mlen = result + 1;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}
