#include "klog.h"
#include "ktask.h"
#include "io.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void klog(msg_type tp,const char *mod_name,const char* format, ...){                                      
    kmutex_lock();
    switch (tp)
    {
    case DEBUG:
        kprint("D/");
        break;
    case INFO:
        kprint("I/");
        break;
    case WARN:
        kprint("W/");
        break;
    case ERROR:
        kprint("E/");
        break;
    case FATAL:
        kprint("F/");
        break;
    default:
        kprint("?/");
        break;
    }
        //Fuck C++ standart!!!!!
    char *_mod_name=(char *)malloc(sizeof(char)*(strlen(mod_name)+1));
    _mod_name=strcpy(_mod_name,mod_name);
    kprint(_mod_name);
    kprint(":");
    /**kprintf**/
    size_t len;
    char *buf; 
    va_list args;                      
    va_start(args, format);                                 
    len=vsnprintf( 0, 0, format, args); 
    buf=(char *)malloc(sizeof(char)*(len+1));  
    vsnprintf(buf, len+1, format, args);             
    va_end(args);
    kprint(buf);
    free(buf);
    /**end:kprintf**/     
    kprint("\n");          
    va_end(args);
    free(_mod_name);
    kmutex_unlock();
}

