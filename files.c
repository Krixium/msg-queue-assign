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


int close_file(FILE * fp)
{
    int result;

    pthread_mutex_lock(&mutex);
    result = fclose(fp);
    pthread_mutex_unlock(&mutex);

    return result;
}


size_t read_file(FILE * file, struct msgbuf * msg)
{
    size_t result;

    pthread_mutex_lock(&mutex);
    // result = fgets(msg->mtext, msg->mlen, file);
    result = fread(msg->mtext, 1, MSGSIZE, file);
    msg->mlen = result;
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
