#include "types/hashtable.h"
#include "kernel.h"
#include "kmodule.h"
#include "klog.h"
#include "kevent.h"
#include "kconfig.h"
#include "ktask.h"

#include <stdlib.h>
/*Modules*/
#include <kernel/modules/gui/module.h>
#include <kernel/modules/powerctl/module.h>
#include <kernel/modules/soundctl/module.h>
#include <kernel/modules/hid/module.h>
#include <kernel/modules/clojurectl/module.h>

void boot_pb(double pb){
    event_create(khandle->event_mgr,"boot_progress_update", &pb);
}

void _kmodule_register(kmodule_t *module){
    kassert(module!=NULL);
    kassert(module->name!=NULL);
    kmutex_lock();
    hashtbl_add(khandle->modules,module->name,(void*)module);
    kmutex_unlock();
    klog(INFO,"kmodule","Registering module:%s",module->name);
    module->init();
}

void kmodule_register(char *name,void (*shared_call)(void *param),void (*init)(void)){
    kmodule_t *module=(kmodule_t *)malloc(sizeof(kmodule_t));
    module->name=name;
    module->shared_call=shared_call;
    module->init=init;
    _kmodule_register(module);
}

void init_modules(void* xParam){
    //Register modules here
#if LOAD_GUI
    kmutex_lock();
    kmodule_register("gui",NULL,&init_gui_module);
    kmutex_unlock();
    boot_pb(20.0);
#endif
#if LOAD_POWERCTL
    kmodule_register("powerctl",NULL,&init_powerctl);
#endif
#if LOAD_SOUNDCTL
    kmodule_register("soundctl",NULL,&init_soundctl);
#endif
#if LOAD_HIDCTL
    kmodule_register("hidctl",NULL,&init_hid);
#endif
#if LOAD_CLOJURE
    kmodule_register("clojurectl",NULL,&init_clojure);
#endif
    boot_pb(100.0);
}
