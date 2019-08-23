#include "statusbar.hpp"

#include <gui/gui.h>
#include <gui/config.h>

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/kevent.h>
#include <kernel/io.h>
#include <kernel/microramfs/microramfs.h>

#include <M5Stack.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

statusbar_t *statusbar;

void draw_bar(void *_statusbar){
    M5.Lcd.fillRect(0,0,SCREEN_WIDTH,STATUSBAR_HEIGHT,STATUSBAR_COLOR);
    M5.Lcd.setTextColor(65535,STATUSBAR_COLOR);
    char *time=(char*)malloc(sizeof(char)*7);
    uint64_t *_rtime=NULL;
    gassert(readf("/dev/utime",(uint8_t**)(&_rtime))==sizeof(uint64_t));
    uint64_t rtime=*_rtime;
    int hh=(rtime/3600)%24;
    int mm=(rtime/60)%60;
    snprintf(time,6*sizeof(char),"%02d:%02d",hh,mm);
    M5.Lcd.drawCentreString(time,160,0,2);
}

void charger_handler(kevent_t *event){
    gassert(statusbar!=NULL);
    if(!strcmp(event->name,POWERCTL_CHARGER_EVENT)){
        draw_bar(statusbar);
    }
}

GUIObject *init_statusbar(void){
    klog(INFO,"gui","Initializing statusbar.");
    statusbar=(statusbar_t *)malloc(sizeof(statusbar_t));
    statusbar->icons=array_create(ARRAY_CAPACITY);
    GUIObject *statusbar=gui_create_object("statusbar",(void *)statusbar, &draw_bar);
    event_handler_add(khandle->event_mgr, &charger_handler);
    return statusbar;
}