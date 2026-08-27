#ifndef MEMORY_H
#define MEMORY_H

#include "kstring.h"
#include "panic.h"
#include "uart.h"
#include <stddef.h>

void kmalloc_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif