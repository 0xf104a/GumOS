#include "battery.h"

#include <kernel/io.h>
#include <kernel/klog.h>
#include <kernel/ktask.h>
#include <kernel/kernel.h>
#include <gui/gui.h>

#include <stdint.h>

#include <M5Stack.h>

bool requires_clean=true;
double lastLevel=-1.0;
uint16_t draw_battery_icon(uint16_t pos){
    kmutex_lock();
    uint8_t *isCharging=NULL;
    gassert(readf("/dev/charge",&isCharging)==sizeof(uint8_t));
    M5.Lcd.drawRoundRect(pos-16,0,24,13,3,65535);
    M5.Lcd.fillRoundRect(pos+7,3,4,6,1,65535);
    if(*isCharging){
        //Draw charge icon
        if(requires_clean){
          M5.Lcd.fillRoundRect(pos-14,2,21,10,1,0);
          requires_clean=false;
        }
        pos+=2;
        M5.Lcd.fillTriangle(pos-6,0,pos-8,6,pos-10,6,65535);
        M5.Lcd.fillTriangle(pos-10,12,pos-4,4,pos-6,4,65535);
    }else{
        if(!requires_clean){
            requires_clean=true;
        }
        double level=M5.Power.getBatteryLevel();
        if(level<lastLevel){
          M5.Lcd.fillRoundRect(pos-14,2,21,10,1,0);
          M5.Lcd.fillRoundRect(pos-14,2,19*(level/100.0),9,1,65535);
          requires_clean=false;
        }
        if(lastLevel=-1.0){
          M5.Lcd.fillRoundRect(pos-14,2,19*(level/100.0),9,1,65535);
        }
        //Handle battery level
        lastLevel=level;
    }
    free(isCharging);
    //klog(DEBUG,"gui","Battery icon handler");
    kmutex_unlock();
    return pos-16;
}