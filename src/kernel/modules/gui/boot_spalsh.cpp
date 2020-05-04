#include "config.h"

#include <kernel/ktimer.h>
#include <kernel/io.h>
#include <kernel/klog.h>
#include <kernel/kernel.h>
#include <kernel/ktask.h>
#include <kernel/kevent.h>
#include <gui/gui.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <M5Stack.h>

double pb=0.0;
double pb_target=1.0;
bool is_booted;

static void draw_pb(double progress){
      M5.Lcd.drawRoundRect(32, 200, 260, 6, 3, 65535);
      M5.Lcd.fillRoundRect(32,200,(int)((progress*260.0)/100.00),6,3,65535);
}

void boot_progress_handler(kevent_t *event){
    klog(DEBUG,"gui","Recieved event:%s",event->name);
    if(!strcmp(event->name,"boot_progress_update")){
        double *progress=NULL;
        progress=(double *)event->data;
        kassert(progress!=NULL);
        pb_target=BOOT_SYS_FACTOR*(*progress);
        klog(DEBUG,"gui","New progress:%lf",pb_target);
        if(*progress>=100.0){
            is_booted=true;
        }
    }
}

void draw_splash(void *p){
    klog(DEBUG,"gui","Started splash with pid %d",kGetPid());
    is_booted=false;
    uint8_t *logo_data=NULL;
    uint64_t _size;
    size_t sz;
    _size=readf(BOOT_LOGO,&logo_data);
    sz=(size_t)_size;
    if(logo_data==NULL){
        klog(FATAL,"gui","Failed to load logo from: %s",BOOT_LOGO);
        kSetConsoleMode(true);
        ktask_kill(kGetPid());
    }else{
        klog(INFO,"gui","Read logo file. Size is %d.",sz);
       // M5.Lcd.drawJpg(logo_data,sz,96,32);
    }
    
    while(pb!=100.0*BOOT_SYS_FACTOR){
        if(pb<pb_target){
            pb+=0.5;
        }
        draw_pb(pb);
        kassert(pb<=100.0);
        ksleep(1000.0/BOOT_FPS);
    }
    klog(INFO,"gui","Boot sequence completed.Initializing gui.");
    for(;pb<=100.0;pb+=0.5){
        draw_pb(pb);
        ksleep(1000.0/BOOT_FPS);
    }
    free(logo_data);
    gui_run();
}