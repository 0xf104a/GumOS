#ifndef STATUSBAR_H
#define STATUSBAR_H

#include <gui/gui.h>
#include <kernel/types/array.h>

typedef struct statusbar
{
    array_t *icons;
} statusbar_t;

GUIObject *init_statusbar(void);

#endif