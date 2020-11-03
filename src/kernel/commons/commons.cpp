#include "commons.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <kernel/kernel.h>
#include <kernel/io.h>



char* clone_s(const char* _str){
    /*
      Clones constant string
     */
    char* str = (char*)malloc(sizeof(char)*strlen(_str));
    return str;
}
char* dtostr(double num){
    char output[64];
    snprintf(output, 64, "%f", num);
    char *_r=(char *)malloc(sizeof(char)*(strlen(output)+1));
    strcpy(_r,output);
    return _r;
}

char* timestr(void){
    char *time=(char*)malloc(sizeof(char)*7);
    uint64_t *_rtime=NULL;
    kassert(readf("/dev/utime",(uint8_t**)(&_rtime)));
    uint64_t rtime=*_rtime;
    int hh=(rtime/3600)%24;
    int mm=(rtime/60)%60;
    snprintf(time,6*sizeof(char),"%02d:%02d",hh,mm);
    free(_rtime);
    return time;
}

uint64_t utime(void){
    uint64_t *_rtime=NULL;
    kassert(readf("/dev/utime",(uint8_t**)(&_rtime)));
    uint64_t rtime=*_rtime;
    free(_rtime);
    return rtime;
}

