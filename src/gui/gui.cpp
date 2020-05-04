#include "gui.h"

#include "config.h"
#include "statusbar.h"
#include "widgets/clock/clock.h"//TODO: Widget controller

#include <kernel/kernel.h>
#include <kernel/ktimer.h>
#include <kernel/ktask.h>
#include <kernel/klog.h>
#include <kernel/io.h>
#include <kernel/types/array.h>

#include <M5Stack.h>

array_t* gui_objects=NULL;

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
        ktask_kill(kGetPid());
    }
}
void gui_add_object(void (*draw)(void)){
    gassert(gui_objects);
    gui_object* obj=(gui_object*)malloc(sizeof(gui_object));
    obj->draw=draw;
    array_add(gui_objects,obj);
}
void gui_init(void){
    M5.Lcd.clear();
    gui_objects=array_create(1);
    //Register objects, widgets, etc. here
    init_statusbar();
    gui_add_object(&draw_statusbar);
    gui_add_object(&draw_clock);
    klog(INFO,"gui","Initialization complete");
}

void gui_run(void){
    gui_init();
    gassert(gui_objects);
    for(;;){
       for(int i=0;i<gui_objects->sz;++i){
           gui_object* obj=(gui_object*)gui_objects->base[i];
           obj->draw();
       }
       ksleep(SLEEP_MS); 
    }
}

