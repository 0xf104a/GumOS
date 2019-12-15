#ifndef KMODULE_H
#define KMODULE_H

#include "kernel.h"

typedef struct{
    char *name;
    void (*shared_call)(void *param);
    void (*init)(void);
}kmodule_t;

void kmodule_register(kmodule_t *module);
void init_modules(void *xParam);

#endif