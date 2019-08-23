#ifndef BOOTSPLASH_H
#define BOOTSPLASH_H

#include <kernel/kevent.h>

void draw_splash(void *p);
void boot_progress_handler(kevent_t *event);

#endif