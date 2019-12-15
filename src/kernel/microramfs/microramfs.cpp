//
//  microramfs.cpp
//  microramfs
//
//  Created by Andre Zay on 14/08/2019.
//  Copyright © 2019 Andre Zay. All rights reserved.
//

#include "microramfs.h"
#include "init.h"

#include <kernel/error.h>
#include <kernel/klog.h>
#include <kernel/ktask.h>
#include <kernel/kernel.h>
#include <kernel/kevent.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static char *extract_dirname(char *path){
    size_t i=strlen(path)-1;
    do{
        --i;
        if(path[i]=='/'){
            ++i;
            break;
        }
    }while(path[i]!='/');
    return path+i;
}
void load_image(microramfs *fs){
       size_t i=0;
    while (i<initramfs_sz) {
        if(init_img[i]==CMD_FILE){
            ++i;
            uint8_t *name=NULL;
            uint8_t *content=NULL;
            uint64_t name_size=0;
            memcpy(&name_size, init_img+i, sizeof(uint64_t));
            //printf("Name size: %llu\n",name_size);
            name=(uint8_t *)malloc(name_size*sizeof(uint8_t));
            i+=sizeof(uint64_t);
            memcpy((uint8_t *)name, init_img+i, name_size*sizeof(uint8_t));
            i+=name_size;
            uint64_t file_size=0;
            memcpy(&file_size, init_img+i, sizeof(uint64_t));
            //printf("File size: %llu\n",file_size);
            i+=sizeof(uint64_t);
            content=(uint8_t *)malloc(file_size*sizeof(uint8_t));
            memcpy((uint8_t *)content, init_img+i, file_size*sizeof(uint8_t));
            int stat=create_file(fs,(char *)name);
            if(stat!=0){
                klog(ERROR,MOD,"Failed to touch %s, stat:%d",name,stat);
            }
            i+=file_size;
            stat=write_file(fs,(char *)name,content,file_size);
            if(stat!=0){
                klog(ERROR,MOD,"Failed to write %s, stat:%d",name,stat);
            }
        }else if(init_img[i]==CMD_DIR){
            ++i;
            uint8_t *name=NULL;
            uint64_t name_size=0;
            memcpy(&name_size, init_img+i, sizeof(uint64_t));
            //printf("Dir name size: %llu\n",name_size);
            name=(uint8_t *)malloc(name_size*sizeof(uint8_t));
            i+=sizeof(uint64_t);
            //++i;
            memcpy((uint8_t *)name, init_img+i, name_size*sizeof(uint8_t));
            //printf("Dir name: %s\n",(char*)name);
            i+=name_size-1;
            size_t j=strlen((char *)name);
            while(j>0){
                j--;
                if(name[j]=='/'){
                    break;
                }
            }
            char *dirname=(char *)malloc(sizeof(char)*(strlen((char *)name)-j+1));
            strcpy(dirname, (char*)(name+j+1));
            name[j+1]=0;
            int stat=create_dir(fs,(char *)name,dirname);
            if(stat!=NO_ERROR){
                klog(ERROR,MOD,"Failed to mkdir %s in %s, stat: %d",dirname,name);
            }
        }else{
            assert(false);
        }
        ++i;
    }
}
static fs_obj *create_file_ent(char *name){
    assert(name!=NULL);
    fs_obj *file=(fs_obj *)malloc(sizeof(fs_obj));
    file->fdata=NULL;
    file->is_dir=false;
    file->name=name;
    file->type=OBJ_FILE;
    file->objects=NULL;
    file->locked=false;
    file->sz=0;
    return file;
}

static fs_obj *create_dir_ent(char *name){
    assert(name!=NULL);
    fs_obj *dir=(fs_obj *)malloc(sizeof(fs_obj));
    dir->is_dir=true;
    dir->fdata=NULL;
    dir->name=name;
    dir->objects=hashtbl_create(HASHTABLE_CAPACITY);
    dir->type=OBJ_DIRECTORY;
    dir->locked=false;
    dir->sz=0;
    return dir;
}

microramfs *init_ram_fs(uint32_t max_size){
    microramfs *_new=(microramfs *)malloc(sizeof(microramfs));
    _new->used=0;
    _new->used+=sizeof(_new);
    _new->size=max_size;
    _new->root=create_dir_ent("/");
    _new->used+=DIRSIZE(_new->root);
    klog(DEBUG,MOD,"init_ram_fs done. Size:%d, Used:%d",_new->size,_new->used);
    return _new;
}

int get_obj(microramfs *fs,char *path,fs_obj **obj){
    if(fs==NULL||path==NULL){
        klog(ERROR,MOD,"Bad argument for get_obj [%s:%d]",__FILE__,__LINE__);
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    char *rest=(char *)malloc((strlen(path)+1)*sizeof(char));
    char *_rest=rest;
    char *token = NULL;
    strcpy(rest,path);
    fs_obj *current_dir=fs->root;
    do {
        token=strtok_r(rest, "/", &rest);
        if(rest==NULL){
            break;
        }
        if(!hashtbl_check_key(current_dir->objects, token)){
            free(_rest);
            klog(ERROR,MOD,"Object not found: %s[%s:%d]",path,__FILE__,__LINE__);
            return E_NO_SUCH_FILE_OR_DIR;//No such file or directory
        }
        current_dir=(fs_obj*)hashtbl_get(current_dir->objects, token);
    } while (rest);
    if(token==NULL){
        if(current_dir==NULL){
            free(_rest);
            klog(ERROR,MOD,"Object not found: %s[%s:%d]",path,__FILE__,__LINE__);
            return E_NO_SUCH_FILE_OR_DIR;
        }
        *obj=current_dir;
    }else{
        fs_obj *_obj=(fs_obj*)hashtbl_get(current_dir->objects, token);
        if(_obj==NULL){
            free(_rest);
            klog(ERROR,MOD,"Object not found: %s[%s:%d]",path,__FILE__,__LINE__);
            return E_NO_SUCH_FILE_OR_DIR;
        }
        *obj=_obj;
    }
    free(_rest);
    return 0;
}

int create_file(microramfs *fs,char *path){
    if(fs==NULL||path==NULL){
        klog(ERROR,MOD,"Bad argument for get_obj [%s:%d]",__FILE__,__LINE__);
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    char *rest=(char *)malloc((strlen(path)+1)*sizeof(char));
    char *_rest=rest;
    char *token = NULL;
    strcpy(rest,path);
    fs_obj *current_dir=fs->root;
    while (rest) {
        token=strtok_r(rest, "/", &rest);
        if(rest==NULL){
            break;
        }
        if(!hashtbl_check_key(current_dir->objects, token)){
            free(_rest);
            klog(ERROR,MOD,"No such file or direcotory:%s",path);
            return E_NO_SUCH_FILE_OR_DIR;//No such file or directory
        }
        current_dir=(fs_obj*)hashtbl_get(current_dir->objects, token);
        if(current_dir->type==OBJ_FILE){
            free(_rest);
            klog(ERROR,MOD,"Object is file:%s",path);
            return E_IS_FILE;//Object is file
        }
    }
    assert(current_dir!=NULL);
    assert(current_dir->is_dir);
    if(hashtbl_check_key(current_dir->objects, token)){
        klog(ERROR,MOD,"Object already exists:%s",path);
        return E_ALREADY_EXISTS; //Already exists
    }
    fs_obj *f=create_file_ent(token);
    f->fdata=(uint8_t*)malloc(sizeof(uint8_t));
    f->fdata[0]='\0';
    fs->used+=FILESIZE(f);
    if(fs->used>=fs->size){
        fs->used-=FILESIZE(f);
        free(f->fdata);
        free(f);
        return E_NOT_ENOUGH_SPACE;//No free space!
    }
    hashtbl_add(current_dir->objects, f->name, (void *)f);
    //free(_rest);
    return 0;//Ok
}

int write_file(microramfs *fs,char *path,uint8_t *data, uint64_t sz){
    if(fs==NULL||path==NULL||data==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);

    if(stat!=0){
        return stat;
    }

    if(obj->is_dir){
        return E_IS_DIR;//Directory
    }
    
    if(obj->fdata!=NULL){
        //klog(DEBUG,MOD,"Free fdata:%s",obj->fdata);
        fs->used-=obj->sz;
        free(obj->fdata);
    }

    if(fs->used+sz*sizeof(uint8_t)>=fs->size){
        return E_NOT_ENOUGH_SPACE;//No free space
    }
    fs->used+=sz*sizeof(uint8_t);
    obj->fdata=(uint8_t *)malloc(sz*sizeof(uint8_t));
    memcpy(obj->fdata,data,sz*sizeof(uint8_t));
    obj->sz=sz*sizeof(uint8_t);
    return 0;
}

int read_file(microramfs *fs,char *path,uint8_t **data, uint64_t *sz){
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);
    if(stat!=0){
        return stat;
    }
    if(obj->is_dir){
        return E_IS_DIR;//Directory
    }
    kassert(*data==NULL);
    *data=(uint8_t *)malloc(obj->sz);
    memcpy(*data,obj->fdata,obj->sz);
    *sz=obj->sz;
    return 0;
}

int rm_file(microramfs *fs,char *path){
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    char *rest=(char *)malloc(strlen(path)*sizeof(char));
    char *_rest=rest;
    char *token = NULL;
    strcpy(rest,path);
    fs_obj *current_dir=fs->root;
    do {
        token=strtok_r(rest, "/", &rest);
        if(rest==NULL){
            break;
        }
        if(!hashtbl_check_key(current_dir->objects, token)){
            free(_rest);
            return E_NO_SUCH_FILE_OR_DIR;//No such file or directory
        }
        current_dir=(fs_obj*)hashtbl_get(current_dir->objects, token);
        if(current_dir->type==OBJ_FILE){
            free(_rest);
            return E_IS_FILE;//Object is file
        }
    } while (rest);
    assert(current_dir!=NULL);
    assert(current_dir->is_dir);
    if(!hashtbl_check_key(current_dir->objects, token)){
        return E_NO_SUCH_FILE_OR_DIR;//No such file or directory
    }
    fs_obj *obj=(fs_obj*)hashtbl_get(current_dir->objects, token);
    if(obj->is_dir){
        return E_IS_DIR; //It is directory.
    }
    fs->used-=FILESIZE(obj);
    hashtbl_rm(current_dir->objects, obj->name);
    free(obj->fdata);
    free(obj);
    free(_rest);
    return 0;//Ok
}

int ls_dir(microramfs *fs, char *path, array_t **array){
    assert(*array==NULL);
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);
    if(stat!=0){
        return stat;
    }
    if(!obj->is_dir){
        return E_IS_FILE;//File
    }
    *array=array_cpy(obj->objects->values);
    return 0;
}

int create_dir(microramfs *fs,char *path, char *name){
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);
    if(stat!=0){
        return stat;
    }
    if(!obj->is_dir){
        return E_IS_FILE;//File
    }
    while(name[strlen(name)-1]=='/'){
        name[strlen(name)-1]=0;
    }
    fs_obj *_new=create_dir_ent(name);
    hashtbl_add(obj->objects, name, (void *)_new);
    fs->used+=DIRSIZE(_new);
    return 0;
}

int lock_file(microramfs *fs,char *path){
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);

    if(stat!=0){
        return stat;
    }

    if(obj->is_dir){
        return E_IS_DIR;//Directory
    }
    kmutex_lock();
    obj->locked=true;
    kmutex_unlock();
    return 0;
}

int unlock_file(microramfs *fs,char *path){
    if(fs==NULL||path==NULL){
        return E_BAD_ARGUMENT;//E_BAD_ARG
    }
    fs_obj *obj;
    int stat=get_obj(fs, path, &obj);

    if(stat!=0){
        return stat;
    }

    if(obj->is_dir){
        return E_IS_DIR;//Directory
    }
    kmutex_lock();
    obj->locked=false;
    kmutex_unlock();
    return 0;
}

