#ifndef FILES_H
#define FILES_H

#include <stdio.h>

#include "threads.h"

FILE * open_file(const char * filename, const char * flags);
int close_file(FILE * fp);

#endif
