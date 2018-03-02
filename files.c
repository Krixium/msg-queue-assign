#include "files.h"

pthread_mutex_t mutex;


FILE * open_file(const char * filename, const char * flags)
{
    FILE * fp;

    pthread_mutex_lock(&mutex);
    fp = fopen(filename, flags);
    pthread_mutex_unlock(&mutex);

    return fp;
}


int close_file(FILE ** fp)
{
    int result = -1;

    pthread_mutex_lock(&mutex);
    if (*fp != NULL)
    {
        result = fclose(*fp);
        *fp = NULL;
    }
    pthread_mutex_unlock(&mutex);

    return result;
}


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


size_t read_file(FILE * file, struct msgbuf * msg)
{
    size_t result;

    pthread_mutex_lock(&mutex);
    if (file == NULL)
    {
        result = 0;
    }
    else
    {
        memset(msg->mtext, 0, sizeof(char) * MSGSIZE);
        result = fread(msg->mtext, 1, MSGSIZE - 1, file);
        msg->mtext[result] = '\0';
        msg->mlen = result + 1;
    }
    pthread_mutex_unlock(&mutex);
    return result;
}


int write_file(const struct msgbuf * msg, FILE * file)
{
    int result;

    pthread_mutex_lock(&mutex);
    result = fprintf(file, "%s", msg->mtext);
    pthread_mutex_unlock(&mutex);

    return result;
}
