#include "module.h"

#include <kernel/kernel.h>
#include <kernel/klog.h>
#include <kernel/ktask.h>
#include <kernel/microramfs/microramfs.h>
#include <kernel/io.h>
#include <kernel/error.h>

#include <M5Stack.h>

uint8_t vol=0;

void soundctl_loop(void *stub){
    for(;;){
        ksleep(SPEAKERCTL_SLEEP_TIME);
        uint16_t *rate=NULL;
        kassert(readf("/dev/speaker_rate",(uint8_t **)(&rate))>0);
        if(rate[0]>0){
            uint8_t *_lock=NULL;
            kassert(readf("/var/lock/speaker",&_lock)>0);
            if(_lock[0]){
                klog(ERROR,"soundctl","Speaker is already locked! Have you loaded any other sound driver?");
                free(_lock);
                continue;
            }
            free(_lock);
            uint8_t lock=1;
            kassert(writef("/var/lock/speaker",&lock,sizeof(uint8_t))==NO_ERROR);
            uint8_t *music_data;
            readf("/dev/speaker",&music_data);
            M5.Speaker.playMusic(music_data,rate[0]);
            lock=0;
            kassert(writef("/var/lock/speaker",&lock,sizeof(uint8_t))==NO_ERROR);
            free(music_data);
        }
        free(rate);
    }
}
void init_soundctl(void){
    klog(INFO,"soundctl","Starting soundctl");
    M5.Speaker.begin();
    M5.Speaker.mute();
    uint16_t rate=0;
    uint8_t lock=0;
    kassert(create_file(khandle->ramfs,"/dev/speaker_vol")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/dev/speaker")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/dev/speaker_rate")==NO_ERROR);
    kassert(create_file(khandle->ramfs,"/var/lock/speaker")==NO_ERROR);
    kassert(writef("/dev/speaker_vol",&vol,sizeof(uint8_t))==NO_ERROR);
    kassert(writef("/dev/speaker_rate",(uint8_t *)(&rate),sizeof(uint16_t))==NO_ERROR);
    kassert(writef("/var/lock/speaker",&lock,sizeof(uint8_t))==NO_ERROR);
    ktask_start("soundctl", &soundctl_loop, NULL,0);
}