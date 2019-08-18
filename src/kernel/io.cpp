#include "io.h"
#include "kconfig.h"
#include "klog.h"
#include "ktask.h"
#include "kernel.h"



#include <stdlib.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kernel/microramfs/microramfs.h>

io *io_state;

#include <M5Stack.h>

void io_init(void){
    io_state=(io *)malloc(sizeof(io));
    io_state->io_output_streams=array_create(1);
}
void io_register_stream(const char *name,void (*on_out)(char *str)){
    io_out_stream *stream=(io_out_stream *)malloc(sizeof(io_out_stream));
    stream->on_out=on_out;
    stream->name=(char *)malloc(sizeof(char)*strlen(name));
    strcpy(stream->name,name);
    array_add(io_state->io_output_streams,stream);
}
void kprint(char *s){
    int i=0;
    io_out_stream *stream;
    for(;i<io_state->io_output_streams->sz;++i){
        stream=(io_out_stream *)io_state->io_output_streams->base[i];
        stream->on_out(s);
    }
}
void kprintf(const char* format, ...){
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
}

void unregister_io_handler(int id){
    klog(INFO,"io","Undregistering handler %d",id);
    kmutex_lock();
    array_del(io_state->io_output_streams,id);
    kmutex_unlock();
}

uint64_t readf(char *path, uint8_t **buf){
    kassert(khandle->ramfs!=NULL);
    uint64_t sz=0;
    kmutex_lock();
    read_file(khandle->ramfs,path,buf,&sz);
    kmutex_unlock();
    return sz;
}

int writef(char *path,uint8_t *buf, size_t sz){
    kmutex_lock();
    int stat= write_file(khandle->ramfs,path,buf,sz);
    kmutex_unlock();
    return stat;
}
