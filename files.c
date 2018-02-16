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


int close_file(const FILE * fp)
{
    int result;

    pthread_mutex_lock(&mutex);
    result = fclose(fp);
    pthread_mutex_unlock(&mutex);

    return result;
}