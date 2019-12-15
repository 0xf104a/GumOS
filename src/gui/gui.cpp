#include "gui.h"
#include "config.h"
#include "objects/statusbar.hpp"
#include "objects/keyboard.h"
#include "objects/widgets.h"
#include "widgets/clock/widget.h"

#include <kernel/kernel.h>
#include <kernel/ktimer.h>
#include <kernel/ktask.h>
#include <kernel/klog.h>
#include <kernel/io.h>

#include <M5Stack.h>



GUI *gui;

uint16_t color16(uint16_t r, uint16_t g, uint16_t b) {
	uint16_t _color;
	_color = (uint16_t)(r & 0xF8) << 8;
	_color |= (uint16_t)(g & 0xFC) << 3;
	_color |= (uint16_t)(b & 0xF8) >> 3;
  return _color;
}

void _gassert(bool a,const char *fname,int line){
    if(!a){
        kSetConsoleMode(true);
        klog(FATAL,"gui","Assertation failed at %s:%d. Returning to console mode.",fname,line);
        kmutex_lock();
        gui->running=false;
        kmutex_unlock();
        ktask_kill(kGetPid());
    }
}
void gui_add_object(GUIObject *object){
    gassert(object!=NULL);
    gassert(object->name!=NULL);
    kmutex_lock();
    hashtbl_add(gui->objects,object->name,(void *)object);
    kmutex_unlock();
    klog(INFO,"gui","Added new object to GUI: %s",object->name);
}

GUIObject *gui_create_object(char *name, void *param, void (*draw)(void *param)){
    GUIObject *object=(GUIObject *)malloc(sizeof(GUIObject));
    object->name=name;
    object->param=param;
    object->draw=draw;
    return object;
}

void init_gui(void){
    gui=(GUI *)malloc(sizeof(GUI));
    gui->objects=hashtbl_create(HASHTABLE_CAPACITY);
    gui_add_object(init_statusbar());
    gui_add_object(init_keyboard());
    gui_add_object(init_widgets());
    //Load images etc.
    klog(INFO,"gui","Initialized gui.");
}

void launch_gui(void){
    klog(INFO,"gui","Launching gui.");
    gui->running=true;
    M5.Lcd.clear();
    add_widget(DebugWidget());
    while (gui->running){
        ksleep(1000.0/GUI_FPS);
        array_t *gui_objects=gui->objects->values;
        GUIObject *object;
        for(int i=0;i<gui_objects->sz;i++){
            object=(GUIObject*)gui_objects->base[i];
            object->draw(object->param);
        }
    }
}

char *timestr(void){
    char *time=(char*)malloc(sizeof(char)*7);
    uint64_t *_rtime=NULL;
    _gassert(readf("/dev/utime",(uint8_t**)(&_rtime))==sizeof(uint64_t),__FILE__,__LINE__);
    uint64_t rtime=*_rtime;
    int hh=(rtime/3600)%24;
    int mm=(rtime/60)%60;
    snprintf(time,6*sizeof(char),"%02d:%02d",hh,mm);
    free(_rtime);
    return time;
}

