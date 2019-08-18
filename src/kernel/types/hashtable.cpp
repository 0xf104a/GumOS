//
//  hashtable.c
//  microlang
//
//  Created by Andre Zay on 05/08/2019.
//  Copyright © 2019 Andre Zay. All rights reserved.
//


#include "hashtable.h"

#include "array.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

unsigned long __hash(unsigned char *str) //djb2 hash by Dan Bernstein
{
    unsigned long hash = 5381;
    int c;
    
    while ((c = *str++)){
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    
    return hash;
}

unsigned long _hash(unsigned char *str, size_t mod){ //wrapper
    unsigned long x= __hash(str)%mod;
    return x;
}

void _hashtbl_register(hashtable *tbl,_hashtable_node *node){
    array_add(tbl->keys, node->key);//add key to keys storage
    array_add(tbl->values, node->value);//add value to values storage
}

_hashtable_node *create_node(char *_key,void *_data){
    _hashtable_node *node=(_hashtable_node *)malloc(sizeof(_hashtable_node));
    node->next=NULL;
    node->value=_data;
    node->key=(char *)malloc(sizeof(char)*strlen(_key));
    strcpy(node->key, _key);
    return node;
}


hashtable *hashtbl_create(size_t capacity){
    hashtable *_tbl=(hashtable *)malloc(sizeof(hashtable));
    _tbl->capacity = capacity;
    _tbl->sz = 0;
    _tbl->base = (_hashtable_node **)malloc(sizeof(_hashtable_node *)*capacity);
    _tbl->keys = array_create(1);
    _tbl->values = array_create(1);
    bzero(_tbl->base, capacity*sizeof(_hashtable_node*));
    return _tbl;
}

void hashtbl_add(hashtable *tbl, char *key, void *data){
    unsigned long hash = _hash((unsigned char*)key, tbl->capacity);
    _hashtable_node *node=create_node(key, data);
    if(tbl->base[hash]==NULL){ //If we have not used this hash
        tbl->base[hash]=node; //add node directly.
    }else{
        _hashtable_node *iterator=tbl->base[hash]; //We used this hash so we need to attach node to last node asscoiated with this hash.
        while (iterator->next!=NULL) {
            iterator=iterator->next;
        }
        iterator->next=node;
    }
    _hashtbl_register(tbl,node);
    tbl->sz+=1;
}

void *hashtbl_get(hashtable *tbl, char *key){
    unsigned long hash = _hash((unsigned char *)key, tbl->capacity);
    if(tbl->base[hash]==NULL){

        return NULL;
    }else{
        _hashtable_node *iterator=tbl->base[hash];
        while (iterator) {
            if(!strcmp(iterator->key, key)){
                return iterator->value;
            }
            iterator=iterator->next;
        }
        assert(iterator&&!strcmp(iterator->key,key));
        return iterator->value;
    }
}

bool hashtbl_check_key(hashtable *tbl,char *key){
    unsigned long hash = _hash((unsigned char *)key, tbl->capacity);
    if(tbl->base[hash]==NULL){
        return false;
    }else{
        _hashtable_node *iterator=tbl->base[hash];
        while (iterator) {
            if(!strcmp(iterator->key, key)){
                return true;
            }
            iterator=iterator->next;
        }
        return false;
    }
}

void hastbl_destroy(hashtable *tbl){
    assert(tbl!=NULL);
    int i=0;
    _hashtable_node *iterator=NULL;
    _hashtable_node *next=NULL;
    
    for(;i<tbl->capacity;++i){
        iterator=tbl->base[i];
        if(!iterator){
            continue;
        }
        next=iterator->next;
        free(iterator);
        iterator=next;
        while(iterator!=NULL){
            next=iterator->next;
            free(iterator);
            iterator=next;
        }
    }
}

void hashtbl_set(hashtable *tbl, char *key,void *value){
    unsigned long hash = _hash((unsigned char *)key, tbl->capacity);
    if(tbl->base[hash]==NULL){
        hashtbl_add(tbl, key, value);
    }else{
        _hashtable_node *iterator=tbl->base[hash];
        while (iterator) {
            if(!strcmp(iterator->key, key)){
                break;
            }
            iterator=iterator->next;
        }
        assert(iterator&&!strcmp(iterator->key,key));
        iterator->value=value;
    }
}

void hashtbl_rm(hashtable *tbl, char *key){
    unsigned long hash = _hash((unsigned char *)key, tbl->capacity);
    if(tbl->base[hash]==NULL){
        return ;
    }else{
        _hashtable_node *prev=NULL;
        _hashtable_node *iterator=tbl->base[hash];
        while (iterator) {
            if(!strcmp(iterator->key, key)){
                break;
            }
            prev=iterator;
            iterator=iterator->next;
        }
        assert(iterator&&!strcmp(iterator->key,key));
        if(prev!=NULL){
            prev->next=iterator->next;
        }
        int i=0;
        for(;i<tbl->sz;++i){
            char *t=(char*)tbl->keys->base[i];
            if(!strcmp(iterator->key,t)){
                array_del(tbl->keys, i);
                array_del(tbl->values, i);
            }
        }
        free(iterator);
        tbl->sz--;
    }
}
