#include "statusbar.h"

#include "icons/battery.h"
#include "gui.h"

#include <stdlib.h>
#include <string.h>

#include <kernel/kconfig.h>
#include <kernel/types/array.h>

array_t* status_icons;

void add_icon(uint16_t (*draw)(uint16_t pos)){
    gassert(status_icons);
    gui_icon* icon=(gui_icon*)malloc(sizeof(gui_icon));
    icon->draw=draw;
    array_add(status_icons,icon);
}
void init_statusbar(void){
    status_icons=array_create(1);
    //Add icon handelrs here
    add_icon(&draw_battery_icon);
}
void draw_statusbar(void){
    int i=0;
    uint16_t pos=LCD_WIDTH-16;
    for(;i<status_icons->sz;++i){
        gui_icon* icon=(gui_icon*)status_icons->base[i];
        pos=icon->draw(pos);
    }
}