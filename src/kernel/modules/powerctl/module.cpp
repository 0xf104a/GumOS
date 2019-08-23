#include "module.h"
#include "ip5306.h"

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/microramfs/microramfs.h>
#include <kernel/io.h>
#include <kernel/error.h>
#include <kernel/ktask.h>
#include <kernel/ktimer.h>
#include <kernel/kevent.h>

#include <M5Stack.h>

uint8_t charge_state;
uint8_t _brightness=BRIGHTNESS_DEFAULT;
void powerctl_loop(void *p){
    for(;;){
        ksleep(POWERCTL_SLEEP);
        uint8_t *power=NULL;
        uint8_t *brightness=NULL;
        kassert(readf("/dev/power",&power)==sizeof(uint8_t));
        kassert(readf("/dev/brightness",&brightness)==sizeof(uint8_t));
        if(_brightness!=brightness[0]){
            klog(INFO,"powerctl","Setting brightness to %d",brightness[0]);
            M5.Lcd.setBrightness(brightness[0]);
            _brightness=brightness[0];
        }
        if(!power[0]){
            klog(INFO,"powerctl","Power set to 0. Sytem halt.");
            M5.Power.powerOFF();
        }
        uint8_t charge;
        M5.I2C.readByte(IP5306_ADDR, IP5306_REG_READ3, &charge);
        uint8_t _charge_state=M5.Power.isCharging();
        if(_charge_state!=charge_state){
           event_create(khandle->event_mgr,"powerctl_trigger_charger",NULL);
           klog(INFO,"powerctl","Charger state has changed to %d",_charge_state);
           charge_state=_charge_state;
           kassert(writef("/dev/charge",&charge_state,sizeof(uint8_t))==NO_ERROR);
        }
        kassert(writef("/dev/battery",&charge,sizeof(uint8_t))==NO_ERROR);
        free(power);
        free(brightness);
    }
}

void init_powerctl(void){
    klog(INFO,"powerctl","Starting powerctl");
    M5.Power.begin();
    kassert(create_file(khandle->ramfs,"/dev/power")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/dev/battery")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/dev/charge")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/dev/brightness")==NO_ERROR);
    uint8_t power=1;
    uint8_t charge;
    uint8_t brightness=_brightness;
    M5.Lcd.setBrightness(brightness);
    charge_state=M5.Power.isCharging();
    M5.I2C.readByte(IP5306_ADDR, IP5306_REG_READ3, &charge);
    kassert(writef("/dev/power",&power,sizeof(uint8_t))==NO_ERROR);
    kassert(writef("/dev/battery",&charge,sizeof(uint8_t))==NO_ERROR);
    kassert(writef("/dev/charge",&charge_state,sizeof(uint8_t))==NO_ERROR);
    kassert(writef("/dev/brightness",&brightness,sizeof(uint8_t))==NO_ERROR);
    klog(INFO,"powerctl","Charger state has changed to %d",charge_state);
    kassert(M5.Power.canControl());
    ktask_start("powerctl", &powerctl_loop, NULL,0);
}