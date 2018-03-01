#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include <string.h>

#include "msgq.h"
#include "threads.h"

FILE * open_file(const char * filename, const char * flags);
int close_file(FILE ** fp);
size_t read_file(FILE * file, struct msgbuf * msg);
int write_file(const struct msgbuf * msg, FILE * file);

#endif
