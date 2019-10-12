#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <gui/gui.h>
#include <kernel/types/array.h>

#define INITIAL_APPNAME "Setup"

typedef struct statusbar
{
    array_t *icons;
    char *appname;
} statusbar_t;

typedef struct
{
    uint8_t id;
    bool (*draw)(uint16_t pos); //returns true if icon was drawn
} icon_t;

GUIObject *init_statusbar(void);
void set_appname(char *name);

#endif