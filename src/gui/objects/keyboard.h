#ifndef GUI_KEYBOARD_H
#define GUI_KEYBOARD_H

#include <stdint.h>
#include <gui/gui.h>

#define BTNROLE_NONE  0
#define BTNROLE_OK    1
#define BTNROLE_DOWN  2
#define BTNROLE_LEFT  3
#define BTNROLE_RIGHT 4
#define BTNROLE_UP    5
#define KEYBOARD_DOWN_PADDING 220
typedef struct _keyboard
{
    uint8_t btnA_role;
    uint8_t btnB_role;
    uint8_t btnC_role;
} keyboard_t;


GUIObject *init_keyboard(void);
#endif