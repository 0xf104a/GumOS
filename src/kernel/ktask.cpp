#include "kernel.h"
#include "kconfig.h"
#include "ktask.h"
#include "klog.h"
#include "io.h"
#include "error.h"
#include "types/array.h"


#include <stdlib.h>

void kmutex_lock(void){
    xSemaphoreTakeRecursive(khandle->kSemaphore,portMAX_DELAY);
}

void kmutex_unlock(void){
    xSemaphoreGiveRecursive(khandle->kSemaphore);
}
ktask *ktask_start(const char *name, void (*execute)(void* xParameter),void *xParameter,int priority){
    klog(DEBUG,"ktask","Creating new task.");
    ktask *_new=(ktask *)malloc(sizeof(ktask));
    kmutex_lock();
    array_t *tasklist=khandle->tasks;
    int pid=tasklist->sz;
    _new->pid=pid;
    _new->execute=execute;
    _new->name=name;
    _new->priority=priority;
    array_add(tasklist,_new);
    kmutex_unlock();
    int status=xTaskCreate(_new->execute,name,TASK_STACK_DEPTH,xParameter,priority,&_new->handle);
    kassert(pdPASS==status);
    klog(INFO,"ktask","Started task %s with priority %d and pid %d",_new->name,_new->priority,_new->pid);
    return _new;
}


int _ktask_kill(ktask *kTask){
    if(kTask==NULL||kTask->pid<0){
        klog(ERROR,"ktask","_ktask_kill: E_BAD_ARGUMENT: kTask");
        return E_BAD_ARGUMENT;
    }
    kassert(kTask->handle!=NULL);
    vTaskSuspend(kTask->handle);
    klog(INFO,"ktask","Killed task:%s(%d)",kTask->name,kTask->pid);
    free(kTask);
    return NO_ERROR;
}

int ktask_kill(int pid){
    int l=0;
    int r=khandle->tasks->sz-1;
    ktask *task;
    while (l<=r)
    {
        int m=(l+r)/2-1;
        task=(ktask *)khandle->tasks->base[m];
        uint8_t _pid=task->pid;
        if(_pid<pid){
            l=m+1;
        }else if(_pid>pid){
            r=m-1;
        }else{
            break;
        }
    }
    if(task==NULL||task->pid!=pid){
        klog(ERROR,"ktask","Failed to find task with pid %d!",pid);
        return E_PROCCESS_NOT_FOUND;
    }
    return _ktask_kill(task);
}
