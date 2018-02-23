#ifndef CLNT_H
#define CLNT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "msgq.h"

int clnt(const int qid, const char * filename);

#endif