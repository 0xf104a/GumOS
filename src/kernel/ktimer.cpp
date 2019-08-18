#include "ktimer.h"
#include "klog.h"
#include "kconfig.h"
#include "ktask.h"
#include "kernel.h"
#include "io.h"
#include "commons/commons.h"
#include "error.h"


#include <FreeRTOS/FreeRTOS.h>
#include <freeRTOS/task.h>
#include <stdlib.h>
#include <string.h>


void ksleep(int ms){
    ms/=portTICK_PERIOD_MS;
    vTaskDelay(ms);
}

void time2fs(ktimer *timer){
    kassert(writef("/dev/rtime",(uint8_t *)(&timer->unix_time),sizeof(double))==0);
    uint64_t t=(uint64_t)timer->unix_time;
    kassert(writef("/dev/utime",(uint8_t*)(&t),sizeof(uint64_t))==0);
}

void ktimer_loop(void *_timer){
    ktimer *timer=(ktimer *)_timer;
    klog(DEBUG,"ktimer","Got timer at address:%d",_timer);
    for(;;){
        ksleep(TIMER_SLEEP_TIME);
        kmutex_lock();
        timer->unix_time+=TIMER_SLEEP_TIME/1000.0;
        time2fs(timer);
        kmutex_unlock();
    }
}
void ktimer_init(ktimer *timer,double current_time){
    klog(INFO,"ktimer","Initializing timer with time %f",current_time);
    timer->unix_time=current_time;
    klog(DEBUG,"ktimer","Initial timer located at address:%d",timer);
    create_file(khandle->ramfs,"/dev/utime");
    create_file(khandle->ramfs,"/dev/rtime");
    ktask_start("ktimer",&ktimer_loop,(void *)timer,TIMER_PRIORITY);
}