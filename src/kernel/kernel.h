#ifndef KERNEL_H
#define KERNEL_H

#include "types/array.h"
#include "types/hashtable.h"
#include "ktimer.h"
#include "kevent.h"

#include <kernel/microramfs/microramfs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <stdbool.h>

#define kassert(A) _kassert(A,__FILE__,__LINE__)


typedef struct{
    hashtable *modules;
    array_t *tasks;//Tasks
    TaskHandle_t __self;
    SemaphoreHandle_t kSemaphore;
    bool kAlive;
    ktimer *timer;
    microramfs *ramfs;
    bool is_console_mode;
    EventManager *event_mgr;
} kernel;

extern kernel *khandle;

void kstart(void);
void _kassert(int a,const char *fName, int fLine);
void kSetConsoleMode(bool state);
bool kGetConsoleMode(void);

#endif
