#include "battery.hpp"

#include <kernel/io.h>
#include <gui/gui.h>

#include <M5Stack.h>

bool draw_battery_icon(uint16_t pos){
    uint8_t *isCharging=NULL;
    uint8_t *icon=NULL;
    gassert(readf("/dev/charge",&isCharging)==sizeof(uint8_t));
    if(*isCharging){
        //Draw charge icon
        gassert(readf("/res/ui/icons/battery/charging.jpg",&icon)>0);
        M5.Lcd.drawJpg(icon,pos,0);
    }else{
        //Handle battery level
    }
    gassert(icon&&isCharging);
    free(isCharging);
    free(icon);
    return true;
}