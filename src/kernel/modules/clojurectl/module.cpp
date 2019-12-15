#include "module.h"

#include <clojure/clojure.hpp>
#include <kernel/klog.h>
#include <kernel/io.h>
#include <kernel/ktask.h>
#include <kernel/microramfs/microramfs.h>

void init_clojure(void){
    klog(INFO,"clojurectl","Launching clojure scripts");
    char *list=NULL;
    sreadf("/etc/launch.list",&list);
    if(!list){
        klog(FATAL,"clojurectl","Failed to load /etc/launch.list!");
        return ;
    }
    std::stringstream ss(list);
    std::string item;
    while(ss>>item){
        klog(DEBUG,"clojurectl","Found startup item %s",item.c_str());
    }
}