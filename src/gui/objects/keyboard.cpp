#include "keyboard.h"

#include <gui/gui.h>
#include <kernel/types/array.h>
#include <kernel/klog.h>
#include <kernel/kevent.h>
#include <stdlib.h>
#include <M5Stack.h>

keyboard_t *xkeyboard;

void draw_btn(uint16_t pos,uint8_t role){
    if(role==BTNROLE_NONE){
        return ;
    }
    if(role==BTNROLE_OK){
        M5.Lcd.fillCircle(pos,KEYBOARD_DOWN_PADDING,7,color16(255,255,255));
    }
    if(role==BTNROLE_RIGHT){
        M5.Lcd.fillTriangle(pos,KEYBOARD_DOWN_PADDING+7,pos,KEYBOARD_DOWN_PADDING-7,pos+7,KEYBOARD_DOWN_PADDING,color16(255,255,255));
    }
    if(role==BTNROLE_LEFT){
        M5.Lcd.fillTriangle(pos,KEYBOARD_DOWN_PADDING+7,pos,KEYBOARD_DOWN_PADDING-7,pos-7,KEYBOARD_DOWN_PADDING,color16(255,255,255));
    }
}
void draw_keyboard(void *kboard){
    draw_btn(70,xkeyboard->btnA_role);
    draw_btn(160,xkeyboard->btnB_role);
    draw_btn(250,xkeyboard->btnC_role);
}
GUIObject *init_keyboard(void){
    klog(INFO,"gui","Initializing statusbar.");
    xkeyboard=(keyboard_t *)malloc(sizeof(keyboard_t));
    xkeyboard->btnA_role=BTNROLE_LEFT;
    xkeyboard->btnB_role=BTNROLE_OK;
    xkeyboard->btnC_role=BTNROLE_RIGHT;
    GUIObject *__keyboard=gui_create_object("keybar",xkeyboard, &draw_keyboard);
    return __keyboard;
}