#include "module.h"

#include <kernel/kevent.h>
#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/ktask.h>
#include <M5Stack.h>
#include <M5Faces.h>



void hidctl_loop(void *placeholder){
    for(;;){
        if(M5.BtnA.wasPressed()){
            event_create(khandle->event_mgr,"btn_a_pressed",NULL);
        }
        if(M5.BtnB.wasPressed()){
            event_create(khandle->event_mgr,"btn_b_pressed",NULL);
        }
        if(M5.BtnC.wasPressed()){
            event_create(khandle->event_mgr,"btn_c_pressed",NULL);
        }
        ksleep(HID_SLEEP);
    }
}
void init_hid(void){
    klog(INFO,"hid","Intializng HID");
    ktask_start("hid", &hidctl_loop, NULL,0);
    //kassert(M5.I2C.writeCommand(KEYBOARD_I2C_ADDR,READI2CSUBADDR));
}