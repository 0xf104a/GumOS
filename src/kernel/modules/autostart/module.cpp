#include "module.h"

#include <string.h>

#include <kernel/klog.h>
#include <kernel/ktask.h>
#include <kernel/microramfs/microramfs.h>
#include <kernel/io.h>
#include <lua/lexec.h>

void lexec_wrapper(void* xParam){
    liblua_exec_file((char*)xParam);
}

void do_autostart(void){
    char* buffer=NULL;
    char* token;
    sreadf("/etc/launch.list",&buffer);
    while ((token = strsep(&buffer, "\n"))){
        klog(INFO,"autostart","Starting: %s",token);
        liblua_exec_file(token);
    }
    klog(INFO,"autostart","Autostart done");
    free(buffer);
}