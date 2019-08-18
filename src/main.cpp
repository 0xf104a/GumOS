#include <M5Stack.h>
#include <rom/rtc.h>
#include <esp_sleep.h>
#include <esp_bt_main.h>
#include <esp_wifi.h>
#include "kernel/kernel.h"
#include "kernel/klog.h"
#include "kernel/io.h"

int main(void){
    kstart();
}

void loop(){
    klog(FATAL,"__init","kernel panic -- dead kernel");
    klog(FATAL,"__init","Please reboot manually");
    for(;;){
        M5.update();
    }
}

void setup(){
    main();
}

