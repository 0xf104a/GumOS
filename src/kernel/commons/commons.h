#ifndef COMMONS_H
#define COMMONS_H

#include <stdint.h>

char* clone_s(const char* _str);
char * dtostr(double num);
char *timestr(void);
uint64_t utime(void);

#endif
