#ifndef IO_H
#define IO_H

#include "types/array.h"

#include <stdint.h>

typedef struct _io
{
    array_t *io_output_streams;
} io_handle;

typedef struct io_out_stream
{
    void (*on_out)(char *str);
    char *name;
} io_out_stream;

void io_init(void);
void io_register_stream(const char *name,void (*on_out)(char *str));
void kprint(char *s);
void kprintf(const char* format, ...);
uint64_t readf(char *path, uint8_t **buf);
int writef(char *path,uint8_t *buf, size_t sz);
uint64_t sreadf(char *path, char **_buf);
void reset_lcd(void);

#endif