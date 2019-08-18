#include "types/hashtable.h"
#include "kernel.h"
#include "kmodule.h"
#include "klog.h"
/*Modules*/
#include <kernel/modules/gui/module.h>

void kmodule_register(kmodule_t *module){
    kassert(module!=NULL);
    kassert(module->name!=NULL);
    hashtbl_add(khandle->modules,module->name,(void*)module);
    klog(INFO,"kmodule","Registering module:%s",module->name);
    module->init();
}

void init_modules(void){
    //Register modules here
    init_gui_module();
}

