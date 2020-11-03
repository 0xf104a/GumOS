#ifndef KEVENT_H
#define KEVENT_H

#include "types/array.h"

typedef struct kevent{
    char *name;
    void *data;
} kevent_t;

typedef struct{
    array_t *handlers;
} EventManager;

typedef struct{
    void (*handle)(kevent_t* event);
} Handler_t;

EventManager *eventmgr_init(void);
void event_handler_add(EventManager *mgr, void (*handle)(kevent_t *event));
void event_create(EventManager *mgr,char *name, void *param);

#endif
