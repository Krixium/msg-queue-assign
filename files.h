#ifndef FILES_H
#define FILES_H


FILE * open_file(const char * filename, const char * flags);
int close_file(const FILE * fp);


#endif