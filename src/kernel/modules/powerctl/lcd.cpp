#include "lcd.h"
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

uint32_t timeout=600;
uint32_t idle=0;
uint8_t software_brightness=200;
bool display_state=true;

void _set_software_brightness(uint8_t val){
    software_brightness=val;
}
bool setPowerBoostKeepOn(bool en) {
  uint8_t data;
  if (M5.I2C.readByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data) == true) {
    return M5.I2C.writeByte(IP5306_ADDR, IP5306_REG_SYS_CTL0, en ? (data | BOOST_OUT_BIT)  : (data & (~BOOST_OUT_BIT)));
  }
  return false;
}
void handler(kevent_t *event){
    if(!strcmp(event->name,"btn_a_pressed")||!strcmp(event->name,"btn_b_pressed")||!strcmp(event->name,"btn_c_pressed")||!strcmp(event->name,"powerctl_charger_plug")){
        if(idle>0&&idle<timeout/2){
            M5.Lcd.setBrightness(software_brightness);
            idle=timeout;
            klog(INFO,"powerctl","Restoring brightness");
            return ;
        }
        idle=timeout;
        if(!display_state){
            klog(INFO,"powerctl","Waking up display");
            M5.Lcd.wakeup();
            M5.Lcd.setBrightness(software_brightness);
            display_state=true;
            event_create(khandle->event_mgr,"screen_wakeup",NULL);
        }
    }
}

void lcd_begin(void){
    event_handler_add(khandle->event_mgr, &handler);
    idle=timeout;
}

void screen_timeout(void){
    if(idle==timeout/2){
        M5.Lcd.setBrightness(software_brightness/2);
        klog(INFO,"powerctl","Reduced display brightness.");
    }
    if(idle==0){
        if(!setPowerBoostKeepOn(true)){
            klog(ERROR,"powerctl","Power boost failed!");
            klog(ERROR,"powerctl","Will not enter display sleep mode!");
        }else{
            display_state=false;
            M5.Lcd.setBrightness(0);
            M5.Lcd.sleep();
            klog(INFO,"powerctl","Display entered sleep mode.");
            event_create(khandle->event_mgr,"screen_sleep",NULL);
        }
    }
    idle--;
}
