#ifndef KTASK_H
#define KTASK_H

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

typedef struct
{
    uint8_t pid;
    void (*execute)(void* xParameter);
    const char *name;
    TaskHandle_t handle;
    int priority;
    void *xParam;
} ktask;

ktask *ktask_start(const char *name, void (*execute)(void* xParameter),void *xParameter,int priority);
int ktask_kill(int pid);
void kmutex_lock(void);
void kmutex_unlock(void);
int kGetPid(void);
void kill_all(void);

#endif