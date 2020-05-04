#ifndef KCONFIG_H
#define KCONFIG_H

#define KERENEL_VERSION "1.0a Donetsk"
#define KLOOP_SLEEP_MS 500
#define ARRAY_CAPACITY 1
#define HASHTABLE_CAPACITY 200
#define THR_SLEEP 10
#define TASK_STACK_DEPTH 2500
#define TIMER_SLEEP_TIME 100 //ms 
#define TIMER_PRIORITY 1 //priority for task timer
#define TIMER_INITIAL_TIME 1588497578.0+10800.0
#define MICRORAMFS_SIZE 512000
#define MIN_FREE_HEAP 2048 //Kernel will panic if free heap is less
#define MAX_CURSOR_POS 224
#define LCD_HEIGHT 240
#define LCD_WIDTH 320
/* Modules */
#define LOAD_GUI       1
#define LOAD_POWERCTL  1
#define LOAD_SOUNDCTL  1
#define LOAD_HIDCTL    1
#define LOAD_AUTOSTART 1
#endif
