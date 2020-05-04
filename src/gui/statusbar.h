#ifndef STATUSBAR_H
#define STATUSBAR_H


#include <stdint.h>
#include <kernel/types/array.h>

typedef struct{
   uint16_t (*draw)(uint16_t pos);
} gui_icon;



void draw_statusbar(void);
void init_statusbar(void);

#endif
