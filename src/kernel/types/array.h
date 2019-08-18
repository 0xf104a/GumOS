//
//  array.h
//  microlang
//
//  Created by Andre Zay on 05/08/2019.
//  Copyright © 2019 Andre Zay. All rights reserved.
//

#ifndef array_h
#define array_h

#include <sys/types.h>
#include <stdbool.h>

typedef struct{
    size_t sz;
    size_t capacity;
    void **base;
} array_t;

array_t *array_create(int capacity);
array_t *array_cpy(array_t *src);
bool in_array(array_t *_array, void *object);
void array_add(array_t *_array,void *object);
void array_free(array_t *_array);
void array_pop(array_t *_array);
void array_cat(array_t *a,array_t *b);
void array_del(array_t *_array,int pos);

#endif /* array_h */

