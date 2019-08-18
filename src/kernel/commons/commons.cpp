#include "commons.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * dtostr(double num){
    char output[64];
    snprintf(output, 64, "%f", num);
    char *_r=(char *)malloc(sizeof(char)*(strlen(output)+1));
    strcpy(_r,output);
    return _r;
}

