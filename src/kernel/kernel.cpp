#include "kernel.h"
#include "klog.h"
#include "ktask.h"
#include "kconfig.h"
#include "kmodule.h"
#include "kevent.h"
#include "io.h"
#include "error.h"
#include "microramfs/microramfs.h"

#include <M5Stack.h>
#include <Arduino.h>


  /*typedef enum {
    JPEG_DIV_NONE,
    JPEG_DIV_2,
    JPEG_DIV_4,
    JPEG_DIV_8,
    JPEG_DIV_MAX
  } jpeg_div_t;*/


kernel *khandle;

void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
  int32_t i, j, byteWidth = (w + 7) / 8;
  for (j = 0; j < h; j++) {
    for (i = 0; i < w; i++) {
      if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        M5.Lcd.drawPixel(x + i, y + j, color);
      }
    }
  }
}


void krnl_log(char *_str){
    if(khandle->is_console_mode){
        M5.Lcd.print(_str);
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
        }
        klog(FATAL,"kernel","Asseratation failed at %s:%d",fName,fLine);
        klog(FATAL,"kernel","Will kill kernel now!!!");
        khandle->kAlive=false;
        kmutex_unlock();
        ktask_kill(kGetPid());
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
    klog(INFO,"kernel","Initilized outputstream.");
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
    init_modules();
    klog(INFO,"kernel","Entering kernel loop.");
    while(khandle->kAlive){
        ksleep(KLOOP_SLEEP_MS);
        uint32_t heap_free=xPortGetFreeHeapSize();
        kassert(heap_free>MIN_FREE_HEAP);
        kassert(writef("/dev/heap",(uint8_t *)(&heap_free),sizeof(uint32_t))==NO_ERROR);
        uint8_t *panic=NULL;
        kassert(readf("/dev/panic",&panic)==sizeof(uint8_t));
        kassert(!panic[0]);
        //M5.update();
        free(panic);
    }
    klog(INFO,"kernel","Kernel loop exited.Will kill all tasks");
    kill_all();
}

void kSetConsoleMode(bool state){
    klog(INFO,"kernel","Set console mode to:%d",state);
    kmutex_lock();
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