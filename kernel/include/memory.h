#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include "uart.h"
#include "string.h"
#include "panic.h"

void kmalloc_init(void);
void *kmalloc(size_t size);
void kfree(void *ptr);

#endif