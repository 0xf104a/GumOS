#ifndef GUI_H
#define GUI_H

#include <stdint.h>

#define gassert(a) _gassert(a,__FILE__,__LINE__)

typedef struct{
   void (*draw)(void);
} gui_object;

void _gassert(bool a,const char *fname,int line);
void gui_run(void);
uint16_t color16(uint16_t r, uint16_t g, uint16_t b);

#endif