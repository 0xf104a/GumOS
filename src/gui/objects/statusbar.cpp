#include "statusbar.hpp"

#include "icons/battery/battery.hpp"

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
    //M5.Lcd.fillRect(0,0,SCREEN_WIDTH,STATUSBAR_HEIGHT,STATUSBAR_COLOR);
    /* Draw time */
    M5.Lcd.setTextColor(65535,STATUSBAR_COLOR);
    char *time=(char*)malloc(sizeof(char)*7);
    uint64_t *_rtime=NULL;
    gassert(readf("/dev/utime",(uint8_t**)(&_rtime))==sizeof(uint64_t));
    uint64_t rtime=*_rtime;
    int hh=(rtime/3600)%24;
    int mm=(rtime/60)%60;
    snprintf(time,6*sizeof(char),"%02d:%02d",hh,mm);
    M5.Lcd.drawCentreString(time,160,0,2);
    M5.Lcd.drawCentreString(statusbar->appname,0,0,2);
    free(_rtime);
    free(time);
    /* Draw icons */
    int i=0;
    uint16_t pos=SCREEN_WIDTH-16;
    for(;i<statusbar->icons->sz;++i){
        icon_t *icon=(icon_t *)statusbar->icons->base[i];
        if(icon->draw(pos)){
            pos-=16;
        }
    }

}

void set_appname(char *name){
    free(statusbar->appname);
    statusbar->appname=name;
}

void add_icon(bool (*draw)(uint16_t pos)){
    icon_t *icon=(icon_t *)malloc(sizeof(icon_t));
    icon->id=statusbar->icons->sz;
    icon->draw=draw;
    array_add(statusbar->icons,icon);
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
    statusbar->appname=(char*)malloc(sizeof(char)*(strlen(INITIAL_APPNAME)+1));
    strcpy(statusbar->appname,INITIAL_APPNAME);
    GUIObject *statusbar=gui_create_object("statusbar",(void *)statusbar, &draw_bar);
    event_handler_add(khandle->event_mgr, &charger_handler);
    add_icon(&draw_battery_icon);
    return statusbar;
}