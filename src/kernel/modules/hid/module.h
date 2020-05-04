#ifndef HID_MODULE_H
#define HID_MODULE_H

#define HID_SLEEP 200 //ms
#define KEYBOARD_I2C_ADDR     0x08
#define KEYBOARD_INT          5
#define READI2CSUBADDR        0

void init_hid(void);
#endif