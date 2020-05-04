#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

void screen_timeout(void);
void _set_software_brightness(uint8_t val);
void lcd_begin(void);

#endif