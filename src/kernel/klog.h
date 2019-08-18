#ifndef KLOG_H
#define KLOG_H

#include <stdarg.h>

typedef enum{
    DEBUG,INFO,WARN,ERROR,FATAL
} msg_type;

void klog(msg_type tp,const char *mod_name,const char* format, ...);

#endif