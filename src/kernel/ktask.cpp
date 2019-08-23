#include "kernel.h"
#include "kconfig.h"
#include "ktask.h"
#include "klog.h"
#include "io.h"
#include "error.h"
#include "types/array.h"


#include <stdlib.h>

static void _wrapper(void *_Task){
    ktask *task=(ktask *)_Task;
    task->execute(task->xParam);
    klog(INFO,"ktask","Finished executing task %s(pid=%d)",task->name,task->pid);
    kmutex_lock();
    int l=0;
    int r=khandle->tasks->sz-1;
    uint8_t _pid;
    uint8_t pid=task->pid;
    int m;
    ktask *current;
    if(khandle->tasks->sz==1){
        array_pop(khandle->tasks);
    }
    while (l<=r)
    {
        m=(l+r)/2-1;
        if(m==-1){
            m=1;   
        }
        current=(ktask *)khandle->tasks->base[m];
        _pid=current->pid;
        if(_pid<pid){
            l=m+1;
        }else if(_pid>pid){
            r=m-1;
        }else{
            break;
        }
    }
    if(current!=task){
        klog(ERROR,"ktask","Failed to find task with pid %d![%s:%d].addr=%d,pid=%d",pid,__FILE__,__LINE__,current,current->pid);
    }else{
        array_del(khandle->tasks,m);
    }
    kmutex_unlock();
    vTaskDelete(NULL);
}

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
    int pid=-1;
    if(khandle->tasks->sz==0){
        pid=0;
    }else{
        ktask *last=(ktask*)tasklist->base[tasklist->sz-1];
        kassert(last!=NULL);
        pid=last->pid+1;
    }
    assert(pid>=0);
    _new->pid=pid;
    _new->execute=execute;
    _new->name=name;
    _new->priority=priority;
    _new->xParam=xParameter;
    array_add(tasklist,_new);
    kmutex_unlock();
    int status=xTaskCreate(&_wrapper,name,TASK_STACK_DEPTH,_new,priority,&_new->handle);
    kassert(pdPASS==status);
    klog(INFO,"ktask","Started task %s with priority %d and pid %d",_new->name,_new->priority,_new->pid);
    return _new;
}


int _ktask_kill(ktask *kTask){
    if(kTask==NULL||kTask->pid<0){
        klog(ERROR,"ktask","_ktask_kill: E_BAD_ARGUMENT: kTask");
        kmutex_unlock();
        return E_BAD_ARGUMENT;
    }
    kmutex_unlock();
    kassert(kTask->handle!=NULL);
    klog(INFO,"ktask","Killing task:%s(%d)",kTask->name,kTask->pid);
    if(xTaskGetCurrentTaskHandle()==kTask->handle){
        klog(WARN,"ktask","Task attempts to kill itself.Memory will not be freed.");
    }
    vTaskDelete(kTask->handle);
    free(kTask);
    return NO_ERROR;
}

int ktask_kill(int pid){
    kmutex_lock();
    int l=0;
    int r=khandle->tasks->sz-1;
    ktask *task;
    if(khandle->tasks->sz==1){
        task=(ktask *)khandle->tasks->base[0];
        return _ktask_kill(task);
    }
    uint8_t _pid;
    while (l<=r)
    {
        int m=(l+r)/2-1;
        if(m==-1){
            m=1;   
        }
        task=(ktask *)khandle->tasks->base[m];
        _pid=task->pid;
        if(_pid<pid){
            l=m+1;
        }else if(_pid>pid){
            r=m-1;
        }else{
            break;
        }
    }
    if(task==NULL||task->pid!=_pid){
        klog(ERROR,"ktask","Failed to find task with pid %d!",pid);
        return E_PROCCESS_NOT_FOUND;
    }
    array_del(khandle->tasks,pid);
    return _ktask_kill(task);
    
}

int kGetPid(void){
    TaskHandle_t handle=xTaskGetCurrentTaskHandle();
    kmutex_lock();
    int i=0;
    for(;i<khandle->tasks->sz;++i){
        if(((ktask*)khandle->tasks->base[i])->handle==handle){
            int _pid=((ktask*)khandle->tasks->base[i])->pid;
            kmutex_unlock();
            return _pid;
        }
    }
    kassert(false);
    return -1;
}

void kill_all(void){
    int i=0;
    for(;i<khandle->tasks->sz;++i){
        ktask *task=(ktask *)khandle->tasks->base[i];
        if(task!=NULL){
            _ktask_kill(task);
        }
    }
}

