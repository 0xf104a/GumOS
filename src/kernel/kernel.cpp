#include "kernel.h"
#include "klog.h"
#include "ktask.h"
#include "kconfig.h"
#include "kmodule.h"
#include "kevent.h"
#include "io.h"
#include "error.h"
#include "microramfs/microramfs.h"

#include <lua/lexec.h>

#include <M5Stack.h>
#include <Free_Fonts.h>
#include <Arduino.h>


kernel* khandle;


int16_t cursor_x=0;
int16_t cursor_y=0;

void update_cursor_pos(char* str){
    int16_t i=0,len=strlen(str);
    for(;i<len;++i){
        if(str[i]=='\n'){
            cursor_y+=8;
            cursor_x=0;
        }else{
            cursor_x+=6;
        }
        if(cursor_x>=LCD_WIDTH-6){
            cursor_x=0;
            cursor_y+=8;
        }
    }
}
void krnl_log(char* _str){//Debug output strem
    if(khandle->is_console_mode){
        if(cursor_y>MAX_CURSOR_POS){
            cursor_x=0;
            cursor_y=0;
            reset_lcd();
        }
        M5.Lcd.fillRect(cursor_x, cursor_y, 4, 8, 0);
        update_cursor_pos(_str);
        M5.Lcd.print(_str);
        M5.Lcd.fillRect(cursor_x, cursor_y, 4, 8, 65535);
    }
    char *str=(char*)malloc(sizeof(char)*(strlen(_str)+1));
    str=strcpy(str,_str);
    if(str[strlen(str)-1]=='\n'){
        str[strlen(str)-1]='\0';
        Serial.println(str);
    }else{
        Serial.print(str);
    }
    free(str);
}

void _kassert(int a,const char *fName, int fLine){
    if(!a){
        kmutex_lock();
        if(!khandle->is_console_mode){
            khandle->is_console_mode=true;
            M5.Lcd.setTextColor(65535,0);
            M5.Lcd.setTextSize(1);
            M5.Lcd.setFreeFont(TT1);
        }
        klog(FATAL,"kernel","Asseratation failed at %s:%d",fName,fLine);
        klog(FATAL,"kernel","Will kill kernel now!!!");
        khandle->kAlive=false;
        kmutex_unlock();
    }
}

double kget_time(void){
    double result=-1.0;
    kmutex_lock();
    result=khandle->timer->unix_time;
    kmutex_unlock();
    kassert(result>=0.0);
    return result;
}

microramfs *initramfs(void){
    microramfs *fs=init_ram_fs(MICRORAMFS_SIZE);
    load_image(fs);
    //create_dir(fs,"/","dev");
    return fs;
}

void kstart(void){
    khandle=(kernel *)malloc(sizeof(kernel));
    khandle->kAlive=true;
    khandle->modules=hashtbl_create(HASHTABLE_CAPACITY);
    khandle->tasks=array_create(ARRAY_CAPACITY);
    khandle->timer=(ktimer *)malloc(sizeof(ktimer));
    khandle->kSemaphore=xSemaphoreCreateRecursiveMutex();
    khandle->is_console_mode=true;
    Serial.begin(115200);
    M5.begin();
    io_init();
    io_register_stream("kernel",&krnl_log);
    kprintf("Initialized IO\n");
    kprintf("Kernel version: %s\n",KERENEL_VERSION);
    klog(INFO,"kernel","Initialized outputstream.");
    khandle->__self=xTaskGetCurrentTaskHandle();
    klog(DEBUG,"kernel","Task handle:%d",khandle->__self);
    khandle->ramfs=initramfs();
    klog(INFO,"kernel","Initialized microramfs. Total size:%d, Used:%d",khandle->ramfs->size,khandle->ramfs->used);
    ktimer_init(khandle->timer,TIMER_INITIAL_TIME);
    create_file(khandle->ramfs, "/dev/heap");//Memoryload
    create_file(khandle->ramfs, "/dev/panic");
    uint8_t panic=0;
    kassert(writef("/dev/panic",&panic,sizeof(uint8_t))==NO_ERROR);
    khandle->event_mgr=eventmgr_init();
    ktask_start("init",&init_modules,NULL,0);
    klog(INFO,"kernel","Entering kernel loop.");
    //liblua_exec("klog(0,\"hello.lua\", \"Hello from lua!\")","hi");
    while(khandle->kAlive){
        ksleep(KLOOP_SLEEP_MS);
        uint32_t heap_free=xPortGetFreeHeapSize();
        kassert(heap_free>MIN_FREE_HEAP);
        kassert(writef("/dev/heap",(uint8_t *)(&heap_free),sizeof(uint32_t))==NO_ERROR);
        uint8_t *panic=NULL;
        kassert(readf("/dev/panic",&panic)==sizeof(uint8_t));
        //klog(DEBUG,"kernel","%d",heap_free);
        kassert(!panic[0]);
        M5.update();
        free(panic);
    }
    klog(INFO,"kernel","Kernel loop exited.");
    kill_all();
}

void kSetConsoleMode(bool state){
    klog(INFO,"kernel","Set console mode to:%d",state);
    kmutex_lock();
    if(!state){
        cursor_x=0;
        cursor_y=0;
    }
    khandle->is_console_mode=state;
    kmutex_unlock();
}

bool kGetConsoleMode(void){
    bool state;
    kmutex_lock();
    state=khandle->is_console_mode;
    kmutex_unlock();
    return state;
}