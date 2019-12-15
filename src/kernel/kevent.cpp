#include "kevent.h"
#include "klog.h"
#include "ktask.h"
#include "kconfig.h"

#include <stdlib.h>
#include <string.h>

EventManager *eventmgr_init(void){
    EventManager *mgr=(EventManager *)malloc(sizeof(EventManager));
    mgr->handlers=array_create(ARRAY_CAPACITY);
    klog(INFO,"kevent","Created event manager");
    return mgr;
}

void event_handler_add(EventManager *mgr, void (*handle)(kevent_t *event)){
     Handler_t *handler=(Handler_t *)malloc(sizeof(Handler_t));
     handler->handle=handle;
     klog(INFO,"kevent","Added new event handler@%d",handle);
     array_add(mgr->handlers,handler);
}

void event_create(EventManager *mgr,char *name, void *param){
    kevent_t *event=(kevent_t*)malloc(sizeof(kevent_t));
    event->data=param;
    event->name=(char *)malloc(sizeof(char)*(strlen(name)+1));
    klog(INFO,"kevent","Broadcasting event %s",name);
    strcpy(event->name,name);
    kmutex_lock();
    int i=0;
    for(;i<mgr->handlers->sz;++i){
        Handler_t *handler=(Handler_t *)mgr->handlers->base[i];
        handler->handle(event);
    }
    free(event->name);
    free(event);
    kmutex_unlock();
}