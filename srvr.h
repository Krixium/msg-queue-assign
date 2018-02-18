#ifndef SRVR_H
#define SRVR_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "files.h"
#include "msgq.h"

int srvr(const int qid);

#endif