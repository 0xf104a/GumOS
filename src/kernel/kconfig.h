#ifndef KCONFIG_H
#define KCONFIG_H

#define KERENEL_VERSION "1.04 f0u7 15 numb3r 0f d3ath"
#define KLOOP_SLEEP_MS 50
#define ARRAY_CAPACITY 1
#define HASHTABLE_CAPACITY 200
#define THR_SLEEP 10
#define TASK_STACK_DEPTH 2048
#define TIMER_SLEEP_TIME 100 //ms 
#define TIMER_PRIORITY 1 //priority for task timer
#define TIMER_INITIAL_TIME 1570896401.0+10800.0
#define MICRORAMFS_SIZE 51200
#define MIN_FREE_HEAP 2048 //Kernel will panic if free heap is less
/* Modules */
#define LOAD_GUI      1
#define LOAD_POWERCTL 1
#define LOAD_SOUNDCTL 1
#define LOAD_HIDCTL   1

#endif