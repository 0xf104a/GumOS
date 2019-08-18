//
//  microramfs.hpp
//  microramfs
//
//  Created by Andre Zay on 14/08/2019.
//  Copyright © 2019 Andre Zay. All rights reserved.
//

#ifndef microramfs_hpp
#define microramfs_hpp

#include <kernel/types/hashtable.h>
#include <kernel/types/array.h>
#include <kernel/kconfig.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef MOD
#warning "Redefinng MOD name constant."
#undef MOD
#endif
#define MOD "microramfs"

typedef enum {
    OBJ_DIRECTORY,OBJ_FILE,MOUNT_POINT
} fs_type;

typedef struct _fs_obj{
    char *name;
    fs_type type;
    bool is_dir;
    uint8_t *fdata;
    hashtable *objects;
    bool locked;
    uint64_t sz;
} fs_obj;

typedef struct{
    fs_obj *root;
    uint32_t size;
    uint32_t used;
} microramfs;

void load_image(microramfs *fs);
microramfs *init_ram_fs(uint32_t max_size);
int create_file(microramfs *fs,char *path);
int write_file(microramfs *fs,char *path,uint8_t *data, uint64_t sz);
int read_file(microramfs *fs,char *path,uint8_t **data, uint64_t *sz);
int rm_file(microramfs *fs,char *path);
int ls_dir(microramfs *fs, char *path, array_t **array);
int create_dir(microramfs *fs,char *path, char *name);

#define FILESIZE(OBJ) (sizeof(fs_obj)+strlen(OBJ->name)*sizeof(char)+OBJ->sz*sizeof(uint8_t));
#define DIRSIZE(OBJ)  (sizeof(fs_obj)+strlen(OBJ->name)*sizeof(char)+sizeof(hashtable)+2*sizeof(array_t)+(OBJ->objects->sz+1)*sizeof(_hashtable_node)+2*(OBJ->objects->sz+1)*sizeof(void*))

#endif /* microramfs_hpp */
