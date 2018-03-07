#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "msgq.h"
#include "threads.h"

FILE * open_file(const char * filename, const char * flags);
int close_file(FILE ** fp);
int close_file_unsafe(FILE ** fp);
size_t read_file(FILE * file, struct msgbuf * msg);

#endif
