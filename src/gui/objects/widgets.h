#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include <kernel/types/array.h>
#include <kernel/types/hashtable.h>
#include <stdint.h>
#include <stdbool.h>
#include <gui/gui.h>

typedef struct widget
{
    char* name;
    void (*on_draw)(void);
    void (*on_button)(uint8_t);
    void (*activate)(void);
} Widget;

extern bool is_widget_active;

typedef struct WidgetViewer
{
    Widget *active_widget;
    array_t *widgets;
    size_t widget_id;
};

GUIObject *init_widgets(void);
void add_widget(Widget *widget);
void background(uint32_t __color);
#endif