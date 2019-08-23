#ifndef GUI_H
#define GUI_H

#include <kernel/types/hashtable.h>

#include <stdint.h>

#define gassert(a) _gassert(a,__FILE__,__LINE__)
typedef struct{
    char *name;
    void *param;
    void (*draw)(void *param);
} GUIObject;

typedef struct{
   hashtable *objects;
   bool running;
} GUI;


void init_gui(void);
void launch_gui(void);
GUIObject *gui_create_object(char *name, void *param, void (*draw)(void *param));
void gui_add_object(GUIObject *object);
uint16_t color16(uint16_t r, uint16_t g, uint16_t b);
void _gassert(bool a,const char *fname,int line);

#endif