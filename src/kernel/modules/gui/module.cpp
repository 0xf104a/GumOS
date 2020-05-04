#include "module.h"
#include "boot_splash.h"

#include <kernel/ktask.h>
#include <kernel/kernel.h>
#include <kernel/klog.h>


#include <M5Stack.h>

void init_gui_module(void){
    kSetConsoleMode(false);
    M5.Lcd.begin();
    M5.Lcd.clear();
    M5.Lcd.setCursor(0, 0);
    klog(INFO,"gui","Starting bootsplash");
    event_handler_add(khandle->event_mgr, &boot_progress_handler);
    ktask_start("gui", &draw_splash, NULL,0);

}