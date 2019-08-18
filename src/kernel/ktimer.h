#ifndef KTIMER_H
#define KTIMER_H

typedef struct{
    double unix_time;
}ktimer;

void ksleep(int ms);
void ktimer_init(ktimer *timer,double current_time);

#endif