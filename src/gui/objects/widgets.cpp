#include "widgets.h"

#include <stdlib.h>
#include <kernel/kconfig.h>
#include <kernel/klog.h>
#include <gui/gui.h>
#include <gui/objects/statusbar.hpp>
#include <M5Stack.h>
#include <Free_Fonts.h>



WidgetViewer *widgets;
bool is_widget_active;

void acqireKeyboard(void){
    is_widget_active=false;
}
void giveKeyboard(void){
    is_widget_active=true;
}
void keyboard_handler(void){

}
void draw(void* placeholder){
    if(!widgets->active_widget){
        M5.Lcd.setTextSize(1);
        M5.Lcd.setFreeFont(FM9);
        M5.Lcd.drawCentreString("No widgets are available",160,120,GFXFF);
    }else{
        widgets->active_widget->on_draw();
    }
}

GUIObject *init_widgets(void){
    is_widget_active=false;
    widgets=(WidgetViewer*)malloc(sizeof(WidgetViewer));
    widgets->active_widget=NULL;
    widgets->widgets=array_create(ARRAY_CAPACITY);
    return gui_create_object("widgets",NULL, &draw);
}

void add_widget(Widget *__widget)
{
    if(widgets->widgets->sz==0){
        klog(DEBUG,"gui","Activating widget:%s",__widget->name);
        set_appname(__widget->name);
        __widget->activate();
        widgets->active_widget=__widget;
        widgets->widget_id=0;
    }
    array_add(widgets->widgets,__widget);
}

