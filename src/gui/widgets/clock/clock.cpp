#include "clock.h"

#include <kernel/commons/commons.h>
#include <kernel/kconfig.h>
#include <gui/gui.h>
#include <time.h>

#include <M5Stack.h>
#include <Free_Fonts.h>

#include <stdint.h>

bool drawn=false;
void draw_clock(void){
    uint64_t now_r=utime();
    time_t now = (time_t)now_r;
    struct tm now_tm = *localtime(&now);
    char* time=(char*)malloc(sizeof(char)*16);
    char* info=(char*)malloc(sizeof(char)*255);
    strftime(time,16*sizeof(char),"%H:%M",&now_tm);
    strftime(info,255*sizeof(char),"%A, %d %B %Y",&now_tm);
    M5.Lcd.setTextColor(color16(255,255,255),color16(0,0,0));
    M5.Lcd.setTextSize(1);
    M5.Lcd.setFreeFont(FSS24);
//    background(color16(255,255,255));
    M5.Lcd.drawCentreString(time,LCD_WIDTH/2,(LCD_HEIGHT-16)/2-24,GFXFF);
    //Draw weekday, month,etc
    if((now_r%3600==0)||(!drawn)){
        M5.Lcd.setFreeFont(FSS9);
        M5.Lcd.drawCentreString(info,LCD_WIDTH/2,(LCD_HEIGHT-16)/2+24,GFXFF);
        drawn=true;
    }
    free(time);
    free(info);
}

