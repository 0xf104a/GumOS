#include "widget.h"
#include <gui/objects/widgets.h>
#include <gui/gui.h>
#include <kernel/klog.h>
#include <kernel/io.h>
#include <M5Stack.h>
#include <Free_Fonts.h>
#include <stdio.h>
#define WIDGET_NAME "Clock"

void activate(void){
    background(color16(0,0,160));
}
void on_draw(void){
    M5.Lcd.setTextColor(color16(255,255,255),color16(0,0,160));
    M5.Lcd.setTextSize(1);
    M5.Lcd.setFreeFont(FSS24);
    uint8_t *freeHeap=NULL;
    gassert(readf("/dev/heap",&freeHeap)==sizeof(uint32_t));
    uint32_t heap=*(uint32_t*)freeHeap;
    char *time=timestr();
//    background(color16(255,255,255));
    M5.Lcd.drawCentreString(time,1,16,GFXFF);
    gassert(freeHeap!=NULL);
    free(freeHeap);
    free(time);
    
}

void on_button(uint8_t id){

}

Widget* DebugWidget(void){
    Widget *w=(Widget*)malloc(sizeof(Widget));
    w->activate=&activate;
    w->name=(char*)malloc(sizeof(char)*(strlen(WIDGET_NAME)+1));
    strcpy(w->name,WIDGET_NAME);
    //klog(DEBUG,"gui/debug_widget","w->name=%s",w->name);
    w->on_button=&on_button;
    w->on_draw=&on_draw;
    return w;
}