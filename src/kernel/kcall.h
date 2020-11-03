// Implement kernel calls protocol
// Brief idea:
// [High-level app(e.g. lua gui)] -> [kcall] -> [kernel] -.-> [kernel module]

#ifndef KCALL_H
#define KCALL_H

#include <stdint.h>

#include <kernel/types/array.h>
#include <kernel/types/hashtable.h>

typedef struct{ 
        uint16_t type;
        void* result;
} kcall_result_t;
typedef struct{
    hashtable* calls;
} kcall_handle_t;

typedef struct{
	array_t params;
} kcall_param_t;

typedef struct{ 
	char* name;
        kcall_result_t* (*func)(kcall_param_t* param);
} kcall_t; 

extern kcall_handle_t* kcalls;

void kCallInit(void);
void kCallRegister(const char* name, kcall_result_t* (*func)(kcall_param_t* param));
kcall_result_t* kCall(char* name, kcall_param_t* param);

#endif
