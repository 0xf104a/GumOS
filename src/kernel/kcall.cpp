#include "kcall.h"

#include <string.h>

#include <kernel/klog.h>
#include <kernel/kconfig.h>
#include <kernel/kernel.h>
#include <kernel/ktask.h>
#include <kernel/commons/commons.h>

kcall_handle_t* kcalls;

void kCallInit(void){
     kcalls=(kcall_handle_t*)malloc(sizeof(kcall_handle_t));
     kcalls->calls = hashtbl_create(HASHTABLE_CAPACITY);
     klog(INFO, "kcall", "Initialized kernel calls table");
}

void kCallRegister(const char* _name, kcall_result_t* (*func)(kcall_param_t* param)){
     char* name=clone_s(_name); 
     kassert(!hashtbl_check_key(kcalls->calls, name));
     kcall_t* call=(kcall_t*)malloc(sizeof(kcall_t));
     call->name=(char*)malloc(sizeof(char)*strlen(name));
     strcpy(call->name, name);
     call->func=func;
     kmutex_lock();
     hashtbl_add(kcalls->calls, name, (void*)call);
     kmutex_unlock();
     free(name);
}

kcall_result_t* kCall(char* name, kcall_param_t* param){
     kcall_t* call=(kcall_t*)hashtbl_get(kcalls->calls,name);
     if(!call){
        klog(ERROR,"kcall", "Illegal system call: %s", name);
        return NULL;
     }
     return call->func(param);
}    
